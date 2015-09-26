#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/X.h>

// http://stackoverflow.com/questions/3909713/xlib-xgetwindowattributes-always-returns-1x1
Window get_toplevel_parent(Display* display, Window window)
{
    Window parent;
    Window root;
    Window* children;
    unsigned int num_children;
    
    while (1)
    {
        XQueryTree(display, window, &root, &parent, &children, &num_children);
        if (children)
        {
            XFree(children);
        }
        if (window == root || parent == root)
        {
            return window;
        }
        else
        {
            window = parent;
        }
    }
}

int main(int argc, char* argv[])
{
    Display* display = XOpenDisplay(NULL);
    Window root = RootWindow(display, DefaultScreen(display));
    // get active window
    Window active;
    int revert_to_return;
    XGetInputFocus(display, &active, &revert_to_return);
    active = get_toplevel_parent(display, active);
    // get mouse position
    Window window_returned;
    unsigned int mask_return;
    int root_x, root_y, win_x, win_y;
    XQueryPointer(display, root, &window_returned, &window_returned, &root_x, &root_y, &win_x, &win_y, &mask_return);
    // http://standards.freedesktop.org/wm-spec/1.3/ar01s04.html#id2522561
    XClientMessageEvent evt;
    memset(&evt, 0, sizeof(evt));
    evt.type = ClientMessage;
    evt.window = active;
    evt.message_type = XInternAtom(display, "_NET_WM_MOVERESIZE", False);
    evt.format = 32;
    evt.data.l[0] = root_x;
    evt.data.l[1] = root_y;
    evt.data.l[2] = 10; //  _NET_WM_MOVERESIZE_MOVE_KEYBOARD;
    // http://standards.freedesktop.org/wm-spec/1.3/ar01s03.html
    XSendEvent(display, root, False, (SubstructureNotifyMask|SubstructureRedirectMask), (XEvent*)&evt);
    XCloseDisplay(display);
    return 0;
}
