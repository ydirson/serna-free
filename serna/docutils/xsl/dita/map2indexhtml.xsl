<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xi="http://www.w3.org/2001/XInclude"
                xmlns:exsl="http://exslt.org/common"
                version='1.1'
                exclude-result-prefixes='xi'
                extension-element-prefixes="exsl">

<xsl:output method="html" encoding="UTF-8" indent="yes"/>
<xsl:strip-space elements="*"/>

<xsl:include href="../dbhtml-utils.xsl"/>

<xsl:param name="base-dir" select="."/>

<xsl:template match="text()|@*"/>

<xsl:template name="output-file">
  <xsl:param name="href" select="''"/>
  <xsl:call-template name="basename">
    <xsl:with-param name="filename" select="$href"/>
  </xsl:call-template>
  <xsl:text>.html</xsl:text>
</xsl:template>

<xsl:template name="input-file">
  <xsl:param name="href"/>
  <xsl:param name="subdir"/>
  <xsl:param name="ext"/>
  <xsl:value-of select="concat(translate($base-dir, '\\', '/'), '/')"/>
  <xsl:if test="$subdir">
    <xsl:value-of select="concat($subdir, '/')"/>
  </xsl:if>
  <xsl:value-of select="$href"/>
</xsl:template>

<xsl:template match="topicref[not(contains(@href, '#'))]">
  <xsl:param name="subdir"/>
  <LI><A>
    <xsl:choose>
      <xsl:when test="@format='html'">
        <xsl:attribute name="HREF"><xsl:value-of select="@href"/></xsl:attribute>
        <xsl:value-of select="@navtitle"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:variable name="href">
          <xsl:call-template name="output-file">
            <xsl:with-param name="href" select="@href"/>
          </xsl:call-template>
        </xsl:variable>
        <xsl:attribute name="HREF">
          <xsl:value-of select="$href"/>
        </xsl:attribute>
        <xsl:variable name="input-file">
          <xsl:call-template name="input-file">
            <xsl:with-param name="href" select="@href"/>
            <xsl:with-param name="subdir" select="$subdir"/>
          </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="input-doc" select="document($input-file)"/>
        <!--xsl:message><xsl:value-of select="$input-file"/></xsl:message-->
        <xsl:choose>
          <xsl:when test="$input-doc/*/@title">
            <xsl:value-of select="$input-doc/*/@title"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$input-doc/*/title"/>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:if test="@format='ditamap'">
          <exsl:document href="{$href}" method="html" encoding="UTF-8" indent="yes">
            <xsl:apply-templates select="document(@href)">
              <xsl:with-param name="subdir">
                <xsl:if test="$subdir">
                  <xsl:value-of select="concat($subdir, '/')"/>
                </xsl:if>
                <xsl:call-template name="dirname">
                  <xsl:with-param name="pathname" select="@href"/>
                </xsl:call-template>
              </xsl:with-param>
            </xsl:apply-templates>
          </exsl:document>
        </xsl:if>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="./topicref">
      <UL>
        <xsl:apply-templates>
          <xsl:with-param name="subdir" select="$subdir"/>
        </xsl:apply-templates>
      </UL>
    </xsl:if>
  </A></LI>
</xsl:template>

<xsl:template match="/map">
  <xsl:param name="subdir"/>
<HTML>
<HEAD><META http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
<TITLE><xsl:value-of select="@title"/></TITLE>
<BASE target="contentwin"/></HEAD>
<BODY>
<H1 class="title"><xsl:value-of select="@title"/></H1>
<UL>
  <xsl:apply-templates>
    <xsl:with-param name="subdir" select="$subdir"/>
  </xsl:apply-templates>
</UL>
</BODY>
</HTML>
</xsl:template>

</xsl:stylesheet>
