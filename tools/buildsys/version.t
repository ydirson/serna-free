#$ DisableOutput() if Config("noversion");
#${
    return if Config("noversion");
    unless (Project("SUBDIRS") || !(Config("app") || Project("APPVER_FILE"))) {
        my $appver = Project("APPVER");
        $text = "APPVER\t\t= $appver\n";
        $appver =~ s/\.[^\.]+$//;
        $text .= "VERSION_ID\t= $appver\n";
        Project("VERSION_ID = $appver");
    }
#$}
#
#  Version information template
#
#${
    return if Config("noversion");
    unless (Project("SUBDIRS") || Config("staticlib")) {
        Project("CLEAN_FILES += buildver.hpp buildver.cpp");
        Project("SOURCES += buildver.cpp");
        Project("VER_FILE = " . join($dir_sep, qw($(srcdir) VERSION)))
            if (!Project("VER_FILE"));
    }
    Project("CLEAN_FILES += buildrev.*");
#$}
PYTHON    = #$ $text = '$(THIRD_DIR)/bin/python'; $text =~ s-[\\\/]-$dir_sep-g;

#$ (Project("SUBDIRS") || Config("staticlib")) && DisableOutput();
MODULE_NAME  = #$ Expand("MODULE_NAME");
PRODUCT_NAME = #$ Expand("PRODUCT_NAME");

VER_MAJOR  = #$ Expand("VER_MAJOR");
VER_MINOR  = #$ Expand("VER_MINOR");
VER_PATCH  = #$ Expand("VER_PATCH");
VER_FILE   = #$ Expand("VER_FILE");

BV_SCRIPT  = $(top_srcdir)/tools/buildsys/buildver.py
BV_OPTIONS = $(VER_FILE) "$(MODULE_NAME)" "$(PRODUCT_NAME)" $(VER_MAJOR) $(VER_MINOR) $(VER_PATCH)

buildver.hpp buildver.cpp: $(VER_FILE) $(BV_SCRIPT)
	$(PYTHON) $(BV_SCRIPT) $@ $(BV_OPTIONS)

all: buildver.hpp buildver.cpp

#$ (Project("SUBDIRS") || Config("staticlib")) && EnableOutput();
BUILDNO   = buildrev.txt
BN_SCRIPT = $(top_srcdir)/tools/buildsys/buildno.py

buildrev.hpp $(BUILDNO): $(BUILD_DEPS)
	$(PYTHON) $(BN_SCRIPT) $(srcdir) $(BUILDNO) $@

all: buildrev.hpp

#$ EnableOutput() if Config("noversion");
