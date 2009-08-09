#${
    my $make = Project( "MAKE");

#!    IncludeTemplate( "3rd/process_pkg_config");
    Project("ARCHIVE") && IncludeTemplate( "3rd/archive");

    Project("PACKAGE_MAKE = $make") unless Project("PACKAGE_MAKE");

    IncludeTemplate("3rd/make_vars");

    if (Project("PACKAGE_MAKEFILE")) {
        $text .= '$(MAKE_DIR)/$(PACKAGE_MAKEFILE): ';
        if (Project("PATCH_FILES")) {
            $text .= '$(PATCH_FLAG)';
        }
        elsif (Project("ARCHIVE")) {
            $text .= '$(UNPACK_FLAG)';
        }
    }
    $text .= "\n\nall_prereq: ";
    if (Project("PATCH_FILES")) {
        $text .= '$(PATCH_FLAG)';
    }
    elsif (Project("ARCHIVE")) {
        $text .= '$(UNPACK_FLAG)';
    }
    else {
        $text .= "force";
    }
#$}

#$ IncludeTemplate("3rd/emit_makefile_targets");
