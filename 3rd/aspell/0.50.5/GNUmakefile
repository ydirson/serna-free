
all: default

install: all

DEFINES  = -DBUILD_ASPELL_LIB
INCLUDES = -Icommon -Iinterfaces/cc -I. -Imodules/speller/default
CXX      = c++
OBJEXT   = o
MODULE   = aspell
LD       = c++
OBJDIR   = .shobj


PLATFORM := $(shell uname -s | tr -d '\n')

BASE     = $(MODULE)

ifeq ($(debug),1)
    LD_DBG_FLAGS  = -g
endif

ASPELL_EXE_LIBS = -lncurses

ifeq ($(PLATFORM),Darwin)
    LIBSFX          := dylib
    LD_LIB_FLAGS    = -arch i386 -arch ppc -Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk \
                      -single_module -flat_namespace -undefined suppress -Wl,-search_paths_first \
                      -install_name lib$(BASE).15.dylib -dynamiclib

    CXX_ARCH_FLAGS  = -arch i386 -arch ppc -isysroot /Developer/SDKs/MacOSX10.4u.sdk
else
    ifeq ($(PLATFORM),SunOS)
        CXX = g++
        LD = g++
        CC = g++
        LD_PF_FLAGS = -Wl,-h,lib$(BASE).so.15 -Wl,-z,defs -L/usr/local/lib \
                      -lc -lstdc++ -Wl,-B,static -lgcc -Wl,-B,dynamic
        ASPELL_EXE_LIBS = -ltermcap
        WLC_EXE_LIBS    = -ltermcap
    else
        LD_PF_FLAGS = -Wl,-soname,lib$(BASE).so.15 -Wl,--no-undefined
    endif
    LIBSFX          := so
    LD_LIB_FLAGS    += -shared $(LD_PF_FLAGS)
    CXX_ARCH_FLAGS  = -fPIC
endif

TARGET   = lib$(BASE).$(LIBSFX)

ifeq ($(debug),1)
    CXX_DBG_FLAGS = -g
else
    CXX_DBG_FLAGS = -O2
    DEFINES += -DNDEBUG
endif

CXX_FLAGS = -pipe $(DEFINES) $(INCLUDES) $(CXX_DBG_FLAGS) $(CXX_ARCH_FLAGS)

LD_FLAGS  = $(LD_DBG_FLAGS)

include ../Makefile.objs

SRCDIR = .

PROGS = aspell word_list_compress

default: $(PROGS)

$(TARGET): $(OBJECTS)
	$(LD) $(LD_FLAGS) $(LD_LIB_FLAGS) -o $@ $(OBJECTS)

aspell: $(ASPELL_OBJECTS) $(TARGET)
	$(LD) $(LD_FLAGS) -o $@ $(ASPELL_OBJECTS) $(TARGET) $(ASPELL_EXE_LIBS)

word_list_compress: $(OBJDIR)/compress.o $(TARGET)
	$(LD) $(LD_FLAGS) -o $@ $(OBJDIR)/compress.o  $(WLC_EXE_LIBS)

MODSRC := $(addprefix modules/,filter speller/default tokenizer)
VPATH := $(addprefix $(SRCDIR)/,common lib prog $(MODSRC))

$(OBJDIR)/%.o: %.cpp
	-@[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	$(CXX) -c -o $@ $(CXX_FLAGS) $<

$(OBJDIR)/%.o: %.c
	-@[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
	$(CC) -c -o $@ $(CXX_FLAGS) $<

CLEANFILES = $(OBJDIR) $(RES_FILE) $(TARGET) $(CLEAN)

clean:
	-rm -fr $(CLEANFILES)

.PHONY: makedirs clean
