#${
    return if Project("NOPATCH");
    my $cwd = getcwd();
    my $pkg_dir = $cwd.$dir_sep.Project("PACKAGE_DIR");
    my $pf = Project( "PLATFORM");
    my $base_dir = relative($cwd, $pkg_dir) . $dir_sep;
#!    printf STDERR "cwd=%s, pdir=%s, pf=%s\n", $cwd, $pkg_dir, $pf;
    my @patchdirs = ("");
    $is_unix and push @patchdirs, "unix";
    push @patchdirs, $pf;
    grep { $_ = join($dir_sep, "_patches", $_) } (@patchdirs);
    my @patchfiles = ();
    my $patch_files = '';
    foreach $pd (@patchdirs) {
        my $pwc = join(' ', map { "$pd$dir_sep$_" } qw/patch-* *.patch/);
#!      print STDERR "pwc = $pwc\n";
        if ($is_unix) {
            $pwc =~ s-/+-/-g;
            $patch_files = `ls $pwc 2>/dev/null`;
        }
        else {
            $patch_files = `cmd /c dir /b $pwc 2>nul`;
            $patch_files = "$pd\\".join(" $pd\\", split(/\s+/, $patch_files)) if $patch_files;
            $patch_files =~ s-\\+-\\-g;
        }
        next unless $patch_files;
        push @patchfiles, split /\s+/, $patch_files;
    }
    $patch_files = join(' ', @patchfiles);
    chomp $patch_files;
    Project("PATCH_CMD = \@echo");
    if ($patch_files) {
#!  printf STDERR "Patching for $pf in %s\n", Project("PACKAGE_DIR");
        $patch_files =~ s-[\\\/]-$dir_sep-g;
        $patch_files =~ s-$dir_sep{2,}-$dir_sep-g;
        $patch_files =~ s/\s+/ /g;
        Project("PATCH_FILES = $patch_files");
    }
    Project("PACKAGE_DIR") && chdir $cwd;
    my $patch = Project("PATCH");
    ($pdir, $patch) = fnsplit($patch);
    if ($pdir) {
        $pdir = relative(normpath($pdir), $pkg_dir);
        $patch = $pdir.$dir_sep.$patch;
    }
    Project("PATCH = $patch");
    Project("PATCH_OPTS = -N -p1") unless Project("PATCH_OPTS");

    Project("FILETOOLS = RM"); IncludeTemplate("filetools");
#$}
#$ !Project("PATCH_FILES") && DisableOutput();
CAT            = #$ Expand("CAT");
PATCH          = #$ Expand("PATCH");
PATCH_OPTS     = #$ Expand("PATCH_OPTS");
PATCH_FILES    = #$ ExpandGlue("PATCH_FILES", "\t\\\n\t\t\t", " \\\n\t\t\t", "\n");
PACKAGE_DIR    = #$ Expand("PACKAGE_DIR");

PATCH_FLAG     = .patch_done
PREPATCH       = #$ $text = Project("PREPATCH"); $text =~ s/\$/\$\$/g;

$(PATCH_FLAG): $(UNPACK_FLAG) $(PATCH_FILES)
	$(CAT) $(PATCH_FILES) | (cd $(PACKAGE_DIR)$(CMD_SEP) $(PREPATCH) $(PATCH) $(PATCH_OPTS))
#$ DisableOutput() unless Project("POST_PATCH_CMD");
	#$ Expand("POST_PATCH_CMD");
#$ EnableOutput() unless Project("POST_PATCH_CMD");
#$ DisableOutput() unless $is_unix && Project("AUTOCONF_AFTER_PATCH");
	cd $(PACKAGE_DIR); autoconf
#$ EnableOutput() unless $is_unix && Project("AUTOCONF_AFTER_PATCH");
	$(TOUCH) $@

clean: patchclean

patchclean:
	-$(CAT) $(PATCH_FILES) | (cd $(PACKAGE_DIR)$(CMD_SEP) $(PATCH) -R $(PATCH_OPTS))
	$(RM) $(PATCH_FLAG)
#$ !Project("PATCH_FILES") && EnableOutput();
