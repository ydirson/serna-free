#$ DisableOutput() unless Project("BISON_INPUT");
#
# 
# A template for generating parser source/header from bison grammar file
#
#${
    if (Project("BISON_INPUT")) { 
        my $parser = Project("BISON_INPUT");
        if ($parser =~ m-.*[/\\]([^/\\\.]+)\..*$-) {
            $parser = $1;
        }
        Project("FILES_TO_CLEAN += " . join(' ', map { $parser.$_ } (".cpp", ".hpp")));
        Project("SOURCES += $parser.cpp", "HEADERS += $parser.hpp");
        Project('ALL_DEPS += $(PARSER)', "PARSER = $parser");
    }
#$}
#$ file_tools("BISON");
BISON_INPUT   = #$ Expand("BISON_INPUT");
PARSER        = #$ Expand("PARSER"); $text .= ".cpp";
PARSER_H      = #$ Expand("PARSER"); $text .= ".hpp";

$(PARSER) $(PARSER_H): $(BISON_INPUT)
	$(BISON) $(BISON_FLAGS) $(BISON_INPUT)
#!#$ (Config("dynamic") && 0 == $is_unix && Project("EXPORT_PARSER")) || DisableOutput();
#!	$(THIRD_DIR)\bin\python $(top_srcdir)\tools\buildsys\extra\export_parser.py \
#!	    #$ Expand("EXPORT_PARSER"); $text .= ' $(PARSER_H)';
#!#$ (Config("dynamic") && 0 == $is_unix && Project("EXPORT_PARSER")) || EnableOutput();

#$ EnableOutput() unless Project("BISON_INPUT");
