#
#

TEMPLATE = serna
LIBRARIES = xs

DEFINES *= BUILD_XS XS_API
debug:DEFINES *= XS_DEBUG XS_FSM_DEBUG

use_python:DEFINES += USE_PYTHON

#DEFINES += XS_DEBUG
# mt:DEFINES += XS_THREADS
#sun-port:DEFINES += NO_XPATH

#win32:WIN32_RESOURCE = $(srcdir)/XsVersion.rc

NOTPARALLEL = 1

INCLUDEPATH = 	\
                .; \
                ..; \
                $(CLIB_SRC); \
                $(srcdir); \
                $(top_srcdir)/sfworks;

USE         = QtCore antlr python
darwin:USE += sp

LIBS                =        \
                                $(CLIB_LIB)/urimgr \
                                $(CLIB_LIB)/common \
                                $(CLIB_LIB)/grove \
                                $(CLIB_LIB)/spgrovebuilder \
                                $(CLIB_LIB)/xpath \
                                $(CLIB_LIB)/dav


use_python:LIBS        +=        $(THIRD_DIR)/lib/python
win32:LIBS += wsock32.lib

SOURCES_PATTERN =         \.cxx$ \
                                        complex/.*\.cxx$ \
                                        components/.*\.cxx$ \
                                        datatypes/.*\.cxx$ \
                                        parser/.*\.cxx$

HEADERS_PATTERN =         \.h$ \
                                        complex/.*\.h$ \
                                        components/.*\.h$\
                                        datatypes/.*\.h$ \
                                        parser/.*\.h$

PARSER=GroveAstParser
ANTLR_FLAGS= 
ANTLR_INPUT=$(srcdir)/parser/XmlSchema.g

TARGET_SUFFIXES += cpp
EXTRA_TEMPLATES += genlist extra/newantlr
ALL_DEPS                += $(msg_h)

MODULE_NAME        = XS
PRODUCT_NAME = XML Schema Validator

linux:JAVA = java
