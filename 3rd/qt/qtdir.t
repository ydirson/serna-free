#${
    my ($third_dir, $pf) = map { Project($_) } qw/THIRD_DIR PLATFORM/;
    
    my $tbh = "$third_dir/qt";
    my $version = get_version("$tbh/TMakefile.pro");
    tmake_error("Can't determine Qt version") unless $version;
    
    my ($dtype, $qplat) = (Config("debug") ? "debug" : "release", "x11");

    if ("win32" eq $pf) {
        $qplat = "win";
    }
    elsif ("darwin" eq $pf) {
        $qplat = "mac";
    }

    my $qtdir = "$tbh/$version/qt-$qplat-opensource-src-$version";
    $qtdir =~ s^[/\\]^$dir_sep^g;
    Project("QT_BUILDDIR=$qtdir");
#$}
