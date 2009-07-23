#
#

TEMPLATE = serna
LIBRARIES = core
CONFIG += moc qtexternal

DEFINES *= MULTI_THREADED
DEFINES *= BUILD_CORE

win32:DEFINES += QT_SHARED
darwin:TMAKE_LFLAGS *= -Wl,-search_paths_first -Wl,-multiply_defined -Wl,suppress -flat_namespace

INCLUDEPATH =   \
                .; \
                ..; \
                $(srcdir); \
                $(srcdir)/..; \
                $(CLIB_SRC); \
                $(top_builddir)/sfworks; \
                $(srcdir)/qt/qtsingleapp; \
                $(THIRD_DIR)/qt/include/QtCore; \
                $(THIRD_DIR)/qt/include/QtGui; 

#kdexecutor:INCLUDEPATH += $(THIRD_DIR)/kdexecutor/include;

darwin:LIBS += -framework Carbon

LIBS += \
        $(CLIB_LIB)/formatter \
        $(CLIB_LIB)/editableview \
        $(CLIB_LIB)/common \
        $(CLIB_LIB)/ui \
        $(CLIB_LIB)/proputils \
        $(CLIB_LIB)/grove \
        $(CLIB_LIB)/proputils \
        $(CLIB_LIB)/spgrovebuilder \
        $(CLIB_LIB)/xpath \
        $(CLIB_LIB)/sxslt \
        $(CLIB_LIB)/groveeditor \
        $(CLIB_LIB)/urimgr \
        $(CLIB_LIB)/xs \
        $(CLIB_LIB)/dav \
        $(top_builddir)/serna/lib/utils \
        $(top_builddir)/serna/lib/docview \
        $(top_builddir)/serna/lib/structeditor \
        $(top_builddir)/serna/lib/plaineditor \
        $(top_builddir)/serna/lib/docutils \
        $(THIRD_DIR)/lib/sp

win32:LIBS +=  Comdlg32.lib User32.lib shell32.lib \
	       Gdi32.lib Advapi32.lib Imm32.lib \
               Winspool.lib Winmm.lib ole32.lib ws2_32.lib 

HEADERS_PATTERN =   \.h$ \
                    qt/.*\.h$ \
                    qt/qtsingleapp/.*\.h$

SOURCES_PATTERN =   \.cxx$ \
                    impl/.*\.cxx$ \
                    qt/.*\.cxx$ \
                    qt/qtsingleapp/qtsingleapplication\.cpp$
                    
linux:SOURCES_PATTERN  += qt/qtsingleapp/qtsingleapplication_x11\.cpp$
sunos:SOURCES_PATTERN *= qt/qtsingleapp/qtsingleapplication_x11\.cpp$
darwin:SOURCES_PATTERN += qt/qtsingleapp/qtsingleapplication_mac\.cpp$

SOURCES_PATTERN  += \
                            impl/unix/.*\.cxx$

win32:SOURCES_PATTERN  += \
                            qt/qtsingleapp/qtsingleapplication_win\.cpp$ 

win32:EXCLUDE_PATTERN = qt/EULA.*

RES2CXX_LIST   = $(srcdir)/icons-list
RES2CXX_OUTPUT = core_resource_data.cpp
RES2CXX_BASE   = $(srcdir)/../icons

UI_DIRS	       = $$srcdir/qt
ui_PATTERN      = $$UI_DIRS/.*\.ui$
ALL_DEPS += $(msg_h) core_resource_data.cpp

qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore \
                   $(THIRD_DIR)/lib/QtGui 

EXTRA_TEMPLATES = genlist resource2cxx
