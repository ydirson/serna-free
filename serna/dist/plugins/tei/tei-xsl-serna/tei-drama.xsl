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

  <xsl:template match="sp">
    <fo:block 
      text-align="justify" 
      padding-top="3pt">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="sp/p">
    <fo:inline><xsl:apply-templates/></fo:inline>
  </xsl:template>

  <xsl:template match="speaker">
    <fo:inline
      border-left-width="0pt"
      border-right-width="0pt">
      <xsl:call-template name="rend">
        <xsl:with-param name="defaultvalue" select="string('italic')"/>
        <xsl:with-param name="defaultstyle" select="string('font-style')"/>
      </xsl:call-template>
      <xsl:apply-templates/>
      <xsl:text> </xsl:text>
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
      <xsl:attribute name="start-indent">1em</xsl:attribute>
      <xsl:call-template name="rend">
        <xsl:with-param name="defaultvalue" select="string('normal')"/>
        <xsl:with-param name="defaultstyle" select="string('font-style')"/>
      </xsl:call-template>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

</xsl:stylesheet>
