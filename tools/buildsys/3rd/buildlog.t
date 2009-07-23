#$ DisableOutput() if (Project("BUILDLOG_T_INCLUDED"));
#!
BUILD_LOGFILE = $(top_srcdir)#$ $text = $dir_sep."build.log";
BUILD_LOG     = > $(BUILD_LOGFILE) 2>&1 || (echo Build failed in $(top_builddir), see $(BUILD_LOGFILE) for details; exit 111)

#$ EnableOutput() if (Project("BUILDLOG_T_INCLUDED")); Project("BUILDLOG_T_INCLUDED = 1")
