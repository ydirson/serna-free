##
#

TEMPLATE = serna
LIBRARIES = sceneareaset
CONFIG += moc qtexternal
CONFIG += noqtmml

linux:TMAKE_CXXFLAGS += -Wno-deprecated

INCLUDEPATH =	\
                $(THIRD_DIR)/qt/include/QtCore; \
                $(THIRD_DIR)/qt/include/QtGui; \
                $(THIRD_DIR)/qt/include/QtXml; \
                $(CLIB_SRC); \
                $(top_builddir)/sfworks; \
                $(CLIB_SRC)/sceneareaset/mml2; 

win32:DEFINES += BUILD_SCENEAREASET SCENEAREASET_API

LIBS += \
        $(CLIB_LIB)/formatter \
        $(CLIB_LIB)/editableview \
        $(CLIB_LIB)/sxslt \
        $(CLIB_LIB)/spgrovebuilder \
        $(CLIB_LIB)/grove \
        $(CLIB_LIB)/dav \
        $(CLIB_LIB)/ui \
        $(CLIB_LIB)/groveeditor \
        $(CLIB_LIB)/common 

darwin:LIBS += \
               $(CLIB_LIB)/xpath \
               $(CLIB_LIB)/urimgr \
               $(CLIB_LIB)/catmgr \
               $(THIRD_DIR)/lib/sp

HEADERS_PATTERN =  \.h$

qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore $(THIRD_DIR)/lib/QtGui $(THIRD_DIR)/lib/QtXml

EXTRA_TEMPLATES = genlist
EXCLUDE_PATTERN = main\.cxx mml2/mml_tables\.cxx mml2/mmlwidget\.cxx

SOURCES_PATTERN = \.cxx$

noqtmml:DEFINES += NO_QT_MML
noqtmml:SOURCES_PATTERN += mml2/.*\.cxx$
noqtmml:INCLUDEPATH += $(srcdir);
