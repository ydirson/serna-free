#
#

TEMPLATE  = serna

LIBRARIES = updateonsave
CONFIG += dll dynamic

win32:DEFINES -= SERNA_DLL SFWORKS_DLL

INCLUDEPATH = 	.; \
                $(srcdir)/../..;\
                $(CLIB_SRC);

darwin:LIBS += $(THIRD_DIR)/lib/$$QTLIB

SOURCES_PATTERN = .*\.cxx$ 

USE_SAPI         = 1
EXTRA_TEMPLATES += genlist ../plugins_common

DESTDIR = $$top_builddir/serna/lib/updateonsave
