
TEMPLATE = serna
LIBRARIES = editableview
CONFIG += qtexternal

win32:DEFINES += BUILD_EDITABLEVIEW EDITABLEVIEW_API

INCLUDEPATH = \
        $(THIRD_DIR)/qt/include/QtCore; \
        $(THIRD_DIR)/qt/include/QtGui; \
        .; \
        ..; \
        $(CLIB_SRC); \
        $(srcdir); \
	$(top_builddir)/sfworks; 

LIBS = 			\
        $(CLIB_LIB)/common \
        $(CLIB_LIB)/grove \
        $(CLIB_LIB)/groveeditor \
        $(CLIB_LIB)/formatter \

darwin:LIBS += \
               $(CLIB_LIB)/xpath \
               $(CLIB_LIB)/sxslt \
               $(CLIB_LIB)/urimgr \
               $(CLIB_LIB)/spgrovebuilder \
               $(THIRD_DIR)/lib/sp
       
LIBS += $(THIRD_DIR)/lib/QtCore \
        $(THIRD_DIR)/lib/QtGui

SOURCES_PATTERN = impl/\.*\.cxx$

HEADERS_PATTERN = \.h$ \
                  impl/.*\.h$ \

EXTRA_TEMPLATES = genlist
ALL_DEPS       += $(msg_h)

SOURCE_SUFFIXES  = msg
msg_PATTERN = impl/.*\.msg$
