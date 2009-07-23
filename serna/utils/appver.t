#$ Project("SUBDIRS") && DisableOutput();
#${
    unless (Project("SUBDIRS")) {
        Project("CLEAN_FILES += appver.hpp appver.cpp");
        Project("SOURCES += appver.cpp");
    }
    unless (Project("APPVER_FILE")) {
	Project('APPVER_FILE = $(srcdir)'.$dir_sep.'APPVER');
    }
#$}
#$ DisableOutput() if Project("PYTHON");
PYTHON         = #$ $text = '$(THIRD_DIR)/bin/python'; $text =~ s-[\\\/]-$dir_sep-g;
#$ EnableOutput() if Project("PYTHON");
APPVER_FILE    = #$ Expand("APPVER_FILE");
APPVER_SCRIPT  = #$ $text = '$(top_srcdir)/tools/buildsys/appver.py'; $text =~ s-\/-$dir_sep-g;
APPVER_MODULE  = #$ Expand("TARGET");
APPVER_INFO    = appverinfo.txt

$(APPVER_INFO): force
	$(PYTHON) $(APPVER_SCRIPT) $@ $(APPVER_FILE)

appver.hpp appver.cpp: $(APPVER_INFO)
#$ DisableOutput() unless $is_unix;
	$(PYTHON) $(APPVER_SCRIPT) $(basename $@) $(APPVER_INFO) $(APPVER_MODULE)
#$ EnableOutput() unless $is_unix;
#$ DisableOutput() if $is_unix;
	$(PYTHON) $(APPVER_SCRIPT) $(*B) $(APPVER_INFO) $(APPVER_MODULE)
#$ EnableOutput() if $is_unix;

all: appver.hpp appver.cpp
#$ Project("SUBDIRS") && EnableOutput();
