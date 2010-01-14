#

TEMPLATE = serna
PROGRAMS = serna
DIST2    = 1
DEFINES *= MULTI_THREADED
CONFIG  *= qtgui
CONFIG  *= moc
#release:CONFIG *= dynamic

win32:CONSOLE     =  console
release:CONFIG   -= $$CONSOLE

win32:DISTDIR = $(top_builddir)\..\dist

static:TMAKE_LFLAGS  += $$static_LFLAGS
win32:TMAKE_LFLAGS += -fixed:no -incremental:no

win32:DEFINES   -= SERNA_DLL SFWORKS_DLL

profile:w32_lflags   = -fixed:no -incremental:no
profile:TMAKE_LFLAGS *= $$w32_lflags

darwin_LFLAGS *= -flat_namespace
release:darwin_LFLAGS *=  -Wl,-framework,Carbon
darwin:TMAKE_LFLAGS *= -Wl,-search_paths_first -Wl,-multiply_defined,suppress $$darwin_LFLAGS
sunos:TMAKE_LIBS -= -L/usr/local/lib

INCLUDEPATH =   \
                .; \
                ..; \
                $(srcdir); \
                $(srcdir)/..; \
                $(top_builddir)/sfworks; \
                $(CLIB_SRC);

unix:SG = -\(
unix:EG = -\)

darwin:SG =
darwin:EG =

static:START_GROUP = $$SG
static:END_GROUP = $$EG

darwin:LIBS += -framework Carbon

LIBS += $$START_GROUP \
        $(top_builddir)/serna/lib/core \
        $(top_builddir)/serna/lib/docview \
        $(top_builddir)/serna/lib/structeditor \
        $(top_builddir)/serna/lib/plaineditor \
        $(top_builddir)/serna/lib/docutils \
        $$END_GROUP \
        $(top_builddir)/serna/lib/utils

sceneareaset:AREAVIEWLIB  = $(CLIB_LIB)/sceneareaset
canvasareaset:AREAVIEWLIB = $(CLIB_LIB)/canvasareaset

dynamic:LIBS += \
        $(CLIB_LIB)/common \
        $(CLIB_LIB)/ui \
        $(CLIB_LIB)/spgrovebuilder \
        $(CLIB_LIB)/grove \
        $(CLIB_LIB)/dav \
        $(CLIB_LIB)/catmgr \
        $(CLIB_LIB)/xpath \
        $(CLIB_LIB)/sxslt \
        $(CLIB_LIB)/formatter \
        $(CLIB_LIB)/groveeditor \
        $(CLIB_LIB)/urimgr \
        $(CLIB_LIB)/xs \
        $(CLIB_LIB)/editableview \
        $(CLIB_LIB)/proputils \
        $$AREAVIEWLIB

USE     =   \
            QtGui \
            QtCore \
            Qt3Support \
            QtXml \
            QtNetwork \
            QtSvg \
            QtSql \
            QtAssistantClient \
            sp \
            qtpropertybrowser \
            qtsingleapplication \
            qscintilla \
            antlr

win32:release_LIBS  = $$top_builddir/serna/lib/serna
sunos:release_LIBS += -ldl -lsocket
#win32:static_RTLIBS = $(THIRD_DIR)/lib/qtmain.lib

RTLIBS = $$static_RTLIBS

static:LIBS += \
                $(CLIB_LIB)/xs \
                $(CLIB_LIB)/urimgr \
                $(CLIB_LIB)/canvasareaset \
                $(CLIB_LIB)/editableview \
                $(CLIB_LIB)/formatter \
                $(CLIB_LIB)/groveeditor \
                $(CLIB_LIB)/catmgr \
                $(CLIB_LIB)/sxslt \
                $(CLIB_LIB)/xpath \
                $(CLIB_LIB)/spgrovebuilder \
                $(CLIB_LIB)/dav \
                $(CLIB_LIB)/grove \
                $(CLIB_LIB)/common \
                $(CLIB_LIB)/ui \
                $(CLIB_LIB)/proputils \
                $$RTLIBS
                
freebsd:release_LIBS = $$LIBS

release:LIBS =  \
                $$release_LIBS \
                $$RTLIBS

#kdexecutor:LIBS +=$(THIRD_DIR)/lib/kdexecutor

unix:LIBS    += $$RTLIBS

SOURCES_PATTERN *= .cxx$
win32:SOURCES_PATTERN *= win32/\.cxx$

rel_EXTRA_TEMPLATES = dynexports
win32:rel_EXTRA_TEMPLATES -= dynexports
win32:rel_EXTRA_TEMPLATES *= dbgpack

EXTRA_TEMPLATES = genlist appver spell $$srcdir/dist pymodules catalog script2 
release:EXTRA_TEMPLATES *= $$rel_EXTRA_TEMPLATES
win32:WIN32_RESOURCE = $(srcdir)/serna.rc

debug:W32_LIB_DBG_SFX = d

unix:SPELL_LIB  = libaspell.so
win32:SPELL_LIB = aspell-15$$W32_LIB_DBG_SFX.dll
PYTHONPATH      = $(THIRD_DIR)/lib
NOTPARALLEL = 1
