#
#

TEMPLATE = serna
LIBRARIES = testmessages

INCLUDEPATH += 	\
				.; \
				$(srcdir); \
				$(srcdir)/../../..; \
				$(srcdir)/../../qt; \
				$(THIRD_DIR)/qt/3.0.0/qt/include; \
				$(THIRD_DIR)/qt/3.0.0/qt/src; 


win32:INCLUDEPATH += 	$(THIRD_DIR)/qt/3.0.0/qt/mkspecs/win32-msvc; 
unix:INCLUDEPATH += 	$(THIRD_DIR)/qt/3.0.0/qt/mkspecs/freebsd-g++; 

LIBS        = $(CLIB_LIB)/common

# win32:TMAKE_CXX = xicl6 
# win32:TMAKE_LINK = xilink6 
# win32:DEFINES *= COMMON_EXPORTS QT_MAKEDLL
# SOURCES = $(srcdir)/msg_test.cxx
EXTRA_TEMPLATES = genlist
DESTDIR = ../main

msg2cpp_FLAGS	= -C $@ $?
msg2h_FLAGS		= -H $@ $?
