#!
#!    $(PACKAGE_xxx_DESTDIR)/$(xxx_TARGET) -> $(xxx_DESTDIR)/$(xxx_TARGET_INST)
#!
#${
    my $bindestdir = Project("BIN_DESTDIR");
    my $libdestdir = Project("LIB_DESTDIR");
    $bindestdir = Project("PACKAGE_BIN_DESTDIR") if(!$bindestdir);
    $libdestdir = Project("PACKAGE_LIB_DESTDIR") if(!$libdestdir);

    $bindestdir =~ s-[\\/]$--;
    $libdestdir =~ s-[\\/]$--;

    if(Project("BIN_PLATFORM_DESTDIRS") eq 'yes' && $bindestdir) {
        $bindestdir .= $dir_sep . Project("PLATFORM");
    }
    if(Project("LIB_PLATFORM_DESTDIRS") eq 'yes'  && $libdestdir) {
        $libdestdir .= $dir_sep . Project("PLATFORM");
    }
    Project("LIB_DESTDIR = $libdestdir", "BIN_DESTDIR = $bindestdir");

    foreach (split / /, "LIB_DESTDIR BIN_DESTDIR BIN_TARGETS LIB_TARGETS PACKAGE_BIN_DESTDIR PACKAGE_LIB_DESTDIR") {
        $project{$_} =~ s-[/\\]+-$dir_sep-g;
        $project{$_} =~ s-[/\\]$--;
    }
#$}
#
#    3rd party component install template
#    
#
PACKAGE_BIN_DESTDIR = #$ Expand("PACKAGE_BIN_DESTDIR");
PACKAGE_LIB_DESTDIR = #$ Expand("PACKAGE_LIB_DESTDIR");

BIN_DESTDIR    = #$ Expand("BIN_DESTDIR");
LIB_DESTDIR    = #$ Expand("LIB_DESTDIR");

DATA_SRCDIR = #$ Expand("DATA_SRCDIR");
DATA_DESTDIR = #$ Expand("DATA_DESTDIR");

#!!!!! BEGIN 3rd installation routines !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#${
    %install = ();
    foreach $type ('BIN', 'LIB') {
        next if expand_path(Project("PACKAGE_${type}_DESTDIR")) eq expand_path(Project("${type}_DESTDIR"));
        my %group = (
            srcdir => Project("PACKAGE_${type}_DESTDIR"),
            dstdir => Project("${type}_DESTDIR"),
            files => [],
            dirs => []
        );
        @targets = split(/\s+/, Project("${type}_TARGETS"));
        @targets_inst = split(/\s+/, Project("${type}_TARGETS_INST"));
        for ($i=0; $i < @targets; $i++) {
#!            print STDERR @targets[$i], " -> ", @targets_inst[$i], "\n";
            push @{$group{'files'}}, [ @targets[$i], @targets_inst[$i]];
        }
        $group{'exec'} = Project("${type}_TARGETS_EXEC");
        $install{$type} = \%group;
    }

    IncludeTemplate("install-data");
    IncludeTemplate("install-common");
#$}
#!!!!! END 3rd installation routines !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
