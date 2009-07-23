#${
    my @db_sources = split(/\s+/, Project("DOCBOOK_SOURCES"));
    my $srcdir = Project("srcdir");
    my @db_pics;
    my $null = $is_unix ? '' : '>nul';
    my $mkdir = $is_unix ? '@test -d $(@D) || mkdir -p $(@D)':
                           '@if not exist $(@D) mkdir $(@D)';
    my $dbp_rule_tpl = <<"EOF";
\$(DITA_HTML_DIR)!PIC!: \$(DITA_SRC_DIR)!PIC!
	$mkdir
	\$(COPY) \$(DITA_SRC_DIR)!PIC! \$\@ $null
EOF
    my $db_srcdir = expand_path("DITA_SRC_DIR");
    my $pics_includes, %pichash;
    foreach (@db_sources) {
        my $db_basedir = /(.*)[\/\\]([^\\\/]+)$/ ? $1 : "";
        my $db_fullpath = expand_path(Project("DITA_SRC_DIR"))."$dir_sep$_";
        my @pics = xsltproc("$srcdir/db-pics.xsl", $db_fullpath, 
                            expand_path(Project("XSLTPROC")));

        next unless @pics;
        my %picdup;
        my @rules = ();
        foreach (@pics) {
            next if defined $picdup{$_};
            $picdup{$_} = 1;
            s/[\\\/]/$dir_sep/g;
            my $pic = $dir_sep.join($dir_sep, $db_basedir, $_);
            my $dbp_rule = $dbp_rule_tpl;
            $dbp_rule =~ s/!PIC!/$pic/g;
            push @rules, $dbp_rule;
            push @db_pics, '$(DITA_HTML_DIR)'.$pic;

            $pic =~ s/^[\\\/]+//;
            $pic =~ s/[\\\/]([^\\\/]+)$//;
            $pichash{$pic} = [] unless defined($pichash{$pic});
            push @{$pichash{$pic}}, $1;
        }
        write_file("Makefile.pics.$db_basedir", join("\n", @rules));
        $pics_includes .= include_makefile("Makefile.pics.$db_basedir")."\n";
        Project("CLEAN_FILES += Makefile.pics.$db_basedir");
    }
    my $mft_tpl = "data:\${inst_prefix}/\${serna}/doc/!DIR!:\${top_builddir}/serna/dist/doc-dita/!DIR!\n";
    my $mft;
    foreach (sort keys(%pichash)) {
        my $mft_section = $mft_tpl;
        $mft_section =~ s/!DIR!/$_/g;
        $mft_section =~ s/[\\]/\//g;
        $mft_section .= "    ".join("\n    ", @{$pichash{$_}})."\n\n";
        $mft .= $mft_section;
    }
    write_file("MANIFEST.dbpics", $mft);
    write_file("Makefile.pics", "DOCBOOK_PICS = \\\n\t\t".join(" \\\n\t\t", @db_pics).
               "\n\n$pics_includes");
    $text .= include_makefile("Makefile.pics")."\n";
    Project("CLEAN_FILES += Makefile.pics ")
#$}
