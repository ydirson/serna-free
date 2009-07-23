#
#

TEMPLATE = serna
LIBRARIES = xslt
CONFIG += qtexternal

win32:DEFINES += BUILD_XSLT XSLT_API

xpath3:DEFINES += USE_XPATH3 NEW_TEMPLATE_SELECTOR

INCLUDEPATH = 	\
                $$QT_INC; \
                $(top_builddir)/sfworks; \
                $(srcdir); \
                $(CLIB_SRC); 

LIBS        = \
              $(CLIB_LIB)/common \
              $(CLIB_LIB)/grove \
              $(CLIB_LIB)/xpath \
              $(CLIB_LIB)/spgrovebuilder \
              $(CLIB_LIB)/urimgr 

darwin:LIBS += $(CLIB_LIB)/qtgrovebuilder
                                
qtexternal:LIBS += $(THIRD_DIR)/lib/$$QTLIB
darwin:LIBS += $(THIRD_DIR)/lib/sp

HEADERS_PATTERN = \.h$ \
                  patterns/.*\.h$ \
                  functions/.*\.h$ \
                  instructions/.*\.h$ \
                  instances/.*\.h$ \
                  result/.*\.h$

SOURCES_PATTERN = \.cxx$ \
                  patterns/.*\.cxx$ \
                  functions/.*\.cxx$ \
                  instructions/.*\.cxx$ \
                  instances/.*\.cxx$ \
                  result/.*\.cxx$
                  
BISON_INPUT=$(srcdir)/xsltParser.y
EXTRA_TEMPLATES += genlist extra/bison
#SOURCES += $$PARSER.cpp
MODULE_NAME = xslt

ALL_DEPS                += $(msg_h)
