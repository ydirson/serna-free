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

  <xsl:template match="byline">
    <fo:block text-align="center">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="epigraph">
    <fo:block 	
      text-align="center"
      padding-top="4pt"
      padding-bottom="4pt"
      start-indent="{$exampleMargin}">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="closer">
    <fo:block 	
      padding-top="4pt"
      padding-bottom="4pt">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="salute">
    <fo:block text-align="left">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="signed">
    <fo:block text-align="left">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="epigraph/lg">
    <fo:block 
      text-align="center"
      padding-top="4pt"
      padding-bottom="4pt">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="l">
    <fo:block 	
      padding-top="0pt"
      padding-bottom="0pt">
      <xsl:choose>
        <xsl:when test="starts-with(@rend,'indent(')">
          <xsl:attribute name="start-indent">
            <xsl:value-of 
              select="concat(substring-before(substring-after(@rend,'('),')'),'em')"/>
          </xsl:attribute>
        </xsl:when>
        <xsl:when test="starts-with(@rend,'indent')">
          <xsl:attribute name="start-indent">1em</xsl:attribute>
        </xsl:when>
      </xsl:choose>
      <xsl:apply-templates/>
    </fo:block> 
  </xsl:template>

  <xsl:template match="lg">
    <fo:block 
      text-align="start"
      padding-top="4pt"
      padding-bottom="4pt">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

</xsl:stylesheet>
