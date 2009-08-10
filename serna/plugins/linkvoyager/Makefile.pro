#
#

TEMPLATE  = serna

LIBRARIES = linkvoyager

CONFIG += dll dynamic

win32:DEFINES -= SERNA_DLL SFWORKS_DLL

INCLUDEPATH = 	.; \
                $(srcdir)/../..;\
                $(CLIB_SRC);
darwin:TMAKE_LFLAGS *= -Wl,-undefined,dynamic_lookup

USE = QtCore

SOURCES_PATTERN = .*\.cxx$ 

USE_SAPI         = 1
EXTRA_TEMPLATES += genlist ../plugins_common

DESTDIR = $$top_builddir/serna/lib/linkvoyager
