#!
#! Template for python as a Syntext 3rd component
#!
#!
#!##############################################################################
#${
    IncludeTemplate("3rd/pkg-utils.t");

    my @ver = split('\.', Project("VERSION"));
    my $versfx = join('.', @ver[0], @ver[1]);
    
    my $third_dir = expand_path(Project("THIRD_DIR"));
    my $script = normpath("$third_dir/bin/python");
    my %package = (
        NAME    => 'python',
        PYTHON  => $script
    );

    if (Config("syspkg") || Config("syspkgonly")) {
        my $pkg = find_package_by_files("python/Python.h", "python$versfx");
        if (!$pkg) {
            tmake_error("Can't find python package") if Config("syspkgonly");
        }
        else {
            my @pathlist = split($is_unix ? ':' : ';', $ENV{'PATH'});
            my ($python) = find_file_in_path('python', @pathlist);
            write_script($script, "exec $python \"\$\@\"");
            grep { $package{$_} = $pkg->{$_} } (keys %{$pkg});
            write_file("$third_dir/python/MANIFEST", '');
            Project("TMAKE_TEMPLATE=");
            write_package("$third_dir/lib/python.pkg", \%package);                                                      
            return;                                                                                                     
        }
    }
    $package{'INCLUDES'} = normpath("$third_dir/python/include");
    my $pylib;
    if ($is_unix) {
        $package{'LFLAGS'} = '-L'.normpath("$third_dir/lib");
        $pylib = "python$versfx";
    }
    else {
        $versfx =~ s/\.//g;
        $pylib = normpath("$third_dir/lib/python$versfx.lib");
    }
    $package{'LIBS'} = $pylib;
    write_package("$third_dir/lib/python.pkg", \%package);
#$}
