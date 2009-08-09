#${
    Project("FILETOOLS = PYTHON BZIP2 TAR"); 
    IncludeTemplate("filetools");
    Project("GREP=grep");
    my ($arch_dir, $arch_base) = fnsplit(Project("ARCHIVE"));
    Project("ARCHIVE_LIST = \$(srcdir)$dir_sep$arch_base.lst");

    my @vars = qw(
        top_srcdir 
        main_srcdir 
        srcdir 
        THIRD_DIR
        PYTHON
        MAKE_DIR
        JRE_LST
        JRE_EXCLUDES
        MANIFEST
        ARCHIVE
        ARCHIVE_LIST
        TAR
        GREP
        RM
        BZIP2
        LST2MFT
        VERSION
    );
    my %nopaths = ( RM => 1, MAKE_DIR => 1 );

    my $makefile = '';
    my $var = '';
    my $eq = $is_unix ? ':=' : '=';
    foreach (@vars) {
        $var = Project($_);
        $var = normpath($var) unless defined $nopaths{$_};
        $makefile .= "$_".(' ' x (15 - length($_)))."$eq $var\n";
    }
    $makefile .= $is_unix ? "\n" : "\n!";
    $makefile .= "include \$(srcdir)/Makefile.common\n";

    write_file('JdkMakefile', $makefile);
    
    IncludeTemplate("3rd/pkg-utils");
    if (Config("syspkg") || Config("syspkgonly")) {
        IncludeTemplate("java-system");
    }
    else {
        IncludeTemplate("java-included");
    }
#$}
