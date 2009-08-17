MAKEFILE_INSTALL    = Makefile.install
linux:MFT_SECTIONS  = linux
sunos:MFT_SECTIONS  = sunos
darwin:MFT_SECTIONS = darwin
win32:MFT_SECTIONS  = win32
vc2003:MFT_SECTIONS  *= win32.vc2003

MFT_VARS            = bindir=bin

darwin:MFT_VARS     = serna=Contents bindir=MacOS
linux:MFT_VARS      += serna=serna-$$VERSION_ID arch=i386
sunos:MFT_VARS      += serna=serna-$$VERSION_ID arch=$$CPU
MFT_VARS            += \
                       pymodext=py \
                       THIRD_DIR=$(THIRD_DIR) \
                       PY_BUILDDIR=$$PY_BUILDDIR \
                       PY_SITE_PACKAGES=$$PY_SITE_PACKAGES \
                       PY_VERSFX=$(PY_VERSFX) \
                       apps_serna=serna

win32:MFT_VARS          += serna_name=Serna%20Free
win32:MFT_VARS_OPTS     += \
                            -r VersionId=version_id \
                            -r AppVersion=app_ver \
                            -r LicenseFile=lic_file \
                            -r Serna=serna_name

PY_BINMODDIR            = PCBuild
vc2003:PY_BINMODDIR     = PC\VS7.1

win32:MFT_VARS          += PY_BINMODDIR=$$PY_BINMODDIR

RPM_PREFIX              = /opt
linux:RPM_SPEC          = $(srcdir)/rpm/serna.spec
linux:RPM_PREP_MAKEFILE = $(srcdir)/rpm/Makefile

sunos:MAKEDIST          = $(srcdir)/make-sun.sh
darwin:MAKEDIST         = $(srcdir)/make-mac.sh

MANIFEST                = $(srcdir)/MANIFEST.all

win32:MAKEDIST          = $(srcdir)/makedist.cmd
win32:ISS_TEMPLATE      = $(srcdir)/serna.iss
win32:LICENSE           = $(srcdir)/EULA.txt
win32:ISS_INFOAFTER     = $(srcdir)/Readme.txt
win32:PRODUCT_NAME      = Serna XML editor

linux:RPM_POSTBUILD     = $(srcdir)/serna_postrpmbuild.sh

TS_DIR                  = $(top_builddir)/serna/i18n/ts
TS_PACK                 = $(top_builddir)/serna/serna-ts-$(APPVER)-$(RELEASE).zip
TS_TARGETS_LIST         = $(top_builddir)/serna/i18n/targets.pro
