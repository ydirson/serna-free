#!
#! Template for qscintilla as a Syntext 3rd component
#!
#!
#!##############################################################################
#${
    IncludeTemplate("3rd/pkg-utils");
    my $third_dir = Project("THIRD_DIR");

    my %package = ( NAME => 'qscintilla' );
    if (Config("syspkg") || Config("syspkgonly")) {
        my $pkg = find_package_by_files("Qsci/qsciscintilla.h", "qscintilla2");
        if (!$pkg) {
            tmake_error("Can't find qscintilla package") if Config("syspkgonly");
        }
        else {
            grep { $package{$_} = $pkg->{$_} } (keys %{$pkg});
            write_package("$third_dir/lib/qscintilla.pkg", \%package);
            write_file("$third_dir/qscintilla/MANIFEST", "");
            Project("TMAKE_TEMPLATE=");
            return;
        }
    }

    write_file("$third_dir/qscintilla/MANIFEST", "include:MANIFEST.qscintilla");
    IncludeTemplate("$third_dir/qt/qtdir.t");
    my $qtdir = Project("QT_BUILDDIR");

    Project("PACKAGE_CFG_ENV += QTDIR=$qtdir");
    Project("PACKAGE_CFG_ENV += PATH=%PATH%;%QTDIR%\\bin") unless ($is_unix);
    Project('darwin:PACKAGE_CFG_ENV += DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${QTDIR}/lib');
    $text .= "\nQTDIR = $qtdir\n";
    Project("INSTALL_ENV_VARS *= QTDIR");
#$}
