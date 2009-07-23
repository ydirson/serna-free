# TMake settings for jade as a Serna Editor 3rd component
#

JADEDIR					= $$top_srcdir

PACKAGE_LIB_DESTDIR		= $$JADEDIR/lib
PACKAGE_BIN_DESTDIR		= $$JADEDIR/bin

INCLUDEPATH				+= ../sp ../generic
CONFIG			        *= mt

FILETAGS				+= JADEDIR LIB_DESTDIR BIN_DESTDIR INSTALL_VARS MSGGEN
VERSION					= 1.3.3
unix:MSGGEN				= $$JADEDIR/msggen.pl
win32:MSGGEN			= perl.exe $$JADEDIR/msggen.pl
DEFINES 				+= SP_NAMESPACE=Sp DSSSL_NAMESPACE=Dsssl GROVE_NAMESPACE=Grove

SRC_PROJECT             = TMakefile.pro

MAKE_DIR = $(srcdir)

debug:PACKAGE_MAKEOPTS  *= debug=1
static:PACKAGE_MAKEOPTS *= static=1

PACKAGE_CLEANOPTS = clean

PACKAGE_MAKEFILEOPT    = -f
PACKAGE_DIR            = .
MAKE_DIR               = libsp
win32:PACKAGE_MAKEFILE = Makefile
unix:PACKAGE_MAKEFILE  = GMakefile

darwin:CONFIG += universal
universal:PACKAGE_MAKEOPTS += universal=1
