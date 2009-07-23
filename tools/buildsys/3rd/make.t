#${
    if( Project( "SUBDIRS")) {
        IncludeTemplate( "3rd/make-subdirs");
    }
    else {
        IncludeTemplate( "3rd/make-project");
    }
#$}
