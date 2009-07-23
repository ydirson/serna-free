#!MAKE    =    #$ Expand("MAKE");
#${
    if (0 != $is_unix) {
        IncludeTemplate("unix-subdirs");
    }
    else{
        IncludeTemplate("win32-subdirs");
    }
#$}
