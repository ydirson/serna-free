#${
	my @src_projects = split /[,; ]/, Project( "SRC_PROJECT");

	if( @src_projects > 1) {
		Project( "MTARGETS = 1");
		my ( $bin_targets, $lib_targets);
		my $makefiles = "";
		my %save_project = %project;
		foreach ( @src_projects) {
			%project = %save_project;
			$_ =~ s-\.pro$--;
			Project( "SRC_PROJECT = $_");
   			IncludeTemplate( "3rd/tmake-project");
			$bin_targets .= " " . Project( "BIN_TARGETS");
			$lib_targets .= " " . Project( "LIB_TARGETS");
			$makefiles .= Project( "MTARGET_MAKEFILE") . " ";
		}
		Project( "MAKEFILES = $makefiles");
		IncludeTemplate( "3rd/tmake-mtargets");
		Project( "BIN_TARGETS = $bin_targets");
		Project( "LIB_TARGETS = $lib_targets");
	}
	elsif( Project( "SUBDIRS")) {
		IncludeTemplate( "3rd/tmake-subdirs");
	}
	else {
    	my $src_proj = $src_projects[0];
    	$src_proj =~ s-\.pro$--;
    	Project( "SRC_PROJECT = $src_proj");
    	IncludeTemplate( "3rd/tmake-project");
	}

  	IncludeTemplate( "3rd/tmake-leaf") if( !Project( "SUBDIRS"));
#$}
