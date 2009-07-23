#!
#! This is a tmake template for building UNIX applications or libraries.
#!
#${
    Project('TMAKE_LIBS += $$LIBS');
    if ( !Project("INTERFACE_DECL_PATH") ) {
	Project('INTERFACE_DECL_PATH = .' );
    }
    if ( Project("TMAKE_LIB_FLAG") && !Config("staticlib") ) {
	Project('CONFIG *= dll');
    } elsif ( Project("TMAKE_APP_FLAG") || Config("dll") ) {
	Project('CONFIG -= staticlib');
    }
    if ( Config("warn_off") ) {
	Project('TMAKE_CFLAGS += $$TMAKE_CFLAGS_WARN_OFF');
	Project('TMAKE_CXXFLAGS += $$TMAKE_CXXFLAGS_WARN_OFF');
    } elsif ( Config("warn_on") ) {
	Project('TMAKE_CFLAGS += $$TMAKE_CFLAGS_WARN_ON');
	Project('TMAKE_CXXFLAGS += $$TMAKE_CXXFLAGS_WARN_ON');
    }
    if ( Config("debug") ) {
	Project('TMAKE_CFLAGS += $$TMAKE_CFLAGS_DEBUG');
	Project('TMAKE_CXXFLAGS += $$TMAKE_CXXFLAGS_DEBUG');
	Project('TMAKE_LFLAGS += $$TMAKE_LFLAGS_DEBUG');
    } elsif ( Config("release") ) {
	Project('TMAKE_CFLAGS += $$TMAKE_CFLAGS_RELEASE');
	Project('TMAKE_CXXFLAGS += $$TMAKE_CXXFLAGS_RELEASE');
	Project('TMAKE_LFLAGS += $$TMAKE_LFLAGS_RELEASE');
    }
    if ( Project("TMAKE_INCDIR") ) {
	AddIncludePath(Project("TMAKE_INCDIR"));
    }
    if ( Project("TMAKE_LIBDIR") ) {
	Project('TMAKE_LIBDIR_FLAGS *= -L$$TMAKE_LIBDIR');
    }
    if ( Config("x11") && (Config("qt") || Config("opengl")) ) {
	Project('CONFIG *= x11lib');
	if ( Config("opengl") ) {
	    Project('CONFIG *= x11inc');
	}
    }
    if ( Config("x11") ) {
	Project('CONFIG *= x11lib');
	Project('CONFIG *= x11inc');
    }
    if ( Config("thread") ) {
	Project('DEFINES *= QT_THREAD_SUPPORT');
        Project("TMAKE_CFLAGS_THREAD") &&
	    Project('TMAKE_CFLAGS *= $$TMAKE_CFLAGS_THREAD');
        Project("TMAKE_CXXFLAGS_THREAD") &&
	    Project('TMAKE_CXXFLAGS *= $$TMAKE_CXXFLAGS_THREAD');
        AddIncludePath(Project("TMAKE_INCDIR_THREAD"));
	Project('TMAKE_LIBS *= $$TMAKE_LIBS_THREAD');
	Project("TMAKE_LFLAGS_THREAD") &&
	    Project('TMAKE_LFLAGS += $$TMAKE_LFLAGS_THREAD');
    }
    if ( Config("qt") ) {
	Project('CONFIG *= moc');
	AddIncludePath(Project("TMAKE_INCDIR_QT"));
	if ( !Config("debug") ) {
	    Project('DEFINES *= NO_DEBUG');
	}
	if ( !(((Project("TARGET") eq "qt") || (Project("TARGET") eq "qt-mt") ||
	        (Project("TARGET") eq "qte") || (Project("TARGET") eq "qte-mt")) &&
               Project("TMAKE_LIB_FLAG")) ) {
	    Project("TMAKE_LIBDIR_QT") &&
		Project('TMAKE_LIBDIR_FLAGS *= -L$$TMAKE_LIBDIR_QT');
	    if (Config("thread") && Project("TMAKE_LIBS_QT_THREAD")) {
	        Project('TMAKE_LIBS *= $$TMAKE_LIBS_QT_THREAD');
	    } else {
	        Project('TMAKE_LIBS *= $$TMAKE_LIBS_QT');
	    }
	}
    }
    if ( Config("opengl") ) {
	AddIncludePath(Project("TMAKE_INCDIR_OPENGL"));
	Project("TMAKE_LIBDIR_OPENGL") &&
	    Project('TMAKE_LIBDIR_FLAGS *= -L$$TMAKE_LIBDIR_OPENGL');
	Project('TMAKE_LIBS *= $$TMAKE_LIBS_OPENGL');
    }
    if ( Config("x11inc") ) {
	AddIncludePath(Project("TMAKE_INCDIR_X11"));
    }
    if ( Config("x11lib") ) {
	Project("TMAKE_LIBDIR_X11") &&
	    Project('TMAKE_LIBDIR_FLAGS *= -L$$TMAKE_LIBDIR_X11');
	Project('TMAKE_LIBS *= $$TMAKE_LIBS_X11');
    }
    if ( Config("moc") ) {
	$moc_aware = 1;
    }
    if ( !Project("TMAKE_RUN_CC") ) {
	Project('TMAKE_RUN_CC = $(CC) -c $(CFLAGS) $(INCPATH) -o $obj $src');
    }
    if ( !Project("TMAKE_RUN_CC_IMP") ) {
	Project('TMAKE_RUN_CC_IMP = $(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<');
    }
    if ( !Project("TMAKE_RUN_CXX") ) {
	Project('TMAKE_RUN_CXX = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $obj $src');
    }
    if ( !Project("TMAKE_RUN_CXX_IMP") ) {
	Project('TMAKE_RUN_CXX_IMP = $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<');
    }
    Project('TMAKE_FILETAGS = HEADERS SOURCES TARGET DESTDIR $$FILETAGS');
    if ( Config("embedded") && Project("PRECOMPH") ) {
	Project('SOURCES += allmoc.cpp');
	$project{'HEADERS_ORIG'} = Project('HEADERS');
	$project{'HEADERS'} = "";
    }
    StdInit();
    $project{"VERSION"} || ($project{"VERSION"} = "1.0.0");
    ($project{"VER_MAJ"},$project{"VER_MIN"},$project{"VER_PAT"})
	= $project{"VERSION"} =~ /(\d+)\.(\d+)\.(\d+)/;
    if ( !defined($project{"VER_PAT"}) ) {
	($project{"VER_MAJ"},$project{"VER_MIN"})
	    = $project{"VERSION"} =~ /(\d+)\.(\d+)/;
	$project{"VER_PAT"} = "0";
    }
    if ( !defined($project{"VER_MIN"}) ) {
	($project{"VER_MAJ"})
	    = $project{"VERSION"} =~ /(\d+)/;
	$project{"VER_MIN"} = "0";
	$project{"VER_PAT"} = "0";
    }
    Project('DESTDIR_TARGET = $(TARGET)');
    if ( Project("TMAKE_APP_FLAG") ) {
	if ( Config("dll") ) {
	    Project('TARGET = $$TARGET.dylib');
	    Project("TMAKE_LFLAGS_SHAPP") ||
		($project{"TMAKE_LFLAGS_SHAPP"} = $project{"TMAKE_LFLAGS_SHLIB"});
	    Project("TMAKE_LFLAGS_SONAME") &&
		($project{"TMAKE_LFLAGS_SONAME"} .= $project{"TARGET"});
	}
	$project{"TARGET"} = $project{"DESTDIR"} . $project{"TARGET"};
    } elsif ( Config("staticlib") ) {
	$project{"TARGET"} = $project{"DESTDIR"} . "lib" .
			     $project{"TARGET"} . ".a";
	Project("TMAKE_AR_CMD") ||
	    Project('TMAKE_AR_CMD = $(AR) $(TARGET) $(OBJECTS) $(OBJMOC)');
    } else {
	$project{"TARGETA"} = $project{"DESTDIR"} . "lib" .
			      $project{"TARGET"} . ".a";
	if ( Project("TMAKE_AR_CMD") ) {
	    $project{"TMAKE_AR_CMD"} =~ s/\(TARGET\)/\(TARGETA\)/g;
	} else {
	    Project('TMAKE_AR_CMD = $(AR) $(TARGETA) $(OBJECTS) $(OBJMOC)');
	}
	$project{"TARGET_"} = "lib" . $project{"TARGET"} . ".dylib";
	$project{"TARGET_x"} = "lib" . $project{"TARGET"} . "." .
				    $project{"VER_MAJ"} . ".dylib";
	$project{"TARGET_x.y"} = "lib" . $project{"TARGET"} . "." .
				     $project{"VER_MAJ"} . "." .
				     $project{"VER_MIN"} . ".dylib";
	$project{"TARGET_x.y.z"} = "lib" . $project{"TARGET"} . "." .
				       $project{"VER_MAJ"} . "." .
				       $project{"VER_MIN"} . "." .
				       $project{"VER_PAT"} . ".dylib";
	$project{"TARGET"} = $project{"TARGET_x.y.z"};
	
	$project{"TMAKE_LN_SHLIB"} = "-ln -s";
	if ( $project{"DESTDIR"} ) {
	    $project{"DESTDIR_TARGET"} = $project{"DESTDIR"} .
					 $project{"TARGET"};
	}
	Project("TMAKE_LFLAGS_SONAME") &&
	    ($project{"TMAKE_LFLAGS_SONAME"} .= ' ' . $project{"TARGET_x"});
	$project{"TMAKE_LINK_SHLIB_CMD"} ||
	    ($project{"TMAKE_LINK_SHLIB_CMD"} =
	      '$(LINK) $(LFLAGS) -o $(TARGETD) $(OBJECTS) $(OBJMOC) $(LIBS)');
    }
    if ( Config("dll") ) {
	Project('TMAKE_CFLAGS *= $$TMAKE_CFLAGS_SHLIB' );
	Project('TMAKE_CXXFLAGS *= $$TMAKE_CXXFLAGS_SHLIB' );
	if ( Project("TMAKE_APP_FLAG") ) {
	    Project('TMAKE_LFLAGS *= $$TMAKE_LFLAGS_SHAPP');
	} else {
	    Project('TMAKE_LFLAGS *= $$TMAKE_LFLAGS_SHLIB $$TMAKE_LFLAGS_SONAME');
	}
    }
    my $tmake_lflags = Project("TMAKE_LFLAGS");
    $tmake_lflags =~ s/-arch\s+(ppc|i386)//g;
    Project("TMAKE_LFLAGS_NOARCH = $tmake_lflags");
