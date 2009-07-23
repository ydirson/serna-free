<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="glossary|glossentry|glossdiv">
    <xsl:call-template name="div"/>
  </xsl:template>

  <xsl:template match="glosslist">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="start-indent" select="'1em'"/>
      <!-- tb lr -->
      <xsl:with-param name="padding" select="'0.6em 0em'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="glossterm">
    <xsl:call-template name="italic.inline"/>
  </xsl:template>

  <xsl:template match="glossentry/acronym|glossentry/abbrev|
                       glossentry/indexterm|glossentry/revhistory">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="start-indent" select="'2em'"/>
      <!-- t r b l  -->
      <!-- t l b    -->
      <xsl:with-param name="padding" select="'.5em 0em 0.5em'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="glossdef|glosssee">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="start-indent" select="'2em'"/>
      <xsl:with-param name="padding" select="'0em 0em 0.5em'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="glosssee">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="start-indent" select="'2em'"/>
      <xsl:with-param name="content">
        <xsl:call-template name="small.inline">
          <xsl:with-param name="content">
            <xsl:text>See</xsl:text>
            <xsl:text> </xsl:text>
          </xsl:with-param>
        </xsl:call-template>
        <xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="glossseealso">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="start-indent" select="'0.5em'"/>
      <xsl:with-param name="content">
        <xsl:call-template name="small.inline">
          <xsl:with-param name="content">
            <xsl:text>See also</xsl:text>
            <xsl:text> </xsl:text>
          </xsl:with-param>
        </xsl:call-template>
        <xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>


</xsl:stylesheet>
