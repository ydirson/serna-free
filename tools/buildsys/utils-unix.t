#${
sub process_libs {
    my ($libstr, $pf_libdir) = @_;
    my ($lpflags, $libs, $syslibs) = ([], [], []);
    my (%lpflags_hash, %deplibs_hash);
    my $darwin = ("darwin" eq Project("PLATFORM"));
    my @slibparts = split(/\s+/, $libstr);
    while (@slibparts) {
        $_ = shift(@slibparts);
        if (m/^--?.*$/) {
            $_ .=  ' '.shift(@slibparts) if ($darwin && "-framework" eq $_);
            push @$libs, $_;
        } elsif (m|(.*/)([^/]+)|) {
            my $libfullname = "";
            my ($libpath, $libname, $libbasename) = get_lib_info($_);
            if ($libpath and $libname and $libbasename) {
                $libfullname = expand_path(join($dir_sep, $libpath, $libname));
                $libname = $libbasename;
            }
            else {
                ($libpath, $libname) = ($1, $2);
                $libpath = fullpath($libpath) . $pf_libdir if $libpath;
                $libpath =~ s^[\\\/]+$^^; #! strip trailing (back)slashes
                if ($libpath) {
                    $libbasename = $libpath.$dir_sep."lib".$libname;
                    my $fullbase = expand_path($libbasename);
                    my $sfx = Config("darwin") ? ".dylib" : ".so";
                    if (Config("static")) {
                        $sfx = ".a" if (! -f $fullbase.$sfx);
                    }
                    else {
                        $sfx = ".a" if (-f $fullbase.".a" && ! -f $fullbase.$sfx);
                    }
                    $libfullname = $libbasename . $sfx;
                }
            }
            unless ($libfullname and $deplibs_hash{$libfullname}) {
                $deplibs_hash{$libfullname} = 1;
            }
            if ($libpath and !$lpflags_hash{$libpath}) {
                push @$lpflags, $libpath;
                $lpflags_hash{$libpath} = 1;
            }
            push @$libs, $libname;
        }
        else {
            push @$syslibs, $_;
        }
    }
    return ($lpflags, $libs, $syslibs, [keys %deplibs_hash]);
}
#$}