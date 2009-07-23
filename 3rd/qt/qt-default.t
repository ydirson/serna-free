#
# Qt template
#
#${
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

