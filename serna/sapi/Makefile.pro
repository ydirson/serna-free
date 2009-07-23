#
#

TEMPLATE = serna
LIBRARIES = sapi
CONFIG += qtexternal
release:CONFIG *= dynamic dll

linux:STRIP_BINARY = 1

DEFINES += TRACE_ON
DEFINES += BUILD_SAPI
DEFINES += DOX_CPP_ONLY

INCLUDEPATH = 	$(srcdir)/..; \
                $(THIRD_DIR)/qt/include/QtCore; \
                $(THIRD_DIR)/qt/include/QtGui; \
                $(top_builddir)/sfworks; \
                $(top_builddir)/serna; \
                $(top_srcdir)/serna; \
                $(CLIB_SRC) 

debug:LIBS  =   $(CLIB_LIB)/common \
                $(CLIB_LIB)/proputils \
                $(CLIB_LIB)/ui \
                $(CLIB_LIB)/grove \
                $(CLIB_LIB)/groveeditor \
                $(CLIB_LIB)/spgrovebuilder \
                $(CLIB_LIB)/editableview \
                $(CLIB_LIB)/formatter \
                $(CLIB_LIB)/xpath \
                $(CLIB_LIB)/dav \
                $(CLIB_LIB)/sxslt \
                $(top_builddir)/serna/lib/utils \
                $(top_builddir)/serna/lib/docview \
                $(top_builddir)/serna/lib/structeditor

release:win32_LIBS = $(top_builddir)/serna/lib/serna
debug:win32_LIBS   = $$LIBS
win32:LIBS  =   $$win32_LIBS
release:DEFINES -= SERNA_DLL SFWORKS_DLL

darwin:TMAKE_LFLAGS *= -Wl,-single_module

qtexternal:LIBS += $(THIRD_DIR)/lib/QtCore \
                   $(THIRD_DIR)/lib/QtGui

HEADERS_PATTERN = \.h$ \
                  common/.*\.h$ \
                  utils/.*\.h$ \
                  app/.*\.h$ \
                  common/impl/.*\.h$ \
                  app/impl/.*\.h$ \
                  utils/impl/.*\.h$ \
                  grove/.*\.h$ \
                  grove/impl/.*\.h$ 

SOURCES_PATTERN = \
                  common/impl/.*\.cxx$ \
                  utils/impl/.*\.cxx$ \
                  app/impl/.*\.cxx$ \
                  grove/impl/.*\.cxx$ 
                  
unix:SOURCES_PATTERN += app/impl/unix/.*\.cxx$
win32:SOURCES_PATTERN += app/impl/win32/.*\.cxx$

EXTRA_TEMPLATES += genlist shlib_undef sapi_version
MODULE_NAME      = sapi

# darwin:FIX_INSTALL_NAMES = libqt-mt.3.dylib @executable_path/../Frameworks/libqt-mt.3.dylib
