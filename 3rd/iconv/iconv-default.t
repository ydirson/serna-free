#${
    IncludeTemplate("3rd/pkg-utils");
    my %pkg = (
        LIBS        => '',
        INCLUDES    => '',
        CFLAGS      => '',
        LFLAGS      => ''
    );
    my $third_dir = expand_path(Project('THIRD_DIR'));
    if (Config("syspkg") || Config("syspkgonly")) {
        write_file("$third_dir/iconv/MANIFEST", '');
        Project("TMAKE_TEMPLATE=");
    }
    else {
        $pkg{'INCLUDES'} = "$third_dir/iconv";
        if ($is_unix) {
            $pkg{'LIBS'}   = 'iconv';
            $pkg{'LFLAGS'} = "-L$third_dir";
        }
        else {
            $pkg{'LIBS'}   = "$third_dir\\iconv.lib";
        }
    }
    write_package("$third_dir/lib/iconv.pkg", \%pkg);
#$}
