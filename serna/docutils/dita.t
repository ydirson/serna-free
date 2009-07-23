DOCDIR              = #$ ExpandPathName("DOCDIR");
DOCBUILDDIR         = #$ ExpandPathName("DOCBUILDDIR");
DB2DITA_DIR         = #$ ExpandPathName("DB2DITA_DIR");
DITA_HTML_DIR       = #$ ExpandPathName("DITA_HTML_DIR");
DITA_SRC_DIR        = #$ ExpandPathName("DITA_SRC_DIR");
DITA_XSL_DIR        = #$ ExpandPathName("DITA_XSL_DIR");

#${
    my $dxgdir = normpath(Project("srcdir")."/../sapi");
    $dxgdir =~ s/\\/\//g;
    my %vdict = ("input" => $dxgdir, "doc" => "doc", "cpp" => "CPP");
    my %vdictpy = ("input" => $dxgdir, "doc" => "doc_py", "cpp" => "PYTHON");
    my ($sed_cxx, $sed_py);
    my $quot = $is_unix ? "'" : '"';
    $sed_cxx = join(" \\\n\t\t\t", map { "-e ${quot}s!\@$_\@!$vdict{$_}!${quot}" } (keys(%vdict)));
    $sed_py = join(" \\\n\t\t\t", map { "-e ${quot}s!\@$_\@!$vdictpy{$_}!${quot}" } (keys(%vdictpy)));
    Project("SED_ARGS_DXG2XML_CPP = $sed_cxx", "SED_ARGS_DXG2XML_PY = $sed_py");

    Project("FILETOOLS = COPY XSLTPROC CHDIR CAT MKDIR RMDIR RM SED PYTHON");
    IncludeTemplate("filetools");
    my $xcf = Project("XML_CATALOG_FILES");
    $xcf =~ s/[\\\/]/$dir_sep/g and Project("XML_CATALOG_FILES = $xcf");
    $ENV{"XML_CATALOG_FILES"} = expand_path($xcf);
    Project('SOURCES +=  doctags.cpp');

    IncludeTemplate("$$srcdir/dita-sources.t");
    my $tempdir = expand_path(Project("DITA_HTML_TEMPDIR"));
    unless ($tempdir =~ /^[\\\/]/ or (!$is_unix and $tempdir =~ /^[a-zA-Z]\:/)) {
        Project("DITA_HTML_TEMPDIR = ".&getcwd().$dir_sep.$tempdir);
    }
#$}

DOXYGEN          = #$ Expand("DOXYGEN");
DITA_HTML_TEMPDIR   = #$ ExpandPathName("DITA_HTML_TEMPDIR");

DITA_DOCDIRS = #$ ExpandGlue("DITA_DOCDIRS", "\t\\\n\t\t\t", " \\\n\t\t\t", "");
DOCBOOK_DIRS = #$ ExpandGlue("DOCBOOK_DIRS", "\t\\\n\t\t\t", " \\\n\t\t\t", "");

DOCDIRS = $(DOCBOOK_DIRS) $(DITA_DOCDIRS)

SERNA_HELP_DITAMAP  = #$ ExpandPathName("SERNA_HELP_DITAMAP");

XML_CATALOG_FILES   = #$ ExpandGlue("XML_CATALOG_FILES", "\t\\\n\t\t\t", " \\\n\t\t\t", "");

$(DOCDIRS) $(DITA_HTML_DIR) $(DITA_HTML_TEMPDIR):
#$ DisableOutput() unless $is_unix;
	@[ -d "$(DITA_HTML_DIR)" ] || mkdir -p $(DITA_HTML_DIR)
	@[ -d "$(DITA_HTML_TEMPDIR)" ] || mkdir -p $(DITA_HTML_TEMPDIR)
	@for d in $(DOCDIRS); do [ -d $$d ] || mkdir -p $$d; done
#$ EnableOutput() unless $is_unix;
#$ DisableOutput() if $is_unix;
	@if not exist $(DITA_HTML_DIR) mkdir $(DITA_HTML_DIR)
	@if not exist $(DITA_HTML_TEMPDIR) mkdir $(DITA_HTML_TEMPDIR)
	@for %D in ($(DOCDIRS)) do @if not exist %D mkdir %D
