#${
    IncludeTemplate("3rd/pkg-utils");
    if (Config("syspkg") || Config("syspkgonly")) {
        my $pkg = find_package_by_files('aspell.h', 'aspell');
        if ($pkg) {
            write_package(expand_path('$(THIRD_DIR)/lib/aspell.pkg'), $pkg);
            write_file(expand_path('$(THIRD_DIR)/aspell/MANIFEST'), '');
            Project("TMAKE_TEMPLATE=");
            return;
        }
        tmake_error("Can't find aspell package") if (Config("syspkgonly"));
    }
    my %pkg = (
        LIBS            => 'aspell',
        LFLAGS          => '-L$(THIRD_DIR)/lib',
        INCLUDES        => '$(THIRD_DIR)/aspell',
    );
    write_package(expand_path('$(THIRD_DIR)/lib/aspell.pkg'), \%pkg);
#$}
