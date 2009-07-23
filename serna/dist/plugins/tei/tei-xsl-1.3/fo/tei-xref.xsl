<!-- 
TEI XSLT stylesheet family version 1.3
RCS: $Date: 2003/11/24 14:49:48 $, $Revision: 1.1 $, $Author: ilia $

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

<!-- cross-referencing -->
<xsl:template match="ptr">
 <fo:basic-link color="{$linkColor}">
  <xsl:attribute name="internal-destination"><xsl:value-of select="translate(@target,'_','-')"/></xsl:attribute>
  <xsl:apply-templates mode="xref" select="id(@target)" />
 </fo:basic-link>
</xsl:template>

<xsl:template match="ref">
 <fo:basic-link color="{$linkColor}">
  <xsl:attribute name="internal-destination"><xsl:value-of select="translate(@target,'_','-')"/></xsl:attribute>
  <xsl:apply-templates/>
 </fo:basic-link>
</xsl:template>

<xsl:template match="xref">
  <xsl:variable name="dest">
       <xsl:choose>
    <xsl:when test="@doc">
     <xsl:value-of select="unparsed-entity-uri(@doc)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="@url"/>
    </xsl:otherwise>
   </xsl:choose>
  </xsl:variable>
  <fo:basic-link color="{$linkColor}" external-destination="{$dest}">
      <xsl:apply-templates/>
   </fo:basic-link>
   <xsl:call-template name="showXrefURL">
       <xsl:with-param name="dest" select="$dest"/>
   </xsl:call-template>
</xsl:template>

<xsl:template match="xptr">
<fo:basic-link font-family="{$typewriterFont}" color="{$linkColor}">
   <xsl:attribute name="external-destination">
    <xsl:choose>
     <xsl:when test="@doc">
      <xsl:value-of select="unparsed-entity-uri(@doc)"/>
     </xsl:when>
     <xsl:otherwise>
       <xsl:value-of select="@url"/>
     </xsl:otherwise>
    </xsl:choose>
   </xsl:attribute>
   <xsl:choose>
    <xsl:when test="@doc">
     <xsl:value-of select="unparsed-entity-uri(@doc)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="@url"/>
    </xsl:otherwise>
   </xsl:choose>
</fo:basic-link>
</xsl:template>

</xsl:stylesheet>
