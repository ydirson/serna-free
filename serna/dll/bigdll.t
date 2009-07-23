#${
    IncludeTemplate("utils");
    IncludeTemplate("dirs");
    my ($third_dir, $top_srcdir, $bigdll, $pf) = map { Project($_) } qw(THIRD_DIR top_srcdir BIGDLL PLATFORM);
    my $qtversion = get_version("$third_dir/qt/TMakefile.pro");
    $qtversion =~ s/[^\d]//g;
    my ($maj, $min, $patch) = get_version("$top_srcdir/serna/app/APPVER");

    IncludeTemplate("autoconf");
    Project("FILETOOLS = PERL RM");
    IncludeTemplate("filetools");
    my ($libname, $libbase, $linkname) = ("", "$bigdll$maj$min", "");
    if ("win32" eq $pf) {
        $libname = "$libbase.dll";
        $linkname = "$libbase.lib";
        Project("DLL_BASE = $libbase");
    }
    elsif ("darwin" eq $pf) {
        $libname = "lib$bigdll.dylib";
    }
    else {
        $libname = "lib$bigdll.so";
    }
    Project("BIGDLL = \$(top_builddir)/serna/lib/$libname");

    IncludeTemplate("extra/maketargetlists.t");
    
    my $dll_makefile = "NMakefile";
    Project("DLL_MAKEFILE = $dll_makefile");
    my $makefile = "LINK = " . Project("TMAKE_LINK") . "\n";
    $makefile .= <<'EOF';
!INCLUDE $(DLL_OBJECTS_LIST)

$(BIGDLL): $(OBJECTS) $(LIBS)
	$(LINK) $(LINK_FLAGS) @<<
        $(OBJECTS) $(LIBS) $(SYS_LIBS)
<<
	if exist $@.manifest mt -nologo -manifest $@.manifest -outputresource:$@;2
EOF
    write_file($dll_makefile, $makefile);
#!
    my $ddir = Project("DESTDIR");
    $ddir =~ s-[\\/]+$--;
    my ($tdir, $tbase) = (expand_path($ddir), "serna");
    my %buildinfo = (
        TARGET_BASE => $tbase,
        TARGET_FILE => $linkname,
        TARGET_DIR  => normpath($tdir),
        VERSION     => $maj.$min.$patch,
        DESTDIR     => normpath($ddir),
        CONFIG      => Project("CONFIG")
    );
    $buf = join("\n", map { "$_ = ".$buildinfo{$_} } keys %buildinfo);
    write_file("buildinfo.pro", $buf);
    my $target = expand_path($tdir).$dir_sep.$buildinfo{"TARGET_BASE"};
    write_file("${target}.pro", $buf);
#$}

LIBS          = #$ ExpandGlue("LIBS", "\\\n\t\t", " \\\n\t\t", "\n");
SYS_LIBS      = #$ ExpandGlue("SYS_LIBS", "\\\n\t\t", " \\\n\t\t", "\n");

LINK          = #$ Expand("TMAKE_LINK");
BIGDLL        = #$ ExpandPathName("BIGDLL");

DLL_OBJECTS_LIST  = objects.lst
TARGET_LISTS      = #$ ExpandGlue("TARGET_LISTS", " \\\n\t\t", " \\\n\t\t", "\n");
DLL_MAKEFILE      = #$ Expand("DLL_MAKEFILE");
MKOBJLST          = $(top_srcdir)/tools/scripts/makeobjlist.pl
LINK_FLAGS        = #$ Expand("LINK_FLAGS");

all: $(BIGDLL)

#$ DisableOutput() if $is_unix;
!IF EXIST($(DLL_OBJECTS_LIST))
! INCLUDE $(DLL_OBJECTS_LIST)
!ENDIF

DLL_BASE          = #$ Expand("DLL_BASE");

#$ EnableOutput() if $is_unix;

$(DLL_OBJECTS_LIST): $(MKOBJLST) $(TARGET_LISTS)
	set OBJECTS_VARIABLE=OBJECTS
	$(PERL) $(MKOBJLST) $@ $(TARGET_LISTS)
	
$(BIGDLL): $(DLL_OBJECTS_LIST) $(OBJECTS)
	$(MAKE) -f $(DLL_MAKEFILE) $@ LIBS="$(LIBS)" SYS_LIBS="$(SYS_LIBS)" \
	LINK_FLAGS="$(LINK_FLAGS)" BIGDLL="$(BIGDLL)" DLL_OBJECTS_LIST=$(DLL_OBJECTS_LIST)

clean:
	-$(RM) $(BIGDLL)
