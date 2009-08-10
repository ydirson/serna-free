#$ DisableOutput();
#
# Syntext Editor source list generation template
# 
#
#$ EnableOutput();
#${
    sub exclude_pattern {
        my ($srcref, $patterns) = @_;
        return unless $patterns;
        foreach $pattern (split(/\s/, $patterns)) {
            for($i = 0; $i < @$srcref; $i++) {
                if(@$srcref[$i] =~ m^$pattern^) {
                    splice @$srcref, $i, 1;
                }
            }
        }
        return @$srcref;
    }
    sub genlist {
        my ($srcref, $pattern, $excl_pattern, $srcdir, $recurse) = @_;
        $recurse = $recurse ? 1 : 0;
        foreach(split(/\s+/, $pattern)) {
            m-(.*/)*([^/]+$)-;
            my $chkdir = expand_path($1);
            unless (is_absolute_path($chkdir)) {
                $chkdir = fullpath($srcdir.$dir_sep.$chkdir);
            }
            my @srcfiles = find_files($chkdir, $2, $recurse);
            push @$srcref, @srcfiles;
        }
        exclude_pattern($srcref, $excl_pattern);
    }
    sub remove_duplicates {
        my ($srcref) = @_;
        my %dict = ();
        foreach (@$srcref) {
            $dict{$_} = 1;
        }
        @$srcref = keys(%dict);
    }
    
    my @sources = ();
    my %srcdict;
    my ($srcdir, $blddir) = (Project("srcdir"), getcwd());
    my $recursive = Project("GENLIST_RECURSIVE") eq 'yes';
    Project('SOURCES_PATTERN*=\.cxx$') unless(Project('SOURCES_PATTERN'));
    Project('HEADERS_PATTERN*=\.h$') unless(Project('HEADERS_PATTERN'));

    if (Project("SOURCES_PATTERN")) {
        my $pattern = Project("SOURCES_PATTERN");
        my $excl_pattern = Project("SOURCES_EXCLUDE_PATTERN") . " " . Project("EXCLUDE_PATTERN");
        genlist(\@sources, $pattern, $excl_pattern, $srcdir, $recursive);
        my @srcinc = ();
        foreach $f (@sources ) {
            if (open(F,"< $f") ) {
                while (<F> ) {
                    if (/^\s*#\s*include\s+\"([^\"]*)\"/ ) {
                        push @srcinc, $1 unless($1 =~ /\.h$/);
#!                      $srcdict{$1} = $1;
                    }
                }
            }
        }
        my @src2;
        foreach $f (@sources ) {
            foreach (@srcinc) {
                if($f =~ /\Q$_\E/) {
                    $f = "";
                    break;
                }
            }
            push(@src2,$f) if($f);
        }

        $sources = join(' ', @src2);
        Project("SOURCES += $sources");
    }

    if (Project("HEADERS_PATTERN")) {
        @sources = ();
        my $pattern = Project("HEADERS_PATTERN");
        my $excl_pattern = Project("HEADERS_EXCLUDE_PATTERN") . " " . Project("EXCLUDE_PATTERN");
        genlist(\@sources, $pattern, $excl_pattern, $srcdir, $recursive);

        $sources = join(' ', @sources);
        Project("HEADERS += $sources");
    }
    my %generated = ();
    foreach $s_sfx (split(/\s+/, Project("SOURCE_SUFFIXES"))) {
        @sources = ();
        my $s_sfx_h = $s_sfx;
        $s_sfx_h =~ s/\./_/g;
        $s_sfx =~ s/\./\\\./g;  #! escape all dots (make regular expression from $s_sfx)
        my %targets = ();
        my $pattern = Project("$s_sfx_h"."_PATTERN");
        $pattern = "\\.$s_sfx\$" unless($pattern);
        my $excl_pattern = Project("$s_sfx_h"."_EXCLUDE_PATTERN") . " " . Project("EXCLUDE_PATTERN");
        genlist(\@sources, $pattern, $excl_pattern, $srcdir, $recursive);
        push @sources, @{$generated{$s_sfx_h}} if defined($generated{$s_sfx_h});
        next unless(@sources);

        Project("$s_sfx_h"."_FILES = " . join(" ", @sources));
        my $var = Project("$s_sfx_h"."_VAR");
        Project("$var = " . Project("$s_sfx_h"."_FILES")) if($var);

        my @target_suffixes = (split(/[\s,;]+/, Project("$s_sfx_h"."_TARGET_SUFFIXES")),
                               split(/[\s,;]+/, Project("$s_sfx_h"."_TARGET_SOURCE_SUFFIXES")),
                               split(/[\s,;]+/, Project("$s_sfx_h"."_TARGET_HEADER_SUFFIXES")));

        remove_duplicates(\@target_suffixes);

        my $is_ui_sfx = ($s_sfx =~ /^ui.?$/);
        foreach $t_sfx (@target_suffixes) {
            my $src2tgt = $s_sfx_h."2".$t_sfx;
            next unless($src2tgt);

            my $rules = "";
            my ($odir, $mdir) = map { Project($_) } qw(OBJECTS_DIR MOC_DIR);
            foreach $src (@sources) {
                $src =~ s^[\\/]^$dir_sep^g;
                my $tgt = $src;
                $tgt =~ s^\.$s_sfx(?=\s|$)^^;
                $tgt =~ s/.*[\\\/](?=[^\\\/]*$)//;
                $tgt = "$blddir$dir_sep$tgt.$t_sfx";
                $rules .= "$tgt: $src\n\t\$($src2tgt) \$($src2tgt"."_FLAGS)\n\n";
                if (0 and $is_ui_sfx) {
                    my $moc_base = "moc_$tgtbase";
                    my $moc_obj = "$odir$dir_sep$moc_base.$obj_ext";
                    my $moc_src = "$mdir$dir_sep$moc_base.cpp";
                    Project("SRCMOC *= $moc_src", "OBJMOC *= $moc_obj");
                    $moc_output{$tgt} = $moc_src;
                    $moc_input{$moc_src} = $tgt;
                }
                push @{ $targets{$t_sfx}}, $tgt unless(Project("EXCLUDE_PATTERN") =~ /\Q$tgt\E/ );
            }
            my $prog = Project("$src2tgt");
            $prog =~ s^[\\\/]^$dir_sep^g;
            $text .= "$src2tgt\t= $prog\n";
            $text .= "$src2tgt" . "_FLAGS\t= " . Project("$src2tgt" . "_FLAGS") . "\n";
            my $tgt_var = "$s_sfx_h"."_$t_sfx"; #! makefile var name for all generated targets with $t_sfx
            $text .= $tgt_var . "\t= \t\\\n\t\t" . join(" \\\n\t\t", @{$targets{$t_sfx}}) . "\n\n";
            !# store the makefile var name (to include in 'all' dependency list)
            Project("GENERATED_SRC *= $tgt_var");
            $text .= $rules;

            my $t_targets = join(' ', @{$targets{$t_sfx}});
            Project("$s_sfx_h"."_$t_sfx = $t_targets");
            my $var = Project("$s_sfx_h"."_$t_sfx"."_VAR");
            Project("$var = $t_targets") if($var);
            Project("CLEANFILES += $t_targets");
            push @{$generated{$t_sfx}}, @{$targets{$t_sfx}};
        }

        foreach (split(/\s+/, Project("$s_sfx_h"."_TARGET_SOURCE_SUFFIXES"))) {
            Project("SOURCES += " . join(' ', @{$targets{$_}})) if(defined($targets{$_}));
        }

        foreach (split(/\s+/, Project("$s_sfx_h"."_TARGET_HEADER_SUFFIXES"))) {
            Project("HEADERS += " . join(' ', @{$targets{$_}})) if(defined($targets{$_}));
        }
    }
    if ($text) {
        my $banner = <<'EOF';
##############################################################################
## Syntext Editor source list generation template
## 
##############################################################################

EOF
        $text = $banner . $text;
        chomp $text;
        my $msggen_mod = Project("MODULE_NAME");
        $msggen_mod = Project("TARGET") unless $msggen_mod;
        $text = "MSGGEN_MODULE = " . $msggen_mod . "\n" . $text;
        Project("GENLIST = 1");
    }
    if (Config("moc")) {
        my $uis = "";
        foreach (split(/\s/, Project("UI_DIRS"))) {
            $uis .= ' ' . join(' ', find_files($_, '\.ui3$'));
        }
        Project("INTERFACES+=" . $uis);
    }
    Project('CLEANFILES *= $(SRCMOC)');
#$}
#$ Project("GENLIST") || DisableOutput();
GENERATED_SRC = #$ ExpandGlue('GENERATED_SRC', '$(', ') $(', ')');

gensrc: $(GENERATED_SRC)

#$ Project("GENLIST") || EnableOutput();
