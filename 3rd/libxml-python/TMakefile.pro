TEMPLATE = 3rd
TMAKE_TEMPLATE = libxml-python

INCPATH      = $(THIRD_DIR)/python/include
XML2_INCPATH = $$INCPATH;$(THIRD_DIR)/iconv;$(THIRD_DIR)/libxml/install/include
XSLT_INCPATH = $$XML2_INCPATH;$(THIRD_DIR)/xsltproc/install/include

XML2_OBJDIR  = xml2obj
XSLT_OBJDIR  = xsltobj

XML2_SOURCES = libxml.c types.c libxml2-py.c
XSLT_SOURCES = libxslt.c types.c libxslt-py.c

debug:SFX    = _d
XML2_LIBS    = libxml2.lib iconv.lib
XSLT_LIBS    = $$XML2_LIBS libxslt.lib libexslt.lib 

debug:PACKAGE_MAKEOPTS = debug=1
