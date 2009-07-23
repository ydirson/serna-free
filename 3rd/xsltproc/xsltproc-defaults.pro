
BASENAME    = libxslt-$$VERSION
PACKAGE_DIR = $$BASENAME

ARCHIVE                 = $$ARCHIVE_DIR/$$PACKAGE_DIR.tar.gz

WITH_PYTHON = $$THIRD_DIR/python/install
darwin:WITH_PYTHON = $$THIRD_DIR/python/install

unix:PACKAGE_CFG_OPTIONS    = \
                              --disable-dependency-tracking \
                              --enable-shared \
                              --without-crypto \
                              --with-python=$$WITH_PYTHON \
                              --disable-rpath \
                              --prefix=$$THIRD_DIR/xsltproc/usr
                              
darwin:PACKAGE_CFG_ENV      = \
                              CFLAGS="-arch i386 -arch ppc -isysroot /Developer/SDKs/MacOSX10.4u.sdk" \
                              LDFLAGS="-arch i386 -arch ppc -Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk" \
                              PYTHON="$$THIRD_DIR/python/install/bin/python"

unix:PACKAGE_CFG_ENV          += PYTHON="$$THIRD_DIR/python/install/bin/python"
