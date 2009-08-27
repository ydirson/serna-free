
PACKAGE_DIR = jdk-$$VERSION
MAKE_DIR    = $$PACKAGE_DIR/jre

linux:JAVA_PF = linux
win32:JAVA_PF = win32

ARCHIVE                 = $$ARCHIVE_DIR/jdk-$$JAVA_PF-$${VERSION}.tar.bz2

PACKAGE_MAKEFILE           = $(srcdir)/JdkMakefile
PACKAGE_CLEANOPTS          = clean
PACKAGE_MAKEFILEOPT        = -f

win32:PACKAGE_MAKEOPTS     *= INST_SRCDIR=%CD%
unix:PACKAGE_MAKEOPTS      *= INST_SRCDIR=`pwd`

JRE_LST  = $(top_srcdir)/jre.lst
MANIFEST = $(top_srcdir)/MANIFEST.jre
LST2MFT  = $(PYTHON) $(main_srcdir)/tools/manifest/lst2mft.py

JRE_EXCLUDES = $(srcdir)/jre.excludes.$$PLATFORM
unix:JRE_EXCLUDES = $(srcdir)/jre.excludes.unix

CLEANFILES += $(top_srcdir)/MANIFEST.jre
