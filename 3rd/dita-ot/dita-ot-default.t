#${
    my ($third_dir, $ver, $pkg_dir) = map { Project($_) } qw/THIRD_DIR VERSION PACKAGE_DIR/;
    my $dita_dir = "$third_dir/dita-ot/$ver/$pkg_dir";
    $dita_dir =~ s/[\\\/]/\//g;
    $dita_dir = "/$dita_dir" unless $is_unix;
    write_file("../dita-dir.ent", "<!ENTITY dita-dir \"$dita_dir\">\n<!ENTITY dita-ot-path \"file://$dita_dir\">");
    my $ddcat = <<"EOF";
<?xml version="1.0"?>
<!DOCTYPE catalog PUBLIC "-//OASIS//DTD XML Catalogs V1.0//EN"
         "http://www.oasis-open.org/committees/entity/release/1.0/catalog.dtd" [
<!ENTITY dita-dir "$dita_dir">
]>
<catalog xmlns="urn:oasis:names:tc:entity:xmlns:xml:catalog"
          prefer="public">
    <public publicId="dita-dir-pub" uri="dita-dir.ent"/>
    <nextCatalog catalog="file://&dita-dir;/catalog-dita.xml"/>
</catalog>
EOF
    write_file("../dita-dir-catalog.xml", $ddcat);
#$}

#$ IncludeTemplate("java");
