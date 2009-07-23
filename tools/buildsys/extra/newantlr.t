#$ unless (Project("ANTLR_T_INCLUDED")) { IncludeTemplate("java"); } else { DisableOutput(); }
#
# 
# A template for generating parser source/header from antlr grammar file
#
#${
unless (Project("ANTLR_T_INCLUDED")) {
    my $parser = Project("PARSER");
    Project('ALL_DEPS += $(PARSER)');
    Project("HEADERS += $parser.hpp");
    
    my $grammar = Project("ANTLR_INPUT");
        if($grammar =~ m-.*[/\\]([^/\\]+$)-) {
            $grammar = $1;
    }
    Project("ANTLR_GRAMMAR = $grammar");
    my $path_sep = $is_unix ? ':' : ';';
    Project('ANTLR_CLASSPATH = '.$path_sep.'$(THIRD_DIR)/antlr');
    foreach (ANTLR, ANTLR_FLAGS, ANTLR_INPUT, PARSER, PARSER_H, 
             ANTLR_CLASSPATH) {
        my $tmpvar = Project($_);
        $tmpvar =~ s/[\/]/$dir_sep/g;
        Project("$_ = $tmpvar");
    }
    my @cleanfiles = map { Project("PARSER").$_ }
        (".cpp", ".hpp", "TokenTypes.hpp", "TokenTypes.txt");
    Project('FILES_TO_CLEAN += $(ANTLR_GRAMMAR) ' . join(' ', @cleanfiles));
}
#$}
ANTLR_CLASSPATH = #$ Expand("ANTLR_CLASSPATH");
ANTLR           = $(JAVA) $(CLASSPATH_OPT) $(CLASSPATH)$(ANTLR_CLASSPATH) \
                  antlr.Tool
ANTLR_FLAGS     = #$ Expand("ANTLR_FLAGS");
ANTLR_INPUT     = #$ Expand("ANTLR_INPUT");

ANTLR_GRAMMAR = #$ Expand("ANTLR_GRAMMAR");
PARSER        = #$ Expand("PARSER"); $text .= ".cpp";
PARSER_H      = #$ Expand("PARSER"); $text .= ".hpp";
COPY          = #$ Expand("COPY");

$(PARSER) $(PARSER_H): $(ANTLR_INPUT)
	$(COPY) $(ANTLR_INPUT) $(ANTLR_GRAMMAR) #$ $text = ' >nul' if (!$is_unix);
	$(ANTLR) $(ANTLR_FLAGS) $(ANTLR_GRAMMAR)
#$ (Config("dynamic") && 0 == $is_unix && Project("EXPORT_PARSER")) || DisableOutput();
	$(THIRD_DIR)\bin\python $(top_srcdir)\tools\buildsys\extra\export_parser.py \
	    #$ Expand("EXPORT_PARSER"); $text .= ' $(PARSER_H)';
#$ (Config("dynamic") && 0 == $is_unix && Project("EXPORT_PARSER")) || EnableOutput();
#$ EnableOutput() if (Project("ANTLR_T_INCLUDED"));
#$ Project("ANTLR_T_INCLUDED = 1")
