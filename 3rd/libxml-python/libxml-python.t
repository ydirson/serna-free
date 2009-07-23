#${
    my $third_dir = Project("THIRD_DIR");
    my $sfx = "_d" if Config("debug");

    my @pyver = get_version("$third_dir/python/TMakefile.pro");
    my $pylib = "python@pyver[0]@pyver[1]$sfx.lib";
    Project("XML2_LIBS *= $pylib");

    my $libxmlver = get_version("$third_dir/libxml/TMakefile.pro");
    my $libxsltver = get_version("$third_dir/xsltproc/TMakefile.pro");
    Project("XML2_SRCDIR = \$(THIRD_DIR)\\libxml\\$libxmlver\\libxml2-$libxmlver\\python");
    Project("XSLT_SRCDIR = \$(THIRD_DIR)\\xsltproc\\$libxsltver\\libxslt-$libxsltver\\python");
    foreach (qw/xslt xml2/) {
        my $pfx = uc $_;
        my @mak;
        push @mak, "NAME = $_";
        push @mak, 'THIRD_DIR = '.Project("THIRD_DIR");
        push @mak, 'INCLUDES = -I'.join(' -I', split(';', Project("${pfx}_INCPATH")));
        push @mak, 'SRCDIR = '.Project("${pfx}_SRCDIR");
        push @mak, 'OBJDIR = '.Project("${pfx}_OBJDIR");
        push @mak, "MODULE = lib${_}mod$sfx";
        push @mak, "INITFUNC = initlib${_}mod";
        push @mak, "LIBS   = ".Project("${pfx}_LIBS");
        push @mak, "LIBPATH = \$(THIRD_DIR)\\lib";
        my (@srcs, @objs, @deps);
        foreach (split /\s+/, Project("${pfx}_SOURCES")) {
            my $obj = "\$(OBJDIR)$dir_sep$_";
            $obj =~ s/.c$/.obj/;
            push @deps, "$obj: \$(SRCDIR)$dir_sep$_";
            push @objs, $obj;
        }

        push @mak, 'OBJECTS = '.join(' ', @objs);
        push @mak, join("\n", @deps);

        open(MAK, ">${_}mod.mak");
        print MAK join("\n", @mak);
        close(MAK);
    }
    Project("FILETOOLS = SED", "MODSFX = $sfx");
    IncludeTemplate("filetools");
#$}
XML2_SRCDIR = #$ ExpandPathName("XML2_SRCDIR");
XSLT_SRCDIR = #$ ExpandPathName("XSLT_SRCDIR");

MODSFX   = #$ Expand("MODSFX");

XML2MOD  = libxmlmod$(MODSFX).pyd
XSLTMOD  = libxsltmod$(MODSFX).pyd

all: xml2 xslt

SEDCMD = "s/import lib\(.*\)mod/from libxmlmods import lib\1mod/"

xml2: $(XML2MOD)
	$(SED) -e $(SEDCMD) < $(XML2_SRCDIR)\libxml.py > libxml2.py
	type $(XML2_SRCDIR)\libxml2class.py >> libxml2.py
	$(SED) -e $(SEDCMD) < $(XML2_SRCDIR)\drv_libxml2.py > drv_libxml2.py

xslt: $(XSLTMOD)
	$(SED) -e $(SEDCMD) < $(XSLT_SRCDIR)\libxsl.py > libxslt.py
	type $(XSLT_SRCDIR)\libxsltclass.py >> libxslt.py

$(XML2MOD): 
	nmake -f ModMakefile PYMODCFG=xml2mod.mak #$ Expand("PACKAGE_MAKEOPTS");

$(XSLTMOD): $(XSLTMOD_OBJ)
    nmake -f ModMakefile PYMODCFG=xsltmod.mak #$ Expand("PACKAGE_MAKEOPTS");

clean:
    nmake -f ModMakefile PYMODCFG=xsltmod.mak clean
    nmake -f ModMakefile PYMODCFG=xml2mod.mak clean
