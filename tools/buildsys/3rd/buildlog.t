#${
    unless (Project("BUILDLOG_T_INCLUDED")) {
        Project('CLEANFILES += $(BUILD_LOGFILE)')
    }
    else {
        DisableOutput() if (Project("BUILDLOG_T_INCLUDED"));
    }
#$}
#!
BUILD_LOGFILE = $(top_srcdir)#$ $text = $dir_sep."build.log";
BUILD_LOG     = 

#$ EnableOutput() if (Project("BUILDLOG_T_INCLUDED")); Project("BUILDLOG_T_INCLUDED = 1")
