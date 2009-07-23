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

<xsl:template match="bibl">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="listBibl/bibl">
 <fo:block>
     <xsl:call-template name="addID"/>
     <xsl:attribute name="space-before.optimum">
<xsl:value-of select="$spaceBeforeBibl"/></xsl:attribute>
     <xsl:attribute name="space-after.optimum">
<xsl:value-of select="$spaceAfterBibl"/></xsl:attribute>
     <xsl:attribute name="text-indent">-<xsl:value-of select="$indentBibl"/>
</xsl:attribute>
     <xsl:attribute name="start-indent"><xsl:value-of select="$indentBibl"/>
</xsl:attribute>
   <xsl:apply-templates/>
 </fo:block>
</xsl:template>

<xsl:template match="listBibl">
<xsl:choose>
<!-- is it in the back matter? -->
<xsl:when test="ancestor::back">
 <fo:page-sequence>
  <fo:block>
    <xsl:call-template name="setupDiv0"/>
     <xsl:call-template name="addID"/>
     <xsl:value-of select="$biblioWords"/>
  </fo:block>
  <xsl:apply-templates/>
 </fo:page-sequence>
</xsl:when>
<xsl:otherwise>
  <xsl:apply-templates/>
</xsl:otherwise>
</xsl:choose>
</xsl:template>

</xsl:stylesheet>
