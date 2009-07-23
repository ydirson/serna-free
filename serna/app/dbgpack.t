#${
    my $cwd = getcwd();
    my $drive = substr($cwd, 0, 2);
    my $basedir = $cwd;
    $basedir =~ s/\\/\//g;
    $basedir =~ s/\/build\/serna\/app$//g;
    my $top_srcdir = Project("top_srcdir");
    chdir($top_srcdir);
    my $svnrev = "";
    foreach (`svn info`) {
        /^Revision: (\d+)$/ and $svnrev = $1;
    }
    chdir($cwd);
    Project("DBG_DIRS = $basedir");
    my $qbd_data = join($dir_sep, Project("THIRD_DIR"), "qt", "mainbuilddir.txt");
    my $qbd_dict = ScanProjectRaw($qbd_data);
    my $qbd_dir = $$qbd_dict{"MAIN_BUILDDIR"};
    $qbd_dir =~  s/\\/\//g;
    Project("DBG_DIRS += $qbd_dir");

    my @tm = localtime();
    my $reldate = sprintf("%4d%02d%02d", ($tm[5]+1900), ($tm[4]+1), $tm[3]);
    my $release = Project("APPVER");
    my $archive = join('/', $drive, "serna-dbg-$release-$reldate-$svnrev.zip");
    Project("DBG_ARCHIVE = $archive");
    Project("FILETOOLS = ZIP");
    IncludeTemplate("filetools");
#$}

DBG_EXCLUDES = -x "*.obj" -x "*/.svn/*-base" -x "*.pch" -x "*.so"
DBG_ARCHIVE  = #$ Expand("DBG_ARCHIVE");
DBG_DIRS     = #$ Expand("DBG_DIRS");

dbgpack: force
	$(ZIP) -qS5r $(DBG_ARCHIVE) $(DBG_DIRS) $(DBG_EXCLUDES)
