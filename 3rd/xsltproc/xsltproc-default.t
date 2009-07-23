#${
    return unless $is_unix;
    IncludeTemplate("3rd/findsubdir");
    
    my $third_dir = Project("THIRD_DIR");
    
    my $libxmldir = findsubdir("libxml", "$third_dir", "2.6.27");
    my $libiconvdir = findsubdir("iconv", "$third_dir", "1.11");
    Project("PACKAGE_CFG_OPTIONS += --with-libxml-prefix=$third_dir/libxml/install");
#!    Project("PACKAGE_CFG_OPTIONS += --with-iconv=$libiconvdir");
#$}
