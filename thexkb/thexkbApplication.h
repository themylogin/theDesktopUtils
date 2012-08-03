#ifndef THEXKBAPPLICATION_H
#define THEXKBAPPLICATION_H

#include <QApplication>
#include <QDebug>
#include <QList>
#include <QSystemTrayIcon>
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

class thexkbApplication : public QApplication
{
    Q_OBJECT

public:
    thexkbApplication(int& argc, char** argv)
        : QApplication(argc, argv)
    {
        dpy = QX11Info::display();

        Atom a_XKB_RULES_NAMES = XInternAtom(this->dpy, "_XKB_RULES_NAMES", False);
        if (a_XKB_RULES_NAMES == None)
        {
            qDebug() << "XInternAtom(this->dpy, \"_XKB_RULES_NAMES\", False) == None";
            return;
        }

        int dummy;
        if (!XkbQueryExtension(this->dpy, &dummy, &this->xkb_event_type, &dummy, &dummy, &dummy))
        {
            qDebug() << "XkbQueryExtension(this->dpy, &dummy, &this->xkb_event_type, &dummy, &dummy, &dummy) == 0";
            return;
        }
        XkbSelectEventDetails(this->dpy, XkbUseCoreKbd, XkbStateNotify, XkbAllStateComponentsMask, XkbGroupStateMask);

        this->icons.push_back(QIcon("./us.png"));
        this->icons.push_back(QIcon("./ru.png"));

        XkbStateRec xkb_state;
        if (XkbGetState(this->dpy, XkbUseCoreKbd, &xkb_state) != Success)
        {
            qDebug() << "XkbGetState(this->dpy, XkbUseCoreKbd, &xkb_state) != Success";
            return;
        }
        this->trayIcon.setIcon(this->icons[xkb_state.group]);
        this->trayIcon.show();
        connect(&this->trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(changeGroup()));
    }

    bool x11EventFilter(XEvent* event)
    {
        if (event->type == this->xkb_event_type)
        {
            XkbEvent* xkbev = (XkbEvent*)event;
            if (xkbev->any.xkb_type == XkbStateNotify)
            {
                this->trayIcon.setIcon(this->icons[xkbev->state.group]);
            }
        }

        return false;
    }

private slots:
    void changeGroup()
    {
        XkbStateRec xkb_state;
        if (XkbGetState(this->dpy, XkbUseCoreKbd, &xkb_state) != Success)
        {
            qDebug() << "XkbGetState(this->dpy, XkbUseCoreKbd, &xkb_state) != Success";
            return;
        }
        XkbLockGroup(this->dpy, XkbUseCoreKbd, (xkb_state.group + 1) % 2);
    }

private:
    Display* dpy;
    int xkb_event_type;
    QList<QIcon> icons;
    QSystemTrayIcon trayIcon;
};

#endif // THEXKBAPPLICATION_H
