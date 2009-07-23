#
#

TEMPLATE = 3rd
TARGET = sp
options = package=jade,buildtype=tmake

dynamic:w32_defs     = SP_USE_DLL
win32:DEFINES        += $$w32_defs
win32-msvc:DEFINES  *= SP_DEFINE_TEMPLATES BUILD_LIBSP
win32:TMAKE_LIBS    *= user32.lib
win32:RC_FILE       *= lib.rc
MSGFILES_PATTERN	*= \@
EXTRA_TEMPLATES		*= genlist
LIB_DESTDIR	        = $(top_builddir)/../lib
