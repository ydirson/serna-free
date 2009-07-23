#${
    IncludeTemplate("utils");
    IncludeTemplate("dirs");
    IncludeTemplate("read_locals");
    IncludeTemplate("autoconf");
    
    my @langs = split /\s+/, Project("SERNA_TRANSLATIONS");
    return unless @langs;
    
    IncludeTemplate("genlist");
    
    Project("FILETOOLS = XSLTPROC PYTHON UIC LUPDATE LRELEASE TOUCH COPY RMDIR PERL");
    IncludeTemplate("filetools");

sub MakeTargetName() {
    my ($src) = (@_);
    $src =~ s/.*?([^\\\/]+)$/$1/;
    $src =~ s-\.[^\.]*$--;
    return $src;
}

sub MakeRules() {
    my ($odir, $srclist, $cvt, $ext, $dep) = @_;
    $ext = $ext ? $ext : "cpp";
    my ($top_sdir, $top_bdir) = map { Project($_); } qw(top_srcdir top_builddir);
    my (@targets, @rules);
    foreach (split /\s+/, $srclist) {
        next if /^\s*$/;
        s-^$top_sdir-\$\(top_srcdir\)-;
        my $tbase = &MakeTargetName($_);
        my $target = "$odir$dir_sep$tbase.$ext";
        my $targetdep = "$odir$dir_sep$tbase.$dep" if ($dep);
        my $rule = <<"EOF";
$target: $_ $targetdep
	$cvt
EOF
        $rule =~ s/\$\?/$_/m;
        push @targets, $target;
        push @rules, $rule;
    }
    return (join("\n", @rules), join(' ', @targets));
}

    my $uis = "";
    foreach (split(/\s+/, Project("UI_DIRS"))) {
        $uis .= ' ' . join(' ', find_files($_, '\.ui$'));
    }
    Project("UI_FILES+=$uis");

    my @lu_items = ();
    push @lu_items, "FORMS = " . ExpandGlue("UI_FILES", "\t\\\n\t\t", " \\\n\t\t", "\n");
    $text = "";
        
    ($rules, $impls) = &MakeRules("msgcpp", Project("MSG_SOURCES"), '$(PYTHON) $(MSG2CPP) $? $@');
    Project("MSGIMPLS = $impls");
    Project("MSGRULES = $rules");
    Project("CPP_FILES += $impls", "MSGCPP = $impls");

    ($rules, $impls) = &MakeRules("suicpp", Project("SUI_SOURCES"), '$(XSLTPROC) -o $@ $(SUI2CPP) $?');
    Project("SUIIMPLS = $impls");
    Project("SUIRULES = $rules");
    Project("CPP_FILES += $impls ", "SUICPP = $impls");

    ($rules, $impls) = &MakeRules("spdcpp", Project("SPD_SOURCES"), '$(XSLTPROC) -o $@ $(SPD2CPP) $?');
    Project("SPDIMPLS = $impls");
    Project("SPDRULES = $rules");
    Project("CPP_FILES += $impls ", "SPDCPP = $impls");

    ($rules, $impls) = &MakeRules("scccpp", Project("SCC_SOURCES"), '$(XSLTPROC) -o $@ $(SCC2CPP) $?');
    Project("SCCIMPLS = $impls");
    Project("SCCRULES = $rules");
    Project("CPP_FILES += $impls ", "SCCCPP = $impls");

    ($rules, $impls) = &MakeRules("cslcpp", Project("CSL_SOURCES"), '$(XSLTPROC) -o $@ $(CSL2CPP) $?');
    Project("CSLIMPLS = $impls");
    Project("CSLRULES = $rules");
    Project("CPP_FILES += $impls ", "CSLCPP = $impls");
    
    Project('CPP_FILES += $$SOURCES');

    push @lu_items, "SOURCES = " . ExpandGlue("CPP_FILES", "\t\\\n\t\t", " \\\n\t\t", "\n");
    my @lu_sources = @lu_items;
    $text = "";
    
    my @cl = ();
    genlist(\@cl, Project("CHECK_LITERALS_PATTERN"), "", "", 0);
    push @lu_items, "CHECK_LITERALS_SOURCES = \\\n\t\t".join(" \\\n\t\t", @cl)."\n";
    
    Project("SERNA_TS_FILES = ts/serna_".join('.ts ts/serna_', @langs).".ts");

    push @lu_items, "TRANSLATIONS = " . ExpandGlue("SERNA_TS_FILES", , "\t\\\n\t\t", " \\\n\t\t", "\n");
    $text = "";
    
