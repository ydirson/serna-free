# 
#
all: default

install: all

DEFINES  = \
            -DWIN32 -DSP_DEFINE_TEMPLATES -DBUILD_LIBSP -DSP_PCH \
            -DSP_NAMESPACE=Sp -DDSSSL_NAMESPACE=Dsssl -DGROVE_NAMESPACE=Grove

INCLUDES = -I..\sp -I..\generic

!if DEFINED(destdir)
DESTDIR = $(destdir)
!else
DESTDIR = ..\lib
!endif

!if DEFINED(debug) && "$(debug)" == "1"
CXX_DBG_FLAGS = -MDd -Zi -Od
LD_DBG_FLAGS  = -debug
!else
CXX_DBG_FLAGS = -MD -O2 -Zi -DNDEBUG
LD_DBG_FLAGS  = -debug -release
!endif

!if DEFINED(static) && "$(static)" == "1"
CXX          = cl
LD           = lib
TARGET       = sp.lib
OBJDIR       = .obj
!else
CXX          = cl
CXX_DYNFLAGS = -DSP_USE_DLL
LD           = link
TARGET       = sp103.dll
LD_DLL_FLAGS = -version:1.3 -dll -pdb:sp103.pdb \
               -nodefaultlib:libmmdd -nodefaultlib:libmmd -subsystem:console
OBJDIR       = .shobj
RES_FILE     = $(OBJDIR)\sp.res
RC_FILE      = lib.rc
LIBS         = user32.lib ws2_32.lib

$(RES_FILE): $(RC_FILE)
	$(RC) -fo$@ $(INCLUDES) $(DEFINES) $(RC_FILE)
!endif

!include Makefile.objs

CXX_FLAGS = -nologo -GX $(CXX_DBG_FLAGS) $(CXX_DYNFLAGS) -Fd"$(OBJDIR)\sp" -W0 -YXsplib.h \
             $(DEFINES) $(INCLUDES) $(CXX_ADD)
LD_FLAGS  = -nologo $(LD_DLL_FLAGS) $(LD_DBG_FLAGS) $(LD_ADD)

default: $(DESTDIR)\$(TARGET)

$(DESTDIR)\$(TARGET): makedirs $(OBJECTS) $(RES_FILE)
	$(LD) $(LD_FLAGS) -out:$@ @<< 
	$(OBJECTS) $(RES_FILE) $(LIBS)
<<


makedirs: force
	-@mkdir $(OBJDIR) 2>nul
	-@mkdir $(DESTDIR) 2>nul

{}.cxx{$(OBJDIR)}.obj::
	$(CXX) -c -Fo$(OBJDIR)\ $(CXX_FLAGS) @<<
	$<
<<

force:

clean: force
	-del /q /f $(OBJDIR)\*.obj $(OBJDIR)\*.?db $(RES_FILE) $(DESTDIR)\sp103.* $(DESTDIR)\sp.lib
