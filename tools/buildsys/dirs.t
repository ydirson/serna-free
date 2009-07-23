#$ DisableOutput() if (Project("DIRS_T_INCLUDED"));
#!  
#!  This is internal template used to determine top_builddir and top_srcdir variables
#!  Sets corresponding project vars as well as full pathname of the configure script
#!
#${
    return if Project("DIRS_T_INCLUDED");

    sub find_dir {
        my ($cwd, $subdir) = @_;
        while ($cwd) {
            last if (-d "$cwd/$subdir");
            last unless $cwd =~ s@[\\/][^\\/]+$@@;
        }
        my $dir = "$cwd${dir_sep}$subdir";
        return (-d $dir) ? $dir : "";
    }
    
    Project("SUBDIRS /= s/,/ /g");

    my $cwd = getcwd();
    my ($top_builddir, $top_srcdir, $third_dir) =
        map { Project($_) } (top_builddir, top_srcdir, THIRD_DIR);
        
    $top_builddir ||= $cwd;
    
    if (!$third_dir || ! -d $third_dir) {
        $third_dir = find_dir($cwd, "3rd");
        tmake_error("Can't determine THIRD_DIR") unless (-d $third_dir);
    }

    if (!$top_srcdir) {
        $third_dir = find_dir($cwd, "src");
        tmake_error("Can't determine top_srcdir") unless (-d $top_srcdir);
    }
    grep { $_ = normpath(fullpath($_)) } ($top_builddir, $third_dir);
    Project("top_builddir = $top_builddir", 
            "top_srcdir = $top_srcdir",
            "THIRD_DIR = $third_dir");
    Project("srcdir = $top_srcdir") unless Project("srcdir");

    my ($srcdir, $tmpath) = map { Project($_) } (srcdir, TMAKEPATH);
    $tmpath =~ s-[\\/]$--; #! strip trailing path_sep
    $tmpath .= $path_sep . $srcdir unless $tmpath =~ m/[;:]\Q$srcdir\E/;
    $tmpath =~ ($is_unix ? s-;-:-g : s-/-\\-g);
    Project("TMAKEPATH = $tmpath");

    $ENV{"top_srcdir"} = $top_srcdir;
    $ENV{"srcdir"} = $srcdir;

#$}
#$ EnableOutput() if (Project("DIRS_T_INCLUDED")); Project("DIRS_T_INCLUDED = 1");
