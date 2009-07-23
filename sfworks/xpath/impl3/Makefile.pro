#
#

TEMPLATE = serna
LIBRARIES = xpath
CONFIG += qtexternal

DEFINES += BUILD_XPATH
DEFINES += XPATH_API

INCLUDEPATH = .;	\
              $$QT_INC; \
              $(top_builddir)/sfworks; \
              $(srcdir); \
              $(CLIB_SRC);

LIBS = \
              $(CLIB_LIB)/common \
              $(CLIB_LIB)/grove
              
qtexternal:LIBS += $(THIRD_DIR)/lib/$$QTLIB

HEADERS_PATTERN = 	\.h$ \
			expressions/.*\.h$ 

SOURCES_PATTERN = 	\.cxx$ \
			expressions/.*\.cxx$

BISON_INPUT=$(srcdir)/xpathParser.y
EXTRA_TEMPLATES += genlist extra/bison 
#SOURCES += $$PARSER.cpp
MODULE_NAME = xpath
ALL_DEPS                += $(msg_h)
