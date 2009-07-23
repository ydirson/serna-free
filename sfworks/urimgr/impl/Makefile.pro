#
#

TEMPLATE = serna
LIBRARIES = urimgr
CONFIG += moc qtexternal

INCLUDEPATH =	$(THIRD_DIR)/qt/include/QtCore; \
                $(CLIB_SRC); 

win32:DEFINES += BUILD_URI URI_API
#win32:WIN32_RESOURCE	= $(CLIB_SRC)/ui/UiVersion.rc

LIBS = 	\
    $(CLIB_LIB)/common \
    $(CLIB_LIB)/grove \
    $(CLIB_LIB)/dav \
    $(CLIB_LIB)/catmgr \
    $(CLIB_LIB)/spgrovebuilder 
    
qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore

HEADERS_PATTERN = \.h$ qt/.*\.h$
SOURCES_PATTERN = \.cxx$ qt/.*\.cxx$

EXCLUDE_PATTERN = \bt\..*

EXTRA_TEMPLATES = genlist
