#
#

TEMPLATE = serna
LIBRARIES = dav
CONFIG += moc qtexternal

INCLUDEPATH =	$(CLIB_SRC);\
                $(THIRD_DIR)/qt/include/QtCore;

win32:DEFINES += BUILD_DAV DAV_API

LIBS = 	    $(CLIB_LIB)/common 
    
qtexternal:LIBS += $(THIRD_DIR)/lib/$$QTLIB

HEADERS_PATTERN = \.h$ impl/\.h$
SOURCES_PATTERN = impl/\.cxx$

EXTRA_TEMPLATES = genlist
