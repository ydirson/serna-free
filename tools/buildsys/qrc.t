#${
        Project('RCC_CMD = $(top_builddir)/bin/rcc');
#$}
#
# 
# A template for generation of Serna documentation 
#
RCC         = #$ Expand("RCC_CMD"); $text =~ s/[\\\/]/$dir_sep/g;
RCC_INPUT   = #$ Expand("RCC_INPUT");
RCC_OUTPUT  = #$ Expand("RCC_OUTPUT");
RCC_BASE    = #$ Expand("RCC_BASE");

$(RCC_OUTPUT):	$(RCC_INPUT)
#$ DisableOutput() if $is_unix;
	set PATH=%PATH%;$(THIRD_DIR)/lib
#$ EnableOutput() if $is_unix;
#$ DisableOutput() unless ($is_unix && !Config("darwin"));
	LD_LIBRARY_PATH=$${LD_LIBRARY_PATH}:$(THIRD_DIR)/lib \
#$ EnableOutput() unless ($is_unix && !Config("darwin"));
#$ DisableOutput() unless ($is_unix && Config("darwin"));
	DYLD_LIBRARY_PATH=$${DYLD_LIBRARY_PATH}:$(THIRD_DIR)/lib \
#$ EnableOutput() unless ($is_unix && Config("darwin"));
	$(RCC) -base $(RCC_BASE) -o $(RCC_OUTPUT) $(RCC_INPUT)
