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
  xmlns:fotex="http://www.tug.org/fotex"
  xmlns:m="http://www.w3.org/1998/Math/MathML" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns:fo="http://www.w3.org/1999/XSL/Format">

<xsl:template match="m:math">
 <m:math>
  <xsl:copy-of select="@*"/>
  <xsl:apply-templates mode="math"/>
 </m:math>
</xsl:template>

<xsl:template match="m:*|@*|comment()|processing-instruction()|text()" mode="math">
 <xsl:copy>
   <xsl:apply-templates mode="math" select="*|@*|processing-instruction()|text()"/>
 </xsl:copy>
</xsl:template>

<xsl:template match="formula">
  <fo:wrapper>
   <xsl:if test="@id">
    <xsl:attribute name="id"><xsl:value-of select="@id"/></xsl:attribute>
   </xsl:if>
   <xsl:apply-templates/>
  </fo:wrapper>
</xsl:template>


<xsl:template match="formula" mode="xref">
 <xsl:number/>
</xsl:template>

<xsl:template match="formula[@type='subeqn']/m:math">
  <xsl:apply-templates mode="math"/>
</xsl:template>

<xsl:template match="table[@rend='eqnarray']">
   <fotex:eqnarray>
     <xsl:for-each select="row">
     <xsl:apply-templates select=".//formula"/>
     <xsl:if test="following-sibling::row">
       <!--        <fo:character character="&#x2028;"/>-->
       <xsl:processing-instruction name="xmltex">\\</xsl:processing-instruction>
     </xsl:if>
     </xsl:for-each>
   </fotex:eqnarray>
</xsl:template>


<xsl:template match="formula[@type='display']/m:math">
 <m:math display="block">
  <xsl:copy-of select="@*"/>
  <xsl:apply-templates mode="math"/>
 </m:math>
</xsl:template>

</xsl:stylesheet>


