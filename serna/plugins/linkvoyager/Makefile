#############################################################################
# Makefile for building linkvoyager
# Project: linkvoyager
##############################################################################

####### Compiler, tools and options
CXX	=	cl
CXXFLAGS=	-GX -GR -DWIN32 -I"../.."
LINK	=	link
LFLAGS	=	-nologo -nodefaultlib -version:2.0 -subsystem:console -dll
LIBS	=	../../lib/sapi21.lib \
                kernel32.lib \
                msvcrt.lib \
                shell32.lib 

####### Files
SOURCES =	linkvoyager.cxx
OBJECTS =	linkvoyager.obj 
TARGET	=	linkvoyager20.dll

####### Build rules
all: $(TARGET)

$(TARGET): $(OBJECTS) 
	$(LINK) $(LFLAGS) -out:$(TARGET) $(OBJECTS) $(LIBS)

clean:
	-del *.obj
	-del linkvoyager20.*
