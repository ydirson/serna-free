#!
#! Template for pyqt as a Syntext 3rd component
#! 
#!
#!##############################################################################
#${
    IncludeTemplate(Project("third_dir")."/qt/qtdir.t");
    my $qtdir = Project("QT_BUILDDIR");

    Project("PACKAGE_CFG_ENV += QTDIR=$qtdir", "PACKAGE_MAKE_ENV += QTDIR", "QTDIR=$qtdir");
    Project("PACKAGE_CFG_ENV += PATH=%PATH%;%QTDIR%\\bin") unless ($is_unix);
    Project("PACKAGE_MAKE_ENV_ADD += PATH=%QTDIR%\\bin;%PATH%;") unless ($is_unix);
    Project('darwin:PACKAGE_CFG_ENV += DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${QTDIR}/lib');
    Project('PACKAGE_CFG_ENV += PYTHON_EXE='.expand_path(Project('PYTHON')));
    
    if (open(QTCONFIG, ">qtconfig.txt")) {
        print QTCONFIG "$qtdir\n";
        print QTCONFIG "$qtdir${dir_sep}include\n";
        print QTCONFIG "$qtdir${dir_sep}lib\n";
        print QTCONFIG "$qtdir${dir_sep}bin\n";
        print QTCONFIG "$qtdir\n$qtdir${dir_sep}plugins\n";
        print QTCONFIG "PyQt_SessionManager\n";
        print QTCONFIG "PyQt_Accessibility\n" if ("darwin" eq $pf);
        close(QTCONFIG);
    }
#$}
QTDIR = #$ Expand("QTDIR");
