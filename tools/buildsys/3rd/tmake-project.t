#${
    my $dynamic = Config("dynamic");
    my $debug = Config("debug");

#!    my $pkg_template = Project("PACKAGE_TEMPLATE");
#!    my $pj_defaults = Project("PACKAGE_DEFAULTS");

#!    my $src_proj = Project("SRC_PROJECT");
    my $save_proj = Project("PROJECT");
    my $save_makefile = Project("MAKEFILE");

    my $overrides .= "DEFAULT_CONFIG PLATFORM_DESTDIRS MAKE TMAKE MTARGETS VERSION";
    my $appends = "";

    my $save_template = Project("TEMPLATE");

    my %save_project = ();
    foreach (split /\s+/, $overrides . " " . $appends) {
    	$save_project{ $_ } = $project{ $_ } if(defined($project{ $_ }));
    }

#!    ScanProject($src_proj . ".pro") if($src_proj);

#!    if($pj_defaults) {
#!        ScanProject(find_template($pj_defaults));
#!    }

    my $pkg_makefile = Project("PACKAGE_MAKEFILE");
    $pkg_makefile = "Makefile" unless($pkg_makefile);

    IncludeTemplate("config");
    SetConfig("debug") if($debug);
    tmake_debug("3rd/tmake-project.t: CONFIG = ", Project("CONFIG"));

    foreach (split /\s+/, $overrides) {
        $project{$_} = $save_project{$_} if(defined($save_project{$_}));
    }
    foreach (split /\s+/, $appends) {
	$project{$_} .= " " . $save_project{$_} if(defined($save_project{$_}));
    }

    Project("PROJECT = $src_proj") if($src_proj);

    if(Project("MTARGETS")) {
	$pkg_makefile .= "." . $src_proj;
    }

    *SAVESTDOUT = *STDOUT;
    open MAKEFILE, ">$pkg_makefile" || tmake_error("3rd/tmake-project.t: Can't open '$pkg_makefile' for writing");
    *STDOUT = *MAKEFILE;

    Project("MAKEFILE = $pkg_makefile");

#!    if($pkg_template) {
#!		IncludeTemplate(find_template($pkg_template));
#!    }

    IncludeTemplate("target");
    IncludeTemplate("platform");

    foreach (split(/\s+/, Project("EXTRA_TEMPLATES"))) {
    	IncludeTemplate($_);
    }

    IncludeTemplate("3rd/pkg_dirs");

    my $template = Project("TEMPLATE");
    if($template eq $save_template) {
	$template = Config("app") ? "app" : "lib";
    }

    my $destdir = Project("DESTDIR");
    $destdir =~ s-[\\/]-$dir_sep-g;

    IncludeTemplate("3rd/tmake-cleanfiles");

    mkdirp(Project("OBJECTS_DIR"), 0755) if(Project("OBJECTS_DIR"));

    if($template =~ /^subdirs[$\.]/ || Project("SUBDIRS")) {
      	IncludeTemplate("superdir");
    }
    else {
   	$stdinit_done = 0;
      	IncludeTemplate($template);
    }

    IncludeTemplate("3rd/tmake-targets");
	
    close MAKEFILE;
    *STDOUT = *SAVESTDOUT;
    Project("PROJECT = $save_proj", "MAKEFILE = $save_makefile");
    Project("MTARGET_MAKEFILE = $pkg_makefile");
#$}
