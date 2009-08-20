#${
    Project("PYTHONPATH = ".normpath(Project("PYTHONPATH"))) if Project("PYTHONPATH");
    my $third_dir = Project("THIRD_DIR");
    my $jp = ScanProjectRaw("$third_dir/jdk/TMakefile.pro");
    Project("JAVA_VER=".$jp->{'VERSION'});
    Project('JAVA_HOME=$(THIRD_DIR)/jdk/$(JAVA_VER)/jdk-$(JAVA_VER)/jre');
#$}
PLATFORM        = #$ Expand("PLATFORM");
CPU             = #$ Expand("CPU");
PYTHONHOME      = #$ Expand("PYTHONHOME");
PYTHONPATH      = #$ Expand("PYTHONPATH");
JAVA_VER        = #$ Expand("JAVA_VER");
JAVA_HOME       = #$ ExpandPathName("JAVA_HOME");
#$ DisableOutput() unless $is_unix
#!
#!  generate script to set environment variables for serna
#!
VARSCRIPT   := $(top_builddir)/serna/bin/envserna.sh

USERSH      := #$ $text = $is_unix ? (getpwuid($<))[8] : "cmd.exe";
SHELLID     := $(notdir $(USERSH))
BOURNE      := $(filter-out tcsh csh,$(SHELLID))
SETENV      := $(if $(BOURNE),export,setenv)
EQ          := $(if $(BOURNE),=, )
GREP        := $(if $(findstring $(PLATFORM),sunos),/usr/xpg4/bin/grep,grep)

links:
	rm -f $(top_builddir)/serna/bin/assistant
	ln -s $(THIRD_DIR)/bin/assistant $(top_builddir)/serna/bin/
	-mkdir -p $(top_srcdir)/serna/dist/bin
	-rm -f $(top_srcdir)/serna/dist/bin/xsltproc
	-ln -s $(THIRD_DIR)/bin/xsltproc $(top_srcdir)/serna/dist/bin/xsltproc
        
LLP         := $(if $(filter $(PLATFORM),darwin),DYLD_LIBRARY_PATH,LD_LIBRARY_PATH)
LDLIBPATH    = $(top_builddir)/serna/lib:$(top_builddir)/sfworks/lib:$(THIRD_DIR)/lib$(if \
                $(filter $(PLATFORM),sunos),:/usr/local/lib,):'$${AXF4_LIB_FOLDER}':'$${JAVA_HOME}/lib/i386:$${JAVA_HOME}/lib/i386/client'

SH_ENV      := $(top_builddir)/serna/bin/sh_env.sh
TCSH_ENV    := $(top_builddir)/serna/bin/tcsh_env.sh

$(TCSH_ENV): $(srcdir)/script2.t
	@echo set BCAT='$(top_builddir)/serna/bin/buildent-catalog.xml' > $@
	@echo if '($${?XML_CATALOG_FILES})' then >> $@
	@echo '  echo $${XML_CATALOG_FILES} | tr " " "\n" | $(GREP) -q "$${BCAT}" || \
            setenv XML_CATALOG_FILES "$${BCAT} $${XML_CATALOG_FILES}"' >> $@
	@echo else >> $@
	@echo '  setenv XML_CATALOG_FILES $${BCAT}' >> $@
	@echo endif >> $@
	@echo if '($${?$(LLP)})' then >> $@
	@echo '  setenv $(LLP) $(LDLIBPATH):$${$(LLP)}' >> $@
	@echo else >> $@
	@echo '  setenv $(LLP) $(LDLIBPATH)' >> $@
	@echo endif >> $@

$(SH_ENV): $(srcdir)/script2.t
	@echo $(SETENV) BCAT$(EQ)'$(top_builddir)/serna/bin/buildent-catalog.xml' > $@
	@echo 'echo $${XML_CATALOG_FILES} | tr " " "\n" | $(GREP) -q "$${BCAT}" || \
            $(SETENV) XML_CATALOG_FILES$(EQ)"$${BCAT}`test -z "$${XML_CATALOG_FILES}" || \
            echo \ $${XML_CATALOG_FILES}`"' >> $@
	@echo $(SETENV) $(LLP)$(EQ)$(LDLIBPATH):'$${$(LLP)}' >> $@
            
SERNA_WRAPPER := $(top_builddir)/serna/bin/serna.sh

