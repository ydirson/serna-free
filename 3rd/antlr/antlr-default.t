#${
    IncludeTemplate("3rd/pkg-utils.t");
    IncludeTemplate("pkg-info");
    my $third_dir = expand_path(Project("THIRD_DIR"));
    my $java = get_package_info("java", "JAVA");
    my $antlr = "$java -classpath $third_dir/antlr antlr.Tool";
    my $antlr_script = "$third_dir/bin/antlr";
    $antlr_script .= '.bat' unless $is_unix;
    write_script($antlr_script, $antlr);

    my %pkg = ( NAME => 'antlr' );
    $pkg{'ANTLR'} = normpath($antlr_script);
    if ($is_unix) {
        $pkg{'INCLUDES'} = '$(THIRD_DIR)/antlr/include';
        $pkg{'LIBS'} 	 = 'antlr';
        $pkg{'LFLAGS'}   = '-L$(THIRD_DIR)/lib';
    }
    else {
        my $d = 'd' if Config("debug");
        $pkg{'INCLUDES'} = '$(THIRD_DIR)/antlr/include';
        $pkg{'LIBS'} 	 = "\$(THIRD_DIR)\\lib\\antlr$d.lib";
    }
    write_package("$third_dir/lib/antlr.pkg", \%pkg);

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
