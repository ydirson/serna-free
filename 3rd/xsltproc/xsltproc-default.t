#${
    IncludeTemplate("3rd/pkg-utils.t");
    my $third_dir = Project("THIRD_DIR");
    if (Config("syspkg") || Config("syspkgonly")) {
        my $pkg = find_package_by_config('xslt', 'xslt-config');
        if (!$pkg) {
            tmake_error("Can't find xsltproc package") if Config("syspkgonly");
        }
        else {
            my @pathlist = split($is_unix ? ':' : ';', $ENV{'PATH'});
            my ($xsltproc) = find_file_in_path('xsltproc', @pathlist);
            tmake_error("Can't find xsltproc") if (! -f $xsltproc &&
                                                   Config("syspkgonly"));
            $pkg{'XSLTPROC'} = $xsltproc;
            write_package("$third_dir/lib/xsltproc.pkg", $pkg);
            Project("TMAKE_TEMPLATE=");
            return;
        }
    }

    return unless $is_unix;
    IncludeTemplate("3rd/findsubdir");

    my $third_dir = Project("THIRD_DIR");

    my $libxmldir = findsubdir("libxml", "$third_dir", "2.6.27");
    my $libiconvdir = findsubdir("iconv", "$third_dir", "1.11");
    Project("PACKAGE_CFG_OPTIONS += --with-libxml-prefix=$third_dir/libxml/install");
#$}
