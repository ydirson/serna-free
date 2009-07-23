#!
#! This is a tmake template for building Win32 applications or libraries.
#!
#${
    Project('CONFIG += qt') if Config("qt_dll");
    if ( !Project("INTERFACE_DECL_PATH") ) {
	Project('INTERFACE_DECL_PATH = .' );
    }
    if ( Config("qt") ) {
	if ( !(Project("DEFINES") =~ /QT_NODLL/) &&
	     ((Project("DEFINES") =~ /QT_(?:MAKE)?DLL/) || Config("qt_dll") ||
	      ($ENV{"QT_DLL"} && !$ENV{"QT_NODLL"})) ) {
	    Project('TMAKE_QT_DLL = 1');
	    if ( (Project("TARGET") eq "qt") && Project("TMAKE_LIB_FLAG") ) {
		Project('CONFIG += dll');
	    }
	}
    }
    if ( Config("dll") || Project("TMAKE_APP_FLAG") ) {
	Project('CONFIG -= staticlib');
	Project('TMAKE_APP_OR_DLL = 1');
    } else {
	Project('CONFIG += staticlib');
    }
    if ( Config("warn_off") ) {
	Project('TMAKE_CFLAGS += $$TMAKE_CFLAGS_WARN_OFF');
	Project('TMAKE_CXXFLAGS += $$TMAKE_CXXFLAGS_WARN_OFF');
    } elsif ( Config("warn_on") ) {
	Project('TMAKE_CFLAGS += $$TMAKE_CFLAGS_WARN_ON');
	Project('TMAKE_CXXFLAGS += $$TMAKE_CXXFLAGS_WARN_ON');
    }
    if ( Config("thread") ) {
        Project('DEFINES += QT_THREAD_SUPPORT');
    }
    if ( Config("debug") ) {
        if ( Config("thread") ) {
	    if ( Config("dll") ) {
	        Project('TMAKE_CFLAGS += $$TMAKE_CFLAGS_MT_DLLDBG');
	        Project('TMAKE_CXXFLAGS += $$TMAKE_CXXFLAGS_MT_DLLDBG');
 	    } else {
		Project('TMAKE_CFLAGS += $$TMAKE_CFLAGS_MT_DBG');
		Project('TMAKE_CXXFLAGS += $$TMAKE_CXXFLAGS_MT_DBG');
	    }
        } else {
            Project('TMAKE_CFLAGS += $$TMAKE_CFLAGS_DEBUG');
            Project('TMAKE_CXXFLAGS += $$TMAKE_CXXFLAGS_DEBUG');
        }
        Project('TMAKE_LFLAGS += $$TMAKE_LFLAGS_DEBUG');
    } elsif ( Config("release") ) {
	if ( Config("thread") ) {
	    if ( Config("dll") ) {
		Project('TMAKE_CFLAGS += $$TMAKE_CFLAGS_MT_DLL');
		Project('TMAKE_CXXFLAGS += $$TMAKE_CXXFLAGS_MT_DLL');
	    } else {
		Project('TMAKE_CFLAGS += $$TMAKE_CFLAGS_MT');
		Project('TMAKE_CXXFLAGS += $$TMAKE_CXXFLAGS_MT');
	    }
	}
	Project('TMAKE_CFLAGS += $$TMAKE_CFLAGS_RELEASE');
	Project('TMAKE_CXXFLAGS += $$TMAKE_CXXFLAGS_RELEASE');
	Project('TMAKE_LFLAGS += $$TMAKE_LFLAGS_RELEASE');
    }

    if ( Project("TMAKE_INCDIR") ) {
	AddIncludePath(Project("TMAKE_INCDIR"));
    }
    if ( Config("qt") || Config("opengl") ) {
	Project('CONFIG += windows' );
    }
    if ( Config("qt") ) {
	Project('CONFIG *= moc');
	AddIncludePath(Project("TMAKE_INCDIR_QT"));
	if ( !Config("debug") ) {
	    Project('DEFINES += NO_DEBUG');
	}
	if ( (Project("TARGET") eq "qt") && Project("TMAKE_LIB_FLAG") ) {
	    if ( Project("TMAKE_QT_DLL") ) {
		Project('DEFINES *= QT_MAKEDLL');
		Project('TMAKE_LFLAGS += $$TMAKE_LFLAGS_QT_DLL');
	    }
	} else {
	    Project('TMAKE_LIBS *= $$TMAKE_LIBS_QT');
	    if ( Project("TMAKE_QT_DLL") ) {
		my $qtver =FindHighestLibVersion($ENV{"QTDIR"} . "/lib", "qt");
		Project("TMAKE_LIBS /= s/qt.lib/qt${qtver}.lib/");
		if (!Config("dll") && !Config("console")) {
		    Project('TMAKE_LIBS *= $$TMAKE_LIBS_QT_DLL');
		}
	    }
	}
    }
    if ( Config("opengl") ) {
	Project('TMAKE_LIBS *= $$TMAKE_LIBS_OPENGL');
    }
    if ( Project("VERSION") && 
         $project{"VERSION"} =~ m/^\s*(\d+)\.(\d+)\.(\d+)\s*$/) {
        $project{"VER_MAJ"} = $1;
        $project{"VER_MIN"} = $2;
    }
    if ( Config("dll") ) {
	Project('TMAKE_LFLAGS_CONSOLE_ANY = $$TMAKE_LFLAGS_CONSOLE_DLL');
	Project('TMAKE_LFLAGS_WINDOWS_ANY = $$TMAKE_LFLAGS_WINDOWS_DLL');
	if ( Project("TMAKE_LIB_FLAG") ) {
            $project{"TARGET_EXT"} = $project{"VER_MAJ"}.$project{"VER_MIN"}.".dll";
	} else {
	    $project{"TARGET_EXT"} = ".dll";
	}
    } else {
	Project('TMAKE_LFLAGS_CONSOLE_ANY = $$TMAKE_LFLAGS_CONSOLE');
	Project('TMAKE_LFLAGS_WINDOWS_ANY = $$TMAKE_LFLAGS_WINDOWS');
	if ( Project("TMAKE_APP_FLAG") ) {
	    $project{"TARGET_EXT"} = ".exe";
	} else {
	    $project{"TARGET_EXT"} = ".lib";
	}
    }
    if ( Config("windows") ) {
	if ( Config("console") ) {
	    Project('TMAKE_LFLAGS *= $$TMAKE_LFLAGS_CONSOLE_ANY');
	    Project('TMAKE_LIBS   *= $$TMAKE_LIBS_CONSOLE');
	} else {
	    Project('TMAKE_LFLAGS *= $$TMAKE_LFLAGS_WINDOWS_ANY');
	}
	Project('TMAKE_LIBS   *= $$TMAKE_LIBS_WINDOWS');
    } else {
	Project('TMAKE_LFLAGS *= $$TMAKE_LFLAGS_CONSOLE_ANY');
	Project('TMAKE_LIBS   *= $$TMAKE_LIBS_CONSOLE');
    }
    if ( Config("moc") ) {
	$moc_aware = 1;
    }
    Project('TMAKE_LIBS *= $$LIBS');
    Project('TMAKE_FILETAGS = HEADERS SOURCES DEF_FILE RC_FILE TARGET TMAKE_LIBS DESTDIR DLLDESTDIR $$FILETAGS');
    foreach ( split(/\s/,Project("TMAKE_FILETAGS")) ) {
	$project{$_} =~ s-[/\\]+-\\-g;
    }
    if ( Project("DEF_FILE") ) {
	Project('TMAKE_LFLAGS *= /DEF:$$DEF_FILE');
    }
    if ( Project("RC_FILE") ) {
	if ( Project("RES_FILE") ) {
	    tmake_error("Both .rc and .res file specified.\n" .
			"Please specify one of them, not both.");
	}
	$project{"RES_FILE"} = $project{"RC_FILE"};
	$project{"RES_FILE"} =~ s/\.rc$/.res/i;
	Project('TARGETDEPS += $$RES_FILE');
    }
    if ( Project("RES_FILE") ) {
	Project('TMAKE_LIBS *= $$RES_FILE');
    }
    StdInit();
    Project('dll:TMAKE_CLEAN += $$TARGET.lib $$TARGET.exp $$TARGET.manifest');
    Project('app:TMAKE_CLEAN *= $$TARGET.manifest');
    Project('debug:TMAKE_CLEAN += $$TARGET.pdb vc*.pdb $$TARGET.ilk');
