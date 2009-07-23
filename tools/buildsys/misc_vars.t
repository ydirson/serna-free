#$ DisableOutput();
#
# 
# This template sets miscellaneous system-dependent variables.
#
#$ EnableOutput();
#${     
    my $pj_file = Project("PROJECT_FILE");
    if (!$pj_file) {
        $pj_file = Project("PROJECT");
        $pj_file .= ".pro" unless $pj_file =~ /\.pro$/;
        Project("PROJECT_FILE = $pj_file");
    }
    Project("MAKEFILE = Makefile") if (!Project("MAKEFILE"));
    Project("MODULE_NAME = $$TARGET") unless (Project("MODULE_NAME"));
#$}

TMAKEPATH       = #$ ExpandWithVars("TMAKEPATH", qw(srcdir top_srcdir));
MAKEFILE        = #$ Expand("MAKEFILE");
MAKE            = #$ Expand("MAKE");
TMAKE           = #$ ExpandWithVars("TMAKE", 'top_srcdir');
TMAKE_OPTS      = #$ Expand("TMAKE_OPTS");
PROJECT_FILE    = #$ Expand("PROJECT_FILE");
MODULE_NAME     = #$ Expand("MODULE_NAME");
