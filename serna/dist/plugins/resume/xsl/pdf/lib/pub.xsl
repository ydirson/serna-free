<?xml version="1.0" encoding="UTF-8"?>

<!--
pub.xsl
Transformations for publications.

Copyright (c) 2002 Sean Kelly
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

$Id: pub.xsl,v 1.1 2002/05/26 03:45:51 bruckie Exp $
-->

<xsl:stylesheet version="1.0"
  xmlns:r="http://xmlresume.sourceforge.net/resume/0.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template name="FormatPub">
    <!-- Format each author, putting separator characters betwixt. -->
    <xsl:apply-templates select="r:author[position() != last()]" mode="internal"/>
    <xsl:apply-templates select="r:author[position() = last()]" mode="final"/>
    <!-- Format the other components of a publication. -->
    <xsl:apply-templates select="r:artTitle"/>
    <xsl:apply-templates select="r:bookTitle"/>
    <xsl:apply-templates select="r:publisher"/>
    <xsl:apply-templates select="r:date"/>
    <xsl:apply-templates select="r:pubDate"/> <!-- Deprecated as of 1.4.0. -->
    <xsl:apply-templates select="r:pageNums"/>
    <xsl:apply-templates select="r:url"/>
    <!-- And for those using free-form paragraphs, format those, too. -->
    <xsl:apply-templates select="r:para"/>
  </xsl:template>

  <!-- Format the all but the last author -->
  <xsl:template match="r:author" mode="internal">
    <xsl:call-template name="DerefAuthor"/>
    <xsl:value-of select="$pub.author.separator"/>
  </xsl:template>

  <!-- Format the last author whose name doesn't end in a period.
  NOTE: This prevents a format like "Fish, X.." from appearing, but
  only when the pub.item.separator is a ".", otherwise it just leaves
  out the pub.item.separator.  Does anyone know how we can test for
  $pub.item.separator instead of '.'? -->
  <xsl:template match="r:author[substring(text(), string-length(text()))='.']" mode="final">
    <xsl:call-template name="DerefAuthor"/><xsl:text> </xsl:text>
  </xsl:template>

  <!-- Format the last author -->
  <xsl:template match="r:author" mode="final">
    <xsl:call-template name="DerefAuthor"/>
    <xsl:value-of select="$pub.item.separator"/>
  </xsl:template>

  <!-- Format an author's name if provided or by reference to the name with the given id. -->
  <xsl:template name="DerefAuthor">
    <xsl:choose>
      <xsl:when test="@name">
        <xsl:apply-templates select="id(@name)"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Title of book -->
  <xsl:template match="r:bookTitle">
    <xsl:apply-templates/>
    <xsl:value-of select="$pub.item.separator"/>
  </xsl:template>

  <!-- Title of article -->
  <xsl:template match="r:artTitle">
    <xsl:text>&quot;</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>&quot;</xsl:text>
    <xsl:value-of select="$pub.item.separator"/>
  </xsl:template>

  <!-- Publisher with a following publication date. -->
  <xsl:template match="r:publisher[following-sibling::pubDate]">
    <xsl:apply-templates/>
  </xsl:template>

  <!-- Publisher without pub date -->
  <xsl:template match="r:publisher">
    <xsl:apply-templates/>
    <xsl:value-of select="$pub.item.separator"/>
  </xsl:template>

  <!-- Format the publication date -->
  <!-- Note: the FormatPubDate name attr is used only so that this template can
  be called from the r:pubDate deprecation message. The name can be removed when
  r:pubDate is removed from the DTD. -->
  <xsl:template match="r:pub/r:date" name="FormatPubDate">
    <xsl:call-template name="FormatDate"/>
    <xsl:value-of select="$pub.item.separator"/>
  </xsl:template>

  <!-- Format the page numbers of the journal in which the article appeared -->
  <xsl:template match="r:pageNums">
    <xsl:apply-templates/>
    <xsl:value-of select="$pub.item.separator"/>
  </xsl:template>
  
  <!-- Format a URL -->
  <xsl:template match="r:pub/r:url">
    <xsl:call-template name="FormatUrl"/>
    <xsl:value-of select="$pub.item.separator"/>
  </xsl:template>

</xsl:stylesheet>
