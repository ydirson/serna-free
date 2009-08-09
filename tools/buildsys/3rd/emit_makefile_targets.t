#$ DisableOutput() if $is_unix;
all: all_prereq force
#$ SetEnvVars(split(/\s+/, Project("PACKAGE_MAKE_ENV")));
#$ AddEnvValue(split(/\s+/, Project("PACKAGE_MAKE_ENV_ADD")));
	@(set TMAKE_DUMP_MAKE_ENV >nul 2>&1 && set > env.txt) || echo.
	@cd $(MAKE_DIR)
	@echo Building in $(top_srcdir)
	@$(PACKAGE_MAKE) $(PACKAGE_MAKEFILEOPT) $(PACKAGE_MAKEFILE) $(PACKAGE_TARGET) $(PACKAGE_MAKEOPTS) $(BUILD_LOG)
	@cd $(PACKAGE_REL_DIR)

buildclean: installclean
	if exist $(MAKE_DIR) cd $(MAKE_DIR) && \
        @$(PACKAGE_MAKE) $(PACKAGE_MAKEFILEOPT) $(PACKAGE_MAKEFILE) $(PACKAGE_CLEANOPTS) && \
        @cd $(PACKAGE_REL_DIR)
#$ EnableOutput() if $is_unix;
#$ DisableOutput() unless $is_unix;
#$ DisableOutput() unless Config("darwin") and Project("MACOSX_DEPLOYMENT_TARGET");
MACOSX_DEPLOYMENT_TARGET = #$ Expand("MACOSX_DEPLOYMENT_TARGET");
export MACOSX_DEPLOYMENT_TARGET

#$ EnableOutput() unless Config("darwin") and Project("MACOSX_DEPLOYMENT_TARGET");
#$ DisableOutput() unless (Config("dynamic"));
BUILD_LIBDIR = #$ Expand("PACKAGE_LIB_DESTDIR");
#$ EnableOutput() unless (Config("dynamic"));
#${
    if ($is_unix && !Config("static")) {
        my $ldpathv = Config("darwin") ? "DYLD_LIBRARY_PATH" : "LD_LIBRARY_PATH";
        my $blibdir = Project("PACKAGE_LIB_DESTDIR") ? '$(BUILD_LIBDIR)' : "";
        my $lpcmd = $blibdir ? ("$ldpathv=$blibdir:" . '$$'."\\{$ldpathv\\}; export $ldpathv") : "true";
        Project("LIBPATH_CMD = $lpcmd");
    }
    else {
        Project("LIBPATH_CMD = true");
    }
#$}
LIBPATH_CMD = #$ Expand("LIBPATH_CMD");

all: all_prereq force
	@echo Building in $(top_srcdir)
	@[ x$${TMAKE_DUMP_MAKE_ENV} = x ] || env > env.txt
	@cd $(MAKE_DIR); $(LIBPATH_CMD) ; \
#$ SetEnvVars(split(/\s+/, Project("PACKAGE_MAKE_ENV")));
	$(PACKAGE_MAKE) $(PACKAGE_MAKEFILEOPT) $(PACKAGE_MAKEFILE) $(PACKAGE_TARGET) $(PACKAGE_MAKEOPTS) $(BUILD_LOG)

buildclean: installclean
	if [ -d $(MAKE_DIR) ]; then \
#$ SetEnvVars(split(/\s+/, Project("PACKAGE_MAKE_ENV")));
	  cd $(MAKE_DIR); $(PACKAGE_MAKE) $(PACKAGE_MAKEFILEOPT) $(PACKAGE_MAKEFILE) $(PACKAGE_TARGET) $(PACKAGE_MAKEOPTS) $(PACKAGE_CLEANOPTS) $(BUILD_LOG); \
	else \
	  true; \
	fi

clean: buildclean

.PHONY: all all_forward clean install install_only installclean buildclean gensrc all_prereq
#$ EnableOutput() unless $is_unix;

installclean:
