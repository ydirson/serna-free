#${
	IncludeTemplate("utils");
	IncludeTemplate("autoconf");
	IncludeTemplate("filetools");
#$}
#
# 
# A template for generation of Serna documentation 
#

XSLTPROC         = #$ Expand("XSLTPROC"); $text =~ s/[\\\/]/$dir_sep/g;
XSLTPROC_PARAMS  = #$ Expand("XSLTPROC_PARAMS");
SRC_GENUI        = #$ Expand("UI_SRCDIR");
SRC_DIST         = $(SRC_GENUI)/../dist/ui
all: uigen

uigen: EmptyDocumentActions.hpp EmptyDocumentBuild.cpp \
       EmptyDocumentEventDecls.hpp \
       StructDocumentActions.hpp StructDocumentBuild.cpp \
       StructDocumentEventDecls.hpp \
       PlainDocumentActions.hpp PlainDocumentBuild.cpp \
       PlainDocumentEventDecls.hpp

#######

EmptyDocumentActions.hpp: $(SRC_GENUI)/make-ui-headers2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/EmptyDocument.sui
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o EmptyDocumentActions.hpp $(SRC_GENUI)/make-ui-headers2.xsl $(SRC_DIST)/EmptyDocument.sui 

EmptyDocumentBuild.cpp: $(SRC_GENUI)/make-ui-init2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/EmptyDocument.sui
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o EmptyDocumentBuild.cpp $(SRC_GENUI)/make-ui-init2.xsl $(SRC_DIST)/EmptyDocument.sui 

EmptyDocumentEventDecls.hpp:  $(SRC_GENUI)/make-event-decls2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/EmptyDocument.sui 
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o EmptyDocumentEventDecls.hpp $(SRC_GENUI)/make-event-decls2.xsl $(SRC_DIST)/EmptyDocument.sui

#######

StructDocumentActions.hpp: $(SRC_GENUI)/make-ui-headers2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/StructDocument.sui
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o StructDocumentActions.hpp $(SRC_GENUI)/make-ui-headers2.xsl $(SRC_DIST)/StructDocument.sui 

StructDocumentBuild.cpp: $(SRC_GENUI)/make-ui-init2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/StructDocument.sui
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o StructDocumentBuild.cpp $(SRC_GENUI)/make-ui-init2.xsl $(SRC_DIST)/StructDocument.sui 

StructDocumentEventDecls.hpp:  $(SRC_GENUI)/make-event-decls2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/StructDocument.sui 
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o StructDocumentEventDecls.hpp $(SRC_GENUI)/make-event-decls2.xsl $(SRC_DIST)/StructDocument.sui

#######

PlainDocumentActions.hpp: $(SRC_GENUI)/make-ui-headers2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/PlainDocument.sui
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o PlainDocumentActions.hpp $(SRC_GENUI)/make-ui-headers2.xsl $(SRC_DIST)/PlainDocument.sui 

PlainDocumentBuild.cpp: $(SRC_GENUI)/make-ui-init2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/PlainDocument.sui
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o PlainDocumentBuild.cpp $(SRC_GENUI)/make-ui-init2.xsl $(SRC_DIST)/PlainDocument.sui 

PlainDocumentEventDecls.hpp:  $(SRC_GENUI)/make-event-decls2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/PlainDocument.sui 
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o PlainDocumentEventDecls.hpp $(SRC_GENUI)/make-event-decls2.xsl $(SRC_DIST)/PlainDocument.sui

#########

clean: ui_clean

ui_clean:
	$(RM) EmptyDocumentActions.hpp EmptyDocumentBuild.cpp \
	      EmptyDocumentEventDecls.hpp \
          StructDocumentActions.hpp StructDocumentBuild.cpp \
          StructDocumentEventDecls.hpp \
	      PlainDocumentActions.hpp PlainDocumentBuild.cpp \
          PlainDocumentEventDecls.hpp
