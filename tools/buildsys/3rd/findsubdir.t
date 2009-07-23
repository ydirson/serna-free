#${

sub findsubdir() {
    my ($base, $third_dir, $defver) = @_;
    
    $third_dir = Project("THIRD_DIR") unless $third_dir;
    my $pkgdir = "$third_dir/$base";
    
    my $cfg = ScanProjectRawEx(1, "$pkgdir/TMakefile.pro", "$pkgdir/$base-defaults.pro");
    
    $subdir = defined($$cfg{'SUBDIRS'}) ? $$cfg{'SUBDIRS'} : $defver;
    return join($dir_sep, $pkgdir, $subdir, $$cfg{'PACKAGE_DIR'});
}
    
#$}
