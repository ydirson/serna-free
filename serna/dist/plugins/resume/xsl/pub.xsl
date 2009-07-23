<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
  xmlns:r="http://xmlresume.sourceforge.net/resume/0.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="FormatPub">
  <fo:block>
    <xsl:apply-templates select="author[position() != last()]" mode="internal"/>
    <xsl:apply-templates select="author[position() = last()]" mode="final"/>
    <xsl:apply-templates select="artTitle"/>
    <xsl:apply-templates select="bookTitle"/>
    <xsl:apply-templates select="publisher"/>
    <xsl:apply-templates select="date"/>
    <xsl:apply-templates select="pubDate"/>
    <xsl:apply-templates select="pageNums"/>
    <xsl:apply-templates select="url"/>
    <xsl:apply-templates select="para"/>
  </fo:block>
</xsl:template>

<xsl:template match="author" mode="internal">
  <xsl:call-template name="DerefAuthor"/>
  <xsl:value-of select="$pub.author.separator"/>
</xsl:template>

<xsl:template match="author[substring(text(), string-length(text()))='.']"
                     mode="final">
  <xsl:call-template name="DerefAuthor"/><xsl:text> </xsl:text>
</xsl:template>

<xsl:template match="author" mode="final">
  <xsl:call-template name="DerefAuthor"/>
  <xsl:value-of select="$pub.item.separator"/>
</xsl:template>

<xsl:template name="DerefAuthor">
  <xsl:choose>
    <xsl:when test="@name">
      <xsl:apply-templates select="id(@name)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="inline"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="bookTitle">
  <xsl:call-template name="inline"/>
  <xsl:value-of select="$pub.item.separator"/>
</xsl:template>

<xsl:template match="artTitle">
  <xsl:text>&quot;</xsl:text>
  <xsl:call-template name="inline"/>
  <xsl:text>&quot;</xsl:text>
  <xsl:value-of select="$pub.item.separator"/>
</xsl:template>

<xsl:template match="publisher[following-sibling::pubDate]">
  <xsl:call-template name="inline"/>
</xsl:template>

<xsl:template match="publisher">
  <xsl:call-template name="inline"/>
  <xsl:value-of select="$pub.item.separator"/>
</xsl:template>

<xsl:template match="pubDate">
  <xsl:call-template name="inline"/>
  <xsl:value-of select="$pub.item.separator"/>
</xsl:template>

<xsl:template match="pub/date" name="FormatPubDate">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
  <xsl:value-of select="$pub.item.separator"/>
</xsl:template>

<xsl:template match="pageNums">
  <xsl:call-template name="inline"/>
  <xsl:value-of select="$pub.item.separator"/>
</xsl:template>
  
<xsl:template match="pub/url">
  <fo:inline font-family="{$url.font.family}">
    <xsl:call-template name="inline"/>
  </fo:inline>
  <xsl:value-of select="$pub.item.separator"/>
</xsl:template>

</xsl:stylesheet>
