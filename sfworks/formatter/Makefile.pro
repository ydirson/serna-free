#
#

TEMPLATE = serna
EXTRA_TEMPLATES = genlist
LIBRARIES = formatter

INCLUDEPATH =   $(CLIB_SRC); \
                .; \
                $(top_builddir)/sfworks; \

LIBS =          $(CLIB_LIB)/common \
                $(CLIB_LIB)/grove \
                $(CLIB_LIB)/sxslt 

darwin:LIBS += $(CLIB_LIB)/xpath \
               $(CLIB_LIB)/urimgr \
               $(CLIB_LIB)/spgrovebuilder \

USE = QtCore sp

HEADERS_PATTERN =         \.h$ \
                          impl/.*\.h$ \
                          impl/areas/.*\.h$ \
                          impl/fos/.*\.h$ \
                          impl/precepts/.*\.h$ \
                          impl/properties/.*\.h$ \
                          impl/traits/.*\.h$ 

SOURCES_PATTERN =         impl/\.cxx$ \
                          impl/areas/.*\.cxx$ \
                          impl/fos/.*\.cxx$ \
                          impl/precepts/.*\.cxx$ \
                          impl/properties/.*\.cxx$ \
                          impl/traits/.*\.cxx$ 

DEFINES *= GROVE_DEBUG
DEFINES *= BUILD_FORMATTER FORMATTER_API

ALL_DEPS                += $(msg_h)

SOURCE_SUFFIXES  = msg
msg_PATTERN = impl/.*\.msg$
