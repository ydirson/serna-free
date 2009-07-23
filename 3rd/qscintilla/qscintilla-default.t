#!
#! Template for qscintilla as a Syntext 3rd component
#! 
#!
#!##############################################################################
#${
    IncludeTemplate(Project("third_dir")."/qt/qtdir.t");
    my $qtdir = Project("QT_BUILDDIR");

    Project("PACKAGE_CFG_ENV += QTDIR=$qtdir");
    Project("PACKAGE_CFG_ENV += PATH=%PATH%;%QTDIR%\\bin") unless ($is_unix);
    Project('darwin:PACKAGE_CFG_ENV += DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${QTDIR}/lib');
    $text .= "\nQTDIR = $qtdir\n";
    Project("INSTALL_ENV_VARS *= QTDIR");
#$}
