#${
    my $xp = expand_path(Project("XSLTPROC"));
    my $shs = expand_path(Project("SERNA_HELP_SRC"));
    my $xsl = <<'EOF';
<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.1'>
  <xsl:output method="text" encoding="ISO-8859-1"/>
  <xsl:template match="text()|@*"/>
  <xsl:template match="xinclude[not(@role='docbook')]">
    <xsl:value-of select="substring-before(@href, '/')"/><xsl:text> </xsl:text>
  </xsl:template>
</xsl:stylesheet>
EOF
    write_file("tmp.xsl", $xsl);
    open(F, "$xp tmp.xsl $shs|") and Project("DITA_DOCDIRS = ".<F>) and close(F);

    $xsl =~ s/not\(([^\)]+)\)/\1/m;
    write_file("tmp.xsl", $xsl);
    open(F, "$xp tmp.xsl $shs|") and Project("DOCBOOK_DIRS = ".<F>) and close(F);
    unlink("tmp.xsl");
#$}
DITA_DOCDIRS = #$ Expand("DITA_DOCDIRS");
DOCBOOK_DIRS = #$ Expand("DOCBOOK_DIRS");

