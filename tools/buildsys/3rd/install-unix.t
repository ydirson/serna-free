#
#  3rd party components installation template (unix flavor)
#
#$ Project("INSTALL_ENV_VARS *= VERSION PLATFORM");
install_only:
#$ IncludeTemplate("install-unix-rules");

install_platform: all_forward
#$ IncludeTemplate("install-unix-rules");

.PHONY: install_only install_platform install