sub write_pro() {
    my ($var, $path, $contents, $noincl) = @_;
    my ($pro_contents);

    if (-r $path and open(WRITEPRO, "<$path")) {
        $pro_contents = join("", <WRITEPRO>);
        close(WRITEPRO);
    }
    unless ($contents eq $pro_contents) {
        open(WRITEPRO, ">$path") || die "Can't write to $path";
        print WRITEPRO $contents;
        close(WRITEPRO);
    }
    
    my $txt = "$var = $path\n\n";
    return $txt if defined $noincl;
    if ($is_unix) {
        $txt .= "-include \$($var)";
    }
    else {
        $txt .= <<"EOF";
!IF EXIST (\$($var))
!INCLUDE \$($var)
!ENDIF
EOF
    }
    return "$txt\n\n";
}
    
    $text .= &write_pro("LUPDATE_PROJECT", "lu_project.pro", join("\n", @lu_items));
    push @lu_sources, "\nTRANSLATIONS = ts${dir_sep}serna_untranslated.ts";
    $text .= &write_pro("LUPDATE_UT_PROJECT", "lu_ut_project.pro", join("\n", @lu_sources), 1);
    
#! -----------------
    my @tssrc = ();
    my $ts_sdir = '$(srcdir)/ts/';
    genlist(\@tssrc, '.*\.ts$', "", expand_path($ts_sdir), 0);
    
    my ($ts_bdir, $qm_bdir) = map { "\$(top_builddir)/serna/i18n/$_/" } (qw/ts qm/);
    grep { $_ =~ s^[\\\/]^$dir_sep^g; } ($ts_sdir, $ts_bdir, $qm_bdir);
    my %tsmap = ();
    foreach (@tssrc) {
        my ($srcpath, $base) = fnsplit($_);
        next unless $base =~ /^(.*)\.[^\.]+$/;
        $tsmap{$base} = { 'srcfile' => "$ts_sdir$base", 
                          'bldfile' => "$ts_bdir$base",
                          'qmfile' => "$qm_bdir$1.qm" };
    }
    foreach $lang (@langs) {
        foreach $base (qw/serna/) {
            my $tsbase = "${base}_${lang}";
            my $tsfile = "$tsbase.ts";
            unless (grep $tsfile eq $_, keys(%tsmap)) {
                $tsmap{$tsfile} = { 'srcfile' => '',
                                    'bldfile' => "$ts_bdir$tsfile",
                                    'qmfile'  => "$qm_bdir$tsbase.qm" };
            }
            $tsmap{$tsfile}->{'lupdate'} = 'yes';
        }
    }
    
    my (@qmfiles, @qmrules, @tsfiles, @tsrules);
    foreach $tsfile (keys %tsmap) {
        my $tsdict = $tsmap{$tsfile};
        my ($tsdep, $tsrule) = ("$tsdict->{'bldfile'}:", "");
        if ($tsdict->{'srcfile'}) {
            $tsdep .= " $tsdict->{'srcfile'}";
            $tsrule .= "\$(COPY) $tsdict->{'srcfile'} \$@";
        }
        else {
#!            $tsdep .= " force";
            $tsrule .= '@$(PERL) -e "print \'<!DOCTYPE TS><TS></TS>\'" > $@';
            $tsrule .= "\n\t".'@set PATH=$(THIRD_DIR)\\lib;%PATH%' unless $is_unix;
            $tsrule .= "\n\t".'$(LUPDATE) $(LUPDATE_UT_PROJECT)';
        }
        if (defined($tsdict->{'lupdate'})) {
            $tsdep .= " \$(LU_SOURCES) \$(LUPDATE_PROJECT)";
        }
        
        push @tsrules, "$tsdep\n\t$tsrule\n";
        push @tsfiles, $tsdict->{'bldfile'};
        
        my $qmrule = "$tsdict->{'qmfile'}: $tsdict->{'bldfile'}\n\t";
        $qmrule .= "set PATH=\$(THIRD_DIR)\\lib;%PATH%\n\t" unless $is_unix;
        $qmrule .= "\$(LRELEASE) $tsdict->{'bldfile'} -qm \$@\n";

        push @qmrules, $qmrule;
        push @qmfiles, $tsdict->{'qmfile'};
    }
    
    my ($tsvar, $qmvar) = ("TS_FILES = \\\n\t\t".join(" \\\n\t\t", @tsfiles),
                           "QM_FILES = \\\n\t\t".join(" \\\n\t\t", @qmfiles));
    Project($tsvar, $qmvar);
    Project("TS_RULES = " . join("\n", @tsrules));
    Project("QM_RULES = " . join("\n", @qmrules));

    $text .= &write_pro("TARGETS", "targets.pro", join("\n\n", $tsvar, $qmvar));
