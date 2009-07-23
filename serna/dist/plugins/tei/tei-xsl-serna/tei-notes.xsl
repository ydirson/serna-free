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

  <xsl:template match="note" mode="endnote">
    <fo:block id="{generate-id()}">
      <xsl:call-template name="calculateEndNoteNumber"/>
      <xsl:text>.</xsl:text>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="note">
    <xsl:choose>
      <xsl:when test="@place='display'">
        <fo:block text-indent="0pt"
                end-indent="{$exampleMargin}"
                start-indent="{$exampleMargin}"
                font-size="{$exampleSize}"
                padding-top="{$exampleBefore}"
                padding-bottom="{$exampleAfter}">
          <xsl:apply-templates/>
        </fo:block>
      </xsl:when>
      <xsl:when test="@place='divtop'">
        <fo:block font-style="italic"
                text-indent="0pt"
                end-indent="{$exampleMargin}"
                start-indent="{$exampleMargin}"
                font-size="{$exampleSize}"
                padding-top="{$exampleBefore}"
                padding-bottom="{$exampleAfter}">
          <xsl:apply-templates/>
        </fo:block>
      </xsl:when>
      <xsl:otherwise>
        <fo:inline>
          <xsl:if test="node()">
            <xsl:text> (</xsl:text>
            <xsl:apply-templates/>
            <xsl:text>)</xsl:text>
          </xsl:if>
        </fo:inline>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="calculateEndNoteNumber">
    <xsl:number format="i" count="note[@place='end']"/>
  </xsl:template>

  <xsl:template name="calculateFootnoteNumber">
    <xsl:number  from="text" count="note[@place='foot']"/>
  </xsl:template>

</xsl:stylesheet>