#$}
#!
# Makefile for building #$ Expand("TARGET")
# Generated by tmake at #$ Now();
#     Project: #$ Expand("PROJECT");
#    Template: #$ Expand("TEMPLATE");
#############################################################################

MACOSX_DEPLOYMENT_TARGET = #$ Expand("MACOSX_DEPLOYMENT_TARGET");
export MACOSX_DEPLOYMENT_TARGET

####### Compiler, tools and options

CC	=	#$ Expand("TMAKE_CC");
CXX	=	#$ Expand("TMAKE_CXX");
DEFINES =       #$ ExpandGlue("DEFINES","\\\n\t\t-D"," \\\n\t\t-D","\n");
CFLAGS	=	#$ Expand("TMAKE_CFLAGS"); $text .= ' $(DEFINES)';
CXXFLAGS=	#$ Expand("TMAKE_CXXFLAGS"); $text .= ' $(DEFINES)';
INCPATH	=	#$ ExpandPath("INCPATH","\\\n\t\t-I"," \\\n\t\t-I","\n");
#$ Config("staticlib") && DisableOutput();
LINK	=	#$ Expand("TMAKE_LINK");
LFLAGS	=	#$ Expand("TMAKE_LFLAGS");
LFLAGS_NOARCH	=	#$ Expand("TMAKE_LFLAGS_NOARCH");
LIBS	=	$(SUBLIBS) #$ Expand("TMAKE_LIBDIR_FLAGS"); Expand("TMAKE_LIBS");
#$ Config("staticlib") && EnableOutput();
#$ Project("TMAKE_LIB_FLAG") || DisableOutput();
AR	=	#$ Expand("TMAKE_AR");
RANLIB	=	#$ Expand("TMAKE_RANLIB");
#$ Project("TMAKE_LIB_FLAG") || EnableOutput();
MOC	=	#$ Expand("TMAKE_MOC");
UIC	=	#$ Expand("TMAKE_UIC");

