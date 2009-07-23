# TMake settings for python as a Syntext 3rd component
#

CONFIG					*= mt dynamic dll

PACKAGE_DIR             = $$PACKAGE-$$VERSION                 # relative to 3rd/python/$$VERSION

ARCHIVE                 = $$ARCHIVE_DIR/$$PACKAGE_DIR.tar.bz2
