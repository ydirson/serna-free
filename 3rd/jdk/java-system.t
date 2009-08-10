#${
    DisableOutput();
    IncludeTemplate("java");
    IncludeTemplate("pkg-info");
    EnableOutput();
    if (!Project("JAVA") && Config("syspkgonly")) {
        tmake_error("Can't find system java");
    }
    my $script = get_package_info('java', 'JAVA');
    my $java = normpath(expand_path(Project("JAVA")));
    if ($is_unix) {
        write_script($script, "exec $java \"\$\@\"");
    }
    else {
        write_script($script, "\@$java \%*");
    }
    write_file(Project("THIRD_DIR")."/jdk/MANIFEST.jre", '');
    Project("TMAKE_TEMPLATE=");
#$}
