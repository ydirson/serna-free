# TMake settings for aspell as a Serna Editor 3rd component
#

CONFIG              += dynamic

PACKAGE_CFG_OPTIONS             *= --enable-shared --disable-dependency-tracking

unix:PACKAGE_CONFIGURE          = ./configure
unix:PACKAGE_MAKEFILE           = ../GNUmakefile
win32:PACKAGE_MAKEFILE          = ../NMakefile
PACKAGE_MAKEFILEOPT             = -f
unix:PACKAGE_MAKEOPTS           += -j2
debug:PACKAGE_MAKEOPTS          += debug=1

ARCHIVE                 = $$ARCHIVE_DIR/$$PACKAGE_DIR.tar.bz2

CLEANFILES      *= $(THIRD_DIR)/aspell/MANIFEST