#$}
#!
# Makefile for building #$ Expand("TARGET")
# Generated by tmake at #$ Now();
#     Project: #$ Expand("PROJECT");
#    Template: #$ Expand("TEMPLATE");
#############################################################################

####### Compiler, tools and options

CC	=	#$ Expand("TMAKE_CC");
CXX	=	#$ Expand("TMAKE_CXX");
DEFINES =       #$ ExpandGlue("DEFINES","\\\n\t\t-D"," \\\n\t\t-D","\n");
CFLAGS	=	#$ Expand("TMAKE_CFLAGS"); $text .= ' $(DEFINES)';
CXXFLAGS=	#$ Expand("TMAKE_CXXFLAGS"); $text .= ' $(DEFINES)';
INCPATH	=	#$ ExpandPath("INCPATH","\\\n\t\t-I"," \\\n\t\t-I","\n");
#$ !Project("TMAKE_APP_OR_DLL") && DisableOutput();
LINK	=	#$ Expand("TMAKE_LINK");
LFLAGS	=	#$ Expand("TMAKE_LFLAGS");
LIBS	=	#$ ExpandGlue("TMAKE_LIBS", "", " \\\n\t\t\t", "");
#$ !Project("TMAKE_APP_OR_DLL") && EnableOutput();
#$ Project("TMAKE_APP_OR_DLL") && DisableOutput();
LIB	=	#$ Expand("TMAKE_LIB");
#$ Project("TMAKE_APP_OR_DLL") && EnableOutput();
MOC	=	#$ Expand("TMAKE_MOC");
UIC	=	#$ Expand("TMAKE_UIC");
IDL	=	midl
#$ ExpandFileTools('$APP_FILETOOLS');

