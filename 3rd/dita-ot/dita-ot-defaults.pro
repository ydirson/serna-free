# TMake settings for dita-ot as a Serna Editor 3rd component

PACKAGE_DIR = DITA-OT$$VERSION
MAKE_DIR    = $$PACKAGE_DIR

ARCHIVE                 = $$ARCHIVE_DIR/DITA-OT$${VERSION}_fullpackage_bin.tar.gz

PACKAGE_MAKE               = echo
PACKAGE_MAKEFILE           = Success
PACKAGE_CLEANOPTS          = clean

TAR_OPTIONS             = -xf
unix:PREPATCH           = for f in `find . -type f -name "*.xsl"`; \
                          do tr -d '\r' < $f > $f.unix && mv $f.unix $f; done &&

INSTALL_ENV_VARS        = JAVA_HOME
