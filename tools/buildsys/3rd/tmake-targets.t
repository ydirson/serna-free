#${
	my $target = Project( "TARGET") . Project( "TARGET_EXT");

	$target =~ m-([^\Q$dir_sep\E]+)$-;
	if( defined( $1)) {
		$target = $1;
	}

	if( Project( "TMAKE_APP_FLAG")) {
		Project( "BIN_TARGETS = $target") unless Project( "BIN_TARGETS");
	}
	else {
        if( !Project( "LIB_TARGETS")) {
    		if( $target =~ /\.dll$/) {
	    		$target .= " " . $target;
		    	$target =~ s/\.dll$/\.lib/;
    		}
	    	elsif( 0 != $is_unix && Config("dll")) {
		    	foreach $p ( '_', '_x', '_x.y') {
			    	my $tgt = "TARGET$p";
				    $target .= " "  . Project( $tgt) if( Project( $tgt));
    			}
	    	}
    		Project( "LIB_TARGETS = $target");
        }
	}
#$}
