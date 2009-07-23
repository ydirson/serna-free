#
# Distribution generation template for Mac OSX
#
CHECK_MFT       := $(top_srcdir)/tools/manifest/check_manifest.py

CWD             := $(shell pwd)
SERNA_APP_DIR   := #$ Expand("SERNA_APP_DIR");
SERNA_PKGROOT   := $(PKG_ROOT)/$(SERNA_APP_DIR)

$(PKG_MANIFEST): $(SRC_MANIFEST) $(MAKEFILE_INSTALL)
#$ SetEnvVars(split(/\s+/, Project('MAKEFILE_INSTALL_VARS')));
	gmake -r -f $(MAKEFILE_INSTALL) 
	python $(CHECK_MFT) $(SERNA_PKGROOT) $(PKG_MANIFEST) || rm -f $@

fixver := sed -e "s/@VERSION@/$(VERSION_ID)/g" -e "s/@YEAR@/`date +%Y`/g"

PLISTDIR    := $(srcdir)/mac
INSTRES_DIR := $(CWD)/Resources

SRC_PROPS   := $(shell find $(PLISTDIR)/Contents -name .svn -prune -o -type f -print)
PROPS       := $(subst $(PLISTDIR),$(SERNA_PKGROOT),$(SRC_PROPS))

SRC_INSTRES     := $(shell find $(PLISTDIR)/Resources -name .svn -prune -o -type f -print)
INSTRES         := $(subst $(PLISTDIR)/Resources,$(INSTRES_DIR),$(SRC_INSTRES)) $(INSTRES_DIR)/License.txt

SRC_PLISTS      := $(shell find $(PLISTDIR) -maxdepth 1 -name "*.plist")
PLISTS          := $(subst $(PLISTDIR),$(CWD),$(SRC_PLISTS))

$(INSTRES_DIR)/%: $(PLISTDIR)/Resources/%
	@[ -d $(@D) ] || mkdir -p $(@D)
	cat $< | $(fixver) > $@

$(INSTRES_DIR)/License.txt: $(srcdir)/EULA.txt
	@[ -d $(@D) ] || mkdir -p $(@D)
	cat $< | $(fixver) > $@

instres: $(INSTRES)

$(SERNA_PKGROOT)/Contents/%: $(PLISTDIR)/Contents/%
	@[ -d $(@D) ] || mkdir -p $(@D)
	cat $< | $(fixver) > $@

props: $(PROPS)

$(CWD)/%.plist: $(PLISTDIR)/%.plist
	cat $< | $(fixver) > $@

plists: $(PLISTS)

PM      := $(srcdir)/mac_package_maker.sh
PM_ARGS :=  \
            -f $(PKG_ROOT) \
            -r $(INSTRES_DIR) \
            -i Info.plist \
            -d Description.plist \
            -ds

PKGNAME         := serna-$(VERSION)-$(RELEASE)
IMAGEDIR        := $(CWD)/image
PKGDIR          := $(IMAGEDIR)/$(PKGNAME).pkg
BOM             := $(PKGDIR)/Contents/Archive.bom

prepkg:
#$ SetEnvVars(qw/srcdir top_srcdir top_builddir THIRD_DIR SERNA_PKGROOT/);
	$(srcdir)/mac_prepkg.sh

dist_forward:

pmfiles: $(PLISTS) $(PROPS) $(INSTRES) 

$(BOM): $(PKG_MANIFEST) pmfiles prepkg
	rm -fr $(IMAGEDIR) && mkdir -p $(IMAGEDIR)
	$(PM) -build -p $(PKGDIR) $(PM_ARGS)

pkgdir: $(BOM)

IMAGE   := ../${PKGNAME}.dmg

MAKE_IMAGE  = hdiutil create -ov -anyowners -srcfolder $(IMAGEDIR) -volname ${PKGNAME} $(IMAGE)

postpkg:
#$ SetEnvVars(qw/srcdir top_srcdir top_builddir THIRD_DIR SERNA_PKGROOT PKGDIR/);
	$(srcdir)/mac_postpkg.sh

$(IMAGE): $(BOM) postpkg
	$(MAKE_IMAGE)

image_only:
	$(MAKE_IMAGE)

pkg_only: $(IMAGE)

mac: all_forward dist_forward pkg_only
        
pkg: mac

CLEANFILES := $(IMAGE) $(PLISTS) $(INSTRES) $(PKG_MANIFEST)
CLEANDIRS  := $(IMAGEDIR) $(PKGROOT) $(INSTRES_DIR)

pkgclean:
	rm -fr $(CLEANFILES) $(CLEANDIRS)

.PHONY: pkg mac dist_forward prepkg pkg_only pkgdir pmfiles pkgclean props instres plists
