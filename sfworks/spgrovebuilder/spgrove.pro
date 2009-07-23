TEMPLATE	= lib.t
COMMON_DIR      = ../../mware/common
TMAKE_CXXFLAGS += -g -DOALLOC_TYPE=1 -DTRACE_ON -DMULTI_THREADED
#                  -DXML_DECL_FILE=\"xml.dcl\"
TMAKE_LIBS     += -L../grove3 -L/home/apg/se/3rd/lib -L/home/apg/se/build/sfworks/lib -L/usr/local/qt/lib -lcommon -lqt-mt -lsp -lutil
INCLUDEPATH     = ..;../common/include;../common/include/qt;../../../3rd/jade
DEPENDPATH      = $$INCLUDEPATH
CONFIG		= warn_on
HEADERS		=  \
		  SpGroveBuilder.h \
		  SpUtils.h 

SOURCES		= \
		  SpGroveBuilder.cxx \
		  SpUtils.cxx \
		  SpXmlDecl.cxx \
                  SpMessages.cpp 

TARGET = spgrovebuilder
