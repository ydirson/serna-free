#
#

TEMPLATE = serna
LIBRARIES = sxslt
CONFIG += qtexternal

win32:DEFINES += BUILD_XSLT XSLT_API XPATH_API

xpath3:DEFINES += USE_XPATH3 NEW_TEMPLATE_SELECTOR

INCLUDEPATH = 	\
                $$QT_INC; \
                $(top_builddir)/sfworks; \
                $(srcdir)/impl; \
                $(CLIB_SRC); \
                $(THIRD_DIR)/qt/include/QtCore

LIBS        = \
              $(CLIB_LIB)/common \
              $(CLIB_LIB)/grove \
              $(CLIB_LIB)/xpath \
              $(CLIB_LIB)/spgrovebuilder \
              $(CLIB_LIB)/urimgr 

qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore
darwin:LIBS += $(THIRD_DIR)/lib/sp

HEADERS_PATTERN = \.h$ \
                  impl/.*\.h$ \
                  impl/patterns/.*\.h$ \
                  impl/functions/.*\.h$ \
                  impl/instructions/.*\.h$ \
                  impl/instances/.*\.h$ \
                  impl/result/.*\.h$

SOURCES_PATTERN = \.cxx$ \
                  impl/.*\.cxx$ \
                  impl/patterns/.*\.cxx$ \
                  impl/functions/.*\.cxx$ \
                  impl/instructions/.*\.cxx$ \
                  impl/instances/.*\.cxx$ \
                  impl/result/.*\.cxx$

msg_PATTERN = \
              impl/.*\.msg
                  
BISON_INPUT=$(srcdir)/impl/xsltParser.y
EXTRA_TEMPLATES += genlist extra/bison
#SOURCES += $$PARSER.cpp
MODULE_NAME = xslt
MSGGEN_MODULE = xslt

ALL_DEPS                += $(msg_h)
