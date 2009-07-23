#!
#! Template for sip as a Syntext 3rd component
#! 
#!
#!##############################################################################
#${
    IncludeTemplate(Project("third_dir")."/qt/qtdir.t");

    Project('PACKAGE_CFG_ENV *= QTDIR=$$QT_BUILDDIR');
#$}
