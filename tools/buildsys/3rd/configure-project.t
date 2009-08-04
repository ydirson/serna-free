#${
#!    IncludeTemplate("3rd/process_pkg_config");
    my $dynamic = Config("dynamic");
    my $debug = Config("debug");
    
    my $config_opts = Project("PACKAGE_CFG_OPTIONS");

    if (Config("dynamic")) {
        $config_opts .= ' ' . Project("PACKAGE_CFG_DYNAMIC");
    }
    else {
        $config_opts .= ' ' . Project("PACKAGE_CFG_STATIC");
    }
    if (Config("debug")) {
        $config_opts .= ' ' . Project("PACKAGE_CFG_DEBUG");
    }
    else {
        $config_opts .= ' ' . Project("PACKAGE_CFG_RELEASE");
    }

    Project("ARCHIVE") && IncludeTemplate("3rd/archive");
    IncludeTemplate("3rd/make_vars");

    my $command = "";

    my $precfg = Project("PACKAGE_PRECONFIG");
    $command .= "$precfg\n" if ($precfg);

    my @ev = split(/\s+/, Project("PACKAGE_CFG_ENV"));
    foreach (@ev) {
        while (/\$\(\w+\)/) {
            s/\$\((\w+)\)/$project{$1}/g;
        }
    }
    if ($is_unix) {
        $command = join(" \\\n\t", @ev) . " \\\n" if @ev;
        $command .= "/bin/sh -c '";
        $command .= './' unless (Project("PACKAGE_CONFIGURE") =~ /^\s*[\/\$\.]/);
        my @cfg_optlist = split(/\s+/, $config_opts);
        $config_opts = " \\\n\t".join(" \\\n\t", @cfg_optlist);
    }
    else {
        if (@ev) {
            grep { s-\/-$dir_sep-g; } (@ev);
            $command = "set " . join("\nset ", @ev) . "\n";
            Project("PACKAGE_CFG_ENV = " . join(' ', @ev));
        }
    }        
    my $top_srcdir = Project("top_srcdir");

    my $configure = Project("PACKAGE_CONFIGURE");
    $command .= "$configure $config_opts > $top_srcdir${dir_sep}configure.log 2>&1";
    $command .= "'" if ($is_unix);

    my $postcfg = Project("PACKAGE_POSTCONFIG");
    $command .= "\n$postcfg" if ($postcfg);
    
    Project("CONFIGURE_COMMAND = " . $command);

    my $cfg_opts = ".configure_options";
    my $already_configured = 0;
    if(-r $cfg_opts) {
        open CONFIG_OPTS, "<$cfg_opts" || tmake_error("Couldn't open $cfg_opts for reading\n");
        $already_configured = "$command\n" eq <CONFIG_OPTS>;
        close CONFIG_OPTS;
    }
    if(!$already_configured) {
            open CONFIG_OPTS, ">$cfg_opts" || tmake_error("Couldn't open $cfg_opts for writing\n");
        print CONFIG_OPTS "$command\n";
        close CONFIG_OPTS;
    }
    Project("FILETOOLS = TOUCH");
    IncludeTemplate("filetools");
#$}
PACKAGE_CFG_ENV        = #$ Expand("PACKAGE_CFG_ENV");
PACKAGE_CONFIGURE      = #$ Expand("PACKAGE_CONFIGURE");
PACKAGE_CFG_OPTIONS    = #$ ExpandGlue("PACKAGE_CFG_OPTIONS", "\t\\\n"."\t"x4, " \\\n"."\t"x4, "");

.configure_done: $(PATCH_FLAG) $(UNPACK_FLAG) .configure_options
	cd $(MAKE_DIR) #$ $text = '; . $(PACKAGE_REL_DIR)/.configure_options' if $is_unix;
#$ $is_unix && DisableOutput();
	@copy $(PACKAGE_REL_DIR)\.configure_options $(PACKAGE_REL_DIR)\.configure_options.cmd
	cmd.exe /c $(PACKAGE_REL_DIR)\.configure_options.cmd
#!	#$ $text = 'cmd.exe < $(PACKAGE_REL_DIR)'.$dir_sep.'.configure_options'
	cd $(PACKAGE_REL_DIR)
#$ $is_unix && EnableOutput();
	$(TOUCH) $@

all_prereq: .configure_done

clean: configclean

configclean:
	$(RM) .configure_done configure.log

#${
    IncludeTemplate("3rd/emit_makefile_targets");
#$}
