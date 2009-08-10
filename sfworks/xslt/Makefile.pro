#
#

TEMPLATE = serna
LIBRARIES = sxslt

win32:DEFINES += BUILD_XSLT XSLT_API XPATH_API

xpath3:DEFINES += USE_XPATH3 NEW_TEMPLATE_SELECTOR

INCLUDEPATH =   \
                $(top_builddir)/sfworks; \
                $(srcdir)/impl; \
                $(CLIB_SRC)

LIBS        = \
              $(CLIB_LIB)/common \
              $(CLIB_LIB)/grove \
              $(CLIB_LIB)/xpath \
              $(CLIB_LIB)/spgrovebuilder \
              $(CLIB_LIB)/urimgr 

USE             += QtCore
darwin:USE      += sp

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

MODULE_NAME = xslt
MSGGEN_MODULE = xslt

ALL_DEPS                += $(msg_h)
