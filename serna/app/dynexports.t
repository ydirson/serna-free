# 
# Template for linking plugin-ready Serna
#
#${
    my ($srcdir, $top_srcdir, $top_blddir, $pf) = 
        map {
            Project($_) 
        } qw(srcdir top_srcdir top_builddir PLATFORM);
    ScanProject("$srcdir/dynexports.pro");

    my $ld_objlist, $ld_explist;
    if ('darwin' eq $pf) {
        $ld_objlist = '-Wl,-filelist,$(OBJECTS_LIST)';
        $ld_explist = '-Wl,-exported_symbols_list,$(EXPORTS_LIST)';
    }
    else {
        $ld_objlist = '`cat $(OBJECTS_LIST)`';
	if ('sunos' eq $pf) {
            $ld_explist = '-Wl,-M$(EXPORTS_LIST)';
	}
	else {
            $ld_explist = '-Wl,--version-script=$(EXPORTS_LIST) -Wl,-export-dynamic';
	}
    }
    Project('LD_ADD += '.$ld_objlist);
    Project('LD_ADD += '.$ld_explist) if Project("MODULES");
    IncludeTemplate("extra/maketargetlists");
    my $plugins = Project("MODULES");
#!    $plugins =~ s^plugins/^^g;
    Project("PLUGINS = $plugins");
    IncludeTemplate("lderrors-makefile");
#$}
#$ DisableOutput() unless Project("MODULES");
EXPORTS_LIST      = #$ Expand("EXPORTS_LIST");
#$ DisableOutput() unless Config("darwin");
STRIP_FLAGS       = -s $(EXPORTS_LIST)
#$ EnableOutput() unless Config("darwin");
#$ EnableOutput() unless Project("MODULES");
TARGET_LISTS      = #$ ExpandGlue("TARGET_LISTS", " \\\n\t\t", " \\\n\t\t", "\n");
OBJECTS_LIST      = #$ Expand("OBJECTS_LIST");

MAKE_OBJLIST      = $(top_srcdir)/tools/scripts/makeobjlist.pl
MAKE_LDSCRIPT     = #$ Expand("MAKE_LDSCRIPT");
GET_UNDEF_SYMS    = $(srcdir)/get_undef_syms.sh
serna_dir         = $(top_builddir)/serna
PLUGINS           = #$ Expand("PLUGINS");

export  MAKE_OBJLIST \
        MAKE_LDSCRIPT \
        GET_UNDEF_SYMS \
        EXPORTS_LIST \
        PLUGINS \
        top_srcdir \
        top_builddir \
        serna_dir

#$ DisableOutput() unless Project("MODULES");
$(EXPORTS_LIST): force
	$(MAKE) -f $(srcdir)/Makefile.dynexports $@

#$ EnableOutput() unless Project("MODULES");
$(OBJECTS_LIST): $(TARGET_LISTS) $(MAKE_OBJLIST)
	$(MAKE_OBJLIST) $@ $(TARGET_LISTS)

gensrc: $(EXPORTS_LIST) $(OBJECTS_LIST)
        
LD_ADD       +=  #$ Expand("LD_ADD");

$(TARGET): $(cat $(OBJECTS_LIST))
