#!
#! Template for sip as a Syntext 3rd component
#!
#!
#!##############################################################################
#${
    IncludeTemplate("3rd/pkg-utils");
    IncludeTemplate("pkg-info");

    my $third_dir = expand_path(Project("THIRD_DIR"));

    my %package = ( NAME => 'sip', SIP => normpath("$third_dir/bin/sip") );
    if (Config("syspkg") || Config("syspkgonly")) {
        my @pyinc = split /\s+/, get_package_info('python', 'INCLUDES');
        my ($sip_h, $incdir, $incsubdir) = find_file_in_path('sip.h', @pyinc);
        if (!$sip_h) {
            tmake_error("Can't find python package") if Config("syspkgonly");
        }
        else {
            $package{'INCLUDES'} = $incsubdir;
            write_script("$third_dir/bin/sip", 'exec sip "$@"');
            write_package("$third_dir/lib/sip.pkg", \%package);
            write_file("$third_dir/sip/MANIFEST", '');
            return;
        }
    }

    IncludeTemplate("$third_dir/qt/qtdir.t");
    Project('PACKAGE_CFG_ENV *= QTDIR=$$QT_BUILDDIR');

    my $pymodext = $is_unix ? 'so' : 'pyd';
    my $mft = <<"EOF";
data:\${inst_prefix}/\${serna}/plugins/pyplugin:\${THIRD_DIR}/lib
    sip.$pymodext
EOF
    write_file("$third_dir/sip/MANIFEST", $mft);
#$}
