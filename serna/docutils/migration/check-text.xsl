<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xi="http://www.w3.org/2001/XInclude"
                xmlns:exsl="http://exslt.org/common"
                version='1.1'
                extension-element-prefixes="exsl">

<xsl:output method="text"
            encoding="UTF-8"/>
<xsl:strip-space elements="*"/>

<xsl:include href="../xsl/dbhtml-utils.xsl"/>
<xsl:param name="base-dir" select="."/>

<xsl:template match="book/text()|set/text()"/>
<xsl:template match="book/title|set/title"/>

<xsl:template match="text()">
  <xsl:value-of select="concat(normalize-space(.), '&#10;')"/>
</xsl:template>

<xsl:template name="output-file">
  <xsl:param name="href" select="''"/>
  <xsl:value-of select="concat(translate($base-dir, '\\', '/'), '/')"/>
  <xsl:call-template name="basename">
    <xsl:with-param name="filename" select="$href"/>
  </xsl:call-template>
  <xsl:text>.dita</xsl:text>
</xsl:template>

<xsl:template match="xinclude">
  <xsl:if test="not(@role='docbook')">
    <xsl:variable name="output-file">
      <xsl:call-template name="output-file">
        <xsl:with-param name="href" select="@href"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="output-base">
      <xsl:call-template name="basename">
        <xsl:with-param name="filename">
          <xsl:call-template name="filename">
            <xsl:with-param name="pathname" select="@href"/>
          </xsl:call-template>
        </xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <exsl:document href="{concat($output-base, '.docbook.text')}"
      method="text" encoding="utf-8">
      <xsl:message>Processing <xsl:value-of select="@href"/></xsl:message>
      <xsl:apply-templates select="document(@href)"/>
      <xsl:text>&#xa;</xsl:text>
    </exsl:document>
    <exsl:document href="{concat($output-base, '.dita.text')}"
      method="text" encoding="utf-8">
      <xsl:message>Processing <xsl:value-of select="$output-file"/>
      </xsl:message>
      <xsl:apply-templates select="document($output-file)"/>
      <xsl:text>&#xa;</xsl:text>
    </exsl:document>
    <xsl:message>
      <xsl:text>  Wrote to </xsl:text>
      <xsl:value-of select="concat($output-base, '.docbook.text, ')"/>
      <xsl:value-of select="concat($output-base, '.dita.text')"/>
    </xsl:message>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
