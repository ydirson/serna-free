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
SRC_DIST         = $(srcdir)/../dist/ui
all: uigen

uigen: EmptyDocumentActions.hpp EmptyDocumentBuild.cpp \
       EmptyDocumentEventDecls.hpp \
       StructDocumentActions.hpp StructDocumentBuild.cpp \
       StructDocumentEventDecls.hpp \
       PlainDocumentActions.hpp PlainDocumentBuild.cpp \
       PlainDocumentEventDecls.hpp

#######

EmptyDocumentActions.hpp: $(srcdir)/make-ui-headers2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/EmptyDocument.sui
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o EmptyDocumentActions.hpp $(srcdir)/make-ui-headers2.xsl $(SRC_DIST)/EmptyDocument.sui 

EmptyDocumentBuild.cpp: $(srcdir)/make-ui-init2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/EmptyDocument.sui
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o EmptyDocumentBuild.cpp $(srcdir)/make-ui-init2.xsl $(SRC_DIST)/EmptyDocument.sui 

EmptyDocumentEventDecls.hpp:  $(srcdir)/make-event-decls2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/EmptyDocument.sui 
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o EmptyDocumentEventDecls.hpp $(srcdir)/make-event-decls2.xsl $(SRC_DIST)/EmptyDocument.sui

#######

StructDocumentActions.hpp: $(srcdir)/make-ui-headers2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/StructDocument.sui
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o StructDocumentActions.hpp $(srcdir)/make-ui-headers2.xsl $(SRC_DIST)/StructDocument.sui 

StructDocumentBuild.cpp: $(srcdir)/make-ui-init2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/StructDocument.sui
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o StructDocumentBuild.cpp $(srcdir)/make-ui-init2.xsl $(SRC_DIST)/StructDocument.sui 

StructDocumentEventDecls.hpp:  $(srcdir)/make-event-decls2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/StructDocument.sui 
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o StructDocumentEventDecls.hpp $(srcdir)/make-event-decls2.xsl $(SRC_DIST)/StructDocument.sui

#######

PlainDocumentActions.hpp: $(srcdir)/make-ui-headers2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/PlainDocument.sui
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o PlainDocumentActions.hpp $(srcdir)/make-ui-headers2.xsl $(SRC_DIST)/PlainDocument.sui 

PlainDocumentBuild.cpp: $(srcdir)/make-ui-init2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/PlainDocument.sui
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o PlainDocumentBuild.cpp $(srcdir)/make-ui-init2.xsl $(SRC_DIST)/PlainDocument.sui 

PlainDocumentEventDecls.hpp:  $(srcdir)/make-event-decls2.xsl $(SRC_DIST)/CommonActions.ent $(SRC_DIST)/PlainDocument.sui 
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o PlainDocumentEventDecls.hpp $(srcdir)/make-event-decls2.xsl $(SRC_DIST)/PlainDocument.sui

#########

clean: ui_clean

ui_clean:
	$(RM) EmptyDocumentActions.hpp EmptyDocumentBuild.cpp \
	      EmptyDocumentEventDecls.hpp \
          StructDocumentActions.hpp StructDocumentBuild.cpp \
          StructDocumentEventDecls.hpp \
	      PlainDocumentActions.hpp PlainDocumentBuild.cpp \
          PlainDocumentEventDecls.hpp
