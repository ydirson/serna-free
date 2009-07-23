#$ DisableOutput()
#
# Syntext Editor special processing template for win32 (msvc)
# 
#
#$ EnableOutput()
#${
    my ($pdb_name, $objdir) = (Project("TARGET"), Project("OBJECTS_DIR"));
    $pdb_name = $objdir . $dir_sep . $pdb_name if($objdir);

    my ($CxxFlags, $CFlags);

    $CxxFlags .= " -EHsc" unless Config("no-exception");
    $CxxFlags .= " -GR" unless Config("no-rtti");

    if (Config("debug")) {
        $CFlags .= " -MDd -Zi -Od -Fd\"$pdb_name\"";
    }
    else {
        $CFlags .= " -MD -O2 -Zi"; #!.(Config("static") ? "-MT" : "-MD");
    }

    $CxxFlags .= $CFlags . ' $(CXX_ADD)';
    $CFlags .= " -TC";

    Project("TMAKE_CFLAGS += $CFlags");
    @TmpOptList = split(/\s+/, $CxxFlags);
    push @TmpOptList, split(/\s+/, Project("TMAKE_CXXFLAGS"));
    (@warn_opts, @warn_level, @cxx_opt_list) = ((), (), ());
    foreach $opt (@TmpOptList) {
        if ($opt =~ m/^[\/\-][Ww]\d$/) {
            push @warn_level, $opt;
        }
        elsif ($opt =~ m/^[\/\-][Ww]\d{2,}$/) {
            push @warn_opts, $opt;
        }
        else {
            push @cxx_opt_list, $opt;
        }
    }
    push @warn_level, @warn_opts;
    $CxxFlags = join(' ', @cxx_opt_list, @warn_level); #! . ' ' . join(' ', @warn_level);
    Project("TMAKE_CXXFLAGS    = $CxxFlags");

    my ($mod, $maj) = (Project("VER_MAJOR"), Project("VER_MINOR"));
#!    Project("TMAKE_LFLAGS *= -debug -pdb:" . Project("TARGET") . "$mod$maj.pdb");
    Project('TMAKE_LFLAGS *= -debug -pdb:$(@R).pdb');
    Project("TMAKE_LFLAGS *= -version:$mod.$maj");
    Project("TMAKE_LFLAGS *= -release") if Config("release");
    Project("CONFIG *= windows");

    foreach $prefix (CFLAGS, CXXFLAGS) {
        foreach $postfix (MT, MT_DBG, MT_DLL, MT_DLLDBG, RELEASE, DEBUG) {
            Project("TMAKE_$prefix" . "_$postfix = ");
        }
    }

    my $pf = Project("PLATFORM_DESTDIRS") eq "yes" ? platform().'\\' : "";
    my ($lflags, $libs, $syslibs, $deplibs) =
        process_libs(Project("LIBS"), $pf);

    Project("DEP_LIBS = " . join(' ', @$deplibs));

    if (Config("app") || Config("dll")) {
        my @applibs = ("msvcrt", "msvcprt");
        grep { $_ .= "d" } (@applibs) if (Config("debug"));
        push @applibs, "oldnames";
        grep { $_ .= ".lib" } (@applibs);
        push @$syslibs, split(/\s/, Project("SYS_LIBS"));
        push @$libs, (@applibs, @$syslibs);
    }
#!    grep { print STDERR "lib: $_\n" } @$libs;
    if (Config("app")) {
        push @$libs, split(/\s/, Project("THR_LIBS")) if (Config("mt"));
        Project('TMAKE_LFLAGS += $$APP_FLAGS');
    }
    Project("LIBS = " . join(" ", @$libs));
#$}
