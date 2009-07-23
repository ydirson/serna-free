#${
    IncludeTemplate("java");
    IncludeTemplate("filetools");

    my $dxgdir = normpath(Project("srcdir")."/../sapi");
    $dxgdir =~ s/\\/\//g;
    my %vdict = ("input" => $dxgdir, "doc" => "doc", "cpp" => "CPP");
    my %vdictpy = ("input" => $dxgdir, "doc" => "doc_py", "cpp" => "PYTHON");
    my ($sed_cxx, $sed_py);
    my $quot = $is_unix ? "'" : '"';
    $sed_cxx = join(" \\\n\t\t\t", map { "-e ${quot}s!\@$_\@!$vdict{$_}!${quot}" } (keys(%vdict)));
    $sed_py = join(" \\\n\t\t\t", map { "-e ${quot}s!\@$_\@!$vdictpy{$_}!${quot}" } (keys(%vdictpy)));
    Project("SED_ARGS_DXG2XML_CPP = $sed_cxx", "SED_ARGS_DXG2XML_PY = $sed_py");
    Project('SOURCES +=  $(DOCBUILDDIR)/doctags.cpp');
#$}
#
# 
# A template for generation of Serna documentation 
#
MKDIR       = #$ $text = $is_unix ? 'mkdir -p' : 'md';
DOCMAKEFILE = #$ $text = $is_unix ? '/mk/GNUmakefile' : '\mk\Makefile';
DOCDIR      = #$ $text = expand_path('$(DOCDIR)');
DOCBUILDDIR = #$ $text = expand_path('$(DOCBUILDDIR)');
builddir    = #$ $text = expand_path('$(top_builddir)/serna/docutils');
PLATFORM    = #$ Expand("PLATFORM");
XSLTPROC         = #$ Expand("XSLTPROC");
XSLTPROC_PARAMS  = #$ Expand("XSLTPROC_PARAMS");
DOXYGEN          = #$ Expand("DOXYGEN");

SED_ARGS_DXG2XML_CPP = #$ Expand("SED_ARGS_DXG2XML_CPP");

SED_ARGS_DXG2XML_PY  = #$ Expand("SED_ARGS_DXG2XML_PY");

#$ DisableOutput() if $is_unix;
SET_XCF     = set XML_CATALOG_FILES=$(SERNA_DIR)\dist\plugins\docbook\dbk42\dtd\catalog.xml&&
#$ EnableOutput() if $is_unix;
#$ DisableOutput() unless $is_unix;
SET_XCF     = XML_CATALOG_FILES=$(SERNA_DIR)/dist/plugins/docbook/dbk42/dtd/catalog.xml
#$ EnableOutput() unless $is_unix;

gensrc: mkdirs sapigen docgen

mkdirs: force 
#$ DisableOutput() if $is_unix;
	@for %D in (sapi sapi_py) do @if not exist $(DOCBUILDDIR)\%D $(MKDIR) $(DOCBUILDDIR)\%D
#$ EnableOutput() if $is_unix;
#$ DisableOutput() unless $is_unix;
	@for d in sapi sapi_py; do [ -d $(DOCBUILDDIR)/$$d ] || $(MKDIR) $(DOCBUILDDIR)/$$d; done
#$ EnableOutput() unless $is_unix;

sapigen: $(DOCBUILDDIR)/sapi/index.xml $(DOCBUILDDIR)/sapi_py/index.xml

$(DOCBUILDDIR)/sapi/index.xml: doc/xml/boost.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) --param is-python false -o $(DOCBUILDDIR)/sapi/index.xml $(top_srcdir)/tools/boostbook/docbook.xsl doc/xml/boost.xml 

doc/xml/boost.xml: doc/xml/collected.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o doc/xml/boost.xml $(top_srcdir)/tools/boostbook/doxygen/doxygen2boostbook.xsl doc/xml/collected.xml

doc/xml/collected.xml: doc/xml/collect.xsl
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o doc/xml/collected.xml doc/xml/collect.xsl doc/xml/index.xml

doc/xml/collect.xsl: doc/xml/index.xml
	cp $(top_srcdir)/tools/boostbook/doxygen/collect.xsl doc/xml
        
$(builddir)/sapi.dxg: $(srcdir)/sapi.dxg
	$(CAT) $(srcdir)/sapi.dxg | $(SED) $(SED_ARGS_DXG2XML_CPP) > $@

doc/xml/index.xml: $(builddir)/sapi.dxg
	$(DOXYGEN) $(builddir)/sapi.dxg 

$(DOCBUILDDIR)/sapi_py/index.xml: doc_py/xml/boost.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o $(DOCBUILDDIR)/sapi_py/index.xml $(top_srcdir)/tools/boostbook/docbook.xsl doc_py/xml/boost.xml 

doc_py/xml/boost.xml: doc_py/xml/collected.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o doc_py/xml/boost.xml $(top_srcdir)/tools/boostbook/doxygen/doxygen2boostbook.xsl doc_py/xml/collected.xml

doc_py/xml/collected.xml: doc_py/xml/collect.xsl
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o doc_py/xml/collected.xml doc_py/xml/collect.xsl doc_py/xml/index.xml

doc_py/xml/collect.xsl: doc_py/xml/index.xml
	cp $(top_srcdir)/tools/boostbook/doxygen/collect.xsl doc_py/xml

$(builddir)/sapi_py.dxg: $(srcdir)/sapi.dxg
	$(CAT) $(srcdir)/sapi.dxg | $(SED) $(SED_ARGS_DXG2XML_PY) > $@
        
doc_py/xml/index.xml: $(builddir)/sapi_py.dxg
	$(DOXYGEN) $(builddir)/sapi_py.dxg 

docgen: force
	(cd $(DOCBUILDDIR) && $(SET_XCF) $(MAKE) -f \
                   $(srcdir)$(DOCMAKEFILE) \
	           THIRD_DIR=$(THIRD_DIR) \
	           srcdir=$(srcdir) \
	           top_srcdir=$(top_srcdir) \
                   docdir=$(DOCDIR) \
                   PLATFORM=$(PLATFORM) \
#$ DisableOutput() if $is_unix;
	&& cd $(builddir) \
#$ EnableOutput() if $is_unix;
        )

clean: doc_clean

doc_clean:
	$(RMDIR) $(DOCBUILDDIR)
	$(RMDIR) doc
	$(RMDIR) doc_py
	$(RM) *.dxg
