<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:attribute-set name="bdr">
    <xsl:attribute name="border-width">1px</xsl:attribute>
    <xsl:attribute name="border-color">black</xsl:attribute>
    <xsl:attribute name="border-style">solid</xsl:attribute>
    <xsl:attribute name="padding-left">2pt</xsl:attribute>
    <xsl:attribute name="padding-right">2pt</xsl:attribute>
    <xsl:attribute name="padding-top">2pt</xsl:attribute>
    <xsl:attribute name="padding-bottom">2pt</xsl:attribute>
    <xsl:attribute name="number-rows-spanned">1</xsl:attribute>
    <xsl:attribute name="display-align">before</xsl:attribute>
    <xsl:attribute name="text-align">center</xsl:attribute>
  </xsl:attribute-set>

  <xsl:attribute-set name="bdr-list">
    <xsl:attribute name="border-right-width">1px</xsl:attribute>
    <xsl:attribute name="border-right-color">black</xsl:attribute>
    <xsl:attribute name="border-right-style">solid</xsl:attribute>
    <xsl:attribute name="border-left-width">1px</xsl:attribute>
    <xsl:attribute name="border-left-color">black</xsl:attribute>
    <xsl:attribute name="border-left-style">solid</xsl:attribute>
    <xsl:attribute name="padding-left">2pt</xsl:attribute>
    <xsl:attribute name="padding-right">2pt</xsl:attribute>
    <xsl:attribute name="padding-top">2pt</xsl:attribute>
    <xsl:attribute name="padding-bottom">2pt</xsl:attribute>
  </xsl:attribute-set>

  <xsl:template match="tbl-tools|tbl-cons">
    <fo:block text-align="center" padding-top="3pt" padding-bottom="3pt">
     <fo:table border-collapse="collapse" 
               border-style="solid" 
               border-width="1px" 
               border-color="#000000" 
               table-layout="fixed" width="100%">
        <fo:table-body text-align="center" font-size="12pt">
          <fo:table-row>
            <fo:table-cell xsl:use-attribute-sets="bdr">
              <fo:block font-weight="bold" text-align="center">REFERENCE</fo:block>
            </fo:table-cell>
            <fo:table-cell xsl:use-attribute-sets="bdr">
              <fo:block font-weight="bold" text-align="center">DESIGNATION</fo:block>
            </fo:table-cell>
          </fo:table-row>
          <xsl:apply-templates select="std|ted|con"/>
        </fo:table-body>
      </fo:table>
    </fo:block>
  </xsl:template>

  <xsl:template match="std|ted|con">
    <fo:table-row>
      <xsl:apply-templates/>
    </fo:table-row>
  </xsl:template>

  <xsl:template match="stdnbr|stdname|toolnbr|toolname|connbr|conname">
    <fo:table-cell xsl:use-attribute-sets="bdr">
      <fo:block>
        <xsl:apply-templates/>
      </fo:block>
    </fo:table-cell>
  </xsl:template>


  <xsl:template match="prtlist">
    <fo:block padding-top="0.83em" span="none">
      <fo:table border-collapse="collapse" 
                border-style="solid" border-width="1px" border-color="#000000" table-layout="fixed" width="100%">
        <fo:table-column column-number="1" column-width="proportional-column-width(12)"/>
        <fo:table-column column-number="2" column-width="proportional-column-width(24)"/>
        <fo:table-column column-number="3" column-width="proportional-column-width(48)"/>
        <fo:table-column column-number="4" column-width="proportional-column-width(10)"/>
        <fo:table-column column-number="5" column-width="proportional-column-width(8)"/>
        <fo:table-body font-size="10pt" font-family="{$serif.font.family}">
          <fo:table-row>
            <fo:table-cell xsl:use-attribute-sets="bdr">
              <fo:block>
                <fo:block padding-top="0em" font-weight="bold">FIG. ITEM</fo:block>
              </fo:block>
            </fo:table-cell>
            <fo:table-cell xsl:use-attribute-sets="bdr">
              <fo:block>
                <fo:block padding-top="0em" font-weight="bold">PART NUMBER</fo:block>
              </fo:block>
            </fo:table-cell>
            <fo:table-cell xsl:use-attribute-sets="bdr">
              <fo:block>
                <fo:block padding-top="0em" font-weight="bold">NOMENCLATURE</fo:block>
              </fo:block>
            </fo:table-cell>
            <fo:table-cell xsl:use-attribute-sets="bdr">
              <fo:block>
                <fo:block padding-top="0em" font-weight="bold">EFFECT FROM TO</fo:block>
              </fo:block>
            </fo:table-cell>
            <fo:table-cell xsl:use-attribute-sets="bdr">
              <fo:block>
                <fo:block padding-top="0em" font-weight="bold">UNITS PER ASSY</fo:block>
              </fo:block>
            </fo:table-cell>
          </fo:table-row>
          <xsl:apply-templates/>
        </fo:table-body>
      </fo:table>
    </fo:block>
  </xsl:template>

  <xsl:template match="itemdata">
    <fo:table-row padding-top="0em" padding-bottom="0em">
      <xsl:if test="not(itemcol)">
        <fo:table-cell xsl:use-attribute-sets="bdr-list">
          <fo:block>
            <xsl:text> </xsl:text>
          </fo:block>
        </fo:table-cell>
      </xsl:if>
      <xsl:apply-templates select="itemcol"/>

      <xsl:if test="not(pnr)">
        <fo:table-cell xsl:use-attribute-sets="bdr-list">
          <fo:block>
            <xsl:text> </xsl:text>
          </fo:block>
        </fo:table-cell>
      </xsl:if>
      <xsl:apply-templates select="pnr"/>

      <xsl:if test="not(iplnom)">
        <fo:table-cell xsl:use-attribute-sets="bdr-list">
          <fo:block>
            <xsl:text> </xsl:text>
          </fo:block>
        </fo:table-cell>
      </xsl:if>
      <xsl:apply-templates select="iplnom"/>

      <xsl:if test="not(effcode)">
        <fo:table-cell xsl:use-attribute-sets="bdr-list">
          <fo:block>
            <xsl:text> </xsl:text>
          </fo:block>
        </fo:table-cell>
      </xsl:if>
      <xsl:apply-templates select="effcode"/>

      <xsl:if test="not(upa)">
        <fo:table-cell xsl:use-attribute-sets="bdr-list">
          <fo:block>
            <xsl:text> </xsl:text>
          </fo:block>
        </fo:table-cell>
      </xsl:if>
      <xsl:apply-templates select="upa"/>
    </fo:table-row>
  </xsl:template>

  <xsl:template match="itemcol">
    <fo:table-cell xsl:use-attribute-sets="bdr-list">
      <fo:block>
        <xsl:variable name="illusind">
          <xsl:value-of select="../@illusind"/>
        </xsl:variable>
        <xsl:if test="$illusind = 0">
            <xsl:text>- </xsl:text>
        </xsl:if>
        <xsl:value-of select="../@itemnbr"/>
        <xsl:apply-templates/>
      </fo:block>
    </fo:table-cell>
  </xsl:template>

  <xsl:template match="iplnom">
    <fo:table-cell xsl:use-attribute-sets="bdr-list">
      <fo:block>
        <xsl:apply-templates/>
      </fo:block>
    </fo:table-cell>
  </xsl:template>

  <xsl:template match="iplnom/nom|iplnom/nom/kwd|iplnom/nom/adt">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="iplnom/vp">
    <fo:inline text-align="right">
      <xsl:if test="not(node())">
        <xsl:text> </xsl:text>
      </xsl:if>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="itemdata/pnr|upa|effcode">
    <fo:table-cell xsl:use-attribute-sets="bdr-list">
      <fo:block>
        <xsl:apply-templates/>
      </fo:block>
    </fo:table-cell>
  </xsl:template>

</xsl:stylesheet>
