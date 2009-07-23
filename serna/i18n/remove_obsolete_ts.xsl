<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<xsl:output method="xml" encoding="utf-8"/>

<xsl:template match="/">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="context">
  <xsl:variable name="obsolete" 
                select="child::message/translation[@type = 'obsolete']"/>
  <xsl:choose>
    <xsl:when test="count($obsolete) = count(child::message)">
      <xsl:message>
        <xsl:text>Obsolete context:</xsl:text>
        <xsl:value-of select="name"/>
      </xsl:message>    
    </xsl:when>
    <xsl:otherwise>
      <xsl:copy>
        <xsl:apply-templates select="@*|node()"/>
      </xsl:copy>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="message">
  <xsl:variable name="is-obsolete" 
                select="child::translation[@type = 'obsolete']"/>
  <xsl:choose>
    <xsl:when test="$is-obsolete">
      <xsl:message>
        <xsl:text>Obsolete message:</xsl:text>
        <xsl:value-of select="source"/>
      </xsl:message>    
    </xsl:when>
    <xsl:otherwise>
      <xsl:copy-of select="."/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="@*|node()">
  <xsl:copy>
    <xsl:apply-templates select="@*|node()"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="context/text()">
  <xsl:variable name="obsolete" 
    select="preceding-sibling::message[1]/translation[@type = 'obsolete']"/>
  <xsl:choose>
    <xsl:when test="$obsolete">
    </xsl:when>
    <xsl:otherwise>
      <xsl:copy/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>

