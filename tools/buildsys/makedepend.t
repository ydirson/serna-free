#$ DisableOutput() if( Project( "TMAKE_OPTS") =~ /-nodepend/);

DEPFILE		= #$ $text = Project( "DEPEND_FILE") ? Project( "DEPEND_FILE") : ".depend";

DEFINES		= #$ ExpandGlue( "DEFINES", "-D", " -D", "");
MKDEP		= #$ Expand( "MKDEP");
MKDEP_OPTS	= #$ Expand( "MKDEP_OPTS");

$(DEPFILE): $(SOURCES)
	$(TOUCH) $(DEPFILE)
	$(MKDEP) $(INCPATH) $(DEFINES) $(MKDEP_OPTS) -f$(DEPFILE) $(SOURCES)

depend: $(DEPFILE)

#$ DisableOutput() if( 0 != $is_unix);
!IF EXISTS ("$(DEPFILE)")
!INCLUDE ("$(DEPFILE)")
!ENDIF
#$ EnableOutput() if( 0 != $is_unix);
#$ DisableOutput() if( 0 == $is_unix);
-include $(DEPFILE)
#$ EnableOutput() if( 0 == $is_unix);
#$ EnableOutput() if( Project( "TMAKE_OPTS") =~ /-nodepend/);
