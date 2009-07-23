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

  <xsl:template match='table' mode="xref">
    <fo:block>
      <xsl:if test="$xrefShowTitle">
        <xsl:value-of select="$tableWord"/>
        <xsl:text> </xsl:text>
      </xsl:if>
      <xsl:call-template name="calculateTableNumber"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="table">
    <fo:block>
      <xsl:choose>
        <xsl:when test="$inlineTables or @rend='inline'">
          <xsl:if test="head">
            <fo:block>
              <xsl:call-template name="tableCaptionstyle"/>
              <xsl:call-template name="addID"/>
              <xsl:if test="$makeTableCaption">
                <xsl:value-of select="$tableWord"/>
                <xsl:call-template name="calculateTableNumber"/>
                <xsl:text>.</xsl:text>
              </xsl:if>
              <xsl:apply-templates select="head"/>
            </fo:block>
          </xsl:if>
        </xsl:when>
        <xsl:otherwise>
          <fo:block text-align="{$tableCaptionAlign}"
                padding-bottom="{$spaceBelowCaption}">
            <xsl:value-of select="$tableWord"/>
            <xsl:call-template name="calculateTableNumber"/>
            <xsl:text>.</xsl:text>
            <xsl:apply-templates select="head"/>
          </fo:block>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:if test="row">
        <fo:table  text-align="{$tableAlign}"
            font-size="{$tableSize}">
          <xsl:call-template name="addID"/>
          <xsl:call-template name="blockTable"/>
        </fo:table>
      </xsl:if>
    </fo:block>
  </xsl:template>

  <xsl:template name="blockTable">
    <fo:table-body text-indent="0pt">
      <xsl:apply-templates select="row"/>
    </fo:table-body>
  </xsl:template>

  <xsl:template match="row">
    <fo:table-row>
      <xsl:apply-templates select="cell"/>
    </fo:table-row>
  </xsl:template>

  <xsl:template match="cell">
    <fo:table-cell>
      <xsl:if test="@cols &gt; 1">
        <xsl:attribute name="number-columns-spanned">
          <xsl:value-of select="@cols"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="@rows &gt; 1">
        <xsl:attribute name="number-rows-spanned">
          <xsl:value-of select="@rows"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:call-template name="cellProperties"/>
      <fo:block>
        <xsl:if test="@role='label' or parent::row[@role='label']">
          <xsl:attribute name="font-weight">bold</xsl:attribute>
        </xsl:if>
        <xsl:apply-templates/>
      </fo:block>
    </fo:table-cell>
  </xsl:template>

  <xsl:template name="cellProperties" >
    <xsl:attribute name="start-indent">3pt</xsl:attribute>
    <xsl:if test="@role='hi' or @role='label' or parent::row/@role='label'">
      <xsl:attribute name="background-color">silver</xsl:attribute>
    </xsl:if>
    <xsl:if test="ancestor::table[1][@rend='frame']">     
      <xsl:if test="not(parent::row/preceding-sibling::row)">
        <xsl:attribute name="border-top-color">#000000</xsl:attribute>
        <xsl:attribute name="border-top-width">1pt</xsl:attribute>
      </xsl:if>
      <xsl:attribute name="border-bottom-color">#000000</xsl:attribute>
      <xsl:attribute name="border-bottom-width">1pt</xsl:attribute>
      <xsl:if test="not(following-sibling::cell)">
        <xsl:attribute name="border-right-color">#000000</xsl:attribute>
        <xsl:attribute name="border-right-width">1pt</xsl:attribute>
      </xsl:if>
      <xsl:attribute name="border-left-color">#000000</xsl:attribute>
      <xsl:attribute name="border-left-width">1pt</xsl:attribute>
    </xsl:if>
    <xsl:if test="not(ancestor::table/@rend='tight')">
      <xsl:attribute name="padding">
        <xsl:value-of select="$tableCellPadding"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:choose>
      <xsl:when test="@halign">
        <xsl:attribute name="text-align">
          <xsl:value-of select="@halign"/>
        </xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <xsl:attribute name="text-align">left</xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="calculateTableNumber">
    <xsl:number  from="text"/>
  </xsl:template>

</xsl:stylesheet>
