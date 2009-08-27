#
#

TEMPLATE = serna

# The last 'c' is intentional, this is a SIP convention for C modules
LIBRARIES = SernaApiCore

CONFIG += qtexternal dynamic dll notaplugin
unix:TMAKE_CXXFLAGS += -Wno-uninitialized

SIP_MODULE  = SernaApiCore
PYQT_DIR    = $(THIRD_DIR)/pyqt/$(PYQT_VER)/PyQt-$$SIP_PLATPKG-gpl-$(PYQT_VER)/sip

SIP_OPTIONS = -c. -w -j1 -I $(PYQT_DIR)

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
                $(top_builddir)/sfworks; \
                $(top_builddir)/serna; \
                $(CLIB_SRC);\

USE = QtCore QtGui sp python sip

USE_SAPI = 1
NO_VER_SFX = 1

SIP = $(THIRD_DIR)/bin/sip

SOURCES_PATTERN = app/.*\.cxx$
HEADERS_PATTERN = app/.*\.cxx$

SOURCES += sip$${SIP_MODULE}part0.cpp

EXTRA_TEMPLATES += genlist sip plugins/plugins_common
CLEAN_FILES += sipAPISernaApiCore.h sipSernaApiCorepart0.cpp
