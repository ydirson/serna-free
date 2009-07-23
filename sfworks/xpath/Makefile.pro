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
              $(srcdir)/impl3; \
              $(CLIB_SRC); \
              $(THIRD_DIR)/qt/include/QtCore

LIBS = \
              $(CLIB_LIB)/common \
              $(CLIB_LIB)/grove
              
qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore

HEADERS_PATTERN = 	\.h$ \
			expressions/.*\.h$ 

SOURCES_PATTERN = 	impl3/.*\.cxx$ \
			impl3/expressions/.*\.cxx$

BISON_INPUT=$(srcdir)/impl3/xpathParser.y
EXTRA_TEMPLATES += genlist extra/bison 
#SOURCES += $$PARSER.cpp
MODULE_NAME = xpath
ALL_DEPS                += $(msg_h)

SOURCE_SUFFIXES  = msg
msg_PATTERN = impl3/.*\.msg$
