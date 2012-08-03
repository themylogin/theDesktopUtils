/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created: Fri Aug 3 23:03:21 2012
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QListWidget *listWidget;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(220, 120);
        Dialog->setMinimumSize(QSize(220, 120));
        Dialog->setMaximumSize(QSize(220, 120));
        listWidget = new QListWidget(Dialog);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setGeometry(QRect(10, 10, 200, 100));
        listWidget->setMinimumSize(QSize(200, 100));
        listWidget->setMaximumSize(QSize(200, 100));

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "@__________________@", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
