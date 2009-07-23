#
# Distribution generation template (all platforms)
# 
#${
    Project("FILETOOLS = ZIP"); IncludeTemplate("filetools");
    my @tm = localtime();
    my $release = sprintf("%4d%02d%02d", ($tm[5]+1900), ($tm[4]+1), $tm[3]);
    Project("RELEASE = $release", "DIST_VARS *= RELEASE");
    my $distpro = Project("srcdir")."/dist.pro";
    if (-r $distpro) {
        Project("DIST_SETTINGS = $distpro");
        ScanProject($distpro);
        my $dist_t = Project("srcdir")."/dist.t";
        IncludeTemplate($dist_t) if (-r $dist_t);
        my $filetags = Project("FILETAGS");
        foreach (split(/\s+/, Project("DIST_VARS"))) {
            my $var = Project($_);
            if ($filetags =~ /$_/) {
                $var =~ s^[\\\/]^$dir_sep^g;
                $var = normpath($var);
            }
            $text .= "\n$_\t\t= " . $var;
        }
    }
    my @mft_vars = split(/\s+/, Project("MFT_VARS"));
    Project("MFT_VARS_OPTS += -v ".join(" -v ", @mft_vars)) if (@mft_vars);
    Project("MAKEFILE_INSTALL = ".getcwd()."$dir_sep".'$$MAKEFILE_INSTALL');

    my $tr_pro = ScanProjectRaw(expand_path(Project("TS_TARGETS_LIST")));
    my @tsfiles = grep { s/.*[\\\/]([^\\\/]+)$/$1/; $_; } (split /\s+/, $tr_pro->{'TS_FILES'});
    Project("TSPACK_FILES = ".join(' ', @tsfiles));
    
    my @sections = split(/\s+/, Project("MFT_SECTIONS"));
    Project("MFT_SECTIONS = " . join(',', @sections));

    sub ExpandOpts {
        my ($pv) = @_;
        my @opts = split /\s+/, Project($pv);
        my $indent = ' ' x length("$pv = ");
        my $rv = (@opts ? " \\\n$indent" : "").shift(@opts);
        foreach (@opts) {
            $rv .= ' ';
            $rv .= "\\\n$indent" if (/^-.*$/);
            $rv .= $_;
        }
        return $rv if defined wantarray;
        return $text = $rv;
    }
#$}
MFT_VARS_OPTS = #$ ExpandOpts("MFT_VARS_OPTS"); #!ExpandGlue("MFT_VARS_OPTS", "\t", " \\\n\t\t\t", "");
#$ IncludeTemplate("dist/".(0 == $is_unix ? "win32" : "unix")) if Project("DIST_SETTINGS");
#$ IncludeTemplate("dist/packages") if Project("PACKAGES");
#$ DisableOutput() unless Project("TS_PACK");
TS_TARGETS_LIST = #$ Expand("TS_TARGETS_LIST");
#${
    Project("FILETOOLS = ZIP"); IncludeTemplate("filetools");
    if ($is_unix) {
        $text = "-include \$(TS_TARGETS_LIST)\n";
    }
    else {
        $text = "!IF EXIST (\$(TS_TARGETS_LIST))\n!INCLUDE \$(TS_TARGETS_LIST)\n!ENDIF\n";
    }
#$}
TS_PACK  = #$ Expand("TS_PACK");
TS_DIR   = #$ ExpandPathName("TS_DIR");

$(TS_PACK): $(TS_FILES)
	cd $(TS_DIR) && $(ZIP) -9j $@ $(TS_FILES) #!$ ExpandGlue("TSPACK_FILES", "\t\\\n\t\t\t", " \\\n\t\t\t", "");

tspack: $(TS_PACK)

dist_forward: tspack
#$ EnableOutput() unless Project("TS_PACK");
