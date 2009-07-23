<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xi="http://www.w3.org/2001/XInclude"
                xmlns:exsl="http://exslt.org/common"
                version='1.1'
                extension-element-prefixes="exsl">

<xsl:output method="text" encoding="UTF-8"/>
<xsl:strip-space elements="*"/>

<xsl:include href="../xsl/dbhtml-utils.xsl"/>
<xsl:include href="docbook-to-dita-param.xsl"/>

<xsl:param name="base-dir" select="."/>

<xsl:template match="text()|@*"/>

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
            <xsl:with-param name="pathname" select="$output-file"/>
          </xsl:call-template>
        </xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <xsl:variable name="db-count"
      select="count(document(@href)//*[contains($topic-elements,
                                                concat('|',
                                                concat(local-name(.),
                                                '|')))])"/>
    <xsl:variable name="topic-count"
      select="count(document($output-file)//*['topic' = local-name(.)])"/>

    <xsl:variable name="report">
      <xsl:value-of select="@href"/>: <xsl:value-of select="$db-count"/>
      <xsl:text>, </xsl:text>
      <xsl:value-of
        select="$output-file"/>: <xsl:value-of select="$topic-count"/>
    </xsl:variable>

    <xsl:if test="number($db-count) != number($topic-count)">
      <xsl:text> </xsl:text>
    </xsl:if>

    <xsl:message>
      <xsl:if test="number($db-count) != number($topic-count)">
        <xsl:text>--- MISMATCH: </xsl:text>
      </xsl:if>
      <xsl:value-of select="$report"/>
    </xsl:message>

  </xsl:if>
</xsl:template>

<xsl:template match="/">
  <xsl:variable name="output">
    <xsl:apply-templates/>
  </xsl:variable>
  <xsl:if test="string($output)">
    <xsl:message terminate="yes">
      <!--xsl:value-of select="$output"/-->
      <xsl:text>&#xa;Topic element count check failed</xsl:text>
    </xsl:message>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
