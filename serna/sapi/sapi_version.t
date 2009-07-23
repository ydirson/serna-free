#${
    my ($maj, $min, $patch) = get_version(Project("srcdir")."/VERSION");
    my $target = Project("TARGET");
    Project("TARGET = $target$maj$min") if $is_unix;
    Project("VERSION = $maj.$min.$patch", 
            "VER_MAJOR = $maj", 
            "VER_MINOR = $min", 
            "VER_PATCH = $patch");
    write_file("build_version", "$maj.$min.$patch");
#$}
