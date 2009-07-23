#${
    my $catalog = <<'EOF';
<?xml version="1.0" ?>
<!DOCTYPE catalog PUBLIC "-//OASIS//DTD XML Catalogs V1.0//EN"
         "http://www.oasis-open.org/committees/entity/release/1.0/catalog.dtd" [
    <!ENTITY cat_dir "@CATDIR@">
    <!ENTITY third_dir "@THIRD_DIR@">
]>
<catalog xmlns="urn:oasis:names:tc:entity:xmlns:xml:catalog"
          prefer="public">
    <system systemId="build.ent" 
            uri="file://&cat_dir;/build.ent"/>
    <system systemId="speller_data.ent" 
            uri="file://&cat_dir;/speller_data.ent"/>
    <system systemId="dita-ot-settings.ent" 
            uri="file://&third_dir;/dita-ot/dita-dir.ent"/>
</catalog>
EOF
    my $buildent = '<!ENTITY build-dir "@TOP_BUILDDIR@">';
    my $spell_data = <<'EOF';
  <dict-dir>@TOP_BUILDDIR@/3rd/aspell/lib/aspell</dict-dir>
  <data-dir>@TOP_BUILDDIR@/3rd/aspell/share/aspell</data-dir>
  <lib>@THIRD_DIR@/lib/@ASPELL_LIB@</lib>
EOF
    my $d = Config("debug") ? 'd' : '';
    my $aspell_lib = "aspell-15$d.dll";
    if ($is_unix) {
        $aspell_lib = "libaspell.";
        $aspell_lib .= (platform() eq "darwin") ? "dylib" : "so";
    }
    my $catdir = expand_path('$(top_builddir)/serna/bin');
    my $top_builddir = expand_path('$(top_builddir)');
    my $third_dir = expand_path('$(THIRD_DIR)');
    $third_dir =~ s|[\\/]+|/|g;
    $catdir =~ s|[\\/]+|/|g;
    my $repl_catdir = $is_unix ? $catdir : '/'.$catdir;
    my $repl_3dir = $is_unix ? $third_dir : '/'.$third_dir;
    $top_builddir =~ s|[\\/]+|/|g;
    $top_builddir =~ s|/[^/]+$||;
    $catalog =~ s/\@CATDIR\@/$repl_catdir/g;
    $catalog =~ s/\@THIRD_DIR\@/$repl_3dir/g;
    $buildent =~ s/\@TOP_BUILDDIR\@/$top_builddir/;
    write_file("$catdir/buildent-catalog.xml", $catalog);
    write_file("$catdir/build.ent", $buildent);
    $spell_data =~ s/\@TOP_BUILDDIR\@/$top_builddir/g;
    $spell_data =~ s/\@THIRD_DIR\@/$third_dir/g;
    $spell_data =~ s/\@ASPELL_LIB\@/$aspell_lib/g;
    write_file("$catdir/speller_data.ent", $spell_data);
#$}
