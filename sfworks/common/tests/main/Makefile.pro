#
#

TEMPLATE = serna
PROGRAMS = common_test

INCLUDEPATH += 	\
				.; ..;\
				$(srcdir)/..; \
				$(srcdir)/../../..; \
				$(srcdir)/../../qt; \
	            $(CLIB_SRC); \
				$(THIRD_DIR)/qt/3.0.0/qt/include; \
				$(THIRD_DIR)/qt/3.0.0/qt/src; 


win32:INCLUDEPATH += 	$(THIRD_DIR)/qt/3.0.0/qt/mkspecs/win32-msvc; 

unix:INCLUDEPATH += 	$(THIRD_DIR)/qt/3.0.0/qt/mkspecs/freebsd-g++; 

LIBS        = $(CLIB_LIB)/common \
              $(CLIB_LIB)/testing

SOURCES += $(CLIB_SRC)/testdriver/test_driver.cxx
EXTRA_TEMPLATES = genlist
DESTDIR=.
