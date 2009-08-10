#$ DisableOutput();
#
# Syntext Editor global template
# 
#
#$ EnableOutput();
#${
    binmode STDOUT;
    IncludeTemplate("utils");

    my %def_compilers = (win32 => "msvc", 
                         linux => "g++", 
                         freebsd => "g++", 
                         sunos => "forte",
                         darwin => "g++");
    my $platform = Project("PLATFORM");
    $platform = $platform ? $platform : platform();
    Project("PLATFORM = $platform", "CONFIG *= $platform");
    Project("CONFIG *= ix") unless ($platform eq "win32");
    my $cpu = cpu();
    Project("CPU = $cpu") unless (Project("CPU"));
    Project("ARCH = $cpu") unless (Project("ARCH"));
    unless (Project("COMPILER")) {
        Project("COMPILER = " . $def_compilers{$platform});
    }

    IncludeTemplate("fix_init_config");
    IncludeTemplate("dirs");
    IncludeTemplate("options");

    IncludeTemplate("read_locals");

    IncludeTemplate("incpath");
    IncludeTemplate("config");
    IncludeTemplate("target");

    my $cwd = getcwd();
    print STDERR "Configuring in \"$cwd\" with CONFIG='", Project("CONFIG"), "', ";
    print STDERR "options='", Project("options"), "' ...\n";

    IncludeTemplate("destdir");
    IncludeTemplate("platform");

    IncludeTemplate("autoconf");
    foreach (split(/\s+/, Project("EXTRA_TEMPLATES"))) {
        IncludeTemplate($_);
    }

    if (Project("SUBDIRS")) {
        IncludeTemplate("autosubdirs");
    }
    else {
        IncludeTemplate("win32-rc") unless $is_unix;
        IncludeTemplate("version");
        my $Template = Project("TMAKE_TEMPLATE");
        IncludeTemplate($Template) if($Template);
        IncludeTemplate("buildinfo.t");
        IncludeTemplate("dep_libs.t");
        IncludeTemplate("install");
        if (Project("DIST2")) {
            IncludeTemplate("dist/dist");
        }
        else {
            IncludeTemplate("dist");
        }
        IncludeTemplate("clean.t");
        IncludeTemplate("test.t") if (defined $Options{"tests"});
        exit 100 if ($ENV{"RECURSIVE_CONFIGURE"} and Project("MAKE_PARALLEL") and not Project("NOTPARALLEL"));
    }
#$}
