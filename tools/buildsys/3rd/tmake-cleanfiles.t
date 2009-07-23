#${
	my $destdir = Project( "DESTDIR");
	my $clean_file = Project( "TARGET");
	my $clean_files = "";
	if( 0 == $is_unix) {
		$clean_file = $destdir . $dir_sep . $clean_file;
		if( !Config( "app")) {
    		if( Config( "dll")) {
    		    my $lversion = Project( "VERSION");
    		    $lversion =~ s/\.//g;
    		    $clean_file .= $lversion;
    		    foreach ( dll, 'exp', lib) {
	    			$clean_files .= $clean_file . ".$_ ";
	    		}
        	}
        	else {
    			$clean_files .= $clean_file . ".lib";
        	}
    	}
    	else {
    		$clean_files .= $clean_file . ".exe";
    	}
   		foreach ( ilk, pdb) {
   			$clean_files .= " " . $clean_file . ".$_";
   		}
	}
	else {
		if( !Config( "app")) {
			$clean_files = $destdir . $dir_sep . "lib" . $clean_file. ".so*";
		}
		else {
			$clean_files = $destdir . $dir_sep . $clean_file;
		}
	}
	Project( "CLEAN_FILES += $clean_files");
#$}
