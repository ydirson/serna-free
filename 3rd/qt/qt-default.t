#
# Qt template
#
#${
    IncludeTemplate("3rd/pkg-utils");
    my $third_dir = Project("THIRD_DIR");

    my @modules = qw/QtCore QtGui QtNetwork QtXml QtHelp QtAssistantClient
                     QtSql QtSvg Qt3Support/;
    my @tools = qw/qmake lupdate lrelease uic moc rcc assistant linguist
                   qhelpgenerator qcollectiongenerator/;

    my %package = ( NAME => "Qt" );
    foreach (@tools) {
        $package{uc($_)} = normpath("$third_dir/bin/$_");
    }
    write_package("$third_dir/lib/Qt.pkg", \%package);

    if (Config("syspkg") || Config("syspkgonly")) {
        foreach (@modules) {
            my $pkg = find_package($_);
            if ($_ eq 'QtAssistantClient') {
                $pkg->{"INCLUDES"} =~ s/QtAssistantClient/QtAssistant/g;
            }
            tmake_error("Can't find $_ package") if Config("syspkgonly") && !$pkg;
            write_package("$third_dir/lib/$_.pkg", $pkg);
        }
        my @pathlist = split($is_unix ? ':' : ';', $ENV{'PATH'});
        foreach (@tools) {
            my ($tool) = find_file_in_path($_, @pathlist);
            unless ($tool) {
                ($tool) = find_file_in_path("$_-qt4", @pathlist);
            }
            tmake_error("Can't find $_") if Config("syspkgonly") && !$tool;
            write_script("$third_dir/bin/$_", "exec $tool ".'"$@"');
            $package{uc($_)} = normpath("$third_dir/bin/$_");
        }
        write_file("$third_dir/qt/MANIFEST", '');
        Project("TMAKE_TEMPLATE=");
        return;
    }

    my $dbgsfx = 'd' if Config("debug");
    foreach (@modules) {
        my %pkg = ( NAME => $_ );
        $pkg{"INCLUDES"} = "\$(THIRD_DIR)/qt/include/$_ \$(THIRD_DIR)/qt/include";
        if ($_ eq 'QtAssistantClient') {
            $pkg{"INCLUDES"} =~ s/QtAssistantClient/QtAssistant/g;
        }
        $pkg{"CFLAGS"} = '-DQT_SHARED';
        if ($is_unix) {
            $pkg{"LIBS"} = $_;
            $pkg{"LFLAGS"}   = '-L$(THIRD_DIR)/lib';
        }
        else {
            $pkg{"LIBS"}     = "\$(THIRD_DIR)\\lib\\$_${dbgsfx}4.lib";
        }
        write_package("$third_dir/lib/$_.pkg", \%pkg);
    }

    Project("FILETOOLS = PERL");
    IncludeTemplate("filetools");
    Project('QCONFIG_CPP = src/corelib/global/qconfig.cpp');
    Project('QCONFIG_PL  = $(top_srcdir)/qconfig.pl');
    my $postcfg = '$(PERL) -- $(QCONFIG_PL) $(QCONFIG_CPP)';
    while ($postcfg =~ /\$\(\w+\)/) {
        $postcfg =~ s/\$\((\w+)\)/$project{$1}/g;
    }
    Project("PACKAGE_POSTCONFIG = $postcfg");
#$}
QMAKESPEC	= #$ Expand("QMAKESPEC");
