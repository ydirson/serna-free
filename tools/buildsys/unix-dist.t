#
# Distribution generation template for unices
# 
#
DISTVERSION  = #$ $text = (Project("APPVER") ? Expand("APPVER") : '$(VER_MAJOR).$(VER_MINOR)');
MFT_SECTIONS = #$ Expand("MFT_SECTIONS");

mac: $(srcdir)/make-mac.sh all_forward
	$< $(srcdir) $(DISTVERSION) $(RELEASE) $(top_srcdir) $(top_builddir) $(THIRD_DIR) $(MFT_SECTIONS)

tgz: $(srcdir)/make-tgz.sh all_forward
	$< $(srcdir) $(DISTVERSION) $(RELEASE) $(top_srcdir) $(top_builddir) $(THIRD_DIR) $(MFT_SECTIONS)

#$ DisableOutput() unless Project("RPMSPEC");
#${
    my $rpmspec = Project("RPMSPEC");
    if ($rpmspec) {
	$rpmspec =~ s-^.*[\\/]--;
	$text  = "RPMSPEC         = ".getcwd()."/$rpmspec\n";
	$text .= "RPM_SUBMAKEFILE = ".getcwd()."/Makefile.rpmbuild";
        Project('CLEANFILES += $(RPMSPEC) $(RPM_SUBMAKEFILE)');
    }
#$}
MANIFEST     = #$ Expand("MANIFEST");
RPMSRCSPEC   = #$ Expand("RPMSPEC");
RPMTOPDIR    = $(dir $(top_builddir))rpm

RPMVERSION        = #$ $text = Project("APPVER");
RPM_PREP_MAKEFILE = #$ Expand("RPM_PREP_MAKEFILE");
ARCH              = #$ Expand("ARCH");
MFT_SERNA_NAME    = #$ Expand("MFT_SERNA_NAME");
BINDIR            = #$ Expand("BINDIR");

MFT_TO_RPM_MFILE  = $(top_srcdir)/tools/manifest/manifest2all.py
MFT_TO_RPM_VARS   = -v serna=$(MFT_SERNA_NAME) -v arch=$(ARCH) -v bindir=$(BINDIR)

$(RPMSPEC): force
	@echo Version: $(RPMVERSION) > $@
	@echo Release: $(RELEASE) >> $@
	@cat $(RPMSRCSPEC) >> $@
	
$(RPM_SUBMAKEFILE): $(MANIFEST) $(MFT_TO_RPM_MFILE)
	$(PYTHON) $(MFT_TO_RPM_MFILE) -t rpm -s $(MFT_SECTIONS) \
                  $(MFT_TO_RPM_VARS) $(MANIFEST) $@
	
rpm: $(srcdir)/make-rpm.sh all_forward $(RPMSPEC) $(RPM_SUBMAKEFILE)
	$< $(srcdir) $(RPMTOPDIR) $(RPM_PREP_MAKEFILE) $(RPM_SUBMAKEFILE) \
        $(RPMSPEC) $(top_srcdir) $(top_builddir) $(THIRD_DIR) $(RELEASE)
#$ EnableOutput() unless Project("RPMSPEC");
