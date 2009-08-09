#${    
    my $libraries = Project("LIBRARIES");
    my $programs = Project("PROGRAMS");

    if ($programs) {
        Project("TARGET = $programs", "CONFIG -= lib", "CONFIG *= app");
    }
    elsif ($libraries) {
        Project("TARGET = $libraries", "CONFIG *= lib", "CONFIG -= app");
    }

    if (!Project("TARGET")) {
        if (!Project("SUBDIRS")){
            tmake_error("target.t: Can't determine target name\n");
        }
    }
    else {
        if (Config("app")) {
            Project("TMAKE_TEMPLATE = app", "CONFIG -= dll", "CONFIG -= staticlib");
            Project("TMAKE_APP_FLAG = 1");
        }
        else {
            Project("TMAKE_TEMPLATE = lib");
            Project("TMAKE_LIB_FLAG = 1");
        }
        if (!$is_unix && Config("win32-batch")) {
            Project("TMAKE_APP_OR_DLL = 1") if (Config("app") || Config("dll"));
            Project("TMAKE_TEMPLATE = win32-project.t");
        }
    }

    my $srcdir = Project("srcdir").$dir_sep;
    my @ver = get_version("${srcdir}APPVER");
    my $verfile = join($dir_sep, qw($(srcdir) APPVER));
    if (!$ver[0]) {
        @ver = get_version("${srcdir}VERSION");
        @ver = (1, 0, 0) unless $ver[0];
        $verfile = join($dir_sep, qw($(srcdir) VERSION));
    }
    else {
        Project("APPVER = " . join('.', @ver));
    }
    $ver[2] = 0 unless ($ver[2] =~ m/^\d+$/);
    Project("VER_FILE = " . $verfile);

    Project("PRODUCT_NAME = " . Project("PROJECT")) unless Project("PRODUCT_NAME");
    Project("MODULE_NAME = " . Project("PROJECT")) unless Project("MODULE_NAME");

    Project("VERSION = " . join('.', @ver));
    Project("VER_MAJOR = $ver[0]", "VER_MINOR = $ver[1]", "VER_PATCH = $ver[2]");
#$}
