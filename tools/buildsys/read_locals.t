#!
#! 
#!
#${
    my $localpro = Project("srcdir").$dir_sep."Makefile.local.pro";
    -f $localpro && ScanProject($localpro);
#$}
