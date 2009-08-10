#$ DisableOutput() if Config("syspkg");
# Template for preparing python modules
#
#${
    my $third_dir = Project('THIRD_DIR');
    my $pyver = get_version("$third_dir/python/TMakefile.pro");
    my $tlibdir = '$(THIRD_DIR)'.$dir_sep.'lib';
    my ($pydir, $pyqt4_dir);
    unless ($is_unix) {
        $pydir = "\$(THIRD_DIR)\\python\\$pyver\\Python-$pyver";
        $pyqt4_dir = "$pydir\\Lib\\site-packages\\PyQt4";
        Project("PY_BINMODDIR = ".(Config('vc2003') ? 'PC\VS7.1' : "PCBuild"));
    }
    else {
        $pyver =~ s/\.[^\.]+$//;
        $pydir = "\$(THIRD_DIR)/python/install";
        $pyqt4_dir = "$pydir/lib/python$pyver/site-packages/PyQt4";
    }
    Project("PYTHONHOME = $pydir", "PYQT4_SRCMODDIR = $pyqt4_dir", "PY_VERSFX = $pyver");
    $pyqt4_dir = '$(PYQT4_SRCMODDIR)';

    my %mdict = (
                  SIP => { base => "sip", srcdir => $tlibdir,
                           dstdir => '$(PYPLUGIN_DIR)' },
                  PYAPI => { base => "SernaApiCore", srcdir => '$(SERNA_LIBDIR)',
                             dstdir => '$(PYPLUGIN_DIR)' }
                );
    my ($dbg_sfx, $mod_sfx) = ("", "");
    my $pyapi_pro = Project("top_builddir")."/serna/lib/SernaApiCore.pro";
    my $pyapi_vars = ScanProjectRaw($pyapi_pro);
    if ($is_unix) {
        Project("PYAPI_BIN = ".$$pyapi_vars{"DESTDIR"}.$dir_sep.$$pyapi_vars{"TARGET_FILE"});
        $mod_sfx = ".so";
    }
    else {
        my $pyapi_base = $$pyapi_vars{"TARGET_FILE"};
        $pyapi_base =~ s/\.lib$//;
        Project("PYAPI_BIN = ".$$pyapi_vars{"DESTDIR"}."$dir_sep$pyapi_base.dll");
        $dbg_sfx = "_d" if Config("debug");
        $mod_sfx = ".pyd";
        foreach $m ($is_unix ? qw(unicodedata _sre datetime) : qw(unicodedata)) {
            $mdict{uc($m)}{"base"} = $m;
            $mdict{uc($m)}{"srcdir"} = '$(PYTHON_MODDIR)';
            $mdict{uc($m)}{"dstdir"} = '$(PYPLUGIN_DIR)';
        }
        my $sip_target = join($dir_sep, $tlibdir, sip).$dbg_sfx.$mod_sfx;
        Project("SIP_BIN = $sip_target");
    }
    Project("FILETOOLS = SYMLINK TOUCH CPDIR");
    IncludeTemplate("filetools");
    my @rules = (), @targets = ();
    foreach $m (keys %mdict) {
        my $varname = uc($m);
        my $modfile = $mdict{$m}{"base"}.$dbg_sfx.$mod_sfx;
        my $target = join($dir_sep, $mdict{$m}{"dstdir"}, $modfile);
        my $dep = $mdict{$m}{"srcdir"}.$dir_sep.$modfile;
        $dep = Project("${varname}_BIN") if Project("${varname}_BIN");
        push @rules, "${target}: ${dep}\n\t\$(SYMLINK) \$? \$@\n";
        push @targets, $target;
    }
    if ($is_unix) {
        push @rules, <<'EOF';
$(PYPLUGIN_DIR)/PyQt4/__init__.py: force
	rm -rf $(PYPLUGIN_DIR)/PyQt4
	ln -sf $(PYQT4_SRCMODDIR) $(PYPLUGIN_DIR)
EOF
        push @targets, '$(PYPLUGIN_DIR)/PyQt4/__init__.py';
    }
    else {
        my $pq4srcdir = expand_path('$(PYQT4_SRCMODDIR)');
        my @pq4files = find_files($pq4srcdir, '\.pyd?$', 1);
        my $pq4sdlen = length($pq4srcdir);
        my %dstdirs = ();
        my @pyqt4_targets = ();
        my $dstbasedir = '$(PYPLUGIN_DIR)\\PyQt4';
        foreach $file (@pq4files) {
            $file =~ s-[\\/]+-\\-g;
            my $tail = substr($file, $pq4sdlen + 1);
            push @pyqt4_targets, $dstbasedir.'\\'.$tail;
            my ($dirname, $basename) = fnsplit($tail);
            $dirname = '' unless $dirname;
            $dstdirs{$dirname} = 1;
        }
        Project("PYQT4_FILES = ".join(' ', @pyqt4_targets));
        push @rules, ".SUFFIXES: .py .pyd\n";
        foreach $dir (keys %dstdirs) {
            $dir = "\\$dir" if $dir;
            push @rules, "\{\$(PYQT4_SRCMODDIR)$dir}.pyd\{$dstbasedir$dir}.pyd::";
            push @rules, "\tif not exist $dstbasedir$dir md $dstbasedir$dir";
            push @rules, "\t\$(CPDIR) \$(PYQT4_SRCMODDIR)$dir\\*.pyd $dstbasedir$dir\n";
            push @rules, "\{\$(PYQT4_SRCMODDIR)$dir}.py\{$dstbasedir$dir}.py::";
            push @rules, "\tif not exist $dstbasedir$dir md $dstbasedir$dir";
            push @rules, "\t\$(CPDIR) \$(PYQT4_SRCMODDIR)$dir\\*.py $dstbasedir$dir\n";
        }
        push @targets, '$(PYQT4_FILES)';
        my $pq4mk = "PYQT4_FILES\t= \\\n\t\t\t".join(" \\\n\t\t\t", @pyqt4_targets);
        $pq4mk .= "\n\n".join("\n", @rules);
        write_file("Makefile.pyplugins", $pq4mk);
    }
    Project("PYMODULES_RULES = ".join("\n", @rules));
    Project("PYPLUGINS = ".join(" ", @targets));
    Project("ALL_DEPS += copy_pyplugins") unless Config("syspkg");
#$}

SERNA_LIBDIR    = $(top_builddir)#$ $text = $dir_sep.join($dir_sep, qw(serna lib));
PYPLUGIN_DIR    = #$ $text = '$(SERNA_LIBDIR)'.$dir_sep."pyplugin";
PYQT4_SRCMODDIR = #$ Expand("PYQT4_SRCMODDIR");
PY_VERSFX       = #$ Expand("PY_VERSFX");
#$ DisableOutput() if $is_unix;
PYTHONHOME      = #$ Expand("PYTHONHOME");
PY_BINMODDIR    = #$ Expand("PY_BINMODDIR");
PYTHON_MODDIR   = $(PYTHONHOME)\$(PY_BINMODDIR)
#$ EnableOutput() if $is_unix;

PYAPI_DLL       = #$ Expand("PYAPI_BIN");

#$ Expand("PYMODULES_RULES") if $is_unix;
#$ $text = "!IF EXIST(Makefile.pyplugins)\n!  INCLUDE Makefile.pyplugins\n!ENDIF" unless $is_unix;

copy_pyplugins: #$ ExpandGlue("PYPLUGINS", "\\\n\t\t", " \\\n\t\t", "\n");
#$ EnableOutput() if Config("syspkg");
