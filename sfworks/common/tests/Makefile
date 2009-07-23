#
# Makefile for Type matching testing
#
#

MWARE   =../..
MODE    =test
SUBSYS  =common

include $(MWARE)/Makefile.defs

ifeq ($(OPSYS),sunos)
    CXX_FLAGS += -D_USE_SPARC_ATOMIC
endif

TEST_SRC        := $(wildcard *.cpp)
TEST_OBJS       := $(TEST_SRC:%.cpp=$(OBJ_DIR)%.$O)
TEST_DRIVERS    := $(patsubst %.cpp,$(TEST_DEST)%$(E),$(TEST_SRC))
SCRIPTS         += ./run_common_test.sh ./run_common_test_long.sh
#TEST_RUNS       := $(patsubst %.cpp,run_%,$(TEST_SRC))
TEST_DATA_DIR   := ../../tests

export TEST_DATA_DIR

RMFILES         += $(TEST_DRIVERS)

INCLUDES    += -I$(COMMON_IFACE)

ALL_SRC=$(TEST_SRC)

LIBS= $(EOLLIBS)

include $(MWARE)/Makefile.common

default:all

all: copyscripts $(TEST_DRIVERS)

copyscripts:
	chmod 755 $(SCRIPTS)
	cp $(SCRIPTS) *.data $(TEST_DEST)

test_run:
	cd $(TEST_DEST) ; $(SHELL) ./run_common_test.sh

test_long_run:
	cd $(TEST_DEST) ; $(SHELL) ./run_common_test_long.sh

$(OBJ_DIR)%.$O: %.cpp
	$(CXX) $(CXX_OFLAGS) $(DEFINES) $(INCLUDES) $(CXX_FLAGS) $(CXX_ADD) -c $<

$(TEST_DEST)%$E: $(OBJ_DIR)%.$O $(LIBS)
	$(LD) $(LD_FLAGS) $(LD_OFLAGS) $^ $(LIBS) $(SSL_IMPL) $(SYS_LIBS)
