#${
    return unless Config("darwin");
    my $sdkroot = "/Developer/SDKs/MacOSX10.4u.sdk";
    my $debug_cflags = Config("debug") ? "-O0 -g" : "-O2";
    my $arch_cflags = (Config("universal")) ? "-arch ppc -arch i386" : "";
    my $debug_lflags = Config("debug") ? "-g" : "";

    my %opts = (
        cflags => "$debug_cflags $arch_cflags -isysroot $sdkroot",
        cxxflags => "$debug_cflags $arch_cflags -isysroot $sdkroot",
        ldflags => "$debug_lflags $arch_cflags -Wl,-syslibroot,$sdkroot -Wl,-search_paths_first"
    );

    my @env;

    foreach (keys %opts) {
        push @env, uc($_).'="'.$opts{$_}.'"';
    }

    Project("PACKAGE_CFG_ENV += ".join(' ', @env));
    if (Config("ppc")) {
        Project("PACKAGE_CFG_OPTIONS += ppc-freebsd");
    }
    else {
        Project("PACKAGE_CFG_OPTIONS += i386-freebsd");
    }
#$}
