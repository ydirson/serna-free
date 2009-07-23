#
#

PACKAGE_CONFIGURE   = configure
PACKAGE_MAKE        = nmake
PACKAGE_MAKEFILE    = Makefile
PACKAGE_MAKEFILEOPT = -f
PACKAGE_CLEANOPTS   = clean

QMAKESPEC           = win32-msvc2008
vc2003:QMAKESPEC    = win32-msvc2003

QTPREFIX            = c:\\qt-$$VERSION

PACKAGE_CFG_OPTIONS     *= \
                           -platform %QMAKESPEC%
                           
BASENAME            = qt-win-opensource-src-$$VERSION

ARCHIVE             = $$ARCHIVE_DIR/$$BASENAME.zip
PACKAGE_DIR         = $$BASENAME
