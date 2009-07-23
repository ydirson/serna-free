#$	SetEnvVars(split(/\s+/, Project("INSTALL_ENV_VARS")));
	if [ -x $(srcdir)/install.sh ]; then \
          $(srcdir)/install.sh $(INST_SRCDIR) $(THIRD_DIR) $(INST_RELDIR) $(VERSION) $(CONFIG); \
        else \
	  [ -x $(top_srcdir)/install.sh ] && \
	  $(top_srcdir)/install.sh $(INST_SRCDIR) $(THIRD_DIR) $(INST_RELDIR) $(VERSION) $(CONFIG); \
        fi >$(BUILD_LOG)
