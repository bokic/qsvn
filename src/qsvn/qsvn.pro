QT          += core gui
CONFIG      += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET       = qsvn
TEMPLATE     = app

SOURCES     += main.cpp qsvncommitdialog.cpp qsvnupdatedialog.cpp qsvnrepobrowserdialog.cpp qsvnselectrevisiondialog.cpp qextcombobox.cpp qsvn.cpp qsvnthread.cpp
HEADERS     +=          qsvncommitdialog.h   qsvnupdatedialog.h   qsvnrepobrowserdialog.h   qsvnselectrevisiondialog.h   qextcombobox.h   qsvn.h   qsvnthread.h
FORMS       +=          qsvncommitdialog.ui  qsvnupdatedialog.ui  qsvnrepobrowserdialog.ui  qsvnselectrevisiondialog.ui

RESOURCES   += qresource.qrc

INCLUDEPATH += /usr/include/subversion-1 /usr/include/apr-1.0

LIBS        += -lapr-1 -lsvn_subr-1 -lsvn_fs-1 -lsvn_client-1
