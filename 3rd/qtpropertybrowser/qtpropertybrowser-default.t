#!
#! Template for qscintilla as a Syntext 3rd component
#!
#!
#!##############################################################################
#${
    my ($third_dir, $srcdir) = map {
        Project($_)
    } (qw/THIRD_DIR srcdir/);

    if (!Config("syspkg") && !Config("syspkgonly")) {

        IncludeTemplate("$third_dir/qt/qtdir.t");
        my $qtdir = Project("QT_BUILDDIR");

        Project("PACKAGE_CFG_ENV += QTDIR=$qtdir");
        Project("PACKAGE_CFG_ENV += PATH=%PATH%;%QTDIR%\\bin") unless ($is_unix);
        Project('darwin:PACKAGE_CFG_ENV += DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${QTDIR}/lib');
        $text .= "\nQTDIR = $qtdir\n";
        Project("INSTALL_ENV_VARS *= QTDIR");
        Project('PACKAGE_CFG_OPTIONS += $$QTSPEC');
        print STDERR Project("PACKAGE_CFG_OPTIONS"), "\n";
        print STDERR Project("QTSPEC"), "\n";

        my $custom_pri = <<'EOF';
QMAKE_INCDIR_QT = $(QTDIR)/include
QMAKE_LIBDIR_QT = $(QTDIR)/lib
QMAKE_RCC       = $(QTDIR)/bin/rcc
QMAKE_MOC       = $(QTDIR)/bin/moc
QMAKE_UIC       = $(QTDIR)/bin/uic
win32:QMAKE_MOC       = $(QTDIR)\bin\moc
win32:QMAKE_UIC       = $(QTDIR)\bin\uic
EOF
        write_file("$srcdir/custom.pri", $custom_pri);
    }
    else {
        Project("PACKAGE_CONFIGURE=qmake");
        Project("PACKAGE_MAKEOPTS");
        write_file("$srcdir/custom.pri", '');
    }
    write_file("$third_dir/qtpropertybrowser/MANIFEST", 'include:MANIFEST.qtpropertybrowser');
#$}
