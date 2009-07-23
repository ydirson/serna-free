#${
    if (Config("win32")) {
        Project('TARGET = serna-$$APPVER');
        Project('PROGRAMS = serna-$$APPVER');
    }
#$}
