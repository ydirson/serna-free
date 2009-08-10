
TEMPLATE = serna
LIBRARIES = editableview

win32:DEFINES += BUILD_EDITABLEVIEW EDITABLEVIEW_API

INCLUDEPATH = \
        .; \
        ..; \
        $(CLIB_SRC); \
        $(srcdir); \
        $(top_builddir)/sfworks; 

LIBS = \
        $(CLIB_LIB)/common \
        $(CLIB_LIB)/grove \
        $(CLIB_LIB)/groveeditor \
        $(CLIB_LIB)/formatter

darwin:LIBS += \
               $(CLIB_LIB)/xpath \
               $(CLIB_LIB)/sxslt \
               $(CLIB_LIB)/urimgr \
               $(CLIB_LIB)/spgrovebuilder
               
USE = QtCore QtGui
darwin:USE += sp
 
SOURCES_PATTERN = impl/\.*\.cxx$

HEADERS_PATTERN = \.h$ \
                  impl/.*\.h$ \

EXTRA_TEMPLATES = genlist
ALL_DEPS       += $(msg_h)

SOURCE_SUFFIXES  = msg
msg_PATTERN = impl/.*\.msg$
