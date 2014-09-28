// g++ -o UsageStats UsageStats.cpp -lX11
#include <arpa/inet.h>
#include <errno.h>
#include <linux/input.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/X.h>

const int poll_interval = 10000; // microseconds

int main(int argc, char* argv[])
{
    int keyboard = -1;

    Display* display = XOpenDisplay(NULL);
    Window root = RootWindow(display, DefaultScreen(display));
        
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("192.168.0.1"); 
    addr.sin_port = htons(46404);

    int root_x_last = 0, root_y_last = 0;
    while (true)
    {
        int keys = 0;
        unsigned long long pixels = 0;
        char* titles[60]; int titles_length = 0;
        for (int t = 0; t < 1 /* minutes */ * 60 /* seconds */ * 1000000 /* microseconds */ / poll_interval; t++)
        {
            //
            struct input_event event;
            while (true)
            {
                if (read(keyboard, &event, sizeof(struct input_event)) > 0)
                {
                    if (event.type != EV_KEY && event.value == 1)
                    {
                        keys++;
                    }
                }
                else
                {
                    if (errno != EWOULDBLOCK)
                    {
                        keyboard = open("/dev/input/by-id/usb-Logitech_USB_Receiver-if02-event-kbd", O_RDONLY);
                        if (keyboard == -1)
                        {
                            break;
                        }

                        int flags = fcntl(keyboard, F_GETFL, 0);
                        fcntl(keyboard, F_SETFL, flags | O_NONBLOCK);
                    }
                    else
                    {
                        break;
                    }
                }
            }

            //
            int root_x_return, root_y_return, win_x_return, win_y_return;
            Window root_return, child_return;
            unsigned int mask_return;
            XQueryPointer(display, root, &root_return, &child_return, &root_x_return, &root_y_return, &win_x_return, &win_y_return, &mask_return);

            pixels += sqrt((root_x_last - root_x_return) * (root_x_last - root_x_return) + (root_y_last - root_y_return) * (root_y_last - root_y_return));
            root_x_last = root_x_return;
            root_y_last = root_y_return;

            //
            if (t % (1000000 / poll_interval) == 0)
            {
                Atom request = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
                Atom actual_type;
                int actual_format;
                unsigned long nitems = 0;
                unsigned long bytes_after;
                unsigned char *current_window_prop;
                XGetWindowProperty(display, root, request, 0, (~0L),
                                   False, AnyPropertyType, &actual_type,
                                   &actual_format, &nitems, &bytes_after,
                                   &current_window_prop);
                if (nitems > 0 && *current_window_prop > 0)
                {
                    Window current_window = *((Window*)current_window_prop);

                    XTextProperty prop;                
                    if (XGetWMName(display, current_window, &prop) >= 0)
                    {
                        char** string_list;
                        int string_count;
                        if (Xutf8TextPropertyToTextList(display, &prop, &string_list, &string_count) >= 0)
                        {
                            titles[titles_length++] = string_count > 0 ? strdup(string_list[0]) : NULL;
                            XFreeStringList(string_list);
                        }
                        else
                        {
                            titles[titles_length++] = NULL;
                        }
                    }
                    else
                    {
                        titles[titles_length++] = NULL;
                    }
                }
                else
                {
                    titles[titles_length++] = NULL;
                }
            }

            usleep(poll_interval);
        }

        char message[131072];
        sprintf(message, "application=usage_stats\nlogger=desktop\nlevel=info\nmsg=data\nkeys=%d\npixels=%llu\nmillimetres=%llu", keys, pixels, pixels * 32 / 1920);
        for (int i = 0; i < titles_length; i++)
        {
            char part[2048];
            sprintf(part, "\ntitles[%d]=%s", i, titles[i] ? titles[i] : "");
            strcat(message, part);
            if (titles[i])
            {
                free(titles[i]);
            }
        }

        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == 0)
        {
            write(fd, message, strlen(message));
            close(fd);
        }
    }

    XCloseDisplay(display);
    return 0;
}
