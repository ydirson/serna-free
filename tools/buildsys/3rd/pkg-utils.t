#!
#${

sub find_package {
    my ($pkgName) = @_;
    my $pkg = {};
    my $cflags = `pkg-config --cflags-only-other $pkgName`;

    unless ($?) {
        my $libs = `pkg-config --libs-only-l $pkgName`;
        my $lflags = `pkg-config --libs-only-other $pkgName`;
        my $includes = `pkg-config --cflags-only-I $pkgName`;
        grep { chomp $_ } ($cflags, $lflags, $libs, $includes );
        my @libs = grep { s/^-l// } (split /\s+/, $libs);
        my @includes = grep { s/^-I// } (split /\s+/, $includes);

        $pkg->{"NAME"} = $pkgName;
        $pkg->{"CFLAGS"} = $cflags;
        $pkg->{"LFLAGS"} = $lflags;
        $pkg->{"LIBS"} = join(' ', @libs);
        $pkg->{"INCLUDES"} = join(' ', @includes);
    }
    return $pkg;
}

sub dump_package {
    my ($pkg) = @_;
    my @info;
    foreach (keys %{$pkg}) {
        push @info, "$_\t\t= $pkg->{$_}";
    }
    return '' unless @info;
    return join("\n", @info)."\n";
}

sub write_package {
    my ($file, $pkg) = @_;
    my $pkgDump = dump_package($pkg);
    return unless $pkgDump;
    open(PKG, ">$file") or tmake_error("Can't open $file for writing");
    print PKG $pkgDump;
    close(PKG);
}

sub find_file_in_path {
    my ($file) = shift;
    my ($found, $fpath, $fdir);
    foreach (@_) {
        tmake_debug("try: $_/$file: ", -f "$_/$file", "\n");
        if (-f "$_/$file") {
            $fdir = "$_/$file";
            $fdir =~ s|[/\\][^/\\]*$||;
            $found = "$_/$file";
            $fpath = $_;
            last;
    	}
    }

    return map { s|[/\\]+|$dir_sep|g; $_ } ($found, $fpath, $fdir);
}

sub find_library {
    my ($libname) = @_;
    my ($found, $fpath, $fdir) = find_file_in_path("lib$libname.so", '/lib',
                                                   '/usr/lib');
    return ($fpath, $fdir);
}

sub find_header {
    my ($header) = @_;
    my ($found, $fpath, $fdir) = find_file_in_path($header, '/usr/include');
    return ($fpath, $fdir);
}

sub find_package_by_config {
    my ($pkgName, $config) = @_;
    my @libs = split /\s+/, `$config --libs`;

    unless ($?) {
        $pkg->{'NAME'} = $pkgName;
        my @cflags = split /\s+/, `$config --cflags`;
        my @lflags = grep { !/^-l/ && '-L/usr/lib' ne $_ } (@libs);
        @libs = grep { /^-l/ } (@libs);
        grep { s/^-l// } (@libs);
        my @includes = grep { /^-I/ } (@cflags);
        my @othercflags = grep { !/^-I/ } (@cflags);
        $pkg->{'LIBS'} = join(' ', @libs);
        $pkg->{'CFLAGS'} = join(' ', @othercflags) if @othercflags;
        $pkg->{'LFLAGS'} = join(' ', @lflags) if @lflags;
        $pkg->{'INCLUDES'} = join(' ', grep { s/^-I// } (@cflags));
    }
    return $pkg;
}

sub find_package_by_files {
    my ($header, $lib) = @_;
    my $pkg = {};
    my ($incdir, $incSubdir) = find_header($header);
    return $pkg unless $incdir;
    my ($libdir, $libSubdir) = find_library($lib);
    return $pkg unless $libdir;
    $incdir =~ s^/+^/^g;
    $pkg->{"LFLAGS"} = "-L$libdir" unless $libdir eq '/usr/lib';
    $pkg->{"LIBS"} = $lib;
    my $includes = '';
    foreach ($incdir, $incSubdir) {
        $includes .= " $_" unless $_ eq '/usr/include';
    }
    $pkg->{"INCLUDES"} = $includes if $includes;
    return $pkg;
}

#$}