#$ EnableOutput() if $is_unix;

SED_ARGS_DXG2XML_CPP = #$ Expand("SED_ARGS_DXG2XML_CPP");
SED_ARGS_DXG2XML_PY  = #$ Expand("SED_ARGS_DXG2XML_PY");

DXG     = $(srcdir)/sapi.dxg

SAPI_CPP_DIR = doc/xml
SAPI_PY_DIR  = doc_py/xml

SAPI_CPP_XML = $(SAPI_CPP_DIR)/index.xml
SAPI_PY_XML  = $(SAPI_PY_DIR)/index.xml

$(SAPI_CPP_XML): $(DXG)
	$(CAT) $(DXG) | $(SED) $(SED_ARGS_DXG2XML_CPP) | \
            $(DOXYGEN) - > $(@F).log 2>&1

$(SAPI_PY_XML): $(DXG)
	$(CAT) $(DXG) | $(SED) $(SED_ARGS_DXG2XML_PY) | \
            $(DOXYGEN) - > $(@F).log 2>&1

BB_DIR      = #$ ExpandPathName("BOOSTBOOK_DIR");

DG2BB_XSL   = $(BB_DIR)/doxygen/doxygen2boostbook.xsl
BB2DB_XSL   = $(BB_DIR)/docbook.xsl
COLLECT_XSL = $(BB_DIR)/doxygen/collect.xsl

$(DITA_HTML_DIR)/sapi/index.xml: $(SAPI_CPP_XML) $(BB2DB_XSL) $(DG2BB_XSL) $(COLLECT_XSL)
	$(XSLTPROC) --path $(SAPI_CPP_DIR) $(COLLECT_XSL) $(SAPI_CPP_XML) | \
	$(XSLTPROC) --path $(SAPI_CPP_DIR) $(DG2BB_XSL) - | \
	$(XSLTPROC) --path $(SAPI_CPP_DIR) --param is-python false -o $@ $(BB2DB_XSL) -

$(DITA_HTML_DIR)/sapi_py/index.xml: $(SAPI_PY_XML) $(BB2DB_XSL) $(DG2BB_XSL) $(COLLECT_XSL)
	$(XSLTPROC) --path $(SAPI_PY_DIR) $(COLLECT_XSL) $(SAPI_PY_XML) | \
	$(XSLTPROC) --path $(SAPI_PY_DIR) $(DG2BB_XSL) - | \
	$(XSLTPROC) --path $(SAPI_PY_DIR) -o $@ $(BB2DB_XSL) -

DOCBOOK2HTML    = #$ ExpandPathName("DOCBOOK2HTML");

DOCBOOK2HTML_XSL= chunk.xsl

XSLTPROC_PARAMS = --param use.id.as.filename 1

SAPI_XMLS       =   \
                    $(DITA_HTML_DIR)/sapi/index.xml \
                    $(DITA_HTML_DIR)/sapi_py/index.xml

SAPI_HTMLS      =   \
                    $(DITA_HTML_DIR)/sapi/index.html \
                    $(DITA_HTML_DIR)/sapi_py/index.html

$(DITA_HTML_DIR)/sapi/index.html: $(DITA_HTML_DIR)/sapi/index.xml
	#$ SetEnvVars(qw/XSLTPROC_PARAMS DOCBOOK2HTML_XSL/);
	$(DOCBOOK2HTML) $@ $(DITA_HTML_DIR)/sapi/index.xml > sapi-cpp.log 2>&1

$(DITA_HTML_DIR)/sapi_py/index.html: $(DITA_HTML_DIR)/sapi_py/index.xml
	#$ SetEnvVars(qw/XSLTPROC_PARAMS DOCBOOK2HTML_XSL/);
	$(DOCBOOK2HTML) $@ $(DITA_HTML_DIR)/sapi_py/index.xml > sapi-py.log 2>&1

sapihtml: $(SAPI_HTMLS)
sapixml: $(SAPI_XMLS)

#$ Expand("DOCBOOK_HTML_RULES");

