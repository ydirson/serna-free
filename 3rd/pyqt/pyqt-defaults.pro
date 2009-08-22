# TMake settings for pyqt as a Syntext 3rd component

PYQT_PLAT        = x11
#win32:PYQT_PLAT  = win
#darwin:PYQT_PLAT = mac

PACKAGE_DIR             = PyQt-$$PYQT_PLAT-gpl-$$VERSION
ARCHIVE                 = $$ARCHIVE_DIR/$$PACKAGE_DIR.tar.gz

PACKAGE_MAKE             = $$MAKE
unix:PACKAGE_MAKEOPTS    = -j2
PACKAGE_MAKEFILE         = Makefile
PACKAGE_MAKEFILEOPT      = -f
PACKAGE_CLEANOPTS        = clean

PACKAGE_CONFIGURE        = $$THIRD_DIR/bin/python configure.py
win32:PACKAGE_CONFIGURE  = $$THIRD_DIR\bin\python configure.py

PACKAGE_CFG_OPTIONS      =   -c -b $$THIRD_DIR/bin -w -j 32 --confirm-license
unix:PACKAGE_CFG_OPTIONS += \
                           -q $$THIRD_DIR/bin/qmake

PACKAGE_CFG_ENV           += CONFIG_3RD="$$CONFIG"
linux:PACKAGE_CFG_ENV     += QMAKESPEC=linux-g++
sunos:PACKAGE_CFG_ENV     += LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/lib

debug:PACKAGE_CFG_OPTIONS    *= -u

darwin:PACKAGE_MAKE_ENV    *= MACOSX_DEPLOYMENT_TARGET=10.4 DYLD_LIBRARY_PATH=$(THIRD_DIR)/lib
darwin:PACKAGE_CFG_OPTIONS +=   \
                                CFLAGS+="-I$$THIRD_DIR/iconv" \
                                CXXFLAGS+="-I$$THIRD_DIR/iconv" \
                                LFLAGS+="-L$$THIRD_DIR/lib"

FILETOOLS += PYTHON
EXTRA_TEMPLATES += filetools
