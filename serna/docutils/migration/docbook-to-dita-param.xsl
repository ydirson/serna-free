<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.1'>

<xsl:param name="topic-elements">
  <xsl:value-of
    select="'|book|part|chapter|refsect1|refsect2|reference|refentry|section|'"/>
</xsl:param>

<xsl:param name="topic-non-body-elements">
  <xsl:value-of select="'|bookinfo|title|'"/>
</xsl:param>

<xsl:param name="topic-section-elements">
  <xsl:value-of select="'|abstract|simplesect|'"/>
</xsl:param>

<xsl:param name="element-map">
classname|function|methodname apiname
command userinput
figure fig
firstterm term
glossterm keyword
keycap|keysym uicontrol
menuchoice menucascade
informaltable table
keycombo|literal|sgmltag codeph
listitem li
orderedlist ol
para p
programlisting codeblock
property b
quote q
refname searchtitle
refpurpose shortdesc
refsection body/section
simplesect section
graphic image,placement=break,href=@fileref,align=@
inlinegraphic image,placement=inline,href=@fileref,align=@
important note,type=important
tip note,type=tip
tgroup tgroup,cols=@
ulink xref,href=@url
warning note,type=other,othertype=Warning
</xsl:param>

</xsl:stylesheet>
