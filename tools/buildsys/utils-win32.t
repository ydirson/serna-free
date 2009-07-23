#${

sub find_highest_lib_version {
    my ($dir,$name, $debug_sfx) = @_;
    my (@files, $f, $v, $highest, $hsfx);
    my @dbg_sfx = ($debug_sfx, 'd');
    $hsfx = $highest = "";
    @files = find_files($dir,"^${name}".'\d*(.*)\.lib');
    my $ndebug_sfx = ' ' x length($debug_sfx);
    my $hver = "";
    foreach $f (@files) {
       if ($f =~ m/^.*${name}(\d*)(.*)\.lib/i) {
          my ($ver, $dsfx) = ($1, $2);
          next if !$1 && 0 == grep {/$dsfx/} (@dbg_sfx);
          $v = $1 . ($dsfx ? $dsfx : $ndebug_sfx);
          if ($highest eq "" || $v gt $highest) {
              $hsfx = $2;
              $highest = $v;
              $hver = $1;
          }
       }
    }
    return ($hver, $hsfx);
}

sub findlibrary {
    my ($libspec, $platform, $debug_sfx) = @_;
    my $lsfx = "";
    if($libspec =~ m-(.*[/\\])([^/\\]+)-) {
        my $lpath = fullpath(expand_path($1)."\\$platform") if(defined($1));
        my $lname = $2;
        $lname =~ s-\.lib$--;
        my ($lver, $lsfx) = find_highest_lib_version($lpath, $lname, $debug_sfx);
        if (Config("dynamic")) {
            if (!$lver && !$lsfx && ! -r $lpath.$dir_sep.$lname.".lib") {
                my $top_builddir = Project("top_builddir");
                my $full_libpath = $lpath;
                $top_builddir =~ s-[\\/]--g; $full_libpath =~ s-[\\/]--g;
                if($full_libpath =~ m-$top_builddir-) {
                    my @ver = split('\.', Project("VERSION"));
                    pop @ver if ($#ver > 1);
                    $lver = join('', @ver);
                }
            }
        }
        return (1, &normpath($lpath)."$dir_sep$lname$lver$lsfx.lib");
    }
    else {
        return (0, $libspec);
    }
}

sub process_libs {
    my ($libstr, $pf_libdir) = @_;
    my ($lpflags, $libs, $syslibs) = ([], [], []);
    my $debug_sfx = &Config("debug") ? '_d' : '';
    foreach (split(/\s/, $libstr)) {
        my ($ldir, $lname) = get_lib_info($_);
        if ($lname) {
            $lname = $ldir.$dir_sep.$lname if $ldir;
            if ($ldir) { push @$libs, $lname; }
            else { push @$syslibs, $lname; }
        }
        else {
            my ($is_syslib, $lib_fullpath) = findlibrary($_, $pf_libdir, $debug_sfx);
            if ($is_syslib) { push @$syslibs, $lib_fullpath; }
            else { push @$libs, $lib_fullpath; }
        }
    }
    return ([], $libs, $syslibs, [@$libs]);
}
#$}
