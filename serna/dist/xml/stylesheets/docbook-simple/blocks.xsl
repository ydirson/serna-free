<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="ackno">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="padding" select="'.5em 0em'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="formalpara">
    <xsl:call-template name="div">
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="address">
    <xsl:call-template name="pre.decoration">
      <xsl:with-param name="padding" select="'.5em 0em'"/>
    </xsl:call-template>
  </xsl:template>

</xsl:stylesheet>
