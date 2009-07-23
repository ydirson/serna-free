
BASENAME    = libiconv-$$VERSION
PACKAGE_DIR = $$BASENAME

ARCHIVE_SVNPATH         = iconv/$$VERSION

#ARCHIVE_DIR             = $$THIRD_HOME/iconv/$$VERSION
ARCHIVE                 = $$ARCHIVE_DIR/$$PACKAGE_DIR.tar.gz

unix:PACKAGE_CFG_OPTIONS    = --disable-dependency-tracking --enable-relocatable --without-libintl

darwin:PACKAGE_CFG_ENV      = CFLAGS="-arch i386 -arch ppc -isysroot /Developer/SDKs/MacOSX10.4u.sdk" LDFLAGS="-arch i386 -arch ppc -Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk"

win32:PACKAGE_MAKEFILE      = Makefile.msvc
win32:PACKAGE_MAKEFILEOPT   = -f
