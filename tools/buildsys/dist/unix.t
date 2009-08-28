#
# Distribution generation template for unices
#
#${
    my %mkdists = ( RPM => '$(top_srcdir)/tools/buildsys/dist/make-rpm.sh' );
    foreach (qw/RPM MAC TGZ SUN/) {
        Project("MAKEDIST_$_ = ".(defined($mkdists{$_}) ? $mkdists{$_}
                                  : '$(srcdir)/make-'.lc($_).".sh"));
    }

    Project('PKG_ROOT = '.getcwd()."/pkg") unless Project('PKG_ROOT');
    Project('inst_prefix = $$PKG_ROOT') unless Project('inst_prefix');
    Project("PKG_MANIFEST = MANIFEST.package") unless Project("PKG_MANIFEST");
    Project('CLEANFILES *= $(PKG_MANIFEST) $(MAKEFILE_INSTALL) .pkg_dir');
    unless (Project('MAKEFILE_INSTALL_VARS')) {
        Project('MAKEFILE_INSTALL_VARS = top_builddir top_srcdir THIRD_DIR inst_prefix');
    }
#$}
VERSION_ID       = #$ Expand("VERSION_ID");
MFT_SECTIONS     = #$ Expand("MFT_SECTIONS");
MFT_TO_MFILE     = $(top_srcdir)/tools/manifest/manifest2all.py
MAKEFILE_INSTALL = #$ Expand("MAKEFILE_INSTALL");
SRC_MANIFEST     = #$ Expand("MANIFEST");
PKG_MANIFEST     = #$ Expand("PKG_MANIFEST");
PKG_ROOT         = #$ ExpandPathName("PKG_ROOT");
CHECK_MFT       := $(top_srcdir)/tools/manifest/check_manifest.py
inst_prefix     ?= #$ ExpandPathName("inst_prefix");
RPM_PREFIX      ?= #$ Expand("RPM_PREFIX");

MFT_TO_MFILE_VARS     = $(MFT_VARS_OPTS)
MAKEFILE_INSTALL_VARS = #$ Expand("MAKEFILE_INSTALL_VARS");

$(PKG_MANIFEST) $(MAKEFILE_INSTALL): $(SRC_MANIFEST) $(MFT_TO_MFILE)
#$ SetEnvVars(split(/\s+/, Project('MAKEFILE_INSTALL_VARS')));
	$(PYTHON) $(MFT_TO_MFILE) -t rpm -s $(MFT_SECTIONS) -m $(PKG_MANIFEST) \
                  $(MFT_TO_MFILE_VARS) $(mft_to_mfile_opts) -e "$(MAKEFILE_INSTALL_VARS)" \
                  $(SRC_MANIFEST) $@

PACKAGE_FILES := $(shell test -r MANIFEST.sources && cat MANIFEST.sources)

.pkg_dir: $(PKG_MANIFEST) $(MAKEFILE_INSTALL) $(PACKAGE_FILES)
	$(MAKE) -f $(MAKEFILE_INSTALL) rpm-install \
                top_builddir=$(top_builddir) \
                THIRD_DIR=$(THIRD_DIR) \
                inst_prefix=$(inst_prefix) \
                rpm_prefix=$(RPM_PREFIX)
	touch $@

#$ DisableOutput() unless Config("sunos");
MAKEDIST_SUN = #$ Expand("MAKEDIST_SUN");

solaris-pkg: $(MAKEDIST_SUN) all_forward tspack .pkg_dir
#$ SetEnvVars(qw/srcdir MAKEFILE_INSTALL top_srcdir top_builddir THIRD_DIR RELEASE inst_prefix/);
#$ SetEnvVars(qw/PLUGIN_NAME PLUGIN_DLL MFT_SECTIONS VERSION VERSION_ID MFT_VARS_OPTS APPVER/);
	$<

pkg: solaris-pkg
#$ EnableOutput() unless Config("sunos");
#$ IncludeTemplate("dist/darwin") if Config("darwin");
#$ DisableOutput() if Config("darwin");
MAKEDIST_TGZ      = #$ Expand("MAKEDIST_TGZ");

tgz: $(srcdir)/make-tgz.sh all_forward tspack .pkg_dir
#$ SetEnvVars(qw/srcdir MAKEFILE_INSTALL top_srcdir top_builddir THIRD_DIR RELEASE inst_prefix/);
#$ SetEnvVars(qw/PLUGIN_NAME PLUGIN_DLL MFT_SECTIONS VERSION VERSION_ID MFT_VARS_OPTS APPVER/);
	$<

pkg: tgz

#$ EnableOutput() if Config("darwin");
#$ DisableOutput() unless Project("RPM_SPEC");
#${
    my ($rpmspec, $cwd) = (Project("RPM_SPEC"), getcwd());
    if ($rpmspec) {
        $rpmspec =~ s-^.*[\\/]--;
        $text  = "RPM_SPEC         = $cwd/$rpmspec\n";
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
RPM_SRCSPEC  = #$ Expand("RPM_SPEC");
RPM_TOPDIR   = $(dir $(top_builddir))rpm

RPM_VERSION       = #$ Expand("RPM_VERSION");
RPM_RELEASE       = #$ Expand("RPM_RELEASE");
RPM_PREP_MAKEFILE = #$ Expand("RPM_PREP_MAKEFILE");
RPM_MANIFEST      = #$ Expand("RPM_MANIFEST");
RPM_POSTBUILD     = #$ Expand("RPM_POSTBUILD");
RPM_BINLIST       = rpm.binaries

MFT_TO_MFILE_VARS = \
                    $(MFT_VARS_OPTS)

$(RPM_BINLIST): $(MAKEFILE_INSTALL)
	$(PYTHON) $(top_srcdir)/tools/buildsys/dist/make-binlist.py MANIFEST.sources $@

$(RPM_SPEC): force $(RPM_SRCSPEC) $(RPM_BINLIST)
#$ SetEnvVars(qw/RPM_MANIFEST VERSION_ID RPM_VERSION RPM_RELEASE RPM_PREFIX top_srcdir/);
        $(PYTHON) $(top_srcdir)/tools/buildsys/dist/make-rpmspec.py $(RPM_SRCSPEC) $@ \
            $(RPM_BINLIST)

MAKEDIST_RPM      = #$ Expand("MAKEDIST_RPM");

rpm: $(MAKEDIST_RPM) all_forward .pkg_dir $(RPM_SPEC) tspack
#$ SetEnvVars(qw/srcdir top_srcdir top_builddir THIRD_DIR RELEASE RPM_TOPDIR/);
#$ SetEnvVars(qw/RPM_PREP_MAKEFILE RPM_SPEC RPM_PREFIX PKG_ROOT/);
#$ SetEnvVars(qw/MAKEFILE_INSTALL RPM_VERSION VERSION_ID PLUGIN_NAME PLUGIN_DLL/);
	$< #$ $text .= Project("RPM_POSTBUILD") ? '&& $(RPM_POSTBUILD)' : "";

pkg: rpm
#$ EnableOutput() unless Project("RPM_SPEC");
