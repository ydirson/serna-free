<!-- 
TEI XSLT stylesheet family version 1.3
RCS: $Date: 2003/11/24 14:49:47 $, $Revision: 1.1 $, $Author: ilia $

XSL FO stylesheet to format TEI XML documents 

 Copyright 1999-2002 Sebastian Rahtz/Oxford University  
   <sebastian.rahtz@oucs.ox.ac.uk>

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

<xsl:template match="sp">
<fo:block 
	text-align="justify" 
	start-indent="1em"
	text-indent="-1em"
 	space-before="3pt"
	>
      <xsl:apply-templates/>
</fo:block>
</xsl:template>

<!-- paragraphs inside speeches do very little-->
 <xsl:template match="sp/p">
  <fo:inline> 
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>


<xsl:template match="speaker">
<fo:inline>
 <xsl:call-template name="rend">
   <xsl:with-param name="defaultvalue" select="string('italic')"/>
   <xsl:with-param name="defaultstyle" select="string('font-style')"/>
 </xsl:call-template>
      <xsl:apply-templates/><xsl:text> </xsl:text>
</fo:inline>
</xsl:template>

<xsl:template match="p/stage">
<fo:inline>
 <xsl:call-template name="rend">
   <xsl:with-param name="defaultvalue" select="string('normal')"/>
   <xsl:with-param name="defaultstyle" select="string('font-style')"/>
 </xsl:call-template>
      <xsl:apply-templates/>
</fo:inline>
</xsl:template>

<xsl:template match="stage">
<fo:block>
 <xsl:attribute name="text-indent">1em</xsl:attribute>
 <xsl:call-template name="rend">
   <xsl:with-param name="defaultvalue" select="string('normal')"/>
   <xsl:with-param name="defaultstyle" select="string('font-style')"/>
 </xsl:call-template>
      <xsl:apply-templates/>
</fo:block>
</xsl:template>

</xsl:stylesheet>
