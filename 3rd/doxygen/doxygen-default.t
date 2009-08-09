#${
    IncludeTemplate("3rd/pkg-utils");
    my ($third_dir) = map { Project($_) } (qw/THIRD_DIR/);
    my $script = "$third_dir/bin/doxygen";
    my %pkg = (
        NAME    => 'doxygen',
        DOXYGEN => $script
    );
    write_package("$third_dir/lib/doxygen.pkg", \%pkg);
    my $doxygen;
    if (Config("syspkg") || Config("syspkgonly")) {
        my @pathlist = split($is_unix ? ':' : ';', $ENV{'PATH'});
        ($doxygen) = find_file_in_path('doxygen', @pathlist);
        if (!$doxygen) {
            tmake_error("Can't find doxygen tool") if Config("syspkgonly");
        }
        else {
            $pkg{'DOXYGEN'} = $doxygen;
            write_script($script, join($dir_sep, "exec $doxygen"));
            Project("TMAKE_TEMPLATE=");
        }
    }
#$}
