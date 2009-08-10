#$ DisableOutput() if (Config("staticlib") || !Project("WIN32_RESOURCE"));
#
# Win32 resource handling template
# 
#
#${
    Project("DEFINES *= " . (Config("debug") ? '_DEBUG' : 'NO_DEBUG'));
    Project("INCLUDEPATH /= s-/-\\-g");
    if (Project("WIN32_RESOURCE") && !Config("staticlib")) {    
    	my $rcfile = Project("WIN32_RESOURCE");
    	$rcfile =~ s-/-\\-g;
    	my ($p, $resfile) = fnsplit($rcfile);
    	$resfile =~ s/\.\w+$/\.res/i;
    	Project("WIN32_RESOURCE = $rcfile", 'RES_FILE = $(OBJECTS_DIR)'."\\$resfile");
    	Project('CLEANFILES *= $(RES_FILE)', 'TARGETDEPS *= $(RES_FILE)');
    }
#$}
CPP_DEFINES = #$ ExpandGlue("DEFINES", "\t-D", " \\\n\t\t-D", "");
CPP_INCPATH = #$ $text = join(" \\\n\t\t-I", map { s-/-\\-g; $_ = '"' . $_ . '"'} split(/\s*;\s*/, Project("INCLUDEPATH"))); $text = "\t-I".$text if $text;

TMAKE_RC          = #$ Expand("TMAKE_RC");
WIN32_RESOURCE    = #$ Expand("WIN32_RESOURCE");
RES_FILE          = #$ Expand("RES_FILE");

$(RES_FILE): $(WIN32_RESOURCE) buildver.hpp buildrev.hpp
	$(TMAKE_RC) -Fo$@ $(CPP_INCPATH) $(CPP_DEFINES) $(WIN32_RESOURCE)

#$ EnableOutput() if (Config("staticlib") || !Project("WIN32_RESOURCE"));
