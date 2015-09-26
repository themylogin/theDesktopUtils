#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/io.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int port;
int smarthome_socket = 0;
struct sockaddr_in smarthome_addr;

void set(int line, int on)
{    
    int state = inb(port);
    if (on)
    {
        state &= ~(1 << line);
    }
    else
    {
        state |= (1 << line);
    }
    outb(state, port);
        
    if (smarthome_socket)
    {
        char buf[2];
        buf[0] = line + '0';
        buf[1] = on + '0';
        sendto(smarthome_socket, buf, 2, MSG_DONTWAIT, (struct sockaddr *)&smarthome_addr, sizeof(smarthome_addr));
    }
}

int get(int line)
{
    int state = inb(port);
    return !(state & (1 << line));
}

int main(int argc, char* argv[])
{
    sscanf(argv[1], "%x", &port);
    if (ioperm(port, 1, 1))
    {
        perror("ioperm(port, 1, 1) failed");
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);    
    if (sock < 0)
    {
        perror("socket(AF_INET, SOCK_STREAM, 0) failed");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_family = AF_INET;
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("bind(sock, (struct sockaddr*)&addr, sizeof(addr)) failed");
        return 1;
    }

    if (listen(sock, 1024) < 0)
    {
        perror("listen(sock, 1024) failed");
        return 1;
    }
    
    if (argc > 4)
    {
        if ((smarthome_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        {
            perror("socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) failed");
            return 1;
        }
        
        inet_aton(argv[3], &smarthome_addr.sin_addr);
        smarthome_addr.sin_port = htons(atoi(argv[4]));
        smarthome_addr.sin_family = AF_INET;
    }

    int sizeof_addr = sizeof(addr);
    while (1)
    {
        int conn = accept(sock, (struct sockaddr*)&addr, (socklen_t *)&sizeof_addr);

        char buf[2], reply;
        read(conn, buf, 2);
        char command = buf[0];
        int line = buf[1] - '0';
        switch (command)
        {
            case '0':
                set(line, 0);
                break;
            case '1':
                set(line, 1);
                break;
            case 't':
                set(line, !get(line));
                break;
            case '?':
                reply = get(line) ? '1' : '0';
                write(conn, &reply, 1);
                break;
        }

        close(conn);
    }

    return 0;
}
