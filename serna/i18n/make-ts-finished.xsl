<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:template match="TS">
    <xsl:copy>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="context|name|message|source|translation">
    <xsl:copy>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="@*"/>

  <!--xsl:template match="message">
    <xsl:if test="translation/@type!='obsolete'">
      <xsl:copy>
        <xsl:apply-templates select="@*"/>
        <xsl:variable name="sourceNode" select="source"/>
        <source>
          <xsl:value-of select="$sourceNode"/>
        </source>
        <xsl:copy-of select="comment"/>
        <xsl:choose>
          <xsl:when test="translation/@type='unfinished'">
            <translation>
              <xsl:value-of select="$sourceNode"/>
            </translation>
          </xsl:when>
          <xsl:otherwise>
            <xsl:copy-of select="translation"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:copy>
    </xsl:if>
  </xsl:template-->

</xsl:stylesheet>
