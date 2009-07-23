#$ DisableOutput() if (Config("darwin"));
#${
    if (!Project("SPELL_LIBDIR")) {
        my $ald = join($dir_sep, Project("THIRD_DIR"), "lib") . $dir_sep;
        $ald .= $dir_sep unless $is_unix;
        Project("SPELL_LIBDIR = $ald");
    }
#$}

SPELL_LIBNAME = #$ Expand("SPELL_LIB");
SPELL_LIBDIR  = #$ Expand("SPELL_LIBDIR"); $text =~ s/[\\\/]/$dir_sep/g;
SPELL_LIB     = #$ $text = '../bin/$(SPELL_LIBNAME)'; $text =~ s/[\\\/]/$dir_sep/g;

$(SPELL_LIB): $(SPELL_LIBDIR)$(SPELL_LIBNAME)
	#$ $text = $is_unix ? 'ln -s $? ../bin/$(@F)' : 'copy /y $? $@';

all: $(SPELL_LIB)
#$ EnableOutput() if (Config("darwin"));
