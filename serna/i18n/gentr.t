#${
	IncludeTemplate("utils");
	IncludeTemplate("dirs");
	IncludeTemplate("autoconf");
	IncludeTemplate("filetools");
	IncludeTemplate("genlist");
    Project('XSLTPROC_CMD = $(THIRD_DIR)/xsltproc/$(PLATFORM)/xsltproc');
    my $uic_sources = &UicImpls(Project("INTERFACES"));
    Project("UICIMPLS = $uic_sources");
#$}
#
# 
# A template for generation of Serna documentation 
#
XSLTPROC         = #$ Expand("XSLTPROC_CMD"); $text =~ s/[\\\/]/$dir_sep/g;
XSLTPROC_PARAMS  = #$ Expand("XSLTPROC_PARAMS");

SOURCES    = #$ Expand("SOURCES");
INTERFACES = #$ Expand("INTERFACES");
UICIMPLS   = #$ Expand("UICIMPLS");

UIC        = #$ Expand("UIC"); $text =~ s/[\\\/]/$dir_sep/g;
LUPDATE    = #$ Expand("LUPDATE"); $text =~ s/[\\\/]/$dir_sep/g;
LRELEASE   = #$ Expand("LRELEASE"); $text =~ s/[\\\/]/$dir_sep/g;
PYTHON     = #$ Expand("PYTHON"); $text =~ s/[\\\/]/$dir_sep/g;

GENUISRC   = EmptyDocUi.cpp StructDocUi.cpp PlainDocUi.cpp 
MSGSRC     = CommonMessages.cpp SernaMessages.cpp \
             ParserMessages.cpp SpMessages.cpp XsMessages.cpp \
             GroveEditorMessages.cpp XslMessages.cpp \
             XpathMessages.cpp EditableViewMessages.cpp \
             XsltMessages.cpp XslMessages.cpp CatMgrMessages.cpp

msg2xml    = $(PYTHON) $(top_srcdir)/tools/msggen/msg2xml.py

CommonMessages.cpp: CommonMessages.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o $@ $(srcdir)/make-tr-msg.xsl $?

CommonMessages.xml: $(top_srcdir)/sfworks/common/CommonMessages.msg
	$(msg2xml) $? $@

SernaMessages.cpp: SernaMessages.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o $@ $(srcdir)/make-tr-msg.xsl $?

SernaMessages.xml: $(top_srcdir)/serna/utils/SernaMessages.msg
	$(msg2xml) $? $@

ParserMessages.cpp: ParserMessages.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o $@ $(srcdir)/make-tr-msg.xsl $?

ParserMessages.xml: $(top_srcdir)/sfworks/qtgrovebuilder/ParserMessages.msg
	$(msg2xml) $? $@

SpMessages.cpp: SpMessages.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o $@ $(srcdir)/make-tr-msg.xsl $?

SpMessages.xml: $(top_srcdir)/sfworks/spgrovebuilder/impl/SpMessages.msg
	$(msg2xml) $? $@

GroveEditorMessages.cpp: GroveEditorMessages.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o $@ $(srcdir)/make-tr-msg.xsl $?

GroveEditorMessages.xml: $(top_srcdir)/sfworks/groveeditor/GroveEditorMessages.msg
	$(msg2xml) $? $@

XsMessages.cpp: XsMessages.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o $@ $(srcdir)/make-tr-msg.xsl $?

XsMessages.xml: $(top_srcdir)/sfworks/xs/XsMessages.msg
	$(msg2xml) $? $@

XpathMessages.cpp: XpathMessages.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o $@ $(srcdir)/make-tr-msg.xsl $?

XpathMessages.xml: $(top_srcdir)/sfworks/xpath/impl/XpathMessages.msg
	$(msg2xml) $? $@

EditableViewMessages.cpp: EditableViewMessages.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o $@ $(srcdir)/make-tr-msg.xsl $?

EditableViewMessages.xml: $(top_srcdir)/sfworks/editableview/impl/EditableViewMessages.msg
	$(msg2xml) $? $@

XslMessages.cpp: XslMessages.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o $@ $(srcdir)/make-tr-msg.xsl $?

XslMessages.xml: $(top_srcdir)/sfworks/formatter/impl/XslMessages.msg
	$(msg2xml) $? $@

XsltMessages.cpp: XsltMessages.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o $@ $(srcdir)/make-tr-msg.xsl $?

XsltMessages.xml: $(top_srcdir)/sfworks/xslt/impl/XsltMessages.msg
	$(msg2xml) $? $@

XslMessages.cpp: XslMessages.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o $@ $(srcdir)/make-tr-msg.xsl $?

XslMessages.xml: $(top_srcdir)/sfworks/formatter/impl/XslMessages.msg
	$(msg2xml) $? $@

CatMgrMessages.cpp: CatMgrMessages.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o $@ $(srcdir)/make-tr-msg.xsl $?

CatMgrMessages.xml: $(top_srcdir)/sfworks/catmgr/impl/CatMgrMessages.msg
	$(msg2xml) $? $@

#$ BuildUicSrc(Project("INTERFACES"));

EmptyDocUi.cpp: $(srcdir)/../genui/CommonCommands.xml $(srcdir)/../genui/EmptyDocument-ui.xml
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o EmptyDocUi.cpp $(srcdir)/make-tr-ui.xsl $(srcdir)/../genui/EmptyDocument-ui.xml 

StructDocUi.cpp:  $(srcdir)/../genui/CommonCommands.xml $(srcdir)/../genui/StructDocument-ui.xml 
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o StructDocUi.cpp $(srcdir)/make-tr-ui.xsl $(srcdir)/../genui/StructDocument-ui.xml 

PlainDocUi.cpp:  $(srcdir)/../genui/CommonCommands.xml $(srcdir)/../genui/PlainDocument-ui.xml 
	$(XSLTPROC) $(XSLTPROC_PARAMS) -o PlainDocUi.cpp $(srcdir)/make-tr-ui.xsl $(srcdir)/../genui/PlainDocument-ui.xml 




all: trgen

trgen: $(UICIMPLS) $(GENUISRC) $(MSGSRC) $(SOURCES)
	$(LUPDATE) $? -ts $(srcdir)/se_ru.ts
	$(LUPDATE) $? -ts $(srcdir)/se_de.ts
	$(LUPDATE) $? -ts $(srcdir)/se_fr.ts
	$(LUPDATE) $? -ts $(srcdir)/se_en.ts
	$(RM) $(UICIMPLS) $(GENUISRC) $(MSGSRC) *.xml

release: $(srcdir)/se_ru.ts $(srcdir)/se_de.ts $(srcdir)/se_fr.ts
	$(LRELEASE) $(srcdir)/se_ru.ts -qm $(top_builddir)/serna/bin/se_ru.qm
	$(LRELEASE) $(srcdir)/se_de.ts -qm $(top_builddir)/serna/bin/se_de.qm
	$(LRELEASE) $(srcdir)/se_fr.ts -qm $(top_builddir)/serna/bin/se_fr.qm

clean: ui_clean

ui_clean:
	$(RM) $(UICIMPLS) $(GENUISRC) $(MSGSRC) *.xml
