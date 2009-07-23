#
#  3rd party components installation template (win32 flavor)
#
#$ Project("INSTALL_ENV_VARS *= WIN32_VERSION");
WIN32_VERSION = #$ $text = Project("VERSION"); $text =~ s/\.//g;

install_only: force
#$	SetEnvVars(split(/\s+/, Project("INSTALL_ENV_VARS")));
	(@if exist $(srcdir)\install.bat \
	($(srcdir)\install.bat $(INST_SRCDIR) $(THIRD_DIR) $(INST_RELDIR) $(VERSION) $(CONFIG)) \
        else ($(top_srcdir)\install.bat $(INST_SRCDIR) $(THIRD_DIR) $(INST_RELDIR) $(VERSION) $(CONFIG))) >$(BUILD_LOG)

install_platform: all_forward install_only
