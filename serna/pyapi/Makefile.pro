#
#

TEMPLATE = serna

# The last 'c' is intentional, this is a SIP convention for C modules
LIBRARIES = SernaApiCore

CONFIG += qtexternal dynamic dll notaplugin
unix:TMAKE_CXXFLAGS += -Wno-uninitialized

# SIP/PyQt settings

linux:SIP_PLATFORM   = WS_X11
linux:SIP_PLATPKG    = x11
sunos:SIP_PLATFORM   = WS_X11
sunos:SIP_PLATPKG    = x11
darwin:SIP_PLATFORM  = WS_MACX
darwin:SIP_PLATPKG   = mac

SIP_PLATPKG    = x11

win32:SIP_PLATFORM   = WS_WIN 
#win32:SIP_PLATPKG    = win

SIP_EXCL    = -x VendorID -x PyQt_OpenSSL -x PyQt_NoPrintRangeBug   
linux:SIP_EXCL += -x PyQt_SessionManager
sunos:SIP_EXCL += -x PyQt_SessionManager
darwin:SIP_EXCL += -x PyQt_SessionManager

SIP_QT_VERS = Qt_4_4_1

###

SIP_MODULE  = SernaApiCore
PYQT_DIR    = $(THIRD_DIR)/pyqt/$(PYQT_VER)/PyQt-$$SIP_PLATPKG-gpl-$(PYQT_VER)/sip 
SIP_OPTIONS = -c. -w -j1 -t $$SIP_QT_VERS -t $$SIP_PLATFORM $$SIP_EXCL -I $(PYQT_DIR)
SIP_SOURCES = $(srcdir)/SernaApiCore.sip \
              $(srcdir)/common/WrappedObject.sip \
              $(srcdir)/common/SString.sip \
              $(srcdir)/common/PropertyNode.sip \
              $(srcdir)/common/Command.sip \
              $(srcdir)/common/Url.sip \
              $(srcdir)/common/SimpleWatcher.sip \
              $(srcdir)/app/SernaConfig.sip \
              $(srcdir)/app/SernaClipboard.sip \
              $(srcdir)/app/UiItem.sip \
              $(srcdir)/app/UiCommand.sip \
              $(srcdir)/app/UiDocument.sip \
              $(srcdir)/app/DocumentPlugin.sip \
              $(srcdir)/app/StructEditor.sip \
              $(srcdir)/app/XsltEngine.sip \
              $(srcdir)/app/PluginLoader.sip \
              $(srcdir)/app/DocTemplateHolder.sip \
              $(srcdir)/app/SernaDoc.sip \
              $(srcdir)/app/MessageView.sip \
              $(srcdir)/grove/GroveNodes.sip \
              $(srcdir)/grove/GroveWatcher.sip \
              $(srcdir)/grove/Grove.sip \
              $(srcdir)/grove/GrovePos.sip \
              $(srcdir)/grove/GroveCommand.sip \
              $(srcdir)/grove/GroveEditor.sip \
              $(srcdir)/grove/GroveIdManager.sip \
              $(srcdir)/grove/GroveStripInfo.sip \
              $(srcdir)/grove/XpathUtils.sip 

DEFINES += TRACE_ON
win32:DEFINES += BUILD_PYAPI

INCLUDEPATH = 	$(srcdir)/..; \
                $(THIRD_DIR)/qt/include/QtCore; \
                $(THIRD_DIR)/qt/include/QtGui; \
                $(top_builddir)/sfworks; \
                $(top_builddir)/serna; \
                $(CLIB_SRC);\
                $(THIRD_DIR)/python/include; \
                $(THIRD_DIR)/sip

win32:LIBS     +=   \
                    $(THIRD_DIR)/lib/python
                    
USE_SAPI = 1
NO_VER_SFX = 1
win32:LIBS += $(THIRD_DIR)/lib/sip

qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore \
                   $(THIRD_DIR)/lib/QtGui

SIP = $(THIRD_DIR)/bin/sip

SOURCES_PATTERN = app/.*\.cxx$
HEADERS_PATTERN = app/.*\.cxx$

SOURCES += sip$${SIP_MODULE}part0.cpp

EXTRA_TEMPLATES += genlist sip plugins/plugins_common
CLEAN_FILES += sipAPISernaApiCore.h sipSernaApiCorepart0.cpp

