#include <map>
#include <string>

#include <Python.h>

#include <Imlib2.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>

Display* display;
Window root;
Screen* screen;
std::map<std::string, Pixmap> pixmaps;

static PyObject*
set_wallpaper(PyObject* self, PyObject* args)
{
    const char* wallpaper;
    if (!PyArg_ParseTuple(args, "s", &wallpaper))
        return NULL;

    Pixmap pixmap;
    auto it = pixmaps.find(wallpaper);
    if (it == pixmaps.end())
    {
        pixmap = XCreatePixmap(display, root, screen->width, screen->height, DefaultDepth(display, DefaultScreen(display)));

        imlib_context_set_display(display);
        imlib_context_set_drawable(pixmap);
        imlib_context_set_visual(DefaultVisual(display, DefaultScreen(display)));

        auto image = imlib_load_image_immediately_without_cache(wallpaper);
        imlib_context_set_image(image);
        imlib_render_image_on_drawable(0, 0);
        imlib_free_image();

        pixmaps[wallpaper] = pixmap;
    }
    else
    {
        pixmap = it->second;
    }

    Atom prop_root = XInternAtom(display, "_XROOTPMAP_ID", False);
    Atom prop_esetroot = XInternAtom(display, "ESETROOT_PMAP_ID", False);

    XChangeProperty(display, root, prop_root, XA_PIXMAP, 32, PropModeReplace, (unsigned char *)&pixmap, 1);
    XChangeProperty(display, root, prop_esetroot, XA_PIXMAP, 32, PropModeReplace, (unsigned char *)&pixmap, 1);

    XSetWindowBackgroundPixmap(display, root, pixmap);
    XClearWindow(display, root);
    XUngrabServer(display);
    XFlush(display);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef WallpaperMethods[] = {
    {"set_wallpaper", set_wallpaper, METH_VARARGS,
     "Set X11 wallpaper."},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
initwallpaper()
{
    Py_InitModule("wallpaper", WallpaperMethods);

    display = XOpenDisplay(NULL);
    root = RootWindow(display, DefaultScreen(display));
    screen = ScreenOfDisplay(display, DefaultScreen(display));
}

int main(int argc, char* argv[])
{
    Py_SetProgramName(argv[0]);
    Py_Initialize();

    initwallpaper();

    return 0;
}
