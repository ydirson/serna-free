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

    my $script;
    my ($pkg_dir, $third_dir, $top_srcdir, $version) = map {
        Project($_)
    } (qw/PACKAGE_DIR THIRD_DIR top_srcdir VERSION/);
    my $java_home = normpath("$top_srcdir/$version/$pkg_dir/jre");
    my $java = get_package_info('java', 'JAVA');
    if ($is_unix) {
        write_script($java, '${JAVA_HOME}/bin/java "$@"', "JAVA_HOME=$java_home");
    }
    else {
        write_script("$java.bat", '%JAVA_HOME%\\bin\\java %*', "JAVA_HOME=$java_home");
    }
#$}
