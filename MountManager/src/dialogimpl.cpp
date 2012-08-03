#include "dialogimpl.h"
#include <stdio.h>
#include <stdlib.h>
#include <QIcon>
#include <QMessageBox>
#include <QKeyEvent>
#include <QDebug>
//
DialogImpl::DialogImpl( QWidget * parent, Qt::WFlags f)
	: QDialog(parent, f)
{
    setupUi(this);
    setWindowIcon(QIcon("./blockdevice.png"));

    ReadMountedMedia();
    connect(listWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(UmountMedia(QListWidgetItem*)));
    installEventFilter(this);
}

void DialogImpl::ReadMountedMedia()
{
    FILE *fp;
    char buf[1024];
    QString mountedMedia;
    fp = popen("df -H | egrep '/dev/sd.*/media/sd.*'", "r");
    while (fgets(buf, sizeof buf, fp))
    {
        mountedMedia.append(QString::fromUtf8(buf));
    }
    pclose(fp);

    listWidget->clear();
    QStringList mountedMediaList = mountedMedia.split("\n", QString::SkipEmptyParts);
    for (int i = 0; i < mountedMediaList.size(); i++)
    {
        QStringList thisMediaData = mountedMediaList.at(i).split(" ", QString::SkipEmptyParts);
        new QListWidgetItem(thisMediaData.at(5), listWidget);
    }

    listWidget->setCurrentRow(0);
}

bool DialogImpl::eventFilter(QObject *ob, QEvent *e)
{
    if (e->type() == QEvent::KeyRelease && ((QKeyEvent*)e)->key() == Qt::Key_A)
    {
        while (listWidget->count())
        {
            UmountMedia(listWidget->itemAt(0, 0));
        }
    }

    return QDialog::eventFilter(ob, e);
}

void DialogImpl::UmountMedia(QListWidgetItem *item)
{
    FILE *fp;
    char buf[127];
    QString umountCommandOutput;
    QString umountCommand = QString("sudo pumount ") + item->text() + QString(" 2>&1");
    fp = popen(umountCommand.toLocal8Bit(), "r");
    while (fgets(buf, sizeof buf, fp))
    {
        umountCommandOutput.append(QString::fromUtf8(buf));
    }
    pclose(fp);

    if (umountCommandOutput != "")
    {
        if (umountCommandOutput.contains("device is busy"))
        {
            QString lsofOutput;
            QString lsofCommand = QString("sudo lsof ") + item->text();
            fp = popen(lsofCommand.toLocal8Bit(), "r");
            while (fgets(buf, sizeof buf, fp))
            {
                lsofOutput.append(QString::fromUtf8(buf));
            }
            pclose(fp);
            QString lsofHtmlOutput = lsofOutput;
            lsofHtmlOutput.replace("\n", "<br />");

            QMessageBox msgBox;
            msgBox.setText(tr("Device is busy"));
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setInformativeText(tr("Should I kill them? <pre style='font-family: monotype;'>%0</pre>").arg(lsofHtmlOutput));
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);
            if (msgBox.exec() == QMessageBox::Yes)
            {
                QStringList processList = lsofOutput.split("\n", QString::SkipEmptyParts);
                for (int i = 1 /* not zero (table header) */; i < processList.size(); i++)
                {
                    QStringList thisProcess = processList.at(i).split(" ", QString::SkipEmptyParts);
                    QString killCommand = QString("sudo kill -9 ") + thisProcess.at(1);
                    system(killCommand.toLocal8Bit());
                }

                UmountMedia(item); // Try again
            }
        } else {
            QMessageBox msgBox;
            msgBox.setText(tr("Unknown pumount error"));
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setInformativeText(umountCommandOutput);
            msgBox.exec();
        }
    } else {
        char buf[127];
        QString rmdirCommand = QString("sudo rmdir ") + item->text() + QString(" 2>&1");
        fp = popen(rmdirCommand.toLocal8Bit(), "r");        
        while (fgets(buf, sizeof buf, fp))
        {
        }
        pclose(fp);
    }

    ReadMountedMedia();
}
//
