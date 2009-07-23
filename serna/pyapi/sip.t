#
# Template for SIP
#
#${
    my $pyqt_pro = ScanProjectRaw(expand_project_var('$$THIRD_DIR/pyqt/TMakefile.pro'));
    Project("PYQT_VER = ".$$pyqt_pro{'VERSION'});
#$}
PYQT_VER    = #$ Expand("PYQT_VER");
PYQT_DIR    = #$ ExpandPathName("PYQT_DIR");

SIP_SOURCES = #$ ExpandGlue("SIP_SOURCES", " \\\n\t\t", " \\\n\t\t", "\n");
SIP_MODULE  = #$ Expand("SIP_MODULE");

SIP_MODULE_CPP = sip$(SIP_MODULE)part0.cpp
SIP            = #$ ExpandPathName("SIP");
SIP_SRCDIR     = #$ Expand("srcdir"); $text =~ s^\\^$\/^g; #! For sip to emit correct #line directives
SIP_OPTIONS    = #$ Expand("SIP_OPTIONS");

$(SIP_MODULE_CPP): $(SIP_SOURCES)
	$(SIP) $(SIP_OPTIONS) $(SIP_SRCDIR)/$(SIP_MODULE).sip
#${
    if (Project("PLATFORM") eq "darwin") {
        @lflags = grep { 0 == /((-dynamic_?lib)|(-install_name)|(.*\.dylib))/ } (split(/\s+/, Project("TMAKE_LFLAGS")));
        push @lflags, ("-bundle");
        Project("TMAKE_LFLAGS = " . join(' ', @lflags));
        Project("TMAKE_LFLAGS_SHLIB = ", "TMAKE_LFLAGS_SONAME = ");
    }
    if (!$is_unix && Config("release")) {
        my $lpath = expand_path('$(top_builddir)/serna/lib/serna.pro');
        my $libvars = ScanProjectRaw($lpath);
        my ($ldir, $lname) = map { $$libvars{$_} } qw/DESTDIR TARGET_FILE/;
        Project("LIBS += $ldir\\$lname");
    }
#$}
#$ Project("FILETOOLS = RM"); IncludeTemplate("filetools");
clean: sipclean

sipclean:
	$(RM) *SernaApiCore*.h *SernaApiCore*.cpp
