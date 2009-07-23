#
RES2CXX_LIST    = #$ Expand("RES2CXX_LIST");
RES2CXX_OUTPUT  = #$ Expand("RES2CXX_OUTPUT");
RES2CXX_BASE    = #$ Expand("RES2CXX_BASE");
RES2CXX_SCRIPT  = $(top_srcdir)/tools/buildsys/resource2cxx.py
PYTHON    = #$ $text = '$(THIRD_DIR)/bin/python'; $text =~ s-[\\\/]-$dir_sep-g;

$(RES2CXX_OUTPUT):	$(RES2CXX_LIST)
#$ DisableOutput() if $is_unix;
	set PATH=%PATH%;$(THIRD_DIR)/lib
#$ EnableOutput() if $is_unix;
#$ DisableOutput() unless ($is_unix && !Config("darwin"));
	LD_LIBRARY_PATH=$${LD_LIBRARY_PATH}:$(THIRD_DIR)/lib \
#$ EnableOutput() unless ($is_unix && !Config("darwin"));
#$ DisableOutput() unless ($is_unix && Config("darwin"));
	DYLD_LIBRARY_PATH=$${DYLD_LIBRARY_PATH}:$(THIRD_DIR)/lib \
#$ EnableOutput() unless ($is_unix && Config("darwin"));
	$(PYTHON) $(RES2CXX_SCRIPT) -b $(RES2CXX_BASE) -o $(RES2CXX_OUTPUT) -l $(RES2CXX_LIST)
