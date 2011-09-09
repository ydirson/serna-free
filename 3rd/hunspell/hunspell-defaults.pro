# TMake settings for hunspell as a Serna Editor 3rd component

PACKAGE_DIR             = hunspell-bin-$$VERSION
MAKE_DIR                = $$PACKAGE_DIR

ARCHIVE                 = $$ARCHIVE_DIR/hunspell-bin-$${VERSION}.tgz

PACKAGE_MAKE               = echo
PACKAGE_MAKEFILE           = Success
PACKAGE_CLEANOPTS          = clean

TAR_OPTIONS             = -xf
