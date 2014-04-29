QT          += core gui
CONFIG      += c++11 debug_and_release link_pkgconfig

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET       = qsvn
DESTDIR      = ../../bin
TEMPLATE     = app

SOURCES     += main.cpp qsvncheckoutdialog.cpp qsvncommitdialog.cpp qsvncommithistorydialog.cpp qsvnupdatedialog.cpp qsvnrepobrowserdialog.cpp qsvnselectrevisiondialog.cpp qsvnupdatetorevisiondialog.cpp qsvnlogindialog.cpp qsvnmessagelogdialog.cpp qextcombobox.cpp qsvn.cpp qsvnthread.cpp qsvncommititemsmodel.cpp qsvnpool.cpp qclickablelabel.cpp qsvnhistory.cpp
HEADERS     +=          qsvncheckoutdialog.h   qsvncommitdialog.h   qsvncommithistorydialog.h   qsvnupdatedialog.h   qsvnrepobrowserdialog.h   qsvnselectrevisiondialog.h   qsvnupdatetorevisiondialog.h   qsvnlogindialog.h   qsvnmessagelogdialog.h   qextcombobox.h   qsvn.h   qsvnthread.h   qsvncommititemsmodel.h   qsvnpool.h   qclickablelabel.h   qsvnhistory.h
FORMS       +=          qsvncheckoutdialog.ui  qsvncommitdialog.ui  qsvncommithistorydialog.ui  qsvnupdatedialog.ui  qsvnrepobrowserdialog.ui  qsvnselectrevisiondialog.ui  qsvnupdatetorevisiondialog.ui  qsvnlogindialog.ui  qsvnmessagelogdialog.ui

SOURCES     += helpers.cpp
HEADERS     += helpers.h

RESOURCES   += qresource.qrc

# Seems like SVN do not have PKGCONFIG yet.
INCLUDEPATH += /usr/include/subversion-1
LIBS        += -lsvn_auth_gnome_keyring-1 \
	       -lsvn_auth_kwallet-1 \
	       -lsvn_client-1 \
	       -lsvn_delta-1 \
	       -lsvn_diff-1 \
	       -lsvn_fs-1 \
#	       -lsvn_fs_base-1 \
#	       -lsvn_fs_fs-1 \
	       -lsvn_fs_util-1 \
	       -lsvn_ra-1 \
#	       -lsvn_ra_dav-1 \
	       -lsvn_ra_local-1 \
#	       -lsvn_ra_neon-1 \
	       -lsvn_ra_svn-1 \
	       -lsvn_repos-1 \
	       -lsvn_subr-1 \
	       -lsvn_wc-1

PKGCONFIG   += apr-1