TAR	=	#$ Expand("TMAKE_TAR");
GZIP	=	#$ Expand("TMAKE_GZIP");

####### Files

HEADERS =	#$ ExpandList("HEADERS");
SOURCES =	#$ ExpandList("SOURCES");
OBJECTS =	#$ ExpandList("OBJECTS");
INTERFACES =	#$ ExpandList("INTERFACES");
UICDECLS =	#$ ExpandList("UICDECLS");
UICIMPLS =	#$ ExpandList("UICIMPLS");
SRCMOC	=	#$ ExpandList("SRCMOC");
OBJMOC	=	#$ ExpandList("OBJMOC");
DIST	=	#$ ExpandList("DISTFILES");
TARGET	=	#$ Expand("TARGET");
#$ (Project("TMAKE_APP_FLAG") || Config("staticlib")) && DisableOutput();
TARGETA	=	#$ Expand("TARGETA");
#$ Project("TMAKE_HPUX_SHLIB") && DisableOutput();
TARGETD	=	#$ Expand("TARGET_x.y.z");
TARGET0	=	#$ Expand("TARGET_");
TARGET1	=	#$ Expand("TARGET_x");
TARGET2	=	#$ Expand("TARGET_x.y");
#$ Project("TMAKE_HPUX_SHLIB") && EnableOutput();
#$ !Project("TMAKE_HPUX_SHLIB") && DisableOutput();
TARGETD	=	#$ Expand("TARGET_x");
TARGET0	=	#$ Expand("TARGET_");
#$ !Project("TMAKE_HPUX_SHLIB") && EnableOutput();
#$ (Project("TMAKE_APP_FLAG") || Config("staticlib")) && EnableOutput();
INTERFACE_DECL_PATH = #$ Expand("INTERFACE_DECL_PATH");

