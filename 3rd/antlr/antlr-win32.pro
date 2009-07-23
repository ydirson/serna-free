#
# TMake settings for antlr as a Syntext 3rd component
#

MAKE_DIR            = $$PACKAGE_DIR

LIB_TARGETS         = $$LIB_PREFIX$$PACKAGE$$DEBUG_SUFFIX$$LIB_EXT

PACKAGE_LIB_DESTDIR = $$PACKAGE_DIR

PACKAGE_MAKE        = nmake
PACKAGE_MAKEFILE    = ..\NMakefile
PACKAGE_MAKEFILEOPT = -f
PACKAGE_CLEANOPTS   = clean
PACKAGE_MAKEOPTS    = 

PREPATCH    = attrib -r /s&&

debug:PACKAGE_TARGET    = debug=1
release:PACKAGE_TARGET  = 
