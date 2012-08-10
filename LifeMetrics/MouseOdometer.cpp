// g++ -o MouseOdometer -lm -lX11 MouseOdometer.cpp
#include <X11/Xlib.h>
#include <X11/X.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
 
int main(int argc, char* argv[])
{
    Display* display = XOpenDisplay(NULL);
    Window root = RootWindow(display, DefaultScreen(display));

    unsigned long long pixels = 0; // 2500px = 40mm
    int root_x_last = 0, root_y_last = 0;
    while (true)
    {
        pixels = 0;
        for (int t = 0; t < 5 /* minutes */ * 60 /* seconds */ * 1000000 /* microseconds */ / 10000 /* poll interval */; t++)
        {
            int root_x_return, root_y_return, win_x_return, win_y_return;
            Window root_return, child_return;
            unsigned int mask_return;
            XQueryPointer(display, root, &root_return, &child_return, &root_x_return, &root_y_return, &win_x_return, &win_y_return, &mask_return);

            pixels += sqrt((root_x_last - root_x_return) * (root_x_last - root_x_return) + (root_y_last - root_y_return) * (root_y_last - root_y_return));
            root_x_last = root_x_return;
            root_y_last = root_y_return;

            usleep(10000);
        }

        FILE* fh = fopen(argv[1], "a");
        fprintf(fh, "%d %d\n", time(NULL), pixels);
        fclose(fh);
    }

    XCloseDisplay(display);
    return 0;
}
