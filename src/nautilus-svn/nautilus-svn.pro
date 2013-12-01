TEMPLATE = lib
VERSION = 1.0.0
CONFIG = debug_and_release

TARGET = libnautilus-svn

INCLUDEPATH += /usr/include/gtk-3.0 \
               /usr/include/glib-2.0 \
               /usr/lib/x86_64-linux-gnu/glib-2.0/include \
               /usr/include/pango-1.0 \
               /usr/include/cairo \
               /usr/include/gdk-pixbuf-2.0 \
               /usr/include/atk-1.0 \
               /usr/include/nautilus/libnautilus-extension \
               /usr/include/apr-1.0 \
               /usr/include/subversion-1

SOURCES = svn-module.c svn-extension.c
HEADERS = svn-extension.h

LIBS += -lgtk-3
# -o /usr/lib/nautilus/extensions-3.0/libnautilus-svn.so
