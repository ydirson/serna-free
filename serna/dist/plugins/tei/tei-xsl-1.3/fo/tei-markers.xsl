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

<xsl:template match="milestone">
    <fo:block>
    <xsl:text>******************</xsl:text>
    <xsl:value-of select="@unit"/>
    <xsl:text> </xsl:text><xsl:value-of select="@n"/>
    <xsl:text>******************</xsl:text>
    </fo:block>
</xsl:template>

<xsl:template match="pb">
<xsl:choose>
  <xsl:when test="$activePagebreaks">
     <fo:block break-before="page">
     </fo:block>
  </xsl:when>
  <xsl:otherwise>
     <fo:block text-align="center">
      <xsl:text>&#x2701;[</xsl:text>
      <xsl:value-of select="@unit"/>
      <xsl:text> Page </xsl:text>
      <xsl:value-of select="@n"/>
      <xsl:text>]&#x2701;</xsl:text>
     </fo:block>
  </xsl:otherwise>
</xsl:choose>
</xsl:template>

<xsl:template match="eg[@rend='kwic']/lb"/>

<xsl:template match="cell//lb">
 <fo:character character="&#x2028;"/>
</xsl:template>

<xsl:template match="lb">
<xsl:choose>
  <xsl:when test="$activeLinebreaks">
<!-- this is a *visible* linebreak character 

PassiveTeX implements it as a real line break
-->
    <fo:character character="&#x2028;"/>
  </xsl:when>
  <xsl:otherwise>
    <fo:inline font-size="8pt">
      <xsl:text>&#x2761;</xsl:text>
    </fo:inline>
  </xsl:otherwise>
</xsl:choose>
<!-- JT's suggestion:
<fo:inline
 xml:space="preserve"
 white-space-collapse="false">&#xA;</fo:inline>
-->
</xsl:template>



</xsl:stylesheet>
