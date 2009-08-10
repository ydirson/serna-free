#
# Win32 main template
# 
#
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
		if ( !Config("dll") ) {
		    Project('TMAKE_LIBS *= $$TMAKE_LIBS_QT_DLL');
		}
	    }
	}
    }
    if ( Config("opengl") ) {
	Project('TMAKE_LIBS *= $$TMAKE_LIBS_OPENGL');
    }
    if ( Config("dll") ) {
	Project('TMAKE_LFLAGS_CONSOLE_ANY = $$TMAKE_LFLAGS_CONSOLE_DLL');
	Project('TMAKE_LFLAGS_WINDOWS_ANY = $$TMAKE_LFLAGS_WINDOWS_DLL');
        $project{"TARGET_EXT"} = ".dll";
	if ( Project("TMAKE_LIB_FLAG") ) {
	    my @ver = split('\.', Project("VERSION"));
	    pop @ver if ($#ver > 1);
	    $project{"TARGET_SFX"} = join('', @ver);
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
    if (Config("windows") || Config("win32" )) {
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
    Project('TMAKE_LIBS += $$LIBS');
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
    Project('dll:TMAKE_CLEAN += $$TARGET$$TARGET_SFX.lib $$TARGET$$TARGET_SFX.exp');
    Project('debug:TMAKE_CLEAN += vc*.pdb $(OBJECTS_DIR)\vc*.pdb');

    my $flags = '-Gm';
    Project("TMAKE_CFLAGS += $flags", "TMAKE_CXXFLAGS += $flags");
    unless (Config("no_pch")) {
        my $pchflags = '-YX$(MODULE_NAME)_pch.hpp -Fp$(MODULE_NAME)_pch.pch';
        Project("TMAKE_PCHFLAGS += $pchflags");
        Project('CLEANFILES += $(MODULE_NAME).pch');
    }
    Project("DEP_DIR = deps") unless Project("DEP_DIR");
#$}
####### Compiler, tools and options
#$ Project("FILETOOLS = MAKEDEPEND"); IncludeTemplate("filetools");

CPP_DEFINES     = #$ ExpandGlue("DEFINES", "-D", " \\\n\t\t -D", "");
CPP_INCPATH     = #$ $text = join(' -I', map { s-/-\\-g; $_ = '"'.$_."\" \\\n\t\t "; } split(/\s*;\s*/, Project("INCLUDEPATH"))); $text = '-I'.$text if $text;
PCH_FLAGS       = #$ Expand("TMAKE_PCHFLAGS");
PCH_DEFINES     = -DUSE_PCH
#$ ExpandGlueEx("TMAKE_CC", "CC\t=\t") if Project("C_SOURCES");
CXX	=	#$ Expand("TMAKE_CXX");
#$ ExpandGlueEx("TMAKE_CFLAGS", "CFLAGS   =\t", " \\\n", "\n") if Project("C_SOURCES");
#$ ExpandGlueEx("TMAKE_CXXFLAGS", "CXXFLAGS =\t", " \\\n");
#$ !(Config("dll") || Config("app")) && DisableOutput();
LINK	=	#$ Expand("TMAKE_LINK");
LFLAGS	=	#$ Expand("TMAKE_LFLAGS");
LIBS	=	#$ ExpandGlue("TMAKE_LIBS", "", " \\\n\t\t\t", "");
#$ !(Config("dll") || Config("app")) && EnableOutput();
#$ (Config("dll") || Config("app")) && DisableOutput();
LIB	=	#$ Expand("TMAKE_LIB");
#$ (Config("dll") || Config("app")) && EnableOutput();

#$ ExpandGlueEx("CXX_SOURCES", "CXX_SOURCES =\t", " \\\n", "\n");
#$ ExpandGlueEx("C_SOURCES", "C_SOURCES =\t", " \\\n", "\n");
SOURCES =	$(CXX_SOURCES) $(C_SOURCES)

#$ ExpandGlueEx("OBJECTS", "OBJECTS = \t", " \\\n", "\n");
#! OBJECTS =	#$ ExpandList("OBJECTS");
#$ Config("qt") || DisableOutput();
INTERFACES =	#$ ExpandList("INTERFACES");
UICDECLS =	#$ ExpandList("UICDECLS");
UICIMPLS =	#$ ExpandList("UICIMPLS");
SRCMOC	=	#$ ExpandList("SRCMOC");
OBJMOC	=	#$ ExpandList("OBJMOC");
INTERFACE_DECL_PATH = #$ Expand("INTERFACE_DECL_PATH");
#$ Config("qt") || EnableOutput();
DESTDIR =       #$ $text = normpath($project{"DESTDIR"});

TARGET  = #$ $text = '$(DESTDIR)\\'.Project("TARGET").Project("TARGET_SFX").Project("TARGET_EXT")."\n";

all: #$ ExpandGlue("ALL_DEPS",""," "," "); $text .= '$(TARGET) $(SOURCES)';

OBJECTS_DIR = #$ Expand("OBJECTS_DIR"); $text =~ s/\\+$//;
DEP_DIR     = #$ Expand("DEP_DIR"); $text =~ s/\\+$//;

!INCLUDE Makefile.deps

#${
    my (@cxx_src, @c_src, @depinc);
    my ($depdir, $objdir) = map { Project($_) } qw(DEP_DIR OBJECTS_DIR);
    my (%srchash, %objs) = ((), ());
    foreach (split(/\s+/, Project("SOURCES"))) {
        my ($srcdir, $base) = fnsplit($_);
        $srchash{$srcdir} = 1;
        $base =~ s/\.([^\.]*)/\./;
        $objs{$1} = [] unless defined($objs{$1});
        $srcs{$1} = [] unless defined($srcs{$1});
        push @{ $objs{$1} }, join('', $objdir, $base).'obj';
        push @{ $srcs{$1} }, join('', $srcdir, $dir_sep, $base, $1);
        my $depfile = join($dir_sep, $depdir, $base).'d';
        push @depinc, "!IF EXIST($depfile)\n! INCLUDE $depfile\n!ENDIF";
    }
    my $pch_depfile = $depdir.$dir_sep.Project("MODULE_NAME")."_pch.d";
    push @depinc, "!IF EXIST($pch_depfile)\n! INCLUDE $pch_depfile\n!ENDIF";
    write_file("Makefile.deps", join("\n\n", @depinc));
    $text .= "CXX_SOURCES = \t\\\n\t\t" . join(" \\\n\t\t", @{ $srcs{'cxx'} })."\n\n";
    $text .= "CXX_OBJECTS = \t\\\n\t\t" . join(" \\\n\t\t", @{ $objs{'cxx'} });
    $text .= "\n\n".'$(CXX_OBJECTS): $(MODULE_NAME)_pch.pch'."\n\n";
    foreach (keys (%srchash)) {
        $text .= "{$_}.cxx".'{$(OBJECTS_DIR)}.obj::'."\n";
        $text .= <<'EOF';
        $(MAKEDEPEND) $(CPP_DEFINES) $(CPP_INCPATH) -S -q -f + -d$(OBJECTS_DIR) -Z.d -z$(DEP_DIR) $<
	$(CXX) -c $(CXXFLAGS) $(PCH_DEFINES) $(CPP_DEFINES) $(CPP_INCPATH) $(PCH_FLAGS) -Fo$(OBJECTS_DIR)\ $<
	
EOF
        $text .= "{$_}.cpp".'{$(OBJECTS_DIR)}.obj::'."\n";
        $text .= <<'EOF';
        $(MAKEDEPEND) $(CPP_DEFINES) $(CPP_INCPATH) -S -q -f + -d$(OBJECTS_DIR) -Z.d -z$(DEP_DIR) $<
	$(CXX) -c $(CXXFLAGS) $(CPP_DEFINES) $(CPP_INCPATH) -Fo$(OBJECTS_DIR)\ $<
	
EOF
        $text .= "{$_}.c".'{$(OBJECTS_DIR)}.obj::'."\n";
        $text .= <<'EOF';
        $(MAKEDEPEND) $(CPP_DEFINES) $(CPP_INCPATH) -S -q -f + -d$(OBJECTS_DIR) -Z.d -z$(DEP_DIR) $<
	$(CC) -c $(CFLAGS) $(CPP_DEFINES) $(CPP_INCPATH) -Fo$(OBJECTS_DIR)\ $<
	
EOF
    }
#$}
$(MODULE_NAME)_pch.hpp: $(CXX_SOURCES)
	$(PYTHON) $(top_srcdir)\tools\buildsys\make_pch.py $@ $(CXX_SOURCES)
        $(MAKEDEPEND) $(CPP_DEFINES) $(CPP_INCPATH) -S -q -f + -o.pch -Z.d -z$(DEP_DIR) $@

$(MODULE_NAME)_pch.pch: $(MODULE_NAME)_pch.hpp
	echo #if !defined(USE_PCH) > $(@B).cpp
	echo #else >> $(@B).cpp
	echo # include "$(MODULE_NAME)_pch.hpp" >> $(@B).cpp
	echo #endif >> $(@B).cpp
	$(CXX) -c $(CXXFLAGS) $(CPP_DEFINES) $(PCH_DEFINES) $(CPP_INCPATH) \
	       -YX$(MODULE_NAME)_pch.hpp -Fp$@ -Fonul $(@B).cpp
	del /q $(@B).cpp

$(TARGET): $(UICDECLS) $(OBJECTS) $(OBJMOC) #$ Expand("TARGETDEPS");
#$ Project("TMAKE_APP_OR_DLL") || DisableOutput();
	$(LINK) $(LFLAGS) -OUT:$(TARGET) @<<
	    $(OBJECTS) $(OBJMOC) $(LIBS)
#$ Project("TMAKE_APP_OR_DLL") || EnableOutput();
#$ Project("TMAKE_APP_OR_DLL") && DisableOutput();
	$(LIB) -OUT:$(TARGET) @<<
	    $(OBJECTS) $(OBJMOC)
#$ Project("TMAKE_APP_OR_DLL") && EnableOutput();
<<
#$ (Config("dll") && Project("DLLDESTDIR")) || DisableOutput();
	-copy $(TARGET) #$ Expand("DLLDESTDIR");
#$ (Config("dll") && Project("DLLDESTDIR")) || EnableOutput();
#$ Project("RC_FILE") || DisableOutput();

#$ Substitute("\$\$RES_FILE: \$\$RC_FILE\n\t\$\$TMAKE_RC \$\$RC_FILE");
#$ Project("RC_FILE") || EnableOutput();

clean:
	-del /q /f $(OBJECTS) $(TARGET) $(DESTDIR)\#$ Expand("TARGET"); $text .= ".ilk";
	-del /q /f $(OBJECTS_DIR)\$(MODULE_NAME).pdb $(OBJECTS_DIR)\$(MODULE_NAME).idb deps\*.d
	-del /q /f $(MODULE_NAME)_pch.*
	
#$ ExpandGlueEx("SRCMOC", "\t-del /q /f ", " \\\n", "");
#$ ExpandGlueEx("OBJMOC", "\t-del /q /y ", " \\\n", "");
#$ ExpandGlueEx("TMAKE_CLEAN", "\t-del /q /f ", " \\\n", "");
