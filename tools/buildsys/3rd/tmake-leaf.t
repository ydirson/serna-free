#
#
#

PKG_MAKEFILE = #$ Expand( "PACKAGE_MAKEFILE");
#$	Project( "CONFIGURE_TARGETS *= \$(PKG_MAKEFILE)");

all: $(PKG_MAKEFILE)
	@echo Building in $(srcdir)
	@$(MAKE) -f $(PKG_MAKEFILE) $@ $(BUILD_LOG)

clean: installclean
	$(MAKE) -f $(PKG_MAKEFILE) $@

installclean:

