#${
    Project("PACKAGE_MAKE") || Project("PACKAGE_MAKE = " . Project("MAKE"));
    Project("MAKE_DIR") || Project("MAKE_DIR = " . Project("PACKAGE_DIR"));
    my $fpath = fullpath(expand_path(Project("MAKE_DIR")));
    my $cwd = getcwd();
    my $reldir = relative( $cwd, $fpath);
    Project("PACKAGE_REL_DIR = $reldir");
#$}
PACKAGE_MAKE            = #$ Expand( "PACKAGE_MAKE");
PACKAGE_MAKEFILE        = #$ Expand( "PACKAGE_MAKEFILE");
PACKAGE_MAKEFILEOPT     = #$ Expand( "PACKAGE_MAKEFILEOPT");
PACKAGE_CLEANOPTS       = #$ Expand( "PACKAGE_CLEANOPTS");
PACKAGE_MAKEOPTS        = #$ Expand( "PACKAGE_MAKEOPTS");
PACKAGE_TARGET          = #$ Expand( "PACKAGE_TARGET");
PACKAGE_REL_DIR         = #$ Expand( "PACKAGE_REL_DIR");

MAKE_DIR                = #$ Expand( "MAKE_DIR");
