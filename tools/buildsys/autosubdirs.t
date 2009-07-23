#!
#! Syntext Editor autoconfiguration template for superdirs
#! 
#${
    my $cwd = getcwd();
    my ($pf, $tmake, $tmakepath, $proj_file, $makefile, $config, $options, $defaults, $local_defaults) = map {
      Project($_)
    } ("PLATFORM", "TMAKE", "TMAKEPATH", "PROJECT_FILE", "MAKEFILE", "CONFIG", "options", "defaults", "local_defaults");

    my $tmake_args = "options+=\"$options\"";
    $tmake_args .= " defaults=$defaults local_defaults=$local_defaults";
    $tmake_args .= " $defaults $local_defaults THIRD_DIR=".Project(THIRD_DIR);
    $tmake_args .= " PROJECT_FILE=$proj_file MAKEFILE=$makefile";
    $tmake_args .= ' "TMAKE='.$tmake.'"';
    my $version_opt = "VERSION=" . Project("VERSION");
    my ($top_srcdir, $top_builddir, $main_srcdir) = map { Project($_) } qw(top_srcdir top_builddir main_srcdir);
    my $dirs_opt = "recursive_configure=1 top_srcdir=$top_srcdir main_srcdir=$main_srcdir top_builddir=$top_builddir";
    my (@leafdirs, @nodedirs, @subdirs) = ((), (), ());
    my $srcdir = Project("srcdir") . $dir_sep;
    my $excludedirs = Project("EXCLUDE_FROM_BUILD");
    $excludedirs =~ s-[\\/]-$dir_sep-g;
    my %dep_dict = ();
    foreach $dir (split(/[,\s]+/, Project("SUBDIRS"))) {
        next if ($dir eq "" or $dir =~ /^\s*:.*/);
        $dir =~ s-[\\/]-$dir_sep-g;
        my ($tdir, $deps) = split ':', $dir;
        if ($deps) {
            $deps =~ s/;/ /g;
            $dir = $tdir;
            $dep_dict{$dir} = $deps;
        }
        my $srcsubdir = $srcdir . $dir;
        if (-d $srcsubdir) {
            mkdirp $dir, 0755 if(! -d $dir);
            chdir $dir;
            $proj_file =~ m/([^\\\/]+$)/;
            $proj_file = $srcsubdir . $dir_sep . $1;
            $ENV{"TMAKEPATH"} = Project("TMAKEPATH");
            $ENV{"RECURSIVE_CONFIGURE"} = '1';
            my $command = "$tmake $dirs_opt srcdir=$srcsubdir";
            $command .= " PLATFORM=$pf $version_opt CONFIG=\"$config\" $tmake_args $proj_file -o $makefile";
            my $leaf = system($command);
            unless ($excludedirs =~ m/$dir/) {
                push @subdirs, $dir;
                if (100 == ($leaf >> 8)) {
                    push @leafdirs, $dir;
                    write_file(".make_options", Project("MAKE_PARALLEL")) if $is_unix;
                }
                else {
                    push @nodedirs, $dir;
                }
            }
            chdir $cwd;
        }
    }
    Project("LEAFSUBDIRS = " . join(' ', @leafdirs));
    Project("NODESUBDIRS = " . join(' ', @nodedirs));
    Project("SUBDIRS = " . join("\n", @subdirs));
    write_file(".subdirs", Project("SUBDIRS")) and Project("CLEANFILES += .subdirs");
    my @depdirs = keys %dep_dict;
    if (@depdirs) {
        Project("PARALLEL_SUBDIRS = 1");
        foreach $dir (@depdirs) {
            $text .= "\n$dir: $dep_dict{$dir}";
        }
    }
    IncludeTemplate("superdir");
#$}
