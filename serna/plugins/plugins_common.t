#!
#! Common tmake template for plugins
#!
#${
#!    print STDERR Project("LIBS"), "\n";
    my ($top_srcdir, $top_builddir, $cfg) = map { Project($_) } qw/top_srcdir top_builddir CONFIG/;
    my @cfg_list = split(/\s+/, $cfg);
    @cfg_list = grep { !($_ =~ "^static.*") } @cfg_list;
    push @cfg_list, "dynamic", "dll";
    push @cfg_list, "plugin" unless (Config("notaplugin"));
    Project("CONFIG = " . join(' ', @cfg_list));
    my $plugins_profile = "$top_srcdir/serna/plugins/plugins_common.pro";
    ScanProject($plugins_profile);
    IncludeTemplate("$top_srcdir/serna/sapi/shlib_undef");
    my @sapi_ver = get_version("$top_builddir/serna/sapi/build_version");
    my $sapi_lib = '-L$(top_builddir)/serna/lib';
    my $sapi_versfx = join("", @sapi_ver[0,1]);
    if ($is_unix) {
        $sapi_lib .= " -lsapi$sapi_versfx";
        if (Config("darwin") && !Config("notaplugin")) {
            Project("TMAKE_CXXFLAGS *= -fvisibility=hidden");
            Project('TMAKE_LFLAGS *= -exported_symbols_list $(top_srcdir)/serna/plugins/exports.darwin');
        }
    }
    else {
        $sapi_lib = "\$(top_builddir)\\serna\\lib\\sapi$sapi_versfx.lib";
    }
    Project('TARGET = $$TARGET$$VER_MAJOR$$VER_MINOR') if ($is_unix && !Project("NO_VER_SFX"));
    if (Project("USE_SAPI")) {
        Project('LIBS = '.$sapi_lib.' $$LIBS');
        Project('LIBS += -L$(top_builddir)/sfworks/lib -lcommon') if Config("darwin");
    }
    else {
        if (!$is_unix && Config("release")) {
            my $lpath = expand_path('$(top_builddir)/serna/lib/serna.pro');
            my $libvars = ScanProjectRaw($lpath);
            my ($ldir, $lname) = map { $$libvars{$_} } qw/DESTDIR TARGET_FILE/;
            Project("LIBS += $ldir\\$lname");
        }
    }
#$}
