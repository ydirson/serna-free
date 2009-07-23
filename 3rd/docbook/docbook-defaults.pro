# TMake settings for dita-ot as a Serna Editor 3rd component

PACKAGE_DIR = docbook-xsl-$$VERSION
MAKE_DIR    = $$PACKAGE_DIR

ARCHIVE_SVNPATH         = docbook/$$VERSION
ARCHIVE                 = $$ARCHIVE_DIR/docbook-xsl-$${VERSION}.tar.bz2

PACKAGE_MAKE               = echo
PACKAGE_MAKEFILE           = Success
PACKAGE_CLEANOPTS          = clean
