#############################################################################
# Makefile for building linkvoyager
# Project: linkvoyager
##############################################################################

####### Compiler, tools and options
CXX       = g++
CXX_FLAGS = -O2 -I../..
LD        = g++
LD_FLAGS  = -shared
LIBS	  = ../../bin/libsapi21.so.2

####### Files
SOURCES   = $(wildcard *.cxx)
OBJECTS   = $(notdir $(SOURCES:.cxx=.o))
TARGET    = liblinkvoyager20.so

####### Build rules
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LD_FLAGS) -o $@ $(OBJECTS) $(LIBS)

%.o: %.cxx
	$(CXX) -c $(CXX_FLAGS) -o $@ $^

clean:
	rm -f *.o $(TARGET)*
