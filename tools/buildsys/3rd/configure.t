#${
    if (Project("SUBDIRS")) {
        IncludeTemplate("3rd/configure-subdirs");
    }
    else {
        IncludeTemplate("3rd/configure-project");
    }
#$}
