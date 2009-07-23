#!
#!`
#! This template removes default CONFIG options taken from platform's tmake.conf
#! and sets project name to the basename of current directory
#!
#${
    my $config = Project("CONFIG");
    if ($config =~ /[,;]/) {
        $config =~ s/^[^,;]+\s+(?=\w+[,;])//;
        $config =~ s/[\s,;]+/ /g;
    }
    else {
        $config =~ s/^qt warn_on release//;
    }
    Project("CONFIG = $config");

    my $cwd = getcwd();
    $cwd =~ m-(.*\Q$dir_sep\E)([^\Q$dir_sep\E]+$)-;
    Project("PROJECT = $2") unless $2 eq "";

    my $makefile = Project("OUTFILE");
    Project("MAKEFILE = $outfile");
    my $defaults = normpath(Project("defaults"));
    Project("defaults = $defaults");
    my $loc_defaults = normpath(Project("local_defaults"));
    Project("local_defaults = $loc_defaults");
    if (!Project("TMAKE")) {
        Project("TMAKE = " . normpath(fullpath($0)));
    }
#!    print STDERR Project("TMAKE")."\n";
#$}
