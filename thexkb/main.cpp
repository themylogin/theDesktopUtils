#include <QtGui/QApplication>
#include <QSystemTrayIcon>

#include "thexkbApplication.h"

int main(int argc, char *argv[])
{
    thexkbApplication a(argc, argv);

    return a.exec();
}
