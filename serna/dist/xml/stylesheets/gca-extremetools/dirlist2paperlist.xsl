<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" indent="yes" encoding="iso-8859-1"/>

<xsl:variable name="uc" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>

<xsl:variable name="lc" select="'abcdefghijklmnopqrstuvwxyz,;:?/.()!@#$%^*-_=+\|'"/>

<xsl:template match="/">
  <paperlist>
    <xsl:apply-templates/>
  </paperlist>
</xsl:template>

<xsl:template match="file">
  <xsl:variable name="info"
    select="document(concat('../xml/',.,'.xml'))/paper/front |
            document(concat('../xml/',.,'.xml'))/paperstub"/>
  <!-- $info is the front matter to the paper, or is a complete paperstub:
       it will be one or the other since these are mutually exclusive. -->
  <paper authorpackage="no">
    <xsl:attribute name="full">
      <xsl:choose>
        <xsl:when test="$info[self::paperstub]">no</xsl:when>
        <xsl:otherwise>yes</xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
    <xsl:if test="$info[@latebreaking='1']">
      <xsl:attribute name="latebreaking">yes</xsl:attribute>
    </xsl:if>
    <file>
      <xsl:value-of select="."/>
    </file>
    <xsl:for-each select="$info/author">
      <author key="{concat(fname, ' ', surname)}">
        <xsl:copy-of select="surname | fname"/>
        <xsl:copy-of select="address/affil | address/subaffil"/>
      </author>
    </xsl:for-each>
    <xsl:copy-of select="$info/title | $info/subt"/>
    <sortingtitle>
      <xsl:variable name="fulltitle">
        <xsl:value-of select="normalize-space($info/title)"/>
        <xsl:if test="$info/subt">
          <xsl:text>: </xsl:text>
          <xsl:value-of select="normalize-space($info/subt)"/>
        </xsl:if>
      </xsl:variable>
      <xsl:choose>
        <xsl:when test="starts-with($fulltitle,'A ')">
          <xsl:value-of select="translate(substring($fulltitle,3), $lc, $uc)" /> 
        </xsl:when>
        <xsl:when test="starts-with($fulltitle,'An ')">
          <xsl:value-of select="translate(substring($fulltitle,4), $lc, $uc)" /> 
        </xsl:when>
        <xsl:when test="starts-with($fulltitle,'The ')">
          <xsl:value-of select="translate(substring($fulltitle,5), $lc, $uc)" /> 
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="translate($fulltitle, $lc, $uc)" /> 
        </xsl:otherwise>
      </xsl:choose>
    </sortingtitle>
    <xsl:copy-of select="$info/keywords/keyword"/>
    <xsl:copy-of select="$info/conference"/>
    <xsl:if test="not($info/conference)">
      <conference>Extreme Markup Languages</conference>
    </xsl:if>
  </paper>
</xsl:template>

</xsl:stylesheet>