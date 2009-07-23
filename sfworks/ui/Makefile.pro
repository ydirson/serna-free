
TEMPLATE = serna
LIBRARIES = ui
CONFIG += moc qtexternal

INCLUDEPATH =	$(CLIB_SRC); \
                .; \
                $(THIRD_DIR)/qt/include/QtCore; \
                $(THIRD_DIR)/qt/include/QtGui; \
                $(THIRD_DIR)/qtpropertybrowser/include;

win32:DEFINES += BUILD_UI UI_API QT_QTPROPERTYBROWSER_IMPORT
#win32:WIN32_RESOURCE	= $(CLIB_SRC)/ui/UiVersion.rc

LIBS = 	\
        $(CLIB_LIB)/common \
        $(CLIB_LIB)/dav \
        $(THIRD_DIR)/lib/sp 
    
qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore \
                   $(THIRD_DIR)/lib/QtGui \
                   $(THIRD_DIR)/lib/qtpropertybrowser

HEADERS_PATTERN =   .h$ \
                    impl/.*\.h$ \
                    impl/qt/.*\.h$

SOURCES_PATTERN =   impl/.*\.cxx$ \
                    impl/qt/.*\.cxx$

EXTRA_TEMPLATES = genlist
