#$ DisableOutput() if (Project("AUTOCONF_T_INCLUDED"));
######################################################################
# Syntext Editor autoconfiguration template
# 
######################################################################
#${
    my %TmpHash;
    foreach $option (split /\s+/, Project("CONFIG")) {
        $TmpHash{$option} = 1;
    }
    my $Config = join(" ", keys %TmpHash);
    Project("CONFIG = $Config");

    %TmpHash = ();
    foreach $option (split /\s+/, Project("options")) {
        $TmpHash{$option} = 1;
    }
    my $options = join(",", keys %TmpHash);
    Project("options = $options");
#$}

all_forward: gensrc all

gensrc: force

CONFIG          = #$ Expand("CONFIG");
options         = #$ Expand("options");
#$ Project("main_srcdir") and $text = "\nmain_srcdir     = " . Project("main_srcdir");
top_srcdir      = #$ Expand("top_srcdir");
top_builddir    = #$ Expand("top_builddir");
srcdir          = #$ Expand("srcdir");
THIRD_DIR       = #$ ExpandPathName("THIRD_DIR")
defaults        = #$ Expand("defaults");
local_defaults  = #$ Expand("local_defaults");
VERSION         = #$ Expand("VERSION");
PLATFORM        = #$ Expand("PLATFORM");
CMD_SEP         = #$ $text = $is_unix ? ';' : '&';

VPATH           = #$ $text = (!Project("NO_VPATH") && $is_unix) ? '$(srcdir)' : ""
#${
    IncludeTemplate("misc_vars");
    my $cwd = getcwd();
    Project("ALL_DEPS *= \$(MAKEFILE)");
    my $extra_tpls = "";
    foreach (split(/\s+/, Project("EXTRA_TEMPLATES"))) {
        $extra_tpls .= fullpath(find_template($_)) . " ";
    }
    Project("EXTRA_TEMPLATES = $extra_tpls");

    my $filetags = Project("FILETAGS");
    foreach (split(/\s+/, Project("CONFIG_VARS"))) {
        my $var = Project($_);
        if ($filetags =~ /$_/) {
            $var =~ s^[\\\/]^$dir_sep^g;
            $var = normpath($var);
        }
        $text .= "\n$_\t\t= " . $var;
    }
#$}
OBJECTS_DIR 		= #$ Expand("OBJECTS_DIR"); $text =~ s/\\+$//;

extra_templates         = #$ ExpandGlue("EXTRA_TEMPLATES", "", " \\\n\t\t\t", "");

$(MAKEFILE) configure:: $(srcdir)/$(PROJECT_FILE) $(defaults) $(local_defaults) $(extra_templates)
#${ 
    $text = "\t" . ($is_unix ? 'TMAKEPATH=$(TMAKEPATH); export TMAKEPATH; ' : '');
    $text .= <<'EOF'
$(TMAKE) $(TMAKE_OPTS) TMAKEPATH=$(TMAKEPATH) CONFIG="$(CONFIG)" \
        VERSION=$(VERSION) defaults=$(defaults) local_defaults=$(local_defaults) \
        main_srcdir=$(main_srcdir) top_srcdir=$(top_srcdir) top_builddir=$(top_builddir) \
        srcdir=$(srcdir) PLATFORM=$(PLATFORM) $(defaults) $(local_defaults) \
        THIRD_DIR=$(THIRD_DIR) $(srcdir)/$(PROJECT_FILE) -o $(MAKEFILE) \
        options+="$(options)" PROJECT_FILE=$(PROJECT_FILE) \
        MAKEFILE=$(MAKEFILE) "TMAKE=$(TMAKE)"
EOF
#$}
configure:: force

force:

#$ EnableOutput() if (Project("AUTOCONF_T_INCLUDED")); Project("AUTOCONF_T_INCLUDED = 1");
