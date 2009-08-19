#
# TMake settings for Syntext components library
#
#

THIRD_DIR           = $$top_builddir/../3rd
SERNA_DIR           = $$top_srcdir/serna
CLIB_SRC            = $$top_srcdir/sfworks
CLIB_LIB            = $$top_builddir/sfworks/lib
PLAT_INCPATH_BASE   = $(CLIB_SRC)/common/sysdep
PKGINFOPATH         = $$THIRD_DIR/lib

CONFIG_VARS         += CLIB_SRC CLIB_LIB SERNA_DIR
FILETAGS            += CLIB_SRC CLIB_LIB SERNA_DIR

CONFIG              *= qtexternal
darwin:CONFIG       *= universal

universal:TMAKE_CFLAGS    += -arch ppc -arch i386 -isysroot /Developer/SDKs/MacOSX10.4u.sdk
universal:TMAKE_CXXFLAGS  += -arch ppc -arch i386 -isysroot /Developer/SDKs/MacOSX10.4u.sdk
universal:TMAKE_LFLAGS    += -headerpad_max_install_names -arch i386 -arch ppc \
                             -Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk

universal:MACOSX_DEPLOYMENT_TARGET = 10.4

release:DEFINES     -= _DEBUG
release:DEFINES     *= NDEBUG NOCVSID
release:DEFINES     += OALLOC_TYPE=4

debug:DEFINES       -= NDEBUG NOCVSID
debug:DEFINES       *= _DEBUG

DEFINES       *= MULTI_THREADED

SUBQT_DIR           =  $(CLIB_SRC)/subqt
qtexternal:SUBQT_DIR = $(THIRD_DIR)/qt
QT_DIR              = $(THIRD_DIR)/qt
#QT_INC              += $$SUBQT_DIR; $$SUBQT_DIR/qt;

win32:QT_INC        += $$SUBQT_DIR/mkspecs/win32-msvc
freebsd:QT_INC      += $$SUBQT_DIR/mkspecs/freebsd-g++
linux:QT_INC        += $$SUBQT_DIR/mkspecs/linux-g++ 
sunos:QT_INC        += $$SUBQT_DIR/mkspecs/solaris-cc

unix:TMAKE_CFLAGS *= $$unix_CFLAGS
darwin:TMAKE_CFLAGS *= $$unix_CFLAGS

unix:TMAKE_CXXFLAGS *= $$unix_CFLAGS
darwin:TMAKE_CXXFLAGS *= $$unix_CFLAGS

# Settings for unix

# without this LLONG_MIN/LLONG_MAX/ULLONG_MAX won't be defined
linux:DEFINES *= __STDC_VERSION__=199901L

unix:TMAKE_RANLIB   = ranlib
unix:TMAKE_LIBS     *= -L/usr/local/lib
darwin:TMAKE_LIBS   -= -L/usr/local/lib
unix:MAKE            = gmake
unix:COPY            = cp
unix:BISON           = bison
unix:TMAKE_CXXFLAGS  += -g

unix:INSTALL_PROGRAM    = cp
unix:INSTALL_OPTS        = -fp
unix:INSTALL_DIR_PROGRAM    = cp
unix:INSTALL_DIR_OPTS        = -fpR
unix:MKDIR    = mkdir -p

unix_QTLIB      = QtCore
mt:unix_QTLIB   = QtCore
unix:QTLIB      = $$unix_QTLIB

unix:QT_SYSLIBS   =  Xmu ICE SM Xext X11 freetype
linux:QT_SYSLIBS  *= Xft Xinerama
darwin:QT_SYSLIBS  = 
darwin:TMAKE_LFLAGS += -Wl,-search_paths_first

unix:APP_LFLAGS   = -L/usr/X11R6/lib

linux:THR_LIBS    += pthread

MAKE_PARALLEL = -j2

# Settings for win32

win32:CONFIG         *= console
win32:TMAKE_CXX       = cl
win32:TMAKE_CXXFLAGS *= -Zc:forScope -w44355 -w44996 -we4700
win32:DEFINES        += WIN32
# STATIC, DYNAMIC
static:win32_defines  += SFWORKS_STATIC SERNA_STATIC
release:win32_defines  -= SFWORKS_STATIC SERNA_STATIC
debug:win32_defines   += _HAS_ITERATOR_DEBUGGING=0

release:win32_defines  += SFWORKS_DLL SERNA_DLL GROVE_API GROVEEDITOR_API
dynamic:win32_defines += QT_DLL SP_USE_DLL

win32:DEFINES         += $$win32_defines
win32:PROFILE_DEFINES *= PROFILE

win32:MAKE            = nmake -nologo
win32:COPY            = copy
win32:BISON           = $(THIRD_DIR)\bin\bison

win32:INSTALL               = xcopy
win32:INSTALL_OPTS          = /q /y /r /i
win32:INSTALL_DIR_PROGRAM   = xcopy
win32:INSTALL_DIR_OPTS      = /e /c /h /q /y /r /i
win32:MKDIR = md
win32:ZIP                   = $(THIRD_DIR)\bin\zip

win32:WIN32_RESOURCE    += $(CLIB_SRC)/common.rc

win32:QTLIB         = QtCore

win32:SYS_LIBS      = user32.lib kernel32.lib ws2_32.lib

win32:QT_SYSLIBS    = comdlg32.lib shell32.lib comsupp.lib oleaut32.lib \
                      gdi32.lib advapi32.lib imm32.lib \
	              winspool.lib winmm.lib ole32.lib uuid.lib

# Sunos settings

#sunos:TMAKE_CXXFLAGS += -xtarget=ultra
#sunos:TMAKE_LFLAGS += -xtarget=ultra
#sunos:TMAKE_CLEAN   = -r SunWS_cache obj/SunWS_cache

# General settings

profile:DEFINES *= $$PROFILE_DEFINES
linux:SYS_LIBS  *= dl util
freebsd:SYS_LIBS   *= util
darwin:SYS_LIBS    *= System

PRODUCT_NAME        = Syntext Components Library
VERSION             = 1.0.0
CONFIG              += mt
qtexternal:DEFINES  *= QT_DLL

TMAKE               = tmake
TMAKE_OPTS          = 
TMAKE_MOC           = $(THIRD_DIR)/bin/moc
TMAKE_UIC           = $(THIRD_DIR)/bin/uic3
win32:TMAKE_UIC     = $(THIRD_DIR)/bin/uic3.bat
JADE                = $(THIRD_DIR)/bin/jade
MOC_DIR             = ./moc

MKDEP               = makedepend
MKDEP_OPT           = -Y

FILETAGS            += TMAKEPATH TMAKE_MOC TMAKE_UIC

TMAKE_INCDIR_QT     = 
TMAKE_LIBDIR_QT     =
TMAKE_LIBS_QT       =

DEFAULT_CONFIG      = warn_on dynamic debug
PLATFORM_DESTDIRS   = no
OBJECTS_DIR         = obj

msg2cpp             = $(THIRD_DIR)/bin/python $(top_srcdir)/tools/msggen/msggen.py
msg2h               = $$msg2cpp

SOURCE_SUFFIXES            = msg ui
msg_TARGET_SOURCE_SUFFIXES = cpp
msg_TARGET_HEADER_SUFFIXES = h

ui_TARGET_HEADER_SUFFIXES = hpp
ui2hpp                    = $(THIRD_DIR)/bin/uic
ui2hpp_FLAGS              = $? -o $@

msg2cpp_FLAGS       = -C -M $(MSGGEN_MODULE) $@ $?
msg2h_FLAGS         = -H -M $(MSGGEN_MODULE) $@ $?
