#
#

TEMPLATE  = $$srcdir/i18n
CONFIG += moc
NO_VPATH = 1

CHECK_LITERALS_PATTERN = \.cxx$ \
                  $$top_srcdir/sfworks/groveeditor/.*\.cxx$ \
                  $$top_srcdir/sfworks/groveeditor/impl/.*\.cxx$ \
                  $$top_srcdir/sfworks/editableview/impl/.*\.cxx$ \
                  $$top_srcdir/sfworks/canvasareaset/impl/.*\.cxx$ 

CHECK_LITERALS_PATTERN += \
                  $$top_srcdir/serna/app/.*\.cxx$ \
                  $$top_srcdir/serna/core/impl/.*\.cxx$ \
                  $$top_srcdir/serna/core/qt/.*\.cxx$ \
                  $$top_srcdir/serna/docview/impl/.*\.cxx$ \
                  $$top_srcdir/serna/docview/qt/.*\.cxx$ \
                  $$top_srcdir/serna/plaineditor/impl/.*\.cxx$ \
                  $$top_srcdir/serna/plaineditor/qt/.*\.cxx$ \
                  $$top_srcdir/serna/structeditor/impl/.*\.cxx$ \
                  $$top_srcdir/serna/structeditor/qt/.*\.cxx$ \
                  $$top_srcdir/serna/utils/.*\.cxx$ \
                  $$top_srcdir/serna/utils/impl/.*\.cxx$ \
                  $$top_srcdir/serna/utils/impl/unix/.*\.cxx$ \
                  $$top_srcdir/serna/utils/impl/win/.*\.cxx$ \
                  $$top_srcdir/serna/utils/spell/.*\.cxx$ \
                  $$top_srcdir/serna/utils/spell/aspell/.*\.cxx$
                  
CHECK_LITERALS_PATTERN += \
                  $$top_srcdir/serna/plugins/docinfo/.*\.cxx$ \
                  $$top_srcdir/serna/plugins/hotkeys/.*\.cxx$ \
                  $$top_srcdir/serna/plugins/linkvoyager/.*\.cxx$ \
                  $$top_srcdir/serna/plugins/updateonsave/.*\.cxx$ \
                  $$top_srcdir/serna/plugins/customcontent/.*\.cxx$ \
                  $$top_srcdir/serna/plugins/tableplugin/.*\.cxx$ \
                  $$top_srcdir/serna/plugins/tableplugin/qt/.*\.cxx$ \
                  $$top_srcdir/serna/plugins/pyplugin/.*\.cxx$ \
                  $$top_srcdir/serna/plugins/contentmap/.*\.cxx$ \
                  $$top_srcdir/serna/plugins/contentmap/csl/.*\.cxx$ \
                  $$top_srcdir/serna/plugins/contentmap/qt/.*\.cxx$ \
                  $$top_srcdir/serna/plugins/xsltparam/.*\.cxx$ \
                  $$top_srcdir/serna/plugins/xsltparam/qt/.*\.cxx$ \
                  $$top_srcdir/serna/plugins/pyconsole/.*\.cxx$ \
                  $$top_srcdir/serna/plugins/ditatopicref/.*\.cxx$ \
                  $$top_srcdir/serna/plugins/speller/.*\.cxx$ \
                  $$top_srcdir/serna/plugins/speller/qt/.*\.cxx$
                  
SOURCES_PATTERN = $$CHECK_LITERALS_PATTERN \
                  $$top_srcdir/sfworks/common/src/.*\.cpp$ \
                  $$top_srcdir/sfworks/common/src/.*\.cxx$ \
                  $$top_srcdir/sfworks/grove/.*\.cxx$ \
                  $$top_srcdir/sfworks/urimgr/impl/.*\.cxx$ \
                  $$top_srcdir/sfworks/xpath/impl/.*\.cxx$ \
                  $$top_srcdir/sfworks/xpath/impl/expressions/.*\.cxx$ \
                  $$top_srcdir/sfworks/xpath/impl/instances/.*\.cxx$ \
                  $$top_srcdir/sfworks/catmgr/impl/.*\.cxx$ \
                  $$top_srcdir/sfworks/formatter/impl/.*\.cxx$ \
                  $$top_srcdir/sfworks/groveeditor/.*\.cxx$ \
                  $$top_srcdir/sfworks/proputils/.*\.cxx$ \
                  $$top_srcdir/sfworks/qtgrovebuilder/impl/.*\.cxx$ \
                  $$top_srcdir/sfworks/spgrovebuilder/.*\.cxx$ \
                  $$top_srcdir/sfworks/ui/impl/qt/.*\.cxx$ \
                  $$top_srcdir/sfworks/ui/impl/.*\.cxx$ \
                  $$top_srcdir/sfworks/xslt/impl/.*\.cxx$ \
                  $$top_srcdir/sfworks/xslt/impl/instances/.*\.cxx$ \
                  $$top_srcdir/sfworks/xslt/impl/instructions/.*\.cxx$ \
                  $$top_srcdir/sfworks/xslt/impl/result/.*\.cxx$ \
                  $$top_srcdir/sfworks/xs/\.cxx$ \
                  $$top_srcdir/sfworks/xs/complex/.*\.cxx$ \
                  $$top_srcdir/sfworks/xs/components/.*\.cxx$ \
                  $$top_srcdir/sfworks/xs/datatypes/.*\.cxx$ \
                  $$top_srcdir/sfworks/xs/parser/.*\.cxx$ \
                  $$top_srcdir/serna/dist/plugins/dita/.*\.py$ \
                  $$top_srcdir/serna/dist/plugins/dita/dialogs/.*\.py$ \

