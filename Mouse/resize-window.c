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
    // move mouse to the bottom border of the window
    XWindowAttributes windowattr;
    XGetWindowAttributes(display, active, &windowattr);
    XWarpPointer(display, None, active, 0, 0, 0, 0, windowattr.width, windowattr.height);
    // http://standards.freedesktop.org/wm-spec/1.3/ar01s04.html#id2522561
    XClientMessageEvent evt;
    memset(&evt, 0, sizeof(evt));
    evt.type = ClientMessage;
    evt.window = active;
    evt.message_type = XInternAtom(display, "_NET_WM_MOVERESIZE", False);
    evt.format = 32;
    evt.data.l[0] = windowattr.x + windowattr.width;
    evt.data.l[1] = windowattr.y + windowattr.height;
    evt.data.l[2] = 4; // _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT
    // http://standards.freedesktop.org/wm-spec/1.3/ar01s03.html
    XSendEvent(display, root, False, (SubstructureNotifyMask|SubstructureRedirectMask), (XEvent*)&evt);
    XCloseDisplay(display);
    return 0;
}
