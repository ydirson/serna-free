#!  
#!  generates file containing object files list, target name and version
#!
#${
    my $objlist = Project("TARGET_OBJECTS");
    unless ($objlist) {
        $objlist = "target_objects.lst";
        Project("TARGET_OBJECTS = $objlist");
    }
    my $allobjs = Project("OBJECTS").' '.Project("OBJMOC");
    my $buf = join("\n", split(/\s+/, $allobjs));
    write_file($objlist, $buf);
    
    my @ver = map { Project($_) } qw/VER_MAJOR VER_MINOR VER_PATCH/;
    my ($ddir, $sdir) = map { Project($_) } qw/DESTDIR srcdir/;
    my ($versfx, $tfile) = (join('.', @ver), "");
    $ddir =~ s-[\\/]+$--;
    my ($tdir, $tbase) = (expand_path($ddir), Project(Config("app") ? "PROGRAMS" : "LIBRARIES"));
    if ($is_unix) {
        $tfile = "lib$tbase.";
        if (Config("dll")) {
            $tfile .= "darwin" eq platform() ? "$versfx.dylib" : "so.$versfx";
        }
        else {
            $tfile .= 'a';
        }
    }
    else {
        $tfile = "$tbase" . (Config("dll") ? join('', @ver[0,1]) : '') . ".lib";
    }
    my %buildinfo = (
        SRCDIR      => $sdir,
        TARGET_BASE => $tbase,
        TARGET_FILE => $tfile,
        TARGET_DIR  => normpath($tdir),
        VERSION     => $versfx,
        DESTDIR     => normpath($ddir),
        CONFIG      => Project("CONFIG")
    );
    $buf = join("\n", map { "$_ = ".$buildinfo{$_} } keys %buildinfo);
    write_file("buildinfo.pro", $buf);
    my $target = expand_path($tdir).$dir_sep.$buildinfo{"TARGET_BASE"};
    write_file("${target}.pro", $buf);
#$}
TARGET_OBJECTS = #$ Expand("TARGET_OBJECTS");
