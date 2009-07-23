<!-- $Date: 
TEI XSLT stylesheet family version 1.3
RCS: 2001/10/01 $, $Revision: 1.1 $, $Author: ilia $

XSL HTML stylesheet to format TEI XML documents 

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

<xsl:template match="div[@type='frontispiece']">
 <xsl:apply-templates/>
</xsl:template>

<xsl:template match="div[@type='epistle']">
 <xsl:apply-templates/>
</xsl:template>

<xsl:template match="div[@type='illustration']">
 <xsl:apply-templates/>
</xsl:template>

<!--
<xsl:template match="div[@type='canto']">
  <xsl:variable name="divlevel" select="count(ancestor::div)"/>
  <xsl:call-template name="NumberedHeading">
    <xsl:with-param name="level"><xsl:value-of select="$divlevel"/></xsl:with-param>
  </xsl:call-template>
  <xsl:apply-templates/>
</xsl:template>

-->

<xsl:template match="byline">
 <p align="center">
   <xsl:apply-templates/>
 </p>
</xsl:template>

<xsl:template match="epigraph">
 <p 		align="center">
   <xsl:apply-templates/>
 </p>
</xsl:template>

<xsl:template match="closer">
 <p 	
	space-before.optimum="4pt"
	space-after.optimum="4pt">
   <xsl:apply-templates/>
 </p>
</xsl:template>

<xsl:template match="salute">
 <p  align="left">
   <xsl:apply-templates/>
 </p>
</xsl:template>

<xsl:template match="signed">
 <p  align="left">
   <xsl:apply-templates/>
 </p>
</xsl:template>

<xsl:template match="epigraph/lg">
    <table>
      <xsl:apply-templates/>
    </table>
</xsl:template>


<xsl:template match="l">
  <tr><td>
  <xsl:choose>
    <xsl:when test="@rend='Alignr'">
      <xsl:attribute name="align">right</xsl:attribute>
    </xsl:when>
    <xsl:when test="@rend='Alignc'">
     <xsl:attribute name="align">center</xsl:attribute>
    </xsl:when>
    <xsl:when test="@rend='Alignl'">
      <xsl:attribute name="align">left</xsl:attribute>
      <xsl:text>&#xA0;&#xA0;</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:attribute name="align"><xsl:value-of select="$cellAlign"/></xsl:attribute>
     <xsl:choose>
     <xsl:when test="starts-with(@rend,'indent(')">
    <xsl:attribute name="text-indent">
      <xsl:value-of select="concat(substring-before(substring-after(@rend,'('),')'),'em')"/>
    </xsl:attribute>
  </xsl:when>
  <xsl:when test="starts-with(@rend,'indent')">
    <xsl:attribute name="text-indent">1em</xsl:attribute>
  </xsl:when>
  </xsl:choose>
</xsl:otherwise>
</xsl:choose>
  <xsl:apply-templates/>
</td></tr> 
</xsl:template>

<xsl:template match="lg">
    <table>
      <xsl:apply-templates/>
    </table>
</xsl:template>

</xsl:stylesheet>
