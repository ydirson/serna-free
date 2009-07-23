#${
    my ($cpu, $cwd) = (cpu(), getcwd());
    Project("PKG_ROOT = $cwd/pkg") unless Project("PKG_ROOT");
    my $third_dir = Project("THIRD_DIR");
    my $pyver = get_version("$third_dir/python/TMakefile.pro");
    
    Project("PY_BUILDDIR = python/$pyver/Python-$pyver");
    if ($is_unix) {
        Project('PY_SITE_PACKAGES = python/install/lib/python${PY_VERSFX}/site-packages');
    }
    else {
        Project("PY_SITE_PACKAGES = python/$pyver/Python-$pyver/Lib/site-packages");
    }

    if (Config("darwin")) {
        my $arch = ("i386" eq $cpu) ? "i386" : "ppc";
        Project("MFT_SECTIONS = darwin,darwin.$cpu");
        Project("MFT_VARS += arch=$arch");
        my $appver = Project("APPVER");
        $appver =~ s/\.[^\.]+$//;
        $appver =~ s/\./_/;
        Project("SERNA_APP_DIR = Serna$appver.app");
        Project('inst_prefix = $$PKG_ROOT/$$SERNA_APP_DIR') unless Project('inst_prefix');
    }
    else {
        Project("MFT_VARS += systemroot=$ENV{SYSTEMROOT}") unless ($is_unix);
        Project("RPM_PREFIX = /usr/local") unless Project("RPM_PREFIX");
        Project('inst_prefix = $$PKG_ROOT$$RPM_PREFIX') unless  Project('inst_prefix');
    }
#$}
