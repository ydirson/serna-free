#${
    return if Config("dita");
    ScanProject(Project("srcdir")."/migration/db2dita.pro");
    Project("FILETOOLS = COPY XSLTPROC CHDIR CAT MKDIR PYTHON");
    IncludeTemplate("filetools");
    my $xcf = Project("XML_CATALOG_FILES");
    $xcf =~ s/[\\\/]/$dir_sep/g and Project("XML_CATALOG_FILES = $xcf");
#$}

DB2DITA_DIR         = #$ ExpandPathName("DB2DITA_DIR");
DITA_GEN_DIR        = #$ ExpandPathName("DITA_GEN_DIR");

DB2DITA_XSL     = #$ ExpandPathName("DB2DITA_XSL");
CHECK_ELEMS     = #$ ExpandPathName("CHECK_ELEMS");
CHECK_TEXT_XSL  = #$ ExpandPathName("CHECK_TEXT_XSL");
CHECK_TEXT_PY   = #$ ExpandPathName("CHECK_TEXT_PY");

SERNA_HELP_MAP  = #$ ExpandPathName("SERNA_HELP_MAP");
SERNA_HELP_SRC  = #$ ExpandPathName("SERNA_HELP_SRC");

XML_CATALOG_FILES  = #$ ExpandGlue("XML_CATALOG_FILES", "\t\\\n\t\t\t", " \\\n\t\t\t", "");

DB2DITA_PARAMS     = #$ ExpandPathName("DB2DITA_PARAMS");
#$ DisableOutput() if Project("DITA_DOCDIRS");
#$ IncludeTemplate("$$srcdir/migration/docdirs");
DOCDIRS = $(DOCBOOK_DIRS) $(DITA_DOCDIRS)

$(DITA_DOCDIRS):
#$ DisableOutput() unless $is_unix;
	@for d in $(DITA_DOCDIRS); do [ -d $(DITA_GEN_DIR)/$$d ] || mkdir -p $(DITA_GEN_DIR)/$$d; done
	@[ -d "$(DITA_GEN_DIR)" ] || mkdir -p $(DITA_GEN_DIR)
#$ EnableOutput() unless $is_unix;
#$ DisableOutput() if $is_unix;
	@for %D in ($(DITA_DOCDIRS)) do @if not exist $(DITA_GEN_DIR)\%D mkdir $(DITA_GEN_DIR)\%D
	@if not exist $(DITA_GEN_DIR) mkdir $(DITA_GEN_DIR)
#$ EnableOutput() if $is_unix;
#$ EnableOutput() if Project("DITA_DOCDIRS");

$(SERNA_HELP_MAP): $(DITA_DOCDIRS) $(SERNA_HELP_SRC) $(DB2DITA_XSL)
	-@$(MKDIR) $(DITA_GEN_DIR)
#$ SetEnvVars("XML_CATALOG_FILES");
	$(CHDIR) $(DITA_GEN_DIR) && \
	$(XSLTPROC) -o $@ --xinclude $(DB2DITA_XSL) $(SERNA_HELP_SRC)

gen_map: $(SERNA_HELP_MAP)

genmap: $(SERNA_HELP_MAP) forcegenmap

forcegenmap:

check_elems: gen_map
#$ SetEnvVars("XML_CATALOG_FILES");
	$(XSLTPROC) --xinclude --path $(DB2DITA_DIR) --stringparam base-dir $(DITA_GEN_DIR) $(CHECK_ELEMS) $(SERNA_HELP_SRC)

check_text: gen_map
#$ SetEnvVars("XML_CATALOG_FILES");
	$(XSLTPROC) --xinclude --path $(DB2DITA_DIR) --stringparam base-dir $(DITA_GEN_DIR) $(CHECK_TEXT_XSL) $(SERNA_HELP_SRC) \
		2>&1 | $(PYTHON) $(CHECK_TEXT_PY) -

convert: gen_map check_text check_elems

