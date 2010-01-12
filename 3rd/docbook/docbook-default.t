#!
#! Template for qscintilla as a Syntext 3rd component
#!
#!
#!##############################################################################
#${
    IncludeTemplate("3rd/pkg-utils");
    my $third_dir = Project("THIRD_DIR");

    my %package = ( NAME => 'docbook' );
    if (Config("syspkg") || Config("syspkgonly")) {
    	my $docbook_dir = "/usr/share/xml/docbook/stylesheet/docbook-xsl";
    	unless (-e $docbook_dir) {
		tmake_error("Could not find docbook stylesheet directory \"$docbook_dir\"\n");
	}
	write_script("$third_dir/bin/docbook2html", <<__EOF__);
DOCBOOK_DIR="$docbook_dir"; export DOCBOOK_DIR
XSLTPROC="$third_dir/bin/xsltproc"; export XSLTPROC
exec $third_dir/docbook/docbook2html.sh "\$@"
__EOF__
	write_file("$third_dir/docbook/MANIFEST", "");
        Project("TMAKE_TEMPLATE=");
        return;
    }

    write_file("$third_dir/docbook/MANIFEST", "include:MANIFEST.docbook");
#$}
