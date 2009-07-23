#!
#! Template for python as a Syntext 3rd component
#! 
#!
#!##############################################################################
#${
    if (0 == $is_unix) {
    	my $ver = Project("VERSION");
    	my $ver_string = $ver;
    	$ver_string =~ s/\.//g;

    	my $debug_postfix = Config("debug") ? "_d" : "";
    	my $py_base = "python" . $ver_string . $debug_postfix;

    	Project("LIB_TARGETS = $py_base.lib", "LIB_TARGETS += $py_base.dll");
    	Project("BIN_TARGETS = python$debug_postfix.exe");
    	Project("BIN_TARGETS_INST = python.exe");
    }
#$}
