<?xml version='1.0'?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:se="http://www.syntext.com/XSL/Format-1.0"
                xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
                extension-element-prefixes="dtm"                
                version='1.0'>


<!-- Changes:
 01 Dec 2003 DRD Added support for up to 4-deep anonymous nested prolog structures
 +-->


<!-- =================== start of prolog and metadata ====================== -->

<!-- prolog data is placed all in brown background indicating content that
     will not show when output to print. -->
<dtm:doc dtm:elements="prolog|bookmeta" dtm:status="finished" dtm:idref="prolog.data.background prolog.metadata.display"/>
<xsl:template match="*[contains(@class,' bookmap/bookmeta ')] |
                     *[contains(@class,' topic/prolog ')]" 
              dtm:id="prolog.data.background">
    <xsl:if test="$SHOWPROLOG='yes'"> 
      <fo:block background-color="#f0f0d0" padding="6pt" 
                border-style="bold" border-color="black" border-width="thin"
                start-indent="{$basic-start-indent}">
        <xsl:apply-templates/>
      </fo:block>
  </xsl:if>
</xsl:template>

<!-- metadata in prolog is displayed using element names as field labels, and
     indented boxes for nested values.  Good for 2 deep, at least. -->

<dtm:doc dtm:elements="prolog/author|prolog/copyright|copyright/copyryear|
                       copyright/copyrholder|prolog/critdates|critdates/created|
                       critdates/revised|topic/permissions|prolog/metadata|
                       metadata/audience|prolog/category|metadata/keywords|
                       metadata/prodinfo|prodinfo/prodname|prodinfo/vrmlist|
                       vrmlist/vrm|metadata/othermeta|prodinfo/brand|prodinfo/component|
                       prodinfo/featnum|prodinfo/platform|prodinfo/prognum|prodinfo/series|
                       prolog/source|prolog/resourceid|prolog/publisher|bookmeta/audience|
                       bookmeta/authorinformation|bookmeta/category|bookmeta/critdates|
                       bookmeta/publisherinformation" dtm:status="finished" 
                       dtm:idref="prolog.metadata.display"/>
<xsl:template match="*[contains(@class,' bookmap/bookmeta ')]//* |
                     *[contains(@class,' map/topicmeta ')]//* |
                     *[contains(@class,' topic/prolog ')]//*" 
              dtm:id="prolog.metadata.display">
  <xsl:variable name="depth" select="concat(count(ancestor::*) - 1, 'em')"/>
  <fo:block start-indent="{concat($basic-start-indent, '+', $depth)}">
  <xsl:variable name="attrs" select="@*[not(self::class)]"/>
    <fo:inline font-weight="bold">
        <xsl:value-of select="name()"/>
        <xsl:if test="$attrs">
            <fo:inline font-style="italic">
                <xsl:text> {</xsl:text>
                <xsl:for-each select="$attrs">
                    <xsl:value-of select="name()"/>
                    <xsl:text>='</xsl:text>
                    <fo:inline color="green">
                        <xsl:value-of select="."/>
                    </fo:inline>
                    <xsl:text>'</xsl:text>
                    <xsl:if test="position() != last()">, </xsl:if>
                </xsl:for-each>
                <xsl:text>}</xsl:text>
            </fo:inline>
        </xsl:if>
        <xsl:text>: </xsl:text>
    </fo:inline>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

</xsl:stylesheet>
