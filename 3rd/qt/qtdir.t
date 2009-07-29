#${
    my $third_dir = Project("THIRD_DIR");

    my $tbh = "$third_dir/qt";
    my $version = get_version("$tbh/TMakefile.pro");
    tmake_error("Can't determine Qt version") unless $version;

    my $qtdir = "$tbh/$version/qt-all-opensource-src-$version";
    $qtdir =~ s^[/\\]^$dir_sep^g;
    Project("QT_BUILDDIR=$qtdir");
#$}
