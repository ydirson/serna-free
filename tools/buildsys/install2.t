#
#  3rd party components installation template
#
#  
#
#${
    if ($is_unix) {
        IncludeTemplate("install-unix");
    }
    else {
        IncludeTemplate("install-win32");
    }
#$}
BIN_DESTDIR = #$ Expand("BIN_DESTDIR"); $text = normpath($text);
LIB_DESTDIR = #$ Expand("LIB_DESTDIR"); $text = normpath($text);
THIRD_DIR   = #$ Expand("THIRD_DIR"); $text = normpath($text);

