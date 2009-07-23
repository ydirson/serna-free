<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<xsl:output method="text" encoding="utf-8"/>

<xsl:template match="/">
/// THIS FILE IS GENERATED AUTOMATICALLY. DO NOT EDIT.
void sui::translate() {    
  <xsl:apply-templates/>
};
/// END OF GENERATED FILE
</xsl:template>

<xsl:template match="inscription|tooltip|accel|caption">
  <xsl:call-template name="translate">
    <xsl:with-param name="text">
      <xsl:value-of select="text()"/>
    </xsl:with-param>
    <xsl:with-param name="context">
      <xsl:call-template name="context"/>
    </xsl:with-param>
  </xsl:call-template>
</xsl:template>

<xsl:template match="sync-property">
    <xsl:variable name="subname" select="to"/>
    <xsl:for-each select="value-map/*">
        <xsl:call-template name="translate">
            <xsl:with-param name="text">
                <xsl:value-of select="text()"/>
            </xsl:with-param>
            <xsl:with-param name="context">
                <xsl:call-template name="context">
                    <xsl:with-param name="name" select="$subname"/>
                </xsl:call-template>
            </xsl:with-param>
        </xsl:call-template>
    </xsl:for-each>
</xsl:template>

<xsl:template name="context">
  <xsl:param name="name" select="''"/>
  <xsl:choose>
    <xsl:when test="ancestor-or-self::uiAction">
      <xsl:text>uiAction&lt;</xsl:text>
      <xsl:value-of select="string(ancestor-or-self::uiAction/name)"/>        
      <xsl:text>&gt;</xsl:text>
    </xsl:when>
    <xsl:when test="ancestor-or-self::properties">
      <xsl:value-of select="local-name(ancestor-or-self::*[properties][1])"/>  
      <xsl:text>&lt;</xsl:text>
      <xsl:value-of select="string(ancestor-or-self::properties/name)"/>  
      <xsl:text>&gt;</xsl:text>
    </xsl:when>
  </xsl:choose>  
  <xsl:text> | </xsl:text>
  <xsl:choose>
    <xsl:when test="string-length($name) > 0">
        <xsl:value-of select="$name"/>
    </xsl:when>
    <xsl:otherwise>
        <xsl:value-of select="local-name()"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>
    
<xsl:template name="translate">
  <xsl:param name="text" select="''"/>  
  <xsl:param name="context" select="''"/> 
  <xsl:if test="string-length($text) > 0">
    <xsl:text> tr(&quot;</xsl:text>
    <xsl:value-of select="normalize-space($text)"/>
    <xsl:text>&quot;</xsl:text>
    <xsl:if test="string-length($context) > 0">
      <xsl:text>, &quot;</xsl:text>
      <xsl:value-of select="$context"/>
      <xsl:text>&quot;</xsl:text>
    </xsl:if>
    <xsl:text>); 
</xsl:text>
    
  </xsl:if>
</xsl:template>

<xsl:template match="*">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>

