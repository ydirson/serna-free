<!-- 
TEI XSLT stylesheet family version 1.3
RCS: $Date: 2003/11/24 14:49:47 $, $Revision: 1.1 $, $Author: ilia $

XSL FO stylesheet to format TEI XML documents 

 Copyright 1999-2002 Sebastian Rahtz/Oxford University  <sebastian.rahtz@oucs.ox.ac.uk>

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and any associated documentation files (the
 ``Software''), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
-->
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns:fo="http://www.w3.org/1999/XSL/Format"
  >


<!-- ignore the header -->
<xsl:template match="teiHeader">
<!--
  <fo:block>
    <xsl:for-each select="@*"> 
      <xsl:text>Value of </xsl:text><xsl:value-of select="name(.)"/>
      <xsl:text> is </xsl:text><xsl:value-of select="."/>
    </xsl:for-each>
  </fo:block>
-->
</xsl:template>

<xsl:template name="textTitle">
  <xsl:apply-templates select="front"/>  
</xsl:template>

<!-- author and title -->
<xsl:template match="docTitle">
    <fo:block text-align="left" font-size="{$titleSize}" >
      <xsl:if test="ancestor::group/text/front">
        <xsl:attribute name="id">
      <xsl:choose>
        <xsl:when test="ancestor::text/@id">
         <xsl:value-of select="translate(ancestor::text/@id,'_','-')"/>
       </xsl:when>
       <xsl:otherwise>
          <xsl:value-of select="generate-id()"/>
       </xsl:otherwise>
       </xsl:choose>
        </xsl:attribute>
      </xsl:if>
	<fo:inline font-weight="bold">
<xsl:apply-templates select="titlePart"/></fo:inline>
    </fo:block>
</xsl:template>

<xsl:template match="docImprint"/>

<xsl:template match="docAuthor" mode="heading">
  <xsl:if test="preceding-sibling::docAuthor">
   <xsl:choose>
     <xsl:when test="not(following-sibling::docAuthor)">
	<xsl:text> and </xsl:text>
     </xsl:when>
     <xsl:otherwise>
	<xsl:text>, </xsl:text>
     </xsl:otherwise>
   </xsl:choose>
 </xsl:if>
 <xsl:apply-templates/>
</xsl:template>

<xsl:template match="docAuthor">
    <fo:block font-size="{$authorSize}">
       <fo:inline font-style="italic">
        <xsl:apply-templates/>
       </fo:inline>
    </fo:block>
</xsl:template>

<xsl:template match="docDate">
    <fo:block font-size="{$dateSize}">
	<xsl:apply-templates/></fo:block>
</xsl:template>

<!-- omit if found outside front matter -->
<xsl:template match="div/docDate"/>
<xsl:template match="div/docAuthor"/>
<xsl:template match="div/docTitle"/>
</xsl:stylesheet>
