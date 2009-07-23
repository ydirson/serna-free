<?xml version="1.0" encoding="utf-8"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                version="1.1">

    <xsl:param name="_lang" select="/dmodule/idstatus/dmaddres/language/@language"/>

    <xsl:template name="lang">
      <xsl:choose>
        <xsl:when test="string-length($_lang) = 0">
          <xsl:text>en</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$_lang"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:template>

    <xsl:template name="gentext">
      <xsl:param name="key" select="local-name(.)"/>
      <xsl:param name="lang">
        <xsl:call-template name="lang"/>
      </xsl:param> 
      <xsl:variable name="lang.file" select="concat('i18n_', $lang, '.xml')"/>
      <xsl:value-of name="lang.eq" 
                    select="document($lang.file)/i18n/token[@id = $key]"/>
    </xsl:template>

</xsl:stylesheet>
