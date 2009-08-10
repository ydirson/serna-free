#
# Distribution generation template for unices
#
VERSION_ID      = #$ Expand("VERSION_ID");
MFT_SECTIONS    = #$ Expand("MFT_SECTIONS");

tgz: $(srcdir)/make-tgz.sh all_forward tspack
#$ SetEnvVars(qw/srcdir top_srcdir top_builddir THIRD_DIR RELEASE PLUGIN_NAME PLUGIN_DLL MFT_SECTIONS VERSION VERSION_ID MFT_VARS_OPTS APPVER/);
	$<

pkg: tgz

#$ EnableOutput() if Config("darwin");
#$ DisableOutput() unless Project("RPM_SPEC");
#${
    my ($rpmspec, $cwd) = (Project("RPM_SPEC"), getcwd());
    if ($rpmspec) {
	$rpmspec =~ s-^.*[\\/]--;
	$text  = "RPM_SPEC         = $cwd/$rpmspec\n";
	$text .= "RPM_SUBMAKEFILE = $cwd/Makefile.rpmbuild";
        Project('CLEANFILES += $(RPM_SPEC) $(RPM_SUBMAKEFILE)');
    }
    unless (Project("RPM_VERSION")) {
        if (Project("APPVER")) {
            Project("RPM_VERSION = ".Project("APPVER"));
        }
        elsif (Project("VERSION")) {
            Project("RPM_VERSION = $$VER_MAJOR.$$VER_MINOR.$$VER_PATCH");
        }
        else {
            Project("RPM_VERSION = 1.0.0");
        }
    }
    Project('RPM_RELEASE = $(RELEASE)') if (Project("RELEASE") && !Project("RPM_RELEASE"));
    Project("RPM_MANIFEST = $cwd/MANIFEST");
#$}
SRC_MANIFEST = #$ Expand("MANIFEST");
RPM_SRCSPEC  = #$ Expand("RPM_SPEC");
RPM_TOPDIR   = $(dir $(top_builddir))rpm

RPM_VERSION       = #$ Expand("RPM_VERSION");
RPM_RELEASE       = #$ Expand("RPM_RELEASE");
RPM_PREP_MAKEFILE = #$ Expand("RPM_PREP_MAKEFILE");
RPM_MANIFEST      = #$ Expand("RPM_MANIFEST");
RPM_POSTBUILD     = #$ Expand("RPM_POSTBUILD");
RPM_BINLIST       = rpm.binaries

MFT_TO_RPM_MFILE  = $(top_srcdir)/tools/manifest/manifest2all.py
MFT_TO_RPM_VARS   = \
                    -v version=$(VERSION_ID) \
                    $(MFT_VARS_OPTS)

$(RPM_SUBMAKEFILE): force # $(SRC_MANIFEST) $(MFT_TO_RPM_MFILE)
#$ SetEnvVars(qw/top_builddir top_srcdir THIRD_DIR/);
	$(PYTHON) $(MFT_TO_RPM_MFILE) -t rpm -s $(MFT_SECTIONS) \
                  $(MFT_TO_RPM_VARS) $(SRC_MANIFEST) $@

$(RPM_BINLIST): $(RPM_SUBMAKEFILE)
	$(PYTHON) $(top_srcdir)/tools/buildsys/dist/make-binlist.py MANIFEST.sources $@

$(RPM_SPEC): force $(RPM_SRCSPEC) $(RPM_BINLIST)
#$ SetEnvVars(qw/RPM_MANIFEST VERSION_ID RPM_VERSION RPM_RELEASE/);
        $(PYTHON) $(top_srcdir)/tools/buildsys/dist/make-rpmspec.py $(RPM_SRCSPEC) $@ \
            $(RPM_BINLIST)
	
rpm: $(top_srcdir)/tools/buildsys/dist/make-rpm.sh all_forward $(RPM_SUBMAKEFILE) $(RPM_SPEC) dist_forward
#$ SetEnvVars(qw/srcdir top_srcdir top_builddir THIRD_DIR RELEASE RPM_TOPDIR RPM_SPEC/);
#$ SetEnvVars(qw/RPM_PREP_MAKEFILE RPM_SUBMAKEFILE RPM_VERSION VERSION_ID PLUGIN_NAME PLUGIN_DLL/);
	$< #$ $text .= Project("RPM_POSTBUILD") ? '&& $(RPM_POSTBUILD)' : "";
        
pkg: rpm
#$ EnableOutput() unless Project("RPM_SPEC");
