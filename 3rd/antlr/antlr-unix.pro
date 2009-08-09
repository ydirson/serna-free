# TMake settings for python as a Syntext 3rd component
#

# unix settings

MAKE_DIR                = $$PACKAGE_DIR/lib/cpp
PACKAGE_LIB_DESTDIR     = $$PACKAGE_DIR/lib/cpp/src/.libs

PACKAGE_MAKE            = $$MAKE
PACKAGE_MAKEFILE        = Makefile
PACKAGE_MAKEOPTS         = -j3
PACKAGE_MAKEFILEOPT     = -f
PACKAGE_CLEANOPTS       = clean
PACKAGE_CFG_ENV         += CXXFLAGS="-pipe -fPIC -g"  
release:PACKAGE_CFG_ENV += CXXFLAGS="-pipe -fPIC -O2" 
PACKAGE_CONFIGURE       = ./configure
PACKAGE_CFG_OPTIONS     += --prefix=`pwd`

LIB_TARGETS             = $$LIB_PREFIX$$PACKAGE$$LIB_EXT

darwin:CONFIG           *= universal
