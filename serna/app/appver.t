#${
    my ($appver, $appver_file) = ("", Project("APPVER_FILE"));
    unless ($appver_file) {
        $appver_file = expand_path('$(srcdir)'.$dir_sep.'APPVER');
	Project("APPVER_FILE = $appver_file");
    }
    if (-r $appver_file && open(APPVER, "<$appver_file")) {
        $appver = <APPVER>;
        close APPVER;
    }
    else {
        $appver = "1.0.0";
    }

    Project("APPVER = ".$appver);
#$}
