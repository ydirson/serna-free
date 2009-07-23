#
# Distribution generation template for win32
# 
#
#${
    my $dv = join('_', (Project("VER_MAJOR"), Project("VER_MINOR")));
    Project("DISTVERSION = $dv");
#$}
zip: $(srcdir)\make-zip.bat all_forward
	$(srcdir)\make-zip.bat $(srcdir) #$ Expand("DISTVERSION"); Expand("ZIP"); Expand("RELEASE");

#$ DisableOutput() unless (Project("MANIFEST"));

MANIFEST     = #$ ExpandPathName("MANIFEST");
MAKEDIST     = #$ ExpandPathName("MAKEDIST");
DISTDIR      = #$ ExpandPathName("DISTDIR");
ISS_SCRIPT   = $(MODULE_NAME).iss
ISS_TEMPLATE = #$ ExpandPathName("ISS_TEMPLATE");
LICENSE      = #$ ExpandPathName("LICENSE");
ISS_INFOAFTER= #$ ExpandPathName("ISS_INFOAFTER");
ARCH         = #$ Expand("ARCH");
BINDIR       = #$ Expand("BINDIR");

MFT_SECTIONS = #$ Expand("MFT_SECTIONS");
MFT_TO_ISS   = $(top_srcdir)\tools\manifest\manifest2all.py

MFT_TO_ISS_VARS  = -v iss_template=$(ISS_TEMPLATE) \
                   -v top_builddir=$(top_builddir) \
                   -v top_srcdir=$(top_srcdir) -v THIRD_DIR=$(THIRD_DIR) \
                   -v lic_file=$(LICENSE) -v app_ver=$(APPVER) \
                   -v serna= -v inst_prefix={app} -v arch=$(ARCH) \
                   -v bindir=$(BINDIR)

$(ISS_SCRIPT): $(MANIFEST) $(ISS_TEMPLATE) $(LICENSE) $(ISS_INFOAFTER) $(MFT_TO_ISS)
	@$(PYTHON) $(MFT_TO_ISS) -t iss -s $(MFT_SECTIONS) $(MFT_TO_ISS_VARS) $(MANIFEST) $@

inno: $(ISS_SCRIPT) all_forward $(MAKEDIST)
	-@$(THIRD_DIR)\bin\StripReloc.exe $(TARGET)
	@$(MAKEDIST) $(ISS_SCRIPT) $(top_srcdir) $(top_builddir) 

clean-dist:
	-if exist app.iss rm app.iss
	-if exist dist rd /s /q dist

distr: clean-dist inno
	-$(MKDIR) $(DISTDIR)
	copy /b Output\setup.exe $(DISTDIR)\serna-$(APPVER)-$(RELEASE).exe

#$ unless (Project("MANIFEST")) { EnableOutput(); } else { Project('FILES_TO_CLEAN += $(ISS_SCRIPT)'); }
