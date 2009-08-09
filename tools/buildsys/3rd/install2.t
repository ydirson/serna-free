#$ DisableOutput() if (Project("INSTALL2_T_INCLUDED"));
#
#  3rd party components installation template
#
#$ IncludeTemplate("3rd/buildlog") unless (Project("INSTALL2_T_INCLUDED"));
BIN_DESTDIR = #$ Expand("BIN_DESTDIR"); $text = normpath($text);
LIB_DESTDIR = #$ Expand("LIB_DESTDIR"); $text = normpath($text);
#!THIRD_DIR   = #$ Expand("top_builddir"); $text = normpath($text."$dir_sep..");
#${
unless (Project("INSTALL2_T_INCLUDED")) {
    my $reldir = fullpath(Project("PACKAGE_DIR"));
    if ("tmake" eq $Options{"buildtype"}) {
        $reldir = Project("top_srcdir");
    }
    $reldir =~ s/\\\s*$//;
    $text = "INST_SRCDIR = $reldir\n";
    $text .= "INST_RELDIR = " . relative($reldir, normpath(Project("top_builddir")."$dir_sep..")) . "\n";
}
#$}
#${
my $req_env_vars = join(' ', qw/INST_SRCDIR THIRD_DIR INST_RELDIR CONFIG PLATFORM/);
Project("INSTALL_ENV_VARS += $req_env_vars");
unless (Project("INSTALL2_T_INCLUDED")) {
    if ($is_unix) {
        IncludeTemplate("3rd/install-unix");
    }
    else {
        IncludeTemplate("3rd/install-win32");
    }
}
#$}
install: install_platform

#$ EnableOutput() if (Project("INSTALL2_T_INCLUDED")); Project("INSTALL2_T_INCLUDED = 1");
