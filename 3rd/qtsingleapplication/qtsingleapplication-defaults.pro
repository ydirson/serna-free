# TMake settings for qtsingleapplication as a Syntext 3rd component
#

PACKAGE_DIR             = qtsingleapplication-$$VERSION-opensource

ARCHIVE                 = $$ARCHIVE_DIR/$$PACKAGE_DIR.tar.gz

PACKAGE_MAKE        = $$MAKE
PACKAGE_MAKEOPTS    = QTDIR=$(QTDIR)
PACKAGE_MAKEFILE    = Makefile
PACKAGE_MAKEFILEOPT = -f
PACKAGE_MAKE_ENV    = MACOSX_DEPLOYMENT_TARGET=10.4

PACKAGE_CLEANOPTS       = clean
PACKAGE_TARGET          = first

PACKAGE_CONFIGURE       = ${QTDIR}/bin/qmake
win32:PACKAGE_CONFIGURE = %QTDIR%\bin\qmake

MAKE_DIR                =   $$PACKAGE_DIR
debug:PACKAGE_CFG_OPTIONS    *= "CONFIG+=debug"
release:PACKAGE_CFG_OPTIONS    *= "CONFIG+=release"
unix:PACKAGE_CFG_ENV         *= PATH=$$THIRD_DIR/bin:${PATH}

PACKAGE_CFG_OPTIONS          += qtsingleapplication.pro
unix:PACKAGE_CFG_OPTIONS     += DESTDIR=$$THIRD_DIR/lib
win32:PACKAGE_CFG_OPTIONS    += DESTDIR=$$THIRD_DIR\lib DEFINES+=QT_QTSINGLEAPPLICATION_EXPORT

linux:QTSPEC += -spec ${QTDIR}/mkspecs/linux-g++
win32:QTSPEC += -spec %QTDIR%\mkspecs\win32-msvc.net
darwin:QTSPEC += -spec ${QTDIR}/mkspecs/macx-g++
sunos:QTSPEC += -spec ${QTDIR}/mkspecs/solaris-g++

CLEANFILES      *= $(THIRD_DIR)/qtsingleapplication/MANIFEST
