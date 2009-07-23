# TMake settings for Syntext Editor
#

THIRD_DIR           = $$top_srcdir
ARCHIVE_DIR         = $(THIRD_DIR)/packages

# Settings for unix

unix:TMAKE_CXX      = g++
unix:TMAKE_CC       = gcc
unix:TMAKE_LINK     = g++
unix:TMAKE_RANLIB   = ranlib

unix:TMAKE_LIBS     *= -L/usr/local/lib

unix:PACKAGE_CONFIGURE  = ./configure
unix:MAKE               = gmake

static:UNIX_LIB_EXT     = .a
dynamic:UNIX_LIB_EXT    = .so
unix:LIB_PREFIX     = lib
unix:DEBUG_SUFFIX   = 
unix:LIB_EXT        = $$UNIX_LIB_EXT
darwin:MACOSX_DEPLOYMENT_TARGET = 10.4
darwin:PACKAGE_MAKE_ENV *= MACOSX_DEPLOYMENT_TARGET=10.4
darwin:PACKAGE_CFG_ENV *= MACOSX_DEPLOYMENT_TARGET=10.4

# Settings for win32

static:WIN32_LIB_EXT   = .lib
dynamic:WIN32_LIB_EXT   = .dll

win32:LIB_PREFIX    = 
win32:DEBUG_SUFFIX  = d
win32:LIB_EXT       = $$WIN32_LIB_EXT

win32:DEFINES       += WIN32
win32:MAKE          = nmake -nologo

# General settings

PACKAGE_DEFAULTS    = defaults.pro
PROJECT_FILE        = TMakefile.pro
TMAKE               = tmake
TMAKE_OPTS          = 
FILETAGS            += TMAKE_UIC TMAKE_MOC JADE TMAKEPATH

DEFAULT_CONFIG      = warn_off,dynamic,debug

LIB_PLATFORM_DESTDIRS   = no
BIN_PLATFORM_DESTDIRS   = no

LIB_DESTDIR         = $(THIRD_DIR)/lib
BIN_DESTDIR         = $(THIRD_DIR)/bin

MKDEP               = makedepend
MKDEP_OPTS          = -Y
OBJECTS_DIR         = .obj
