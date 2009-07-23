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

<xsl:template match="bibl">
  <fo:inline><xsl:apply-templates/></fo:inline>
</xsl:template>

<xsl:template match="listBibl/bibl">
 <fo:block>
   <xsl:call-template name="addID"/>
   <xsl:attribute name="padding-top">
     <xsl:value-of select="$spaceBeforeBibl"/>
   </xsl:attribute>
   <xsl:attribute name="padding-bottom">
     <xsl:value-of select="$spaceAfterBibl"/>
   </xsl:attribute>
   <xsl:attribute name="text-indent">-<xsl:value-of select="$indentBibl"/></xsl:attribute>
   <xsl:attribute name="start-indent">
     <xsl:value-of select="$indentBibl"/>
   </xsl:attribute>
   <xsl:apply-templates/>
 </fo:block>
</xsl:template>

<xsl:template match="listBibl">
<fo:block>
  <xsl:choose>
    <xsl:when test="ancestor::back">
      <xsl:call-template name="setupDiv0"/>
      <xsl:call-template name="addID"/>
      <xsl:value-of select="$biblioWords"/>
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
</fo:block>
</xsl:template>

</xsl:stylesheet>
