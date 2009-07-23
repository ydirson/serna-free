#!
#!	Sets target directories for projects with single type targets (either lib or app)
#!
#${
	my $top_builddir = Project( "top_builddir");
	if( !Project( "PACKAGE_DESTDIR")) {
		Project( "PACKAGE_DESTDIR = $top_builddir");
	}
	my $pkg_destdir = Project( "PACKAGE_DESTDIR");
	if( !Project( "PACKAGE_BIN_DESTDIR")) {
		Project( "PACKAGE_BIN_DESTDIR = $pkg_destdir/bin");
	}
	if( !Project( "PACKAGE_LIB_DESTDIR")) {
		Project( "PACKAGE_LIB_DESTDIR = $pkg_destdir/lib");
	}
	
	foreach ( PACKAGE_BIN_DESTDIR, PACKAGE_DESTDIR, PACKAGE_LIB_DESTDIR, BIN_DESTDIR, LIB_DESTDIR) {
		my $t = Project( "$_");
		$t =~ s+[\/\\]$++; $t =~ s+[\/\\]+$dir_sep+g;
		Project( "$_ = $t");
	}

	my $d;
   	if( Config( "app")) {
   		$d = Project( "PACKAGE_BIN_DESTDIR");
   	}
   	else {
   		$d = Project( "PACKAGE_LIB_DESTDIR");
   	}
   	Project( "DESTDIR = $d");
    
	foreach ( LIB_DESTDIR, BIN_DESTDIR) {
		if( !Project( $_)) {
			my $t = Project( "PACKAGE_" . $_);
	        Project( "$_ = $t");
		}
	}
#$}