####### Implicit rules

.SUFFIXES: .cpp .cxx .cc .C .c

.cpp.o:
	#$ Expand("TMAKE_RUN_CXX_IMP");

.cxx.o:
	#$ Expand("TMAKE_RUN_CXX_IMP");

.cc.o:
	#$ Expand("TMAKE_RUN_CXX_IMP");

.C.o:
	#$ Expand("TMAKE_RUN_CXX_IMP");

.c.o:
	#$ Expand("TMAKE_RUN_CC_IMP");

####### Build rules

#${
	if ( Project("SUBLIBS") ) {
	    $text = "SUBLIBS=";
	    for $m ( split / /, Project("SUBLIBS") ) {
		$text .= "tmp/lib$m.a ";
	    }
	    $text .= "\n";
	}
#$}

#$ Project("TMAKE_APP_FLAG") || DisableOutput();
all: #$ ExpandGlue("ALL_DEPS",""," "," "); $text .= '$(TARGET)';

STRIP_FLAGS = #$ Expand("STRIP_FLAGS");

$(TARGET): $(UICDECLS) $(OBJECTS) $(OBJMOC) #$ Expand("TARGETDEPS");
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(LIBS)
#$ DisableOutput() unless (Config("app") && Config("release") && Project("STRIP_BINARY"));
	strip -u -r $(STRIP_FLAGS) $@
#!	@ls $(OBJECTS) $(OBJMOC) > $(TARGET_OBJECTS)
#$ EnableOutput() unless (Config("app") && Config("release") && Project("STRIP_BINARY"));
#$ Project("TMAKE_APP_FLAG") || EnableOutput();
#$ (Config("staticlib") || Project("TMAKE_APP_FLAG")) && DisableOutput();
all: #$ ExpandGlue("ALL_DEPS",""," ",""); Expand("DESTDIR_TARGET");

STRIP_FLAGS = #$ Expand("STRIP_FLAGS");

bin.noarch: $(UICDECLS) $(OBJECTS) $(OBJMOC) #$ Expand("TARGETDEPS");
	$(LINK) $(LFLAGS_NOARCH) -o $(TARGET).noarch $(OBJECTS) $(OBJMOC) $(LIBS)

#$ Substitute('$$DESTDIR_TARGET: $(OBJECTS) $(OBJMOC) $(SUBLIBS) $$TARGETDEPS');
	#$ Project("TMAKE_HPUX_SHLIB") && DisableOutput();
	-rm -f $(TARGET) $(TARGET0) $(TARGET1) $(TARGET2)
	#$ Expand("TMAKE_LINK_SHLIB_CMD");
	#$ ExpandGlue("TMAKE_LN_SHLIB",""," "," \$(TARGET) \$(TARGET0)");
	#$ ExpandGlue("TMAKE_LN_SHLIB",""," "," \$(TARGET) \$(TARGET1)");
	#$ ExpandGlue("TMAKE_LN_SHLIB",""," "," \$(TARGET) \$(TARGET2)");
	#$ Project("TMAKE_HPUX_SHLIB") && EnableOutput();
	#$ !Project("TMAKE_HPUX_SHLIB") && DisableOutput();
	-rm -f $(TARGET) $(TARGET0)
	#$ Expand("TMAKE_LINK_SHLIB_CMD");
	#$ ExpandGlue("TMAKE_LN_SHLIB",""," "," \$(TARGET) \$(TARGET0)");
	#$ !Project("TMAKE_HPUX_SHLIB") && EnableOutput();
	#${
	    $d = Project("DESTDIR");
	    if ( $d ) {
		$d =~ s-([^/])$-$1/-;
		$text =  "-rm -f $d\$(TARGET)\n\t" .
		     "-rm -f $d\$(TARGET0)\n\t" .
		     "-rm -f $d\$(TARGET1)\n\t" .
		     "-rm -f $d\$(TARGET2)\n\t" .
		     "-mv \$(TARGET) \$(TARGET0) \$(TARGET1) \$(TARGET2) $d";
	    }
	#$}
