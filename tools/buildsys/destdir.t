#${
    if(!Project("SUBDIRS")) {
        my $destdir = Project("DESTDIR");
        if(!$destdir) {
            $destdir = '$(top_builddir)';
            if($Options{"package"}) {
                $destdir .= $dir_sep . $Options{"package"};
            }
            if(Config("app")) {
                $destdir .= $dir_sep . "bin";
            }
            else {
                $destdir .= $dir_sep . "lib";
            }
        }
        $destdir =~ s-[\\/]-$dir_sep-g;
        my $cwd = getcwd();

        if(!is_absolute_path($destdir)) {
            $destdir = $cwd . ($destdir ? $dir_sep . $destdir : "");
        }

        if (Project("PLATFORM_DESTDIRS") eq "yes") {
            my $platform = Project("PLATFORM");
            $destdir .= $dir_sep . $platform;
        }
        $destdir =~ s-[\\/]-$dir_sep-g;

        $destdir = fullpath($destdir) if(-d $destdir);
        Project("DESTDIR = $destdir");
        
        my $exp_destdir = expand_path($destdir);
        mkdirp($exp_destdir, 0755) unless(-d $exp_destdir);
    }
#$}
