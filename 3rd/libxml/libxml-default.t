#${
    return unless $is_unix;
    IncludeTemplate("3rd/findsubdir");
    
    my $third_dir = Project("THIRD_DIR");
    
    my $libiconvdir = findsubdir("iconv", "$third_dir", "1.11");
#!    Project("PACKAGE_CFG_OPTIONS += --with-iconv=$libiconvdir");
#$}
