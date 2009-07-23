<!-- 
TEI XSLT stylesheet family version 1.3
RCS: $Date: 2003/11/24 14:50:28 $, $Revision: 1.1 $, $Author: ilia $

XSL stylesheet to format TEI XML documents to HTML or XSL FO

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
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">



<!-- top-level stuff -->

<xsl:template match="docImprint"/>


<xsl:template match="front|titlePart">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="titlePage">
  <hr/>
  <table>
    <tr><td><b><xsl:apply-templates mode="print" select="docTitle"/></b></td></tr>
    <tr><td><i><xsl:apply-templates mode="print" select="docAuthor"/></i></td></tr>
    <tr><td><xsl:apply-templates mode="print" select="docDate"/></td></tr>
  </table>
  <hr/>
</xsl:template>

<xsl:template match="body|back" mode="split">
  <xsl:for-each select="*">
   <xsl:choose>
    <xsl:when test="starts-with(name(.),'div')">
       <xsl:apply-templates select="." mode="split"/>
    </xsl:when>
    <xsl:otherwise>
       <xsl:apply-templates select="."/>
    </xsl:otherwise>
   </xsl:choose>
  </xsl:for-each>
</xsl:template>

<xsl:template match="teiHeader"/>

<xsl:template match="text">
 <xsl:apply-templates/>
</xsl:template>

<!-- author and title -->
<xsl:template match="docTitle"/>
<xsl:template match="docAuthor"/>
<xsl:template match="docDate"/>

<xsl:template match="docDate" mode="print">
    <xsl:apply-templates/>
</xsl:template>

<xsl:template match="docAuthor" mode="author">
     <xsl:if test="preceding-sibling::docAuthor">
	<xsl:text>, </xsl:text>
     </xsl:if>
    <xsl:apply-templates/>
</xsl:template>



</xsl:stylesheet>
