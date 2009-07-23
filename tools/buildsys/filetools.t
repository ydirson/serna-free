#$ DisableOutput() if (Project("FILETOOLS_T_INCLUDED")); 
# 
# A template for exporting information about file tools to resulting makefile
# Called with FILETOOLS = #$ Expand("FILETOOLS");
#
#$ EnableOutput() if (Project("FILETOOLS_T_INCLUDED")); 

#$ file_tools();
#$ Project("FILETOOLS_T_INCLUDED = 1");
