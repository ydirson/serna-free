#############################################################################
# Makefile for building updateonsave
# Project: updateonsave
##############################################################################

####### Compiler, tools and options
CXX	=	cl
CXXFLAGS=	-GX -GR -DWIN32 -I"../.."
LINK	=	link
LFLAGS	=	-nologo -nodefaultlib -version:2.0 -subsystem:console -dll
LIBS	=	../../lib/sapi21.lib \
                kernel32.lib \
                msvcrt.lib 

####### Files
SOURCES =	updateonsave.cxx
OBJECTS =	updateonsave.obj 
TARGET	=	updateonsave20.dll

####### Build rules
all: $(TARGET)

$(TARGET): $(OBJECTS) 
	$(LINK) $(LFLAGS) -out:$(TARGET) $(OBJECTS) $(LIBS)

clean:
	-del *.obj
	-del updateonsave20.*
