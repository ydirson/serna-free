<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:doc="http://nwalsh.com/xsl/documentation/1.0"
                exclude-result-prefixes="doc"
                version='1.0'>

<xsl:template name="head.content">
  <xsl:param name="title">
    <xsl:call-template name="document.title"/>
  </xsl:param>

  <title>
    <xsl:copy-of select="$title"/>
  </title>

  <xsl:if test="$html.stylesheet != ''">
    <xsl:call-template name="output.html.stylesheets">
      <xsl:with-param name="stylesheets" select="normalize-space($html.stylesheet)"/>
    </xsl:call-template>
  </xsl:if>

  <xsl:if test="$html.base != ''">
    <base href="{$html.base}"/>
  </xsl:if>

  <meta name="generator" content="Syntext XSL Bricks"/>
</xsl:template>

<xsl:template name="output.html.stylesheets">
  <xsl:param name="stylesheets" select="''"/>
  <xsl:if test="$stylesheets != ''">
    <link rel="stylesheet" href="{$stylesheets}">
      <xsl:if test="$html.stylesheet.type != ''">
        <xsl:attribute name="type">
          <xsl:value-of select="$html.stylesheet.type"/>
        </xsl:attribute>
      </xsl:if>
    </link>
  </xsl:if>
</xsl:template>

<xsl:template match="/*">
  <xsl:variable name="doc" select="self::*"/>
  <html>
    <head>
      <xsl:call-template name="head.content"/>
      <xsl:call-template name="user.head.content"/>
    </head>
    <body>
      <xsl:call-template name="body.attributes"/>
      <xsl:call-template name="user.header.content"/>
      <xsl:apply-templates/>
      <xsl:call-template name="user.footer.content"/>
    </body>
  </html>
</xsl:template>

<xsl:template match="text()">
  <xsl:value-of select="."/>
</xsl:template>

</xsl:stylesheet>
