TEMPLATE     = lib
VERSION      = 1.0.0
CONFIG      += debug_and_release link_pkgconfig

TARGET       = nautilus-svn
DESTDIR      = ../../bin

# Seems like SVN do not have PKGCONFIG yet.
INCLUDEPATH += /usr/include/subversion-1

SOURCES      = svn-module.c svn-extension.c svn-dir-status.c
HEADERS      =              svn-extension.h svn-dir-status.h

#QMAKE_CXXFLAGS += -fPIC

# Seems like SVN do not have PKGCONFIG yet.
LIBS        += -lsvn_auth_gnome_keyring-1 \
               -lsvn_auth_kwallet-1 \
               -lsvn_client-1 \
               -lsvn_delta-1 \
               -lsvn_diff-1 \
               -lsvn_fs-1 \
#               -lsvn_fs_base-1 \
#               -lsvn_fs_fs-1 \
#               -lsvn_fs_util-1 \
               -lsvn_ra-1 \
#               -lsvn_ra_dav-1 \
	       -lsvn_ra_local-1 \
               -lsvn_ra_neon-1 \
               -lsvn_ra_svn-1 \
               -lsvn_repos-1 \
               -lsvn_subr-1 \
               -lsvn_wc-1

PKGCONFIG   += glib-2.0 gtk+-3.0 libnautilus-extension apr-1
