#
#

TEMPLATE  = serna

LIBRARIES = hotkeys

CONFIG += moc qtexternal dll dynamic

win32:DEFINES -= SERNA_DLL SFWORKS_DLL

INCLUDEPATH = 	.; \
                $(srcdir)/../..;\
                $(CLIB_SRC); \
                $(THIRD_DIR)/qt/include/QtCore; \
                $(THIRD_DIR)/qt/include/QtGui;

darwin:TMAKE_LFLAGS *= -Wl,-undefined,dynamic_lookup
#darwin:FIX_INSTALL_NAMES *= libsapi21.2.dylib @executable_path/libsapi21.dylib
                
qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore \
                   $(THIRD_DIR)/lib/QtGui

HEADERS_PATTERN = \.h$ \
                  impl/.*\.h$ 

SOURCES_PATTERN = .*\.cxx$ 

UI_DIRS += $$srcdir

USE_SAPI         = 1
EXTRA_TEMPLATES += genlist ../plugins_common

DESTDIR = $$top_builddir/serna/lib/hotkeys