#!	@ls $(OBJECTS) > $(TARGET_OBJECTS)
#$ DisableOutput() unless (Config("dll") && Config("release") && Project("STRIP_BINARY"));
	strip -u -r $(STRIP_FLAGS) $@
#$ EnableOutput() unless (Config("dll") && Config("release") && Project("STRIP_BINARY"));

staticlib: $(TARGETA)

$(TARGETA): $(UICDECLS) $(OBJECTS) $(OBJMOC) #$ Expand("TARGETDEPS");
	-rm -f $(TARGETA)
	#$ Expand("TMAKE_AR_CMD");
	#$ ExpandGlue("TMAKE_RANLIB","",""," \$(TARGETA)");
#$ (Config("staticlib") || Project("TMAKE_APP_FLAG")) && EnableOutput();
#$ Config("staticlib") || DisableOutput();
all: #$ ExpandGlue("ALL_DEPS",""," "," "); $text .= '$(TARGET)';

staticlib: $(TARGET)

$(TARGET): $(UICDECLS) $(OBJECTS) $(OBJMOC) #$ Expand("TARGETDEPS");
	-rm -f $(TARGET)
	#$ Expand("TMAKE_AR_CMD");
	#$ ExpandGlue("TMAKE_RANLIB","",""," \$(TARGET)");
#!	@ls $(OBJECTS) > $(TARGET_OBJECTS)
#$ Config("staticlib") || EnableOutput();

moc: $(SRCMOC)

#$ TmakeSelf();

dist:
	#$ Substitute('$(TAR) $$PROJECT.tar $$PROJECT.pro $(SOURCES) $(HEADERS) $(INTERFACES) $(DIST)');
	#$ Substitute('$(GZIP) $$PROJECT.tar');

clean:
	-rm -f $(OBJECTS) $(OBJMOC) $(SRCMOC) $(UICIMPLS) $(UICDECLS) $(TARGET)
#$ (Config("staticlib") || Project("TMAKE_APP_FLAG")) && DisableOutput();
	-rm -f $(TARGET0) $(TARGET1) $(TARGET2) $(TARGETA)
#$ (Config("staticlib") || Project("TMAKE_APP_FLAG")) && EnableOutput();
	#$ ExpandGlue("TMAKE_CLEAN","-rm -f "," ","");
	-rm -f *~ core
	#$ ExpandGlue("CLEAN_FILES","-rm -f "," ","");

####### Sub-libraries

#${
	if ( Project("SUBLIBS") ) {
	    for $m ( split / /, Project("SUBLIBS") ) {
		$text .= "tmp/lib$m.a:\n\t";
		$text .= $project{"MAKELIB$m"}."\n";
	    }
	}
#$}

###### Combined headers

#${
	if ( Config("embedded") && Project("PRECOMPH") ) {
	    $t = "allmoc.cpp: ".Project("PRECOMPH")." ".$original_HEADERS;
	    ExpandList("HEADERS_ORIG");
	    $t.= $text;
	    $t.= "\n\techo '#include \"".Project("PRECOMPH")."\"' >allmoc.cpp";
	    $t.= "\n\t\$(CXX) -E -DQT_MOC_CPP \$(CXXFLAGS) \$(INCPATH) >allmoc.h allmoc.cpp";
	    $t.= "\n\t\$(MOC) -o allmoc.cpp allmoc.h";
	    $t.= "\n\tperl -pi -e 's{\"allmoc.h\"}{\"".Project("PRECOMPH")."\"}' allmoc.cpp";
	    $t.= "\n\trm allmoc.h";
	    $t.= "\n";
	    $text = $t;
	}
#$}

####### Compile

#$ BuildObj(Project("OBJECTS"),Project("SOURCES"));
#$ BuildUicSrc(Project("INTERFACES"));
#$ BuildObj(Project("UICOBJECTS"), Project("UICIMPLS"));
#$ BuildMocObj(Project("OBJMOC"),Project("SRCMOC"));
#$ BuildMocSrc(Project("HEADERS"));
#$ BuildMocSrc(Project("SOURCES"));
#$ BuildMocSrc( Project("UICDECLS"));
