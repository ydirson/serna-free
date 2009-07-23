<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">
  
  <xsl:template name="process-common-attributes-and-children">
    <xsl:call-template name="process-common-attributes"/>
    <xsl:apply-templates/>
  </xsl:template>
  
  <xsl:template name="process-common-attributes">
    <xsl:choose>
      <xsl:when test="@xml:lang">
        <xsl:attribute name="xml:lang">
          <xsl:value-of select="@xml:lang"/>
        </xsl:attribute>
      </xsl:when>
      <xsl:when test="@lang">
        <xsl:attribute name="xml:lang">
          <xsl:value-of select="@lang"/>
        </xsl:attribute>
      </xsl:when>
    </xsl:choose>
  </xsl:template>
    
</xsl:stylesheet>
