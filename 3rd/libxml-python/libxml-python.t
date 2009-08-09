#${
    if (Config("syspkg") || Config("syspkgonly")) {
        IncludeTemplate("libxml-python-system");
    }
    IncludeTemplate("libxml-python-included");
#$}