ZIP	=	#$ Expand("TMAKE_ZIP");

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
TARGET	=	#$ ExpandGlue("TARGET",$project{"DESTDIR"},"",$project{"TARGET_EXT"});
INTERFACE_DECL_PATH = #$ Expand("INTERFACE_DECL_PATH");
TLB_FILE =	#$ Expand("TLB_FILE");
IDL_FILE =	#$ Expand("IDL_FILE");

####### Implicit rules

.SUFFIXES: .cpp .cxx .cc .c

.cpp.obj:
	#$ Expand("TMAKE_RUN_CXX_IMP");

.cxx.obj:
	#$ Expand("TMAKE_RUN_CXX_IMP");

.cc.obj:
	#$ Expand("TMAKE_RUN_CXX_IMP");

.c.obj:
	#$ Expand("TMAKE_RUN_CC_IMP");

####### Build rules

all: #$ ExpandGlue("ALL_DEPS",""," "," "); $text .= '$(TARGET)';

MFT_RESNO = #$ $text = Config("app") ? '1' : '2';

$(TARGET): $(UICDECLS) $(OBJECTS) $(OBJMOC) #$ Expand("TARGETDEPS");
#$ Project("TMAKE_APP_OR_DLL") || DisableOutput();
	$(LINK) $(LFLAGS) -out:$(TARGET) @<<
	    $(OBJECTS) $(OBJMOC) $(LIBS)
#$ Project("TMAKE_APP_OR_DLL") || EnableOutput();
#$ Project("TMAKE_APP_OR_DLL") && DisableOutput();
	$(LIB) -out:$(TARGET) @<<
	    $(OBJECTS) $(OBJMOC)
#$ Project("TMAKE_APP_OR_DLL") && EnableOutput();
<<
#$ Project("TMAKE_APP_OR_DLL") || DisableOutput();
	if exist $@.manifest mt -nologo -manifest $@.manifest -outputresource:$@;$(MFT_RESNO)
#$ Project("TMAKE_APP_OR_DLL") || EnableOutput();
#$ Config("tlb") or DisableOutput();
#$ AddEnvValue(Project("IDC_PATH")) if Project("IDC_PATH");
	-$(IDC) $@ /idl $(IDL_FILE) -version 1.0
#$ Project("PROCESS_IDL") or DisableOutput();
	#$ Expand("PROCESS_IDL");
#$ Project("PROCESS_IDL") or EnableOutput();
	-$(IDL) $(IDL_FILE) /nologo /tlb $(TLB_FILE)
#$ Config("tlb") or EnableOutput();
#$ Config("embedtlb") or DisableOutput();
	-$(IDC) $@ /tlb $(TLB_FILE)
#$ Config("embedtlb") or EnableOutput();
#$ Project(TARGET_OBJECTS)
#!	@type nul > $(TARGET_OBJECTS)
#!	@for %f in ($(OBJECTS) $(OBJMOC)) do @echo %f >> $(TARGET_OBJECTS)
#$ (Config("dll") && Project("DLLDESTDIR")) || DisableOutput();
	-copy $(TARGET) #$ Expand("DLLDESTDIR");
#$ (Config("dll") && Project("DLLDESTDIR")) || EnableOutput();
#$ Project("RC_FILE") || DisableOutput();

#$ Substitute("\$\$RES_FILE: \$\$RC_FILE\n\t\$\$TMAKE_RC \$\$RC_FILE");
#$ Project("RC_FILE") || EnableOutput();

moc: $(SRCMOC)

#$ TmakeSelf();

dist:
	#$ Substitute('$(ZIP) $$PROJECT.zip $$PROJECT.pro $(SOURCES) $(HEADERS) $(DIST)');

clean:
	#$ ExpandGlue("OBJECTS","-del ","\n\t-del ","");
	#$ ExpandGlue("SRCMOC" ,"-del ","\n\t-del ","");
	#$ ExpandGlue("OBJMOC" ,"-del ","\n\t-del ","");
	-del $(TARGET)
	#$ ExpandGlue("TMAKE_CLEAN","-del ","\n\t-del ","");
	#$ ExpandGlue("CLEAN_FILES","-del ","\n\t-del ","");

####### Compile

#$ BuildObj(Project("OBJECTS"),Project("SOURCES"));
#$ BuildUicSrc(Project("INTERFACES"));
#$ BuildObj(Project("UICOBJECTS"), Project("UICIMPLS"));
#$ BuildMocObj(Project("OBJMOC"),Project("SRCMOC"));
#$ BuildMocSrc(Project("HEADERS"));
#$ BuildMocSrc(Project("SOURCES"));
#$ BuildMocSrc( Project("UICDECLS"));
