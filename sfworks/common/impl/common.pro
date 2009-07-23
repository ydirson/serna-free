TEMPLATE    = lib.t
TMAKE_LFLAGS= -pthread
CONFIG      = debug
TARGET      = common
INCLUDEPATH =../..;.;../../../../3rd/qt/3.0.0/qt-3.0.0/mkspecs/freebsd-g++;../../../../3rd/qt/3.0.0/qt-3.0.0/include;../../../../3rd/qt/3.0.0/qt-3.0.0/src
DEPENDPATH  = $$INCLUDEPATH
SOURCES     =  Singleton.cxx SmallObject.cxx common_inst.cxx \
	       QuadInt.cxx Cookie.cxx FixedDecimal.cxx \
	       Message.cxx MessageUtils.cxx CompiledInStrings.cxx \
	       TimeZone.cxx IPAddress.cxx DynamicLibrary.cxx \
	       String.cxx asserts.cxx Trace.cxx qtools.cxx Encodings.cxx
