<!-- 
     TEI XSL Stylesheet for Syntext Serna (c) 2003, Syntext Inc.

     The Stylesheet is based on Sebastian Rahtz/Oxford University 
     XSL TEI Stylesheet v1.3. See file SR-COPYING for 
     Sebastian Rahtz/Oxford University Copyright 
     information.
-->
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns:fo="http://www.w3.org/1999/XSL/Format">

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
 <xsl:text>&#x2028;</xsl:text>
</xsl:template>

<xsl:template match="lb">
<fo:inline
 xml:space="preserve"
 white-space-collapse="false">&#xA;</fo:inline>
</xsl:template>


</xsl:stylesheet>
