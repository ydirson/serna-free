#${
    my $pj_defaults = Project( "PACKAGE_DEFAULTS");
    if( $pj_defaults) {
		ScanProject( find_template( $pj_defaults));
    }

   	my $pkg_makefile = Project( "PACKAGE_MAKEFILE");

	tmake_error( "3rd/tmake-subdirs.t: Can't make makefile ''") if( !$pkg_makefile);

    *SAVESTDOUT = *STDOUT;
   	open MAKEFILE, ">$pkg_makefile" || tmake_error( "Can't open '$pkg_makefile' for writing");
   	*STDOUT = *MAKEFILE;

   	my $save_makefile = Project( "MAKEFILE");
   	Project( "MAKEFILE = $pkg_makefile");
   	IncludeTemplate( "superdir");
   	Project( "MAKEFILE = $save_makefile");

	close MAKEFILE;
	*STDOUT = *SAVESTDOUT;
#$}
