#${
    if ((Config("qtgui") || (Config("darwin") && Config("qtexternal"))) && (Config("app") || Config("dll"))) {
        Project('SYS_LIBS = $$QT_SYSLIBS $$SYS_LIBS');
    }
    Project("DEFINES *= QT3_SUPPORT") if !Config("qt4") and (Config("qt") or Config("moc") or Config("qtexternal"));
    IncludeTemplate("pkg-info");
    if (0 != $is_unix) {
        IncludeTemplate("unix");
    }
    else {
        IncludeTemplate("win32");
    }
#$}
