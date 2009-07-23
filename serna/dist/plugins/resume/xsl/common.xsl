<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
  xmlns:r="http://xmlresume.sourceforge.net/resume/0.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="header">
  <xsl:choose>
    <xsl:when test="$header.format = 'centered'">
      <xsl:apply-templates select="." mode="centered"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates select="." mode="standard"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="Title">
  <xsl:param name="Title">NO DEFAULT TITLE DEFINED</xsl:param>
  <xsl:param name="Separator"></xsl:param>
  <xsl:choose>
    <xsl:when test="title">
      <xsl:apply-templates select="./title"/>
      <xsl:value-of select="$Separator"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$Title"/>
      <xsl:value-of select="$Separator"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="title">
  <xsl:call-template name="inline"/>
</xsl:template>

<xsl:template match="location/*">
  <xsl:apply-templates/>
  <xsl:if test="following-sibling::*">
    <xsl:text>, </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="subjects">
  <xsl:if test="subject">
    <xsl:choose>
      <xsl:when test="$subjects.format = 'table'">
        <xsl:apply-templates select="." mode="table"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="." mode="comma"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>

