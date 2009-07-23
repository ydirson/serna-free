#${
	sub install_data {
		my ($item, $srcdir, $dstdir) = @_;
		$srcdir =~ s-[\\/]-$dir_sep-g;
		$dstdir =~ s-[\\/]-$dir_sep-g;
		my %ins = (
			srcdir => $srcdir,
			dstdir => $dstdir,
			files => [],
			dirs => []
		);
		foreach (split(/\s+/, Project("${item}_FILES"))) {
			push @{$ins{'files'}}, [$_, $_];
		}
		foreach (split(/\s+/, Project("${item}_DIRS"))) {
			push @{$ins{'dirs'}}, $_;
		}
		foreach (split(/;/, Project("${item}_EXEC"))) {
			push @{$ins{'exec'}}, $_;
		}
		return \%ins;
	}
#$}
#${
	if(Project("DATA")) {
        foreach (split(/\s+/, Project("DATA"))) {
            my $destdir = Project("$_" . "_DESTDIR");
            $destdir = Project("DATA_DESTDIR") unless($destdir);
            $destdir = '.' unless($destdir);
                    
            my $srcdir = Project("$_" . "_SRCDIR");
            $srcdir = Project("DATA_SRCDIR") unless($srcdir);

            $install{$_} = install_data($_, $srcdir, $destdir);
        }
	Project("INSTALL_VARS += DATA_DESTDIR DATA_SRCDIR");
    }
#$}
