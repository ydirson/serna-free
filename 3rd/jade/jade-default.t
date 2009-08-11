#${
    IncludeTemplate("3rd/pkg-utils.t");
    my %pkg = ( 
	NAME 		=> 'sp',
	INCLUDES	=> '$(THIRD_DIR)/jade'
    );
    if ($is_unix) {
	$pkg{'LIBS'} 	 = 'sp';
	$pkg{'LFLAGS'}   = '-L$(THIRD_DIR)/lib';
    }
    else {
	$pkg{'LIBS'} 	 = "\$(THIRD_DIR)\\lib\\sp.lib";
    }
    my $third_dir = expand_path(Project("THIRD_DIR"));
    write_package("$third_dir/lib/sp.pkg", \%pkg);

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
