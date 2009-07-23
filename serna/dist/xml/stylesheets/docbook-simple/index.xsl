<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="index|setindex|indexdiv|indexentry|primaryie">
    <xsl:call-template name="div"/>
  </xsl:template>

  <xsl:template match="secondaryie">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="start-indent" select="'1em'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="tertiaryie">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="start-indent" select="'2em'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="seeie|seealsoie">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="start-indent" select="'3em'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="seeie">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="start-indent" select="'3em'"/>
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

  <xsl:template match="seealsoie">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="start-indent" select="'3em'"/>
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
