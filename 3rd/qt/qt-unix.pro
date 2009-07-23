#
#

PACKAGE_CONFIGURE   = ./configure
PACKAGE_MAKE        = gmake
PACKAGE_MAKEFILE    = Makefile
PACKAGE_MAKEFILEOPT = -f
PACKAGE_CLEANOPTS   = clean

linux:QMAKESPEC           = linux-g++
sunos:QMAKESPEC           = solaris-g++
darwin:QMAKESPEC          = macx-g++

PACKAGE_CFG_OPTIONS     += \
                           -platform ${QMAKESPEC}

QTPREFIX            = /usr/local/qt-$$VERSION
                           
BASENAME            = qt-x11-opensource-src-$$VERSION
ARCHIVE             = $$ARCHIVE_DIR/$$BASENAME.tar.gz
PACKAGE_DIR         = $$BASENAME
