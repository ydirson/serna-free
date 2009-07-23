# TMake settings for sip as a Syntext 3rd component

PACKAGE_DIR             = $$PACKAGE-$$VERSION

ARCHIVE                 = $$ARCHIVE_DIR/$$PACKAGE_DIR.tar.bz2

PACKAGE_MAKE        = $$MAKE
PACKAGE_MAKEOPTS    = 
PACKAGE_MAKEFILE    = Makefile
PACKAGE_MAKEFILEOPT = -f
PACKAGE_CLEANOPTS	= clean

PACKAGE_CONFIGURE	= $$THIRD_DIR/bin/python configure.py
win32:PACKAGE_CONFIGURE	= $$THIRD_DIR\bin\python configure.py

PACKAGE_CFG_OPTIONS	= \
                            -b $$THIRD_DIR/bin \
                            -d $$THIRD_DIR/sip \
                            -e $$THIRD_DIR/sip \
                            -v $$THIRD_DIR/sip

debug:PACKAGE_CFG_OPTIONS    *= -u

SIP_PLAT = win32-msvc2008
vc2003:SIP_PLAT = win32-msvc.net
                            
win32:PACKAGE_CFG_OPTIONS  += -p $$SIP_PLAT
darwin:PACKAGE_CFG_OPTIONS += -n -p macx-g++
sunos:PACKAGE_CFG_OPTIONS  += -p solaris-g++
