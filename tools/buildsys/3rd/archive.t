#${
    ScanProject(Project("THIRD_DIR")."/tools.pro");
    my $archive = expand_path(Project("ARCHIVE"));
    my $suffix = "";
    if ($archive =~ /\.([^\.]+)$/) {
        $suffix = $1;
    }

    if ($suffix eq 'gz' || $suffix eq 'bz2') {
        if ($archive =~ /\.([^\.]+)\.$suffix$/) {
            $suffix = $1. '.' . $suffix;
        }
    }

    unless (Project('ARCHIVE_DIR')) {
        my $archive_dir = Project('THIRD_DIR') + $dir_sep + 'packages';
        Project("ARCHIVE_DIR = $archive_dir");
    }

    my $command;
    my $unpack_prog;
    my $tar = Project("TAR"); 
    if (grep $_ eq $suffix, ('tar.bz2', 'tar.gz', 'tgz')) {
        $unpack_prog = ($suffix eq 'tar.bz2') ? Project("BZIP2") : Project("GZIP");
        my $win32_opts = $is_unix ? "" : "--atime-preserve -m";
        my $tar_opts = Project("TAR_OPTIONS");
        $tar_opts = '-xif' unless $tar_opts;
        $command = $unpack_prog . " -dc \$(ARCHIVE) | $tar -v $win32_opts $tar_opts - >\$(ARCHIVE_LIST)";
    }
    elsif ($suffix eq 'zip') {
        $unpack_prog = Project("UNZIP");
        $command = $unpack_prog . " -o \$(ARCHIVE) | \$(SED) -e \"s/^Archive:.*//; s/.*: //\" >\$(ARCHIVE_LIST)";
    }

    $command || tmake_error("Unrecognized suffix '" . $suffix . "'");
    $unpack_prog || tmake_error("Bad unpack program name");
    Project("UNPACK = $command");

    Project("ARCHIVE = $archive");
    Project('ARCHIVE_LIST = $$ARCHIVE.lst') unless Project("ARCHIVE_LIST");
#$}
#$ DisableOutput() unless(Project("ARCHIVE"));
#$ Project("FILETOOLS = TOUCH MKDIR CHDIR RMDIR RM SED"); IncludeTemplate("filetools");
ARCHIVE		= #$ ExpandPathName("ARCHIVE");

UNPACK_FLAG     = .unpack_done
ARCHIVE_LIST    = #$ Expand("ARCHIVE_LIST");

$(UNPACK_FLAG): $(ARCHIVE)
#$ $text = "\t( " . Project("UNPACK") . ' ) && $(TOUCH) $@';
#!	$(TOUCH) $@

ARCHIVE_DIR	        = #$ ExpandPathName("ARCHIVE_DIR");

#$ EnableOutput() unless(Project("ARCHIVE"));
#${
    IncludeTemplate("3rd/patch2");
#$}

PACKAGE_DIR     = #$ Expand("PACKAGE_DIR");

clean: archiveclean

archiveclean: #$ $text = Project("PATCH_FILES") ? "patchclean" : "";
	$(RM) $(UNPACK_FLAG) $(ARCHIVE_LIST)
	$(RMDIR) $(PACKAGE_DIR)

