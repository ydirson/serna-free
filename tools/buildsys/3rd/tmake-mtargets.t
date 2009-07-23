#${
	foreach (split /\s+/, Project( "MAKEFILES")) {
		/(\w+)$/;
		Project( "TARGETS += $1");
	}

   	my $pkg_makefile = Project( "PACKAGE_MAKEFILE");
    *SAVESTDOUT = *STDOUT;
   	open MAKEFILE, ">$pkg_makefile" || tmake_error( "3rd/mtargets.t: Can't open '$pkg_makefile' for writing");
   	*STDOUT = *MAKEFILE;
#$}

MAKE 	= #$ Expand( "MAKE");
TARGETS	= #$ Expand( "TARGETS");

all: $(TARGETS)

#${
	foreach ( split /\s+/, Project( "MAKEFILES")) {
		/(\w+)$/;
		$text .= "$1: FORCE\n\t\$(MAKE) -f $_\n\n";
	}
#$}

clean install: FORCE
#${
	foreach ( split /\s+/, Project( "MAKEFILES")) {
		/(\w+)$/;
		$text .= "\t\$(MAKE) -f $_ \$@\n";
	}
#$}

FORCE:
#${
	close MAKEFILE;
	*STDOUT = *SAVESTDOUT;
#$}
