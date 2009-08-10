#${
    IncludeTemplate("3rd/pkg-utils.t");
    my $third_dir = Project("THIRD_DIR");
    if (Config("syspkg") || Config("syspkgonly")) {
        my $pkg = find_package_by_config('xml2', 'xml2-config');
        if (!$pkg) {
            tmake_error("Can't find libxml2 package") if Config("syspkgonly");
        }
        else {
            write_package("$third_dir/lib/libxml2.pkg", $pkg);
            write_file("$third_dir/libxml/MANIFEST", '');
            Project("TMAKE_TEMPLATE=");
            return;
        }
    }

    return unless $is_unix;
    IncludeTemplate("3rd/findsubdir");

    my $libiconvdir = findsubdir("iconv", "$third_dir", "1.11");
#$}
