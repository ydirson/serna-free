#
#  Clean files
#

CLEAN_CMD = #$ $text .= ($is_unix ? "rm -f" : "del /q /f");
CLEAN_FLIST = #$ Expand("FILES_TO_CLEAN");

#$ DisableOutput() unless (Project("FILES_TO_CLEAN"));
clean: clean_files

clean_files: force
	-$(CLEAN_CMD) $(CLEAN_FLIST)
#$ EnableOutput() unless (Project("FILES_TO_CLEAN"));