#$}
all: makedirs gen_qm # copy_qm

copy_qm: force
	$(COPY) #$ Expand("COPYQM_ARGS");

gen_qm: gen_ts $(QM_FILES)

gen_ts: $(TS_FILES)
#$ AddEnvValue('PATH=$(THIRD_DIR)\lib;') unless $is_unix;
	$(LUPDATE) $(LUPDATE_PROJECT)

prep_ts: makedirs $(SOURCES)

check_ts: prep_ts $(TS_FILES)
	@echo $(TS_FILES) | $(PYTHON) $(srcdir)/check_ts.py $(srcdir)/check_ts.xsl $(THIRD_DIR) -
        
check_literals:
	@$(PYTHON) $(srcdir)/check_literals.py \
#$ DisableOutput unless $is_unix;
	$(CHECK_LITERALS_SOURCES)
#$ EnableOutput unless $is_unix;
#$ DisableOutput if $is_unix;
@<<
	    $(CHECK_LITERALS_SOURCES)
<<
#$ EnableOutput if $is_unix;

XSLTPROC_PARAMS  = #$ Expand("XSLTPROC_PARAMS");

MSG_FILES  = #$ ExpandGlue("MSG_SOURCES", "\t\\\n\t\t", " \\\n\t\t", "\n");
SUI_FILES  = #$ ExpandGlue("SUI_SOURCES", "\t\\\n\t\t", " \\\n\t\t", "\n");

MSG2CPP    = #$ Expand("MSG2CPP");
SUI2CPP    = #$ Expand("SUI2CPP");
SPD2CPP    = #$ Expand("SPD2CPP");
SCC2CPP    = #$ Expand("SCC2CPP");
CSL2CPP    = #$ Expand("CSL2CPP");

#$ Expand("MSGRULES");

#$ Expand("SUIRULES");

#$ Expand("SPDRULES");

#$ Expand("SCCRULES");

#$ Expand("CSLRULES");

NUL        = #$ $text = $is_unix ? "/dev/null" : "nul";
DIRS       = suicpp spdcpp msgcpp scccpp cslcpp ts qm

makedirs: force
#$DisableOutput() if $is_unix;
	@for %D in ($(DIRS)) do if not exist %D mkdir %D
#$EnableOutput() if $is_unix;
#$DisableOutput() unless $is_unix;
	@for d in $(DIRS); do [ -d $$d ] || mkdir -p $$d; done
#$EnableOutput() unless $is_unix;

LU_SOURCES = $(FORMS) $(SOURCES) $(HEADERS)
        
#$ Expand("TS_RULES");

#$ Expand("QM_RULES");

clean: force
	$(RMDIR) $(DIRS)
