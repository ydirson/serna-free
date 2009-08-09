#${
    ($uic_impl, $uic_decl) = qw(cpp hpp);

    $indent = 0;

    sub tmake_debug {
        print STDERR " " x $indent, @_, "\n" if ($ENV{"TMAKE_DEBUG"} == 1);
    }

    sub getcwd {
        my $cwd = (0 == $is_unix) ? `cd` : `pwd`;
        $cwd =~ s-[\n/\\]+$--;
        return $cwd;
    }

    sub get_version {
        my ($ver_file) = @_;
        my @ver_list = (undef) x 3;
        if (-r $ver_file && open(VERSION, "<$ver_file")) {
            while(<VERSION>) {
                if (s-^(?:\s*VERSION\s*=)*\s*([^\.]+)\.([^\.]+)\.([^\s]*)--) {
                    @ver_list[0,1] = ($1, $2);
                    @ver_list[2] = $3 if defined($3);
                    last;
                }
            }
            close(VERSION);
        }
        return wantarray ? @ver_list : join('.', @ver_list);
    }

    sub ExpandWithVars {
        my $pv = Project(shift @_);
        my %vhash = map { $_, Project($_) } @_;
        grep { $pv =~ s/\Q$vhash{$_}\E/\$\($_\)/g } @_;
        return $text = $pv;
    }

    sub write_file {
        my ($fname, $str, $append) = @_;
        open(TMPFILE, ($append ? ">" : "").">$fname")
            or die "Can't open $fname for write\n";
        syswrite(TMPFILE, $str, length($str));
        close(TMPFILE);
    }

    *SavedIncludeTemplate = *IncludeTemplate;
    *IncludeTemplate = *IncludeTemplateEx;

    sub IncludeTemplateEx {
      my ($name) = @_;
      $template_name = find_template_ex($name);
      undef %tmake_template_dict;
      if ($tmake_template_dict_ex{$name}++) {
          tmake_error("Recursive template inclusion in IncludeTemplateEx($name )\n");
      }
      tmake_debug "Processing template $name ($tmake_template_dict_ex{$name}, $output_count)...";
      $indent += 2;
      SavedIncludeTemplate(@_);
      $tmake_template_dict_ex{$name}--;
      $indent -= 2;
      tmake_debug "Ended processing template $name ($tmake_template_dict_ex{$name}, $output_count)...";
    }

    *mkdirp = *ourmkdirp;

    sub ourmkdirp {
      my($filename, $mode) = @_;
      $mode ||= 0755;
      if ($filename =~ /\$\(\w+\)/ ) { #! ignore "$(something)"
          return 0;
      }
      $filename =~ s-[\\:/]+-/-g unless(0 == $is_unix);
      if (-d $filename ) {
          return 1;
      }
      $filename =~ m-^((.*)[\\/])?(.*)-;
      if (defined($2) && ! ourmkdirp($2,$mode) ) {
          return 0;
      }
      return mkdir($filename,$mode);
    }

    *TmakeSelf = *do_nothing;

    sub do_nothing {
      $text = "";
    }

    sub expand_path {
        my ($path) = @_;
        while ($path =~ s/\$\(([^)]*)\)/$project{$1}/gs) {};
        $path =~ s|[\\/]|$dir_sep|g;
        return $path;
    }

    sub is_absolute_path {
        if ($is_unix) {
            return $_[0] =~ m-^[\$/]-;
        }
        else {
            return $_[0] =~ m/^([a-zA-Z]{1}:)|(\$\(.*\))/;
        }
    }

    sub fullpath {
        my ($tpath) = @_;
        return $tpath if(is_absolute_path($tpath));
        my $cwd = getcwd();
        if (chdir $tpath) {
            $tpath = getcwd();
            chdir $cwd;
        }
        else {
            $tpath =~ s-^${dir_sep}+--;
            $tpath = $cwd . $dir_sep . $tpath;
        }
        return $tpath;
    }

    sub normpath {
        my ($path) = @_;
        my @parts = split(/[\/\\]+/, $path);
        for($i = 0; $i < @parts; $i++) {
            if(@parts[$i] eq '..' && $i > 1) {
                splice(@parts, $i-1, 2);
                $i -= 2;
            }
        }
        $path = join($dir_sep, @parts);
        $path =~ s|[\\/]{2,}|$dir_sep|g;
        $path =~ s|[\\/]$||;
        return $path;
    }

    sub relative {
#!      this subroutine computes relative path that's needed
#!      to be prepended to $src in order to get $dst
        my ($dst, $src) = @_;
        $dst = getcwd() unless($dst =~ /[^\s]/);
        grep {$_ = fullpath($_)} ($dst, $src);
        return "" if $dst eq $src;
        my $i = 0;
        while (substr($dst, $i, 1) eq substr($src, $i, 1)) {
            $i++;
        }
        return $dst if($i == 0);
        my ($d, $s) = (substr($dst, $i), substr($src, $i));
        return "" if($d eq $s);
        my $prefix = "";
        if ($s) {
            $s =~ s-(^\Q$dir_sep\E)|(\Q$dir_sep\E$)--;
            $prefix = (('..' . $dir_sep) x ($s =~ s-\Q$dir_sep\E--g)) . '..';
            $prefix .= $dir_sep if ($d);
        }
        $d =~ s-(^\Q$dir_sep\E)|(\Q$dir_sep\E$)--;
        return $prefix . $d;
    }

    %ConfigBits = (
        dll => 'staticlib',
        dynamic => 'static',
        debug => 'release',
        warn_on => 'warn_off',
        app => 'lib'
    );
    foreach (keys %ConfigBits) {
        $ConfigBits{ $ConfigBits{ $_ }} = $_;
    }

    sub SetConfig {
        my ($bit) = @_;
        my $rst = $ConfigBits{ $bit };
        Project("CONFIG -= $rst") if(defined($rst));
        Project("CONFIG *= $bit");
        return Project("CONFIG *= $bit");
    }

    sub IsConfigSet {
        my ($bit) = @_;
        my $rst = $ConfigBits{ $bit };
        return Config("$bit") || (defined($rst) && Config("$rst"));
    }

    sub platform {
        my $platform = Project("PLATFORM");
        return $platform if ($platform);
        $platform = $is_unix ? `uname -s` : 'win32';
        chomp($platform);
        return lc($platform);
    }

    sub cpu {
        my $cp = "i386";
        if ($is_unix) {
            $cp = `uname -p`;
            chomp($cp);
            if ($cp =~ m/^(i\d86|unknown|athlon)/) {
                $cp = "i386";
            }
            elsif ($cp =~ m/^power.*/i) {
                $cp = "powerpc";
            }
        }
        return lc($cp);
    }

    #!    *find_template = *find_template_ex;

    sub find_template_ex {
        my($filename) = @_;
        my($tb,$d,$p,@dirs);
        if (!defined($template_base) || ($template_base eq "") ) {
             $tb = "";
        } else {
             $tb = $template_base . ";";
        }
        $d = $tb . $project{"TMAKEPATH"};
        @dirs = ("");
        push @dirs, &split_path($d);
        $filename .= ".t" unless ($filename =~ /\.\w+$/);
        for $d (@dirs) {
           $p = $d . $filename;
           if (-f &fix_path($p) ) {
               if ($filename eq "tmake.conf" ) {
                   $tmake_platform = $d;
                   $tmake_platform =~ s-.*[/\\]([^/\\]*)[/\\]-$1-;
                   &tmake_verb("Detected platform $tmake_platform");
               }
               return $p;
           }
           return ($d . $filename) if (-f &fix_path($d . $filename) );
        }
        return "";
    }

    sub expand_by_dict {
        my ($v, $dict) = @_;
        return "" unless (defined($v) and defined($dict));
        my ($c) = 0;
        while ($c < 100) {            #! expand $$
           if ($v =~ s/(\$\$\{?\w+\}?)/\035/ ) {
               $_ = $1;
               s/[\$\{\}]+//g;
               if (!defined($$dict{$_}) ) {
                  $v =~ s/\035//g;
               } else {
                  $v =~ s/\035/$$dict{$_}/g;
               }
               $c++;
           } else {
               $c = 100;
           }
        }
        if ($v =~ /\\\$/) {
            $v =~ s-\\\044-\044-g;
        }
        return $v;
    }

    *SaveExpand_PV = *expand_project_var;
    *expand_project_var = *expand_project_var_ex;

    sub expand_project_var_ex {
        return expand_by_dict(@_, \%project);
    }

    sub fnsplit {
        my ($path) = @_;
        my @lst = split(/[\\\/]/, $path);
        return ("", "") if 0 == @lst;
        my $file = pop @lst;
        return (join($dir_sep, @lst), $file);
    }

    sub bn_change_ext {
        #! strips path and replaces extension from space separated list of files
        my ($files, $newext, $prefix) = @_;
        return map {
           s-(^|.*[/\\])([^/\\]*)\.\w+$-${prefix}$2\.${newext}-;
           $_
        } (split(/\s+/, $files));
    }

    sub UicImplsEx {
        return join(' ', &bn_change_ext($_[0], $uic_impl, ''));
    } *UicImpls = *UicImplsEx;

    sub UicDeclsEx {
       return join(' ', &bn_change_ext($_[0], $uic_decl, ''));
    } *UicDecls = *UicDeclsEx;

    sub BuildUicSrcEx { #! emits build rules for sources from uics
        my $uic_command = $uic_cmd . ' -o $@ ';
        foreach $ui (split(/\s+/, $_[0])) {
           next unless $ui =~ m-(^|.*[/\\])([^/\\]*)\.ui3$-;
           my ($bn, $ucpp, $uhpp) = ($2, $2.".$uic_impl", $2.".$uic_decl");
           $text .= "$uhpp: $ui\n\t" . $uic_command . '$?' . "\n\n";
           $text .= "$ucpp: $ui\n\t" . $uic_command . '-i $*.' . $uic_decl;
           $text .= ' $?' . "\n\n";
        }
        chop $text;
    } *BuildUicSrc = *BuildUicSrcEx;

    sub MocSourcesEx { #! returns a list of moc_xxx sources for uics
        my $mocpfx = $moc_pre;
        $mocpfx = $project{"MOC_DIR"}.$mocpfx if (defined($project{"MOC_DIR"}));
        my @files = split(/\s+/, $_[0]);
        foreach $ui (@files) {
           next unless $ui =~ s-(^|.*[/\\])([^/\\]*)\.ui3$-${mocpfx}$2-;
           my ($ucpp, $uhpp) = ($mocpfx.$2.".$uic_impl", $2.".$uic_decl");
           $moc_output{$uhpp} = $ucpp;
           $moc_input{$ucpp}  = $uhpp;
           $ui .= ".$uic_impl";
        }

        return join(" ", @files);
    } *MocSources = *MocSourcesEx;

    sub scan_dep_ex {
        my ($file) = @_;
        my ($dir,$path,$found,@allincs,@includes,%incs);
        $path = $file;
        @includes = ();
        return @includes if $file =~ /\.$moc_ext$/; #! avoid .moc files
        if (! (-f &fix_path($path)) ) {
            $found = 0;
            for $dir (@cur_dep_path ) {
                $path = $dir . $file;
                last if ($found = (-f &fix_path($path)) );
            }
            return @includes if ! $found;
        }
        undef $/;
        if (open(TMP, &fix_path($path))) {
            $full_path{$file} = $path;
            $_ = <TMP>;
            s-/\*.*?\*/--gs;                        #! strip C/C++ comments
            s-//.*\n-\n-g;
            @allincs = split(/\n/,$_);
            @allincs = grep(/^\s*\#\s*include/,@allincs);
            foreach (@allincs) {                  #! all #include lines
                next if !(/^\s*\#\s*include\s+\"([^\"]*)\"/) || defined($incs{$1});
                push (@includes,$1);
                $incs{$1} = "1";
                my $headerfile = $1;
                my $hfile = $headerfile;
                $hfile =~ s/\+/\\\+/g;
                $hfile =~ s-(^|.*[/\\])([^/\\]*)$-$2-g;
                if ($project{"UICDECLS"} =~ /\b$hfile\b/ ) {
                   $full_path{$headerfile} = $hfile;
                }
            }
            close(TMP);
        }
        $/ = "\n";
        return @includes;
    } *scan_dep = *scan_dep_ex;

    sub ExpandGlueEx {
        my ($var, $start, $sep, $trail, $width) = @_;
        return $text unless $project{$var};

        $width = 80 unless $width;
        my ($stabs, $etabs) = ();

        $stabs = length($1) if ($start =~ /^(\t+)/);
        $etabs = length($1) if ($start =~ /(\t+)$/);

        my $slen = length($start) - $etabs - $stabs;
        my $indent = "\t" x $stabs;
        $width -= ($stabs << 3);
        if ($etabs) {
            my $n = $etabs + ($slen >> 3);
            $indent .= "\t" x $n;
            $width -= ($n << 3);
        }
        else {
            $indent .= " " x $slen;
            $width -= $slen;
        }
        my $txt = $start;
        my ($line, $spaces) = ("", 0);
        my @items = split(/\s+/, $project{$var});
        foreach $item (@items) {
            if (length($line) + length($item) > $width) {
                $txt .= $line.$sep.(($sep =~ /\n/) ? $indent : "");
                $line = $item;
            }
            else {
                $line .= (' ' x $spaces).$item;
                $spaces = 1;
            }
        }
        $txt .= $line.$trail;
        return $txt if defined wantarray;
        $text = $txt;
    }

    sub ExpandPathName {
        return $text = normpath(Expand(@_[0]));
    }

    sub expand_multi {
        my ($rv, $maxlen) = ('', 0);
        grep { $maxlen = length($_) if $maxlen < length($_); 0 } (@_);
        $maxlen = 4 * (($maxlen + 4) / 4);
        foreach (@_) {
            my ($var, $val) = ($_, Project($_));
            if (/([^=]+)=(.*)/) {
                $var = $1;
                $val = $2;
            }
            $rv .= $var.(' ' x ($maxlen - length($var)))."= $val\n";
        }
        return $rv if defined wantarray;
        $text = $rv;
    }

    sub file_tools {
        unless (Project("FILETOOLS_READ")) {
            my $tools_pro = Project("THIRD_DIR")."/tools.pro";
            ScanProject($tools_pro) if (-r $tools_pro);
            Project("FILETOOLS_READ = 1");
        }
        my @utils = qw(SED TAR BZIP2 GZIP ZIP UNZIP CAT PATCH RM RMDIR CHDIR
                       CVS CP COPY PERL PYTHON CPDIR MKDIR TOUCH BISON);
        my $tools = '';
        my @show = @_ ? @_ : split(/\s/, Project("FILETOOLS"));
        @show[0] =~ /^\$/ and @show = split(/\s/, Project(substr(@show[0], 1)));
        foreach $t ($#show + 1 ? @show : @utils) {
            my $val = Project(uc($t));
            tmake_error("$t is undefined!") unless ($val);
            $val =~ s/(?<! )[\\\/]/$dir_sep/g;
            $tools .= $t."\t\t= $val\n";
        }
        return $tools if defined wantarray;
        $text = $tools;
    }
    *ExpandFileTools = *file_tools;

    sub set_env_vars {
        return unless @_;
        my ($template, $rv) = ('', '');
        unless ($is_unix) {
            $template = 'set @VAR@=@VAL@';
        }
        else {
            $template = '@VAR@="@VAL@"; export @VAR@; \\';
        }
        foreach $var (@_) {
            my ($tmp, $val) = ($template, "\$($var)");
            if ($var =~ /([^=]+)=(.*)/) {
                $var = $1;
                $val = $2;
            }
            $tmp =~ s/\@VAR\@/$var/g;
            $tmp =~ s/\@VAL\@/$val/g;
            $rv .= "\t$tmp\n";
        }
        chomp $rv;
        return $rv;
    }

    sub SetEnvVars {
        $text = set_env_vars(@_);
    }

    sub set_env_vars_raw {
        return unless @_;
        my ($template, $rv) = ('', '');
        unless ($is_unix) {
            $template = 'set @VAR@=@VAL@';
        }
        else {
            $template = '@VAR@="@VAL@"; export @VAR@; \\';
        }
        foreach $var (@_) {
            my ($name, $val) = split(/\s*=\s*/, $var, 2);
            my $tmp = $template;
            $tmp =~ s/\@VAR\@/$name/g;
            $tmp =~ s/\@VAL\@/$val/g;
            $rv .= "\t$tmp\n";
        }
        chomp $rv;
        return $rv;
    }

    sub SetEnvVarsRaw {
        $text = set_env_vars_raw(@_);
    }

    sub AddEnvValue {
        return unless @_;
        my $tpl = '';
        unless ($is_unix) {
            $tpl = 'set @VAR@=@VAL1@@SEP@@VAL2@';
        }
        else {
            $tpl = '@VAR@=@VAL1@@SEP@@VAL2@; export @VAR@; \\';
        }
        my $empty = 1;
        foreach (@_) {
            my $spec = $_;
            next unless $spec;
            chomp $spec;
            my $sep = chop($spec);
            my ($var, $val) = split('=', $spec, 2);
            my $tmp = $tpl;

            my $varref = $is_unix ? "\$${var}" : "%$var%";
            my ($val1, $val2) = ($varref, $val);
            ($val1, $val2) = ($val, $varref.($is_unix?'':'%')) if ($val =~ s/\+(.*)/\1/);

            $tmp =~ s/\@VAR\@/$var/g and $empty = 0;
            $tmp =~ s/\@VAL1\@/$val1/g and $empty = 0;
            $tmp =~ s/\@VAL2\@/$val2/g and $empty = 0;
            $tmp =~ s/\@SEP\@/$sep/g and $empty = 0;
            $text .= "\t$tmp\n";
        }
        chomp $text;
        $text = $is_unix ? '@true' : '@rem' if $empty;
    }

    sub ScanProjectRawEx {
        my $expand = shift @_;
        my $rv = {};
        foreach $proj (@_) {
            $ENV{"TMAKE_DEBUG"} and print STDERR "Reading $proj\n";
            next unless open(TMPJ, "<$proj");
            my $line = "";
            while (<TMPJ>) {
                next if /^\s*$/ || /^\s*#.*$/;
                s/^(\s+)//g;
                s/(\s+)$//g;
                $line .= $_;
                next if ($line =~ s/\\$//);
                my ($var, $val) = map { s/^\s+//; s/\s+$//; $_ } split('=', $line, 2);
                $val = expand_by_dict($val, $rv) if $expand;
                $ENV{"TMAKE_DEBUG"} and print STDERR "  Setting $var to '$val'\n";
                $$rv{$var} = $val;
                $line = "";
            }
            close(TMPJ);
        }
        return $rv;
    }

    sub ScanProjectRaw {
        return ScanProjectRawEx(1, @_);
    }

    sub get_lib_info {
        my ($ldir, $lname) = fnsplit(@_[0]);
        return ($ldir, $lname) unless ($ldir);
        my $linfo = expand_path("$ldir/$lname.pro");
        return ("", "") unless -r $linfo;
        my $libvars = ScanProjectRaw($linfo);
        my $lbase;
        ($ldir, $lname, $lbase) = map { 
            my $lv = $$libvars{$_};
            $lv =~ s/^[\s"]+//;
            $lv =~ s/[\s"]+$//;
            $lv;
        } qw/DESTDIR TARGET_FILE TARGET_BASE/;
        return ("", "", "") unless $lname;
        return ($ldir, $lname, $lbase);
    }

    sub set_env_cmd {
	my ($var, $val) = @_;
        if ($is_unix) {
            return "$var=\"$val\"; export $var\n";
	}
        else {
	    return "set $var=$val\n";
	}
    }
    
    sub write_script {
        my $scriptfile = shift;
        my $cmd = shift;
        my ($script, $setenvs);
        foreach (@_) {
            my ($var, $val) = split('=', $_, 2);
            $setenvs .= set_env_cmd($var, $val);
        }
        if ($is_unix) {
            $script .= "\#\!/bin/sh\n\n";
            $script .= "$setenvs\n" if $setenvs;
            $script .= "$cmd\n";
        }
        else {
            $script .= "\@echo off\n\n";
            $script .= "$setenvs\n" if $setenvs;
            $script .= "\n$cmd\n";
        }
        write_file($scriptfile, $script);
        chmod 0755, $scriptfile if $is_unix;
    }

    my $pt = "utils-".platform().".t";
    IncludeTemplate($pt) if (find_template_ex($pt));
#$}
