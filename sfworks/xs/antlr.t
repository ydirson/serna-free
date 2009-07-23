ANTLR       = antlr
ANTLR_FLAGS = #$ Expand("ANTLR_FLAGS");
ANTLR_INPUT = #$ Expand("ANTLR_INPUT");
PARSER      = #$ Expand("PARSER");
PARSER_H    = #$ Expand("PARSER_H");

maintarget: $(PARSER) all

$(PARSER): $(ANTLR_INPUT)
	$(ANTLR) $(ANTLR_FLAGS) $(ANTLR_INPUT)

$(PARSER_H): $(PARSER)

#$ IncludeTemplate("app.t");

