#!
#! Syntext Editor special processing template for apps and shared libs
#! 
#!
#${
    Project("QT3TO4 = ".join($dir_sep, '$(THIRD_DIR)', qw/bin qt3to4/));
    my @incpaths = split /[\s;]+/, Project("INCPATH");
    my $incopts = '-I "'.join("\" \\\n\t\t-I \"", @incpaths).'"';
    Project("QT3TO4_INC_OPTS = $incopts");
#$}

QT3TO4_RULES    = $(THIRD_DIR)/qt/q3porting.xml
QT3TO4_INC_OPTS = \
                #$ Expand("QT3TO4_INC_OPTS");

QT3TO4          = #$ Expand("QT3TO4");

qt3to4: force
	$(QT3TO4) -alwaysOverwrite -missingFileWarnings -rulesFile $(QT3TO4_RULES) $(QT3TO4_INC_OPTS) $(HEADERS) $(SOURCES)
        
#$ DisableOutput() if (!(Config("app") || Config("dll")));
# libraries dependency list
$(TARGET):	#$ ExpandGlue("DEP_LIBS", "", " \\\n\t\t", "\n");

#$ EnableOutput() if (!(Config("app") || Config("dll")));
