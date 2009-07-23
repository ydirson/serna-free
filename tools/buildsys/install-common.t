#${
    if(0 == $is_unix) {
        Project("INSTALL = copy") unless Project("INSTALL");
        Project("INSTALL_OPTS = /b") unless Project("INSTALL_OPTS");
        Project("INSTALL_DIR = xcopy") unless Project("INSTALL_DIR");
        Project("INSTALL_DIR_OPTS = /q/e/c/h/k/y/r/i") unless Project("INSTALL_DIR_OPTS");
        Project("CLEAN = del") unless Project("CLEAN");
        Project("CLEAN_DIR = rmdir") unless Project("CLEAN_DIR");
        Project("CLEAN_DIR_OPTS = /s /q") unless Project("CLEAN_DIR_OPTS");
        Project("MKDIR = mkdir") unless Project("MKDIR");
    }
    else {
        Project("INSTALL = ln") unless Project("INSTALL");
        Project("INSTALL_OPTS = -sf") unless Project("INSTALL_OPTS");
        Project("INSTALL_DIR = ln") unless Project("INSTALL_DIR");
        Project("INSTALL_DIR_OPTS = -sf") unless Project("INSTALL_DIR_OPTS");
        Project("CLEAN = rm") unless Project("CLEAN");
        Project("CLEAN_DIR = rm") unless Project("CLEAN_DIR");
        Project("CLEAN_DIR_OPTS = -rf") unless Project("CLEAN_DIR_OPTS");
        Project("MKDIR = mkdir -p") unless Project("MKDIR");
        if(Project("PLATFORM") eq 'linux') {
          Project("INSTALL_DIR_OPTS += -d");
        }
        Project("CLEAN_OPTS = -f") unless Project("CLEAN_OPTS");
    }
    Project("INSTALL_VARS *= INSTALL INSTALL_OPTS INSTALL_DIR INSTALL_DIR_OPTS");
    Project("INSTALL_VARS *= CLEAN CLEAN_OPTS CLEAN_DIR CLEAN_DIR_OPTS MKDIR");
    foreach (split /[\s,;]+/, Project("INSTALL_VARS")) {
        $text .= "$_\t\t= " . Project($_) . "\n";
    }
#$}
#${
    sub dumpitem {
        my ($item,$iref) = @_;
        print STDERR "------------------------------------\n";
        print STDERR "$item\n";
        print STDERR "SRCDIR: ", $$iref{srcdir}, "\n";
        print STDERR "DSTDIR: ", $$iref{dstdir}, "\n";
        foreach $ref (@{$$iref{files}}) {
            print STDERR "FILE: ", @$ref[0] . " -> " . @$ref[1] . "\n";
        }
        foreach (@{$$iref{dirs}}) {
            print STDERR "DIR: ", "$_\n";
        }
        print STDERR "------------------------------------\n";
    }
#!  Output install/clean info
    foreach $item (keys %install) {
        my $iref = $install{$item};
        my $srcdir = $$iref{srcdir};
        my $dstdir = $$iref{dstdir};
        my $execstr = $$iref{'exec'};
        next unless($dstdir); #! won't be making dirs in root!
#!		dumpitem($item,$iref);
    #!  if it's unix let's check if we're making links
        my $reldir = $srcdir;
        if(Project("INSTALL") =~ /^\s*ln\s*$/ && $is_unix) {
            $reldir = relative(normpath(expand_path($srcdir)), normpath(expand_path($dstdir)));
#!            print STDERR normpath(expand_path($srcdir)), "\n", normpath(expand_path($dstdir)), "\n";
#!	     	print STDERR Project("INSTALL"), ", S: $srcdir, D: $dstdir, R: $reldir\n";
        }
        $reldir .= $dir_sep if $reldir;

    #!  Install targets

        if(@{$$iref{files}} || @{$$iref{dirs}} || @{$$iref{exec}}) {
            $text .= "${item}_install:\n";
            if(0 == $is_unix) {
	            $text .= "\tif not exist $dstdir";
            }
            else {
            	$text .= "\t[ -d $dstdir ] ||";
            }
            $text .= " \$(MKDIR) $dstdir\n";
        }
        foreach $fileref (@{$$iref{files}}) {
            if(@$fileref[0]) {
                @$fileref[0] =~ s-[\\\/]-$dir_sep-g;
                if(@$fileref[0] =~ /[\*\?]/) {
                    @$fileref[1] = '';
                }
                elsif(!@$fileref[1]) {
                    @$fileref[1] = @$fileref[0];
                }
                if(@$fileref[1] =~ /[\\\/]/) {
                    @$fileref[1] =~ s-^.*[\\\/]--;
                }
                $text .= "\t";
                my $fdstdir = $dstdir . $dir_sep;
                if(0 != $is_unix) {
                    $text .= "(cd $dstdir; ";
                    $fdstdir = ".$dir_sep";
                }
                @$fileref[0] =~ s-[\\\/]-$dir_sep-;
                $text .= "\$(INSTALL) \$(INSTALL_OPTS) $reldir" . @$fileref[0] . " $fdstdir" . @$fileref[1];
                $text .= ")" if(0 != $is_unix);
                $text .= "\n";
            my $cleanfile = @$fileref[1] ? @$fileref[1] : @$fileref[0];
                push @{$$iref{'clean'}}, "$dstdir$dir_sep" . $cleanfile;
            }
        }
        if($execstr) {
            if(0 != $is_unix) {
                $text .= "\t(cd $dstdir; $execstr)\n";
            }
            else {
                my $cwd = getcwd();
                $text .= "\tcd $dstdir\n\t$execstr\n\tcd $cwd\n";
            }
        }
        foreach $dir (@{$$iref{dirs}}) {
            if($dir) {
                $dir =~ m-[\\\/]- && tmake_error("Directories to install must not contain dir_seps\n");
                my $fulldir = $dstdir.$dir_sep.$dir;
#!                $fulldir .= $dir unless Project("INSTALL") =~ /^\s*ln\s*$/ && $is_unix;
                $text .= "\t\$(INSTALL_DIR) \$(INSTALL_DIR_OPTS) $reldir$dir $fulldir\n";
                push @{$$iref{'clean'}}, $fulldir.$dir_sep;
            }
        }
        foreach (@{$$iref{exec}}) {
            $text .= "\t$_\n";
        }
        $text .= "\n";

    #!  Clean targets

        if(@{$$iref{'clean'}}) {
            push @cleans, $item;
            $text .= "${item}_clean:\n";
            foreach (@{$$iref{'clean'}}) {
                if($_ =~ s-[\\/]$--) {
                    $text .= "\t\$(CLEAN_DIR) \$(CLEAN_DIR_OPTS) $_\n";
                }
                else {
                    $text .= "\t\$(CLEAN) \$(CLEAN_OPTS) $_\n";
                }
            }
            $text .= "\n";
        }
    }

    #! handle-them-all targets

    $text .= "install: all";
    if(@cleans) {
        foreach (@cleans) {
            $text .= " \\\n\t${_}_install";
        }
        $text .= "\n\n";

        $text .= "installclean:";
        foreach (@cleans) {
            $text .= " \\\n\t${_}_clean";
        }
        $text .= "\n";
    }
    $text .= "\n";
#$}
