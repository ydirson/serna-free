#!
#! Syntext Editor special processing template for unices
#! 
#!
#${
    my ($CxxFlags, $CFlags);
    my $platform = Project("PLATFORM");
    my $gpp = Project("COMPILER") eq "g++";

    if ($gpp) {
        $CxxFlags .= " -fno-exceptions" if Config("no-exception");
        $CxxFlags .= " -fno-rtti" if Config("no-rtti");
    }
    Config("mt") && Project("CONFIG *= thread");

    $CxxFlags .= $CFlags . ' $(CXX_ADD)';
    Project("TMAKE_CXXFLAGS += $CxxFlags");

    my $pf_libdir = (Project("PLATFORM_DESTDIRS") eq "yes") ? $dir_sep . $platform : "";
    my ($lpflags, $libs, $syslibs, $deplibs) = process_libs(Project("LIBS"));
    Project("TMAKE_LFLAGS *= -L" . join(' -L', @$lpflags)) if (@$lpflags);

    my $darwin = ("darwin" eq $platform);
    if (Config("app")) {
        my @slibparts = split(/\s/, Project("SYS_LIBS"));
        while ($_ = shift @slibparts) {
            if (m/^--?.*$/) {
                $_ .=  ' '.shift(@slibparts) if $darwin && "-framework" eq $_;
            }
            push @$syslibs, $_;
        }
        push @$syslibs, split(/\s/, Project("THR_LIBS")) if (Config("mt"));
        Project('TMAKE_LFLAGS += $$APP_LFLAGS');
    }
    if ($darwin and $gpp and Config("debug")) {
        if (Config("dll") or Config("app")) {
            Project('TMAKE_LFLAGS *= -Wl,-undefined,dynamic_lookup');
            Project("TMAKE_CXXFLAGS *= -fimplement-inlines");
        }
    }

    Project('TMAKE_LFLAGS += $(LD_ADD)');
    push @$libs, @$syslibs;
    foreach (@$libs) {
        if (!/^\s*$/) {
            if (/^(--?.*)$/) {
                $_ = ($darwin && /^-framework\s+/) ? $_ : "-Wl,$1";
                next;
            }
            $_ = "-l$_";
        }
    }
    Project("LIBS = " . join(' ', @$libs)) if(@$libs);
    Project("DEP_LIBS = " . join(' ', @$deplibs));
#$}
