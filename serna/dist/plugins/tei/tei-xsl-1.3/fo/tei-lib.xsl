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

<!-- enable, for new elements to get commented as such-->
<xsl:template match="*">
 <xsl:comment><xsl:text>PASS THROUGH </xsl:text>
   <xsl:value-of select="name()"/>
 </xsl:comment>
 <xsl:apply-templates/>
</xsl:template>

<xsl:template name="rend">
 <xsl:param name="defaultvalue"/>
 <xsl:param name="defaultstyle"/>
 <xsl:param name="rend"/>
<xsl:choose>
 <xsl:when test="$rend=''">
     <xsl:attribute name="{$defaultstyle}">
         <xsl:value-of select="$defaultvalue"/>
    </xsl:attribute>  
 </xsl:when>
 <xsl:when test="contains($rend,';')">
   <xsl:call-template name="applyRend">
     <xsl:with-param name="rendvalue" select="substring-before($rend,';')"/>
   </xsl:call-template>
   <xsl:call-template name="rend">
     <xsl:with-param name="rend" select="substring-after($rend,';')"/>
   </xsl:call-template>
 </xsl:when>
 <xsl:otherwise>
   <xsl:call-template name="applyRend">
     <xsl:with-param name="rendvalue" select="$rend"/>
   </xsl:call-template>   
 </xsl:otherwise>
</xsl:choose>
</xsl:template>

<xsl:template name="applyRend">
  <xsl:param name="rendvalue"/>
   <xsl:choose>
   <xsl:when test="$rendvalue='gothic'">
     <xsl:attribute name="font-family">fantasy</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='calligraphic'">
     <xsl:attribute name="font-family">cursive</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='ital' or $rendvalue='italic' or $rendvalue='it' or $rendvalue='i'">
     <xsl:attribute name="font-style">italic</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='sc'">
     <xsl:attribute name="font-variant">small-caps</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='code'">
     <xsl:attribute name="font-family">
       <xsl:value-of select="$typewriterFont"/>
     </xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='bo' or $rendvalue='bold'">
     <xsl:attribute name="font-weight">bold</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='BO'">
     <xsl:attribute name="font-style">italic</xsl:attribute>
     <xsl:attribute name="text-decoration">underline</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='UL' or $rendvalue='ul'">
     <xsl:attribute name="text-decoration">underline</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='sub'">
     <xsl:attribute name="vertical-align">sub</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='small'">
     <xsl:attribute name="font-size">small</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='strike'">
     <xsl:attribute name="text-decoration">line-through</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='sup'">
     <xsl:attribute name="vertical-align">super</xsl:attribute>
   </xsl:when>
</xsl:choose>
</xsl:template>

<xsl:template name="addID">
      <xsl:attribute name="id">
        <xsl:call-template name="idLabel"/>
      </xsl:attribute>
</xsl:template>

<xsl:template name="idLabel">
   <xsl:choose>
       <xsl:when test="@id">
         <xsl:value-of select="translate(@id,'_','-')"/>
       </xsl:when>
       <xsl:otherwise>
          <xsl:value-of select="generate-id()"/>
       </xsl:otherwise>
   </xsl:choose>
</xsl:template>

<xsl:template match="processing-instruction()[name()='xmltex']" >
<xsl:message>xmltex pi <xsl:value-of select="."/></xsl:message>
   <xsl:copy-of select="."/>
</xsl:template>

</xsl:stylesheet>