DOCBOOOK_HTMLS =    \
                    $(SAPI_HTMLS) \
                    #$ ExpandGlue("DOCBOOK_HTMLS", "", " \\\n\t\t\t", "");

copypics: $(DOCBOOK_PICS)

dbhtmls: $(DOCBOOOK_HTMLS) copypics

DITA_HTML_TOC       = $(DITA_HTML_DIR)/serna-help.html
DITA_HTML_INDEX     = $(DITA_HTML_DIR)/index.html
DITA2HTML           = #$ ExpandPathName("DITA2HTML");
DITA_OT_LOGDIR      = #$ Project("DITA_OT_LOGDIR = ".getcwd()); ExpandPathName("DITA_OT_LOGDIR");
DITA_HTML_XSL       = #$ ExpandPathName("DITA_HTML_XSL");

$(DITA_HTML_TOC): $(SAPI_XMLS) $(SERNA_HELP_DITAMAP) $(DITA_SOURCES)
#$SetEnvVars(qw/DITA_OT_LOGDIR DITA_HTML_XSL/);
	$(DITA2HTML) $(SERNA_HELP_DITAMAP) $(DITA_HTML_DIR) \
            $(DITA_HTML_TEMPDIR) $(DITA_SRC_DIR)

MAP2INDEXHTML_XSL = $(DITA_XSL_DIR)/map2indexhtml.xsl

$(DITA_HTML_INDEX): $(SAPI_XMLS) $(SERNA_HELP_DITAMAP) $(DITA_SOURCES) $(MAP2INDEXHTML_XSL)
#$SetEnvVars("XML_CATALOG_FILES");
	$(XSLTPROC) -o $@ --stringparam base-dir $(DITA_SRC_DIR) \
             $(MAP2INDEXHTML_XSL) $(SERNA_HELP_DITAMAP)

ditahtmlindex: $(DITA_HTML_INDEX)

ditahtmltoc: $(DITA_HTML_TOC) $(DITA_HTML_INDEX)

dita2html: sapihtml ditahtmltoc

DITA2ADP_XSL = $(DITA_XSL_DIR)/ditamap-to-adp.xsl

$(DITA_HTML_DIR)/serna.adp: $(SERNA_HELP_DITAMAP) $(DITA_SOURCES) $(DITA2ADP_XSL) $(SAPI_XMLS)
#$SetEnvVars("XML_CATALOG_FILES");
	$(XSLTPROC) -o $@ \
            --stringparam base-dir $(DITA_HTML_DIR) --path $(DITA_HTML_DIR) \
            $(DITA2ADP_XSL) $(SERNA_HELP_DITAMAP)

dita2adp: $(DITA_HTML_DIR)/serna.adp

forceadp:
#$SetEnvVars("XML_CATALOG_FILES");
	$(XSLTPROC) -o $(DITA_HTML_DIR)/serna.adp \
            --stringparam base-dir $(DITA_HTML_DIR) --path $(DITA_HTML_DIR) \
            $(DITA2ADP_XSL) $(SERNA_HELP_DITAMAP)

doctags.lst: $(DITA_HTML_DIR)/serna.adp
	$(XSLTPROC) $(srcdir)/xsl/mkdoctags2.xsl $(DITA_HTML_DIR)/serna.adp
	
MAKE_DOCTAGS_SCRIPT = $(srcdir)/make_tags_cpp.py

doctags.cpp: doctags.lst $(MAKE_DOCTAGS_SCRIPT)
	$(PYTHON) $(MAKE_DOCTAGS_SCRIPT) doctags.lst $@

doctags: doctags.cpp

clean_docs:
	$(RMDIR) $(DITA_HTML_DIR) doc doc_py
	$(RM) doctags.cpp doctags.lst sapi-py.log sapi-cpp.log

clean: clean_docs
#$ Project('ALL_DEPS += $(DOCDIRS) doctags dita2html copypics dbhtmls');
#$ $text = ".PHONY: doctags copypics dita2adp dita2html ditahtmltoc ditahtmlindex sapihtml sapixml dbhtmls" if $is_unix;
