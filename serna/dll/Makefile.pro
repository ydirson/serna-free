TEMPLATE = $$srcdir/bigdll

BASEDIRS = sfworks serna

sfworks_MODULES = common grove spgrovebuilder qtgrovebuilder catmgr \
                  groveeditor urimgr xpath xslt xs formatter editableview \
                  sceneareaset proputils ui dav
                  
serna_MODULES = utils structeditor plaineditor \
                       docutils core docview
                       
BASEDIR = $$top_builddir/serna
SAPI    = sapi
MODULES = plugins/ahplugin app
          
DESTDIR =  $(top_builddir)/serna/lib
BIGDLL  =  serna

debug:WIN32_LIBDBG = d
debug:W32_LINK_FLAGS = -debug
release:W32_LINK_FLAGS = -debug -release -pdb:$(@R).pdb \
                         -subsystem:windows 
# -def:$(srcdir)\bigdll.def

win32:LIBDBG = $$WIN32_LIBDBG

win32:LIBS = \
               $(THIRD_DIR)/lib/qscintilla2.lib \
               $(THIRD_DIR)/lib/sp.lib \
               $(THIRD_DIR)/lib/antlr.lib \
               $(THIRD_DIR)/lib/Qt3Support4.lib \
               $(THIRD_DIR)/lib/QtAssistantClient4.lib \
               $(THIRD_DIR)/lib/QtCore4.lib \
               $(THIRD_DIR)/lib/QtGui4.lib \
               $(THIRD_DIR)/lib/QtNetwork4.lib \
               $(THIRD_DIR)/lib/QtPropertyBrowser.lib \
               $(THIRD_DIR)/lib/QtXml4.lib
               
win32:SYS_LIBS += \
                  msvcprt$$LIBDBG.lib \
                  msvcrt$$LIBDBG.lib \
                  advapi32.lib gdi32.lib shell32.lib ole32.lib oleaut32.lib

linux:LIBS      = -lqt-mt -lsp -lantlr -lqassistantclient
linux:SYS_LIBS  = -ldl -lutil

win32:LINK_FLAGS = -nologo -out:$@ -dll $$W32_LINK_FLAGS
unix:LINK_FLAGS = -shared -o $@ \
                  -L$(THIRD_DIR)/lib

darwin:LINK_FLAGS = -dynamiclib -o $@ \
                  -L$(THIRD_DIR)/lib
