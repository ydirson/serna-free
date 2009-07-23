#${
    return unless Config("darwin");

    my $sdkroot = "/Developer/SDKs/MacOSX10.4u.sdk";
    my $arch_cflags = "-arch ppc -arch i386";
    $arch_cflags = (Config("ppc") ? "-arch ppc" : "-arch i386") unless Config("universal");

    my %opts = (
        cxx_add => "$arch_cflags -isysroot $sdkroot"
    );

    $opts{"ld_add"} = "$arch_cflags -Wl,-syslibroot,$sdkroot -Wl,-search_paths_first" if Config("dynamic");

    my @env;

    foreach (keys %opts) {
        push @env, uc($_).'="'.$opts{$_}.'"';
    }

    Project("PACKAGE_MAKEOPTS += ".join(' ', @env));
#$}