SUI_SOURCES     = \
                  $$top_srcdir/serna/dist/ui/PlainDocument.sui \
                  $$top_srcdir/serna/dist/ui/StructDocument.sui \
                  $$top_srcdir/serna/dist/ui/EmptyDocument.sui


MSG_SOURCES     = \
                  $$top_srcdir/serna/utils/SernaMessages.msg \
                  $$top_srcdir/sfworks/grove/impl/GroveMessages.msg \
                  $$top_srcdir/sfworks/xpath/impl3/XpathMessages.msg \
                  $$top_srcdir/sfworks/common/CommonMessages.msg \
                  $$top_srcdir/sfworks/groveeditor/impl/GroveEditorMessages.msg \
                  $$top_srcdir/sfworks/xs/XsMessages.msg \
                  $$top_srcdir/sfworks/xslt/impl/XsltMessages.msg \
                  $$top_srcdir/sfworks/formatter/impl/XslMessages.msg \
                  $$top_srcdir/sfworks/editableview/impl/EditableViewMessages.msg \
                  $$top_srcdir/sfworks/catmgr/impl/CatMgrMessages.msg
                  
SPD_SOURCES     = \
                  $$top_srcdir/serna/dist/plugins/docinfo/docinfo.spd \
                  $$top_srcdir/serna/dist/plugins/dita/oasis-dita-task.spd \
                  $$top_srcdir/serna/dist/plugins/dita/oasis-dita-ditamap.spd \
                  $$top_srcdir/serna/dist/plugins/dita/oasis-dita-composite.spd \
                  $$top_srcdir/serna/dist/plugins/dita/oasis-dita-topic.spd \
                  $$top_srcdir/serna/dist/plugins/dita/oasis-dita-concept.spd \
                  $$top_srcdir/serna/dist/plugins/dita/oasis-dita-reference.spd \
                  $$top_srcdir/serna/dist/plugins/hotkeys/hotkeys.spd \
                  $$top_srcdir/serna/dist/plugins/linkvoyager/linkvoyager.spd \
                  $$top_srcdir/serna/dist/plugins/dita-examples/open_topicref.spd \
                  $$top_srcdir/serna/dist/plugins/dita-examples/empty_topic.spd \
                  $$top_srcdir/serna/dist/plugins/dita-examples/ditatopicref_cxx.spd \
                  $$top_srcdir/serna/dist/plugins/dita-examples/ditatopicref_py.spd \
                  $$top_srcdir/serna/dist/plugins/dita-examples/new_topic.spd \
                  $$top_srcdir/serna/dist/plugins/updateonsave/updateonsavepy.spd \
                  $$top_srcdir/serna/dist/plugins/updateonsave/updateonsave.spd \
                  $$top_srcdir/serna/dist/plugins/customcontent/customcontent.spd \
                  $$top_srcdir/serna/dist/plugins/tableplugin/tableplugin.spd \
                  $$top_srcdir/serna/dist/plugins/pyplugin/pyconsole.spd \
                  $$top_srcdir/serna/dist/plugins/pyplugin-tutorial/check_empty_para.spd \
                  $$top_srcdir/serna/dist/plugins/pyplugin-tutorial/hello_world.spd \
                  $$top_srcdir/serna/dist/plugins/pyplugin-tutorial/link_navigator.spd \
                  $$top_srcdir/serna/dist/plugins/pyplugin-tutorial/insert_address.spd \
                  $$top_srcdir/serna/dist/plugins/contentmap/contentmap.spd \
                  $$top_srcdir/serna/dist/plugins/xsltparam/xsltparam.spd \
                  $$top_srcdir/serna/dist/plugins/syntext/element-help.spd \
                  $$top_srcdir/serna/dist/plugins/speller/speller.spd \
                  $$top_srcdir/serna/dist/plugins/speller/speller_startup.spd
                  
