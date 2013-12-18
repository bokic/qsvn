QT          += core gui
CONFIG      += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET       = qsvn
TEMPLATE     = app

SOURCES     += main.cpp qsvncheckoutdialog.cpp qsvncommitdialog.cpp qsvnupdatedialog.cpp qsvnrepobrowserdialog.cpp qsvnselectrevisiondialog.cpp qsvnupdatetorevisiondialog.cpp qextcombobox.cpp qsvn.cpp qsvnthread.cpp
HEADERS     +=          qsvncheckoutdialog.h   qsvncommitdialog.h   qsvnupdatedialog.h   qsvnrepobrowserdialog.h   qsvnselectrevisiondialog.h   qsvnupdatetorevisiondialog.h   qextcombobox.h   qsvn.h   qsvnthread.h
FORMS       +=          qsvncheckoutdialog.ui  qsvncommitdialog.ui  qsvnupdatedialog.ui  qsvnrepobrowserdialog.ui  qsvnselectrevisiondialog.ui  qsvnupdatetorevisiondialog.ui

SOURCES     += helpers.cpp
HEADERS     += helpers.h

RESOURCES   += qresource.qrc

INCLUDEPATH += /usr/include/subversion-1 /usr/include/apr-1.0

LIBS        += -lapr-1 -lsvn_subr-1 -lsvn_fs-1 -lsvn_client-1
