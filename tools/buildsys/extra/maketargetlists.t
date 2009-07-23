#!
#! 
#!
#${
    my @objlists;
    my ($top_srcdir, $top_blddir) = map { Project($_) } qw(top_srcdir top_builddir);

    foreach $bd (split(/\s+/, Project("BASEDIRS"))) {
        my $bd_pfx = $bd;
        $bd_pfx =~ s/[\\\/]/_/g;
        foreach $d (split(/\s/, Project("${bd_pfx}_MODULES"))) {
            my ($objlst) = find_files(join($dir_sep, $top_blddir, $bd, $d),
                                    'target_objects.lst', 1);
            next unless ($objlst);
            $objlst =~ s/[\\\/]/$dir_sep/g;
            push @objlists, $objlst;
        }
    }
    Project("TARGET_LISTS = " . join(' ', @objlists));
#$}
