#${
    sub xsltproc {
        my ($xsl, $xml, $xsltproc) = @_;
        unless (-f $xsl) {
            write_file("tmp.xsl", $xsl);
            $xsl = "tmp.xsl";
        }
        my $cmd = "$xsltproc $xsl $xml|";
        open(F, $cmd) || die("Can't run '$cmd'");
        my @rv;
        while (<F>) {
            chomp;
            push @rv, $_;
        }
        close(F);
        unlink("tmp.xsl") if -f "tmp.xsl";
        return @rv;
    }
    sub include_makefile {
        my ($makefile) = @_;
        return "-include $makefile" if $is_unix;
        return "!if exist ($makefile)\n! include $makefile\n!endif";
    }
    my $xp = expand_path(Project("XSLTPROC"));
    my $shs = expand_path(Project("SERNA_HELP_DITAMAP"));

    my $db_rule_tpl = <<"EOF";
\$(DITA_HTML_DIR)/!DIR!/!BASENAME!.html: \$(DITA_SRC_DIR)/!DIR!/!BASENAME!.xml \$(DOCBOOK_DIRS)
!XCF!
!DBXSL!
	\$(DOCBOOK2HTML) \$\@ \$(DITA_SRC_DIR)/!DIR!/!BASENAME!.xml
EOF
    my $dbp_rule_tpl = <<"EOF";
\$(DITA_HTML_DIR)!PIC!: \$(DITA_SRC_DIR)!PIC!
	\$(COPY) \$(DITA_SRC_DIR)!PIC! \$\@
EOF

    $db_rule_tpl =~ s/!XCF!/&set_env_vars("XML_CATALOG_FILES")/e;
    $db_rule_tpl =~ s/!DBXSL!/&set_env_vars_raw("DOCBOOK2HTML_XSL=docbook.xsl")/e;
    my (@db_rules, @db_htmls);
    my (@dita_dirs, @db_dirs, %dita_dirs_dict, %db_dirs_dict);
    my $dita_srcdir = expand_path(Project("DITA_SRC_DIR"));
    my @db_sources = ();
    my @db_pics = ();

    my $srcdir = Project("srcdir");
    my @trefs = xsltproc("$srcdir/topicrefs.xsl", $shs, $xp);

    foreach (@trefs) {
        next unless /^href=/;
        s/\s+$//;
        my %src = ();
        foreach (split("\t", $_, 3)) {
            my ($k, $v) = split('=', $_, 2);
            $src{$k} = $v;
        }
        my $format = $src{'format'} ? $src{'format'} : 'dita';
        my $href = $src{'href'};
        my ($dir, $basename, $ext);
        if ($href =~ /(.*)\/([^\/]+)\.([^\/\.]+)$/) {
            ($dir, $basename, $ext) = ($1, $2, $3);
        }
        $dir =~ s/^\s+//;
        $dir =~ s/\s+$//;
        next unless $dir;
        if ($format =~ /^dita/) {
            $dita_dirs_dict{$dir} = 1;
        }
        else {
            $db_dirs_dict{$dir} = 1;
            unless ($dir =~ /^sapi/) {
                push @db_sources, "$dir/$basename.xml";
                push @db_htmls, "\$(DITA_HTML_DIR)/$dir/$basename.html";
                my $db_rule = $db_rule_tpl;
                $db_rule =~ s/!DIR!/$dir/g;
                $db_rule =~ s/!BASENAME!/$basename/g;
                push @db_rules, $db_rule;
            }
        }
    }
    @dita_dirs = keys(%dita_dirs_dict);
    @db_dirs = keys(%db_dirs_dict);
    Project("DOCBOOK_SOURCES = ".join(' ', @db_sources));
    IncludeTemplate("db-pics");

    Project("DOCBOOK_HTML_RULES = ".join("\n", @db_rules));
    Project("DOCBOOK_HTMLS = ".join(' ', @db_htmls));

    my @srcdirs = map { "$dita_srcdir$dir_sep$_" } (@dita_dirs, @db_dirs);
    my @dita_sources;

    foreach $dsrc ($dita_srcdir, @srcdirs) {
        foreach $dpat (qw/.\.dita(map)?$ .\.xml$ .\.ent$/) {
            push @dita_sources,
                map { s/[\/\\]/$dir_sep/g; $_; } find_files($dsrc, $dpat, 0);
        }
    }

    @dita_sources = map { s/\Q$dita_srcdir\E/\$\(DITA_SRC_DIR\)/; $_ } @dita_sources;
    my $sources = "DITA_SOURCES = \t\\\n\t\t".join(" \\\n\t\t", @dita_sources);
    $sources .= "\n\nDOCBOOK_SOURCES = \t\\\n\t\t".join(" \\\n\t\t", @db_sources);  
    write_file("Makefile.sources", $sources);
    $text = include_makefile("Makefile.sources");
    Project("CLEAN_FILES += Makefile.sources");

    Project("DITA_SOURCES = ".join(' ', @dita_sources));

    my $pfx = ' $(DITA_HTML_DIR)'.$dir_sep;
    Project("DITA_DOCDIRS = $pfx". join($pfx, @dita_dirs));
    Project("DOCBOOK_DIRS = $pfx". join($pfx, @db_dirs));
#$}
