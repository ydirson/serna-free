#${
    my @respath = ();
    my $plat_inc = join('-', map(Project("$_"), "CPU", "PLATFORM", "COMPILER"));
    $plat_inc = "universal-darwin" if (Config("darwin") and Config("universal"));
    push @respath, Project("PLAT_INCPATH_BASE") . $dir_sep . $plat_inc;

    my $incpath = Project("INCLUDEPATH");
    my $sep = ($incpath =~ m/;/) ? ';' : '\s+';
    my %hpath = ();
    foreach $part (split(/$sep/, $incpath)) {
        $part =~ s/^\s*//;
        push @respath, $part unless($hpath{$part});
        $hpath{$part} = 1;
    }
    $incpath = join('; ', @respath);
    Project("INCLUDEPATH = $incpath");
    while($incpath =~ s^\$\((\w*)\)^$project{$1}^g) {}
    while($incpath =~ s^$dir_sep[\w]+$dir_sep\.\.^^g) {}
    Project("DEPENDPATH += $incpath");
#$}
