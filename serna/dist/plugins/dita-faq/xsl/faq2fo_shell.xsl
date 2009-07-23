<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
		xmlns:fo="http://www.w3.org/1999/XSL/Format"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:import href="http://www.syntext.com/dita-1.2/dita2fo_shell.xsl"/>

<xsl:import href="faq2fo.xsl"/>
<xsl:import href="faq2fo_toc.xsl"/>

<xsl:template match="*[@conref]" priority="100">
  <xsl:param name="conrefs-queue"/>
  <xsl:variable name="id">
    <xsl:value-of select="generate-id(.)"/>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="string-length(@conref) = 0">
      <xsl:variable name="error-message">
        <xsl:text>[Conref is empty]</xsl:text>
      </xsl:variable>
      <xsl:call-template name="show-conref-error">
        <xsl:with-param name="message" select="$error-message"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="contains($conrefs-queue, $id)">
      <xsl:variable name="error-message">
        <xsl:text>[Cyclic conref]</xsl:text>
      </xsl:variable>
      <xsl:call-template name="show-conref-error">
        <xsl:with-param name="message" select="$error-message"/>
      </xsl:call-template>      
    </xsl:when>
    <xsl:when test="$SHOW-CONREF-RESOLVED='yes'">
      <xsl:variable name="queue" select="concat($conrefs-queue, '/', $id)"/>
      <xsl:call-template name="href">
        <xsl:with-param name="href" select="@conref"/>
        <xsl:with-param name="type" select="'conref'"/>
        <xsl:with-param name="conrefs-queue" select="$queue"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="hide-resolved-conref"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="show-conref-error">
  <xsl:param name="message"/>
  <xsl:choose>
    <xsl:when test="self::*[contains(@class, ' topic/topic ')] or
                    self::*[contains(@class, ' map/map ')]">
      <fo:block color="red" font-weight="bold">
        <xsl:value-of select="$message"/>
      </fo:block>
    </xsl:when>
    <xsl:when test="self::*[contains(@class, ' topic/li ')]">
      <fo:list-item padding-bottom="0.4em">
        <fo:list-item-label end-indent="label-end()" text-align="end">
          <xsl:call-template name="generate-listitem-label">
            <xsl:with-param name="list-level">
              <xsl:call-template name="get-list-level"/>
            </xsl:with-param>
          </xsl:call-template>
        </fo:list-item-label>
        <fo:list-item-body start-indent="body-start()">
          <fo:block color="red" font-weight="bold">
            <xsl:value-of select="$message"/>
          </fo:block>
        </fo:list-item-body>
      </fo:list-item>
    </xsl:when>
    <xsl:when test="self::*[contains(@class, ' faq/faqquest ')]">
      <fo:list-item>
	<xsl:call-template name="faqquest-label"/>
	<fo:list-item-body start-indent="body-start()"> 
	  <fo:block color="red" font-weight="bold">
            <xsl:value-of select="$message"/>
	  </fo:block>
	</fo:list-item-body> 
      </fo:list-item>
    </xsl:when>
    <xsl:when test="self::*[contains(@class, ' faq/faqans ')]">
      <fo:list-item>
	<xsl:call-template name="faqans-label"/>
	<fo:list-item-body start-indent="body-start()"> 
	  <fo:block color="red" font-weight="bold">
            <xsl:value-of select="$message"/>
	  </fo:block>
	</fo:list-item-body> 
      </fo:list-item>
    </xsl:when>
    <xsl:otherwise>
      <fo:inline color="red" font-weight="bold">
        <xsl:value-of select="$message"/>
      </fo:inline>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