$(VARSCRIPT): $(srcdir)/script2.t $(SH_ENV) $(TCSH_ENV) $(SERNA_WRAPPER)
	@echo $(SETENV) SERNA_DATA_DIR$(EQ)$(top_srcdir)/serna/dist > $@
	@echo $(SETENV) PYTHONHOME$(EQ)'$(THIRD_DIR)/python/install' >> $@
	@echo $(SETENV) JAVA_HOME$(EQ)'$(JAVA_HOME)' >> $@
	@echo $(SETENV) QT_PLUGIN_PATH$(EQ)'$(THIRD_DIR)/qt/plugins' >> $@
	@echo $(SETENV) AXF4_HOME$(EQ)/usr/XSLFormatterV4 >> $@
	@echo $(SETENV) AXF4_ETC_FOLDER$(EQ)'$${AXF4_HOME}/etc' >> $@
	@echo $(SETENV) AXF4_FONT_CONFIGFILE$(EQ)'$${AXF4_ETC_FOLDER}/font-config.xml' >> $@
	@echo $(SETENV) AXF4_LIB_FOLDER$(EQ)'$${AXF4_HOME}/lib' >> $@
	@echo $(SETENV) AXF4_BIN_FOLDER$(EQ)'$${AXF4_HOME}/bin' >> $@
	@echo $(SETENV) AXF4_SDATA_FOLDER$(EQ)'$${AXF4_HOME}/sdata' >> $@
	@echo $(SETENV) AXF4_LIC_PATH$(EQ)'$${AXF4_ETC_FOLDER}' >> $@
	@echo $(SETENV) AXF4_HYPDIC_FOLDER$(EQ)'$${AXF4_ETC_FOLDER}/hyphenation' >> $@
	@echo $(SETENV) AXF4_DMC_TBLPATH$(EQ)'$${AXF4_HOME}/sdata/base2' >> $@
	@echo 'test `basename $${SHELL}` = tcsh && source $(TCSH_ENV)' >> $@
	@echo 'test `basename $${SHELL}` != tcsh && source $(SH_ENV)' >> $@
	chmod 755 $@
	
$(SERNA_WRAPPER):
	@echo -e '\043!/bin/sh' > $@
	@echo 'SDIR=`dirname $$0`' >> $@
	@echo . '$${SDIR}/envserna.sh' >> $@
	@echo exec '$${SDIR}/serna' '"$$@"' >> $@
	chmod 755 $@

.PHONY: links envscript rmscript

#$ EnableOutput() unless $is_unix
#$ DisableOutput() if $is_unix
#$ Project("FILETOOLS = SED"); IncludeTemplate("filetools") unless $is_unix;
VARSCRIPT    = $(top_builddir)\serna\bin\sernaexec.cmd
SERNA_START  = #$ Expand("SERNA_START");
SERNA_ARGS   = #$ Expand("SERNA_ARGS");

links: \
        $(top_builddir)\serna\bin\assistant.exe \
	$(top_builddir)\serna\bin\xsltproc.cmd

$(top_builddir)\serna\bin\assistant.exe: $(THIRD_DIR)\bin\assistant_bin.exe
	copy /b $(THIRD_DIR)\bin\assistant_bin.exe $@
        
$(top_builddir)\serna\bin\xsltproc.cmd: $(THIRD_DIR)\bin\xsltproc.cmd
	copy /b $(THIRD_DIR)\bin\xsltproc.cmd $@

$(VARSCRIPT): $(srcdir)\script2.t
	@echo @set SERNA_DATA_DIR=$(top_srcdir)\serna\dist> $@
	@echo @set XML_CATALOG_FILES=$(top_builddir)\serna\bin\buildent-catalog.xml \
	    %%XML_CATALOG_FILES%%>> $@
	@echo @set JAVA_HOME=$(JAVA_HOME)>> $@
	@echo @set SERNA_BUILD_DIR=$(top_builddir)>> $@
	@echo @set PATH=$(top_builddir)\serna\lib;$(top_builddir)\sfworks\lib;$(THIRD_DIR)\lib;\
            % PATH% | $(SED) -e "s/\([;%]\) /\1/g">> $@
	@echo @set XML_CATALOG_FILES=$(top_builddir)\serna\bin\buildent-catalog.xml \
                    %%XML_CATALOG_FILES%%>> $@
	@echo set QT_PLUGIN_PATH=$(THIRD_DIR)/qt/plugins>> $@
	@echo @set PYTHONHOME=$(PYTHONHOME)>> $@
	@echo @set PYTHONPATH=$(PYTHONPATH)>> $@
	@copy $@ $(@R).bat
	@echo @if not "%GUITEST_PREFIX%" == "" set DLLPRELOAD=%GUITEST_PREFIX%\bin\dllpreload.exe>> $(@R).bat
	@echo @%DLLPRELOAD% $(SERNA_START) serna.exe %%* $(SERNA_ARGS)>> $(@R).bat
	@echo $(SERNA_START) %~dp0serna.exe %%* $(SERNA_ARGS)>> $@
#$ EnableOutput() if $is_unix
#$ DisableOutput() unless "darwin" eq Project("PLATFORM");
MACAPP = $(top_builddir)/serna/bin/serna.app/Contents/MacOS/serna

$(MACAPP): $(top_builddir)/serna/bin/serna
	mkdir -p $(@D)
	ln -sf ../../../serna $(@D)

all: $(MACAPP)

#$ EnableOutput() unless "darwin" eq Project("PLATFORM");
#$ Project("FILETOOLS = RM"); IncludeTemplate("filetools");
rmscript:
	-@$(RM) $(VARSCRIPT)
	
envscript: rmscript $(VARSCRIPT)
        
all: $(VARSCRIPT) links
