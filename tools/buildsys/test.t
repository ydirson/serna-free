#$ DisableOutput() unless (Config("app"));
#
# template for generating test run makefile commands
# 
#
# In order to invoke test program you should either have correctly set 
# the TESTSCRIPT make variable or create run_tests.(sh|cmd) script in $(srcdir)
# 'make test' will run this script with arguments as specified in rules below
# 'make test targs="arg1 arg2..."' will run test script with additional args
#
#${
    unless (!Config("app") && Project("TESTSCRIPT")) {
	my $ts = Project("srcdir") . $dir_sep . (($is_unix) ? "run_tests.sh" : "run_tests.cmd");
        Project("TESTSCRIPT = $ts");
    }
#$}
TESTSCRIPT = #$ Expand("TESTSCRIPT");

#$ DisableOutput() unless $is_unix;
export top_builddir top_srcdir srcdir
#$ EnableOutput() unless $is_unix;

test: all
	@echo Running $(TESTSCRIPT) $(srcdir) $(top_builddir) $(TARGET) $(targs)
#$ DisableOutput() if $is_unix;
	@if not exist $(TESTSCRIPT) echo "Test script " $(TESTSCRIPT) "does not exist" & exit 1
	@set top_srcdir=$(top_srcdir)
	@set top_builddir=$(top_builddir)
	@set srcdir=$(srcdir)
	@$(TESTSCRIPT) $(srcdir) $(top_builddir) $(TARGET) $(targs)
#$ EnableOutput() if $is_unix;
#$ DisableOutput() unless $is_unix;
	@if [ -x $(TESTSCRIPT) ]; then $(TESTSCRIPT) $(srcdir) $(top_builddir) $(TARGET) $(targs); \
	else echo "Test script" $(TESTSCRIPT) "does not exist"; exit 1; fi
#$ EnableOutput() unless $is_unix;
#$ EnableOutput() unless (Config("app"));
