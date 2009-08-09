#${
    IncludeTemplate("3rd/pkg-utils");

    my ($third_dir) = map { Project($_) } (qw/THIRD_DIR/);
    my %pkg = ( NAME => 'java' );
    if ($is_unix) {
        $pkg{'JAVA'} = normpath("$third_dir/bin/java");
    }
    else {
        $pkg{'JAVA'} = normpath("$third_dir\\bin\\java");
    }
    write_package("$third_dir/lib/java.pkg", \%pkg);

    if (Config("syspkg") || Config("syspkgonly")) {
        IncludeTemplate("java-system");
    }
    else {
        IncludeTemplate("java-included");
    }
#$}
