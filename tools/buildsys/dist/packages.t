#
# Package generation template
#
#${
    my $testdir = $is_unix ? 'test -d @DIR@ &&' : 'if exist @DIR@';
    Project('PKG_INSTALL_VARS = top_builddir top_srcdir THIRD_DIR');
    my $mft2mfile_vars = "-v serna= -v pymodext=py";
    my $makefile = "# Packages generation template\n";
    $makefile .= file_tools(qw/RM RMDIR PYTHON ZIP GZIP TAR/);
    $makefile .= expand_multi(qw(top_srcdir top_builddir srcdir THIRD_DIR
                                 MFT_SECTIONS APPVER RELEASE));
    $makefile .= "MFT_VARS_OPTS = ".ExpandOpts("MFT_VARS_OPTS");
    $makefile .= <<'EOF';

MAKEFILE_INSTALL_VARS   = top_builddir top_srcdir THIRD_DIR inst_prefix
MFT_TO_MFILE            = $(top_srcdir)/tools/manifest/manifest2all.py
CHECK_MFT               = $(top_srcdir)/tools/manifest/check_manifest.py

all: all-default
EOF
    my $tpl = <<'EOF';

PKG_MAKEFILE_@PKG@ = Makefile.pkg.@PKG@
PKG_DIR_@PKG@      = @PKG_DIR_@PKG@@
PKG_@PKG@          = @PKG_@PKG@@
PKG_MANIFEST_@PKG@ = $(srcdir)/@PKG_MANIFEST_@PKG@@

PKG_DST_MANIFEST_@PKG@  = MANIFEST.pkg.$(PKG_DIR_@PKG@)
MFT_TO_MFILE_VARS_@PKG@ = @MFT_TO_MFILE_VARS_@PKG@@
PKG_INST_PREFIX_@PKG@   = @PKG_INST_PREFIX_@PKG@@

$(PKG_MAKEFILE_@PKG@): force
	@PKG_ENV_VARS_@PKG@@
	$(PYTHON) $(MFT_TO_MFILE) -t make -s $(MFT_SECTIONS) \
            -m $(PKG_DST_MANIFEST_@PKG@) $(MFT_VARS_OPTS) \
            $(MFT_TO_MFILE_VARS_@PKG@) \
            -e "$(MAKEFILE_INSTALL_VARS)" \
            $(PKG_MANIFEST_@PKG@) $@

$(PKG_@PKG@): $(PKG_MANIFEST_@PKG@) $(PKG_MAKEFILE_@PKG@)
	$(MAKE) -f $(PKG_MAKEFILE_@PKG@) inst_prefix=$(PKG_DIR_@PKG@)
	@PKG_PACK_@PKG@@
#!	-@PKG_TESTDIR_@PKG@@ \
#!            $(PYTHON) $(CHECK_MFT) $(PKG_DIR_@PKG@) $(PKG_DST_MANIFEST_@PKG@)

@pkg@: $(PKG_@PKG@)

clean_@pkg@:
	$(RM) $(PKG_@PKG@)
	$(RMDIR) $(PKG_DIR_@PKG@)

EOF
    my @packages = split /\s+/, Project("PACKAGES");
    foreach $pkglabel (@packages) {
        my $pkg = uc($pkglabel);
        my ($name, $mft, $destdir) = map {
            Project($_)
        } (map { "${pkg}_$_" } qw/PKGNAME MANIFEST DESTDIR/);

        Project("PKG_DIR_$pkg = $name");
        my $pkg_prefix = getcwd()."$dir_sep".Project("PKG_DIR_$pkg");
        Project("PKG_INST_PREFIX_$pkg = $pkg_prefix");
        Project("PKG_INSTALL_VARS += inst_prefix=\$(PKG_INST_PREFIX_$pkg)");
        my $env_vars = set_env_vars(split(/\s+/, Project('PKG_INSTALL_VARS')));
        Project("PKG_ENV_VARS_$pkg = $env_vars");
        Project("PKG_INSTALL_VARS -= inst_prefix");

        my $td = $testdir;
        $td =~ s/\@DIR\@/\$(PKG_DIR_$pkg)/;
        Project("PKG_TESTDIR_$pkg = $td");
        Project("PKG_DESTDIR_$pkg = $destdir");

        my $pkgfp = "$destdir$dir_sep$name".($is_unix ? ".tar.gz" : ".zip");
        $pkgfp =~ s-[\\/]+-$dir_sep-g;
        Project("PKG_$pkg = $pkgfp");
        Project("PKG_MANIFEST_$pkg = $mft");
        if ($is_unix) {
            my $pack = "bash -c 'set -o pipefail; ";
            $pack .= "\$(TAR) -cf - $name | \$(GZIP) -9c > \$\@'";
            Project("PKG_PACK_$pkg = $pack");
        }
        else {
            Project("PKG_PACK_$pkg = \$(ZIP) -9r \$@ $name");
        }
        Project("MFT_TO_MFILE_VARS_$pkg = $mft2mfile_vars");

        my $mk = $tpl;
        $mk =~ s/\@PKG\@/$pkg/gm;
        $mk =~ s/\@pkg\@/$pkglabel/gm;
        foreach (qw/MAKEFILE DIR PKG MANIFEST PACK DESTDIR
                    ENV_VARS TESTDIR INST_PREFIX/) {
            $mk =~ s/\@(\w+)\@/$project{$1}/gm;
        }
        $makefile .= $mk;
        my $pkgstr = join(" ", @packages);
        $makefile .= "all-default: $pkgstr\n\nforce:\n\n";
        my $clean = "clean_".join("clean_", @packages);
        $makefile .= "clean: clean_packages\n\nclean_packages: $clean";
        $makefile .= "\n\n.PHONY: $clean $pkgstr" if $is_unix;
        write_file("Makefile.packages", $makefile);
    }
#$}
PACKAGES = #$ Expand("PACKAGES");

$(PACKAGES):
	@$(MAKE) -f Makefile.packages $@ >$@.log 2>&1 || \
            echo Error building package $@, see $@.log for details

packages:
	@$(MAKE) -f Makefile.packages $(PACKAGES) >$@.log 2>&1 || \
            echo Error building packages '$(PACKAGES)', see $@.log for details

clean_packages:
	$(MAKE) -f Makefile.packages clean

pkg: packages

clean: clean_packages

