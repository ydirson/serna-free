#
# Distribution generation template for win32
# 
#
#${
    if (Project("VERSION_ID")) {
        Project("ISS_VER = \$(VERSION_ID)");
    }
    else {
        Project("ISS_VER = \$(VERSION)");
    }
#$}

#$ unless (Project("MANIFEST")) { DisableOutput(); } else { $text .= "\npkg:\n"; }

MANIFEST     = #$ ExpandPathName("MANIFEST");
MAKEDIST     = #$ ExpandPathName("MAKEDIST");
DISTDIR      = #$ ExpandPathName("DISTDIR");
ISS_SCRIPT   = $(MODULE_NAME).iss
ISS_TEMPLATE = #$ ExpandPathName("ISS_TEMPLATE");
LICENSE      = #$ ExpandPathName("LICENSE");
ISS_INFOAFTER= #$ ExpandPathName("ISS_INFOAFTER");
ISS_VER      = #$ Expand("ISS_VER");

MFT_SECTIONS = #$ Expand("MFT_SECTIONS");
MFT_TO_ISS   = $(top_srcdir)\tools\manifest\manifest2all.py

MFT_TO_ISS_VARS  = -v iss_template=$(ISS_TEMPLATE) \
                   -v top_builddir=$(top_builddir) \
                   -v top_srcdir=$(top_srcdir) -v THIRD_DIR=$(THIRD_DIR) \
                   -v lic_file=$(LICENSE) -v app_ver=$(APPVER) -v version=$(APPVER) \
                   -v version_id=$(VERSION_ID) -v serna= -v inst_prefix={app} \
                   $(MFT_VARS_OPTS)

$(ISS_SCRIPT): $(MANIFEST) $(ISS_TEMPLATE) $(LICENSE) $(ISS_INFOAFTER) $(MFT_TO_ISS)
	$(PYTHON) $(MFT_TO_ISS) -t iss -s $(MFT_SECTIONS) $(MFT_TO_ISS_VARS) $(MANIFEST) $@
        
dist_forward:

inno: $(ISS_SCRIPT) all_forward $(MAKEDIST) dist_forward
#$ DisableOutput() unless (Project("TARGET") =~ /.*\.exe$/);
	-@$(THIRD_DIR)\bin\StripReloc.exe $(TARGET)
#$ EnableOutput() unless (Project("TARGET") =~ /.*\.exe$/);
#$ SetEnvVars(qw/srcdir RELEASE PLUGIN_NAME DISTDIR ISS_VER APPVER VERSION ISS_SCRIPT/);
	@$(MAKEDIST)

clean-dist:
	-if exist $(ISS_SCRIPT) del $(ISS_SCRIPT)
	-if exist dist rd /s /q dist

pkg: clean-dist inno

distr: pkg
        
#$ unless (Project("MANIFEST")) { EnableOutput(); } else { Project('FILES_TO_CLEAN += $(ISS_SCRIPT)'); }