SCC_SOURCES     = \
                  $$top_srcdir/serna/dist/plugins/docbook/dblite05.scc \
                  $$top_srcdir/serna/dist/plugins/docbook/dblite05htmlents.scc \
                  $$top_srcdir/serna/dist/plugins/docbook/dblite05noents.scc \
                  $$top_srcdir/serna/dist/plugins/docbook/dbk42.scc \
                  $$top_srcdir/serna/dist/plugins/docbook/dbk43.scc \
                  $$top_srcdir/serna/dist/plugins/docbook/dbk44.scc \
                  $$top_srcdir/serna/dist/plugins/dita/oasis-dita-topic-10.scc \
                  $$top_srcdir/serna/dist/plugins/dita/oasis-dita-concept-10.scc \
                  $$top_srcdir/serna/dist/plugins/dita/oasis-dita-reference-10.scc \
                  $$top_srcdir/serna/dist/plugins/dita/oasis-dita-task-10.scc \
                  $$top_srcdir/serna/dist/plugins/dita/oasis-dita-composite-10.scc \
                  $$top_srcdir/serna/dist/plugins/dita/oasis-dita-topic-11.scc \
                  $$top_srcdir/serna/dist/plugins/dita/oasis-dita-concept-11.scc \
                  $$top_srcdir/serna/dist/plugins/dita/oasis-dita-reference-11.scc \
                  $$top_srcdir/serna/dist/plugins/dita/oasis-dita-task-11.scc \
                  $$top_srcdir/serna/dist/plugins/dita/oasis-dita-composite-11.scc \
                  $$top_srcdir/serna/dist/plugins/tei/tei.scc \
                  $$top_srcdir/serna/dist/plugins/tei/teifull.scc \
                  $$top_srcdir/serna/dist/plugins/idealliance/conference-paper-1-3.scc \
                  $$top_srcdir/serna/dist/plugins/idealliance/conference-paper.scc \
                  $$top_srcdir/serna/dist/plugins/syntext/timesheet.scc \
                  $$top_srcdir/serna/dist/plugins/syntext/element-help.scc

CSL_SOURCES     = \
                  $$top_srcdir/serna/dist/plugins/docbook/docbook.csl \
                  $$top_srcdir/serna/dist/plugins/dita/dita-topic.csl \
                  $$top_srcdir/serna/dist/plugins/dita/dita-map.csl \
                  $$top_srcdir/serna/dist/plugins/syntext/default/default-content-map.csl
                  
UI_DIRS		+=    $$top_srcdir/sfworks/ui/impl/qt 
UI_DIRS		+=    $$top_srcdir/serna/docview/qt 
UI_DIRS		+=    $$top_srcdir/serna/core/qt 
UI_DIRS		+=    $$top_srcdir/serna/plaineditor/qt 
UI_DIRS		+=    $$top_srcdir/serna/structeditor/qt 

UI_DIRS         += \
                  $$top_srcdir/serna/plugins/customcontent \
                  $$top_srcdir/serna/plugins/docinfo \
                  $$top_srcdir/serna/plugins/hotkeys \
                  $$top_srcdir/serna/plugins/pyconsole \
                  $$top_srcdir/serna/plugins/speller/qt \
                  $$top_srcdir/serna/plugins/tableplugin/qt \
                  $$top_srcdir/serna/plugins/xsltparam/qt \
                  $$top_srcdir/serna/dist/plugins/syntext/element-help/py \
                  $$top_srcdir/serna/dist/plugins/dita/dialogs \
                  $$top_srcdir/serna/dist/plugins/dita/dita-reference/py/sernaSpecific/serna4/ui

UIC	    =   $(THIRD_DIR)/bin/uic
LUPDATE	=	$(THIRD_DIR)/bin/lupdate
LRELEASE=	$(THIRD_DIR)/bin/lrelease
PYTHON  =	$(THIRD_DIR)/bin/python

MSG2CPP         = $(srcdir)/msg2cpp.py
SUI2CPP         = $(srcdir)/make-tr-ui.xsl
SPD2CPP         = $(srcdir)/make-tr-ui.xsl
SCC2CPP         = --stringparam context-string scc $(srcdir)/make-tr-scc.xsl
CSL2CPP         = $(srcdir)/make-tr-csl.xsl

SERNA_TRANSLATIONS = ru untranslated de fr nl it zh-cn ig nb-no se

EXTRA_TEMPLATES = clean.t
 
