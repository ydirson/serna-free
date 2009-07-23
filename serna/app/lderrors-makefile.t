#!
#!
#${
    my $lderr_rule = <<'EOF';
$(@PLUGIN_ID@_OBJECTS):
	cd $(serna_dir)/@PLUGIN@; $(MAKE)

$(serna_dir)/@PLUGIN@/lderrors.txt: $(@PLUGIN_ID@_OBJECTS) $(GET_UNDEF_SYMS)
	serna_dir=$(serna_dir) plugin=@PLUGIN@ plugin_dir=$(serna_dir)/@PLUGIN@ \
	$(GET_UNDEF_SYMS) $@

EOF
    my $makefile;
    for $p (split /\s+/, Project("PLUGINS")) {
        my $prule = $lderr_rule;
        $prule =~ s/\@PLUGIN\@/$p/gm;
        $p =~ s^/^_^g;
        $prule =~ s/\@PLUGIN_ID\@/$p/gm;
        $makefile .= $prule;
    }
    write_file("Makefile.lderrors", $makefile);
#$}
