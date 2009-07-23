#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#!
#! Syntext Editor template for building 3rd components
#! 
#!
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#${
    binmode STDOUT;
    IncludeTemplate("utils");

    my $platform = platform();
    Project("PLATFORM = $platform", "CONFIG *= $platform");

    IncludeTemplate("fix_init_config");
    IncludeTemplate("dirs");
    IncludeTemplate("options");

    tmake_debug("3rd.t: Initial CONFIG = '", Project("CONFIG"), "'");

    if (Project("SUBDIRS")) {
        IncludeTemplate("autosubdirs");
    }
    else {
        my $top_builddir = Project("top_builddir");
        my $top_srcdir = Project("top_srcdir");
        my $pkg = lc($Options{"package"});

        if ($pkg) {
            my $cwd = getcwd();
            my @dirs = ();
            foreach $name (split /[\\\/]+/, $cwd) {
                push @dirs, $name;
                last if $name eq $pkg;
            }
            my $tdir = join($dir_sep, @dirs);
            grep { 
                $_ = $tdir unless /\Q$dir_sep\E$pkg/; 
            } ($top_builddir, $top_srcdir);
            Project("top_srcdir = $top_srcdir", "top_builddir = $top_builddir");
            Project("PACKAGE = $Options{'package'}");
        }

        -d $top_srcdir || tmake_error("top_srcdir <$top_srcdir> doesn't exist");

        IncludeTemplate("config");

        my $cwd = getcwd();
        print STDERR "Configuring in \"$cwd\" with CONFIG='", Project("CONFIG"), "', ";
        print STDERR "options='", Project("options"), "' ...\n";

        tmake_debug("3rd.t: CONFIG = '", Project("CONFIG"), "'");

        IncludeTemplate("3rd/target");
        
        unless (Project("THIRD_HOME")) {
            my $home = $ENV{HOME};
            if (!$is_unix) {
                $home = 'c:\\3rd\\'.$ENV{USERNAME}.'\\dist';
            }
            else {
                $home .= "/3rd/dist";
            }
            Project("THIRD_HOME=$home");
        }
        my $tbh = Project("THIRD_BUILD_HOME");
        unless ($tbh) {
            my @th = split($is_unix ? '/' : '\\\\', Project("THIRD_HOME"));
            pop @th;
            push @th, "build";
            Project("THIRD_BUILD_HOME = ".join($dir_sep, @th));
        }

        my ($pkg_def, $pkg_def_local, $pkg_unix, $pkg_plat, $pkg_tpl) = map { 
            $top_srcdir.$dir_sep.$_
        } ("$pkg-defaults.pro", "$pkg-defaults.local.pro", 
             "$pkg-unix.pro", "$pkg-$platform.pro", "$pkg-default.t");

        -r $pkg_def && ScanProject($pkg_def);
        -r $pkg_def_local && ScanProject($pkg_def_local);
        if ($is_unix && -r $pkg_unix) { 
            ScanProject($pkg_unix);
        }

        -r $pkg_plat && ScanProject($pkg_plat);

        IncludeTemplate($_) foreach (split(/\s+/, Project("EXTRA_TEMPLATES")));

        -r $pkg_tpl && IncludeTemplate($pkg_tpl);

        IncludeTemplate("autoconf");

        my $Template = Project("TMAKE_TEMPLATE");

        IncludeTemplate("3rd/buildlog");
        IncludeTemplate($Template) unless !$Template;
        IncludeTemplate("3rd/install2");
    }
#$} 
