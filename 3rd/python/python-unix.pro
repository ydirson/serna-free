
PACKAGE_MAKE        = $$MAKE
PACKAGE_MAKEOPTS    = -j3
PACKAGE_MAKEFILE    = Makefile
PACKAGE_MAKEFILEOPT = -f
PACKAGE_CLEANOPTS	= clean
PACKAGE_CONFIGURE	= configure
PACKAGE_CFG_OPTIONS	= --with-threads --enable-shared --prefix=$$THIRD_DIR/python/install
darwin:PACKAGE_CFG_OPTIONS *= --with-suffix=no --enable-universalsdk=/Developer/SDKs/MacOSX10.4u.sdk
