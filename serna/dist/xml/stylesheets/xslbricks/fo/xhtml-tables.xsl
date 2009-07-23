<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template name="tabletitle">
    <xsl:apply-templates select="caption"/>
  </xsl:template>

  <xsl:template match="table/caption">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="table|TABLE">
    <fo:block>
      <xsl:if test="caption[not(self::processing-instruction('se:choice'))]">
        <fo:block font-size="12pt" font-weight="bold" text-align="center">
          <xsl:call-template name="tabletitle"/>
        </fo:block>
      </xsl:if>
      <xsl:if test="(col|colgroup)[not(self::processing-instruction('se:choice'))]">
        <fo:table xsl:use-attribute-sets="xhtml-table">
          <xsl:apply-templates select="(col|colgroup)[not(self::processing-instruction('se:choice'))]"/>
          <fo:table-body xsl:use-attribute-sets="xhtml-tbody">
            <fo:table-row xsl:use-attribute-sets="xhtml-tr">
              <xsl:apply-templates select="(col|colgroup)
                                   [not(self::processing-instruction('se:choice'))]" mode="draft"/>
            </fo:table-row>
          </fo:table-body>
        </fo:table>  
      </xsl:if>
      <fo:table xsl:use-attribute-sets="xhtml-table">
        <xsl:call-template name="process-table"/>
      </fo:table>
      <xsl:if test="tfoot[not(self::processing-instruction('se:choice'))]">
        <fo:table xsl:use-attribute-sets="xhtml-table">
          <xsl:apply-templates select="(col|colgroup)[not(self::processing-instruction('se:choice'))]"/>
          <xsl:apply-templates select="tfoot
                                   [not(self::processing-instruction('se:choice'))]"/>
        </fo:table>  
      </xsl:if>
    </fo:block>
  </xsl:template>

  <xsl:template name="process-table">
    <xsl:if test="@border or @frame">
      <xsl:choose>
        <xsl:when test="@border &gt; 0">
          <xsl:attribute name="border-width">
            <xsl:value-of select="@border"/>px</xsl:attribute>
        </xsl:when>
      </xsl:choose>
      <xsl:choose>
        <xsl:when test="@border = '0' or @frame = 'void'">
          <xsl:attribute name="border-width">0px</xsl:attribute>
        </xsl:when>
        <xsl:when test="@frame = 'above'">
          <xsl:attribute name="border-top-color">black</xsl:attribute>
        </xsl:when>
        <xsl:when test="@frame = 'below'">
          <xsl:attribute name="border-bottom-color">black</xsl:attribute>
        </xsl:when>
        <xsl:when test="@frame = 'hsides'">
          <xsl:attribute name="border-top-color">black</xsl:attribute>
          <xsl:attribute name="border-bottom-color">black</xsl:attribute>        
        </xsl:when>
        <xsl:when test="@frame = 'vsides'">
          <xsl:attribute name="border-left-color">black</xsl:attribute>
          <xsl:attribute name="border-right-color">black</xsl:attribute>
        </xsl:when>
        <xsl:when test="@frame = 'lhs'">
          <xsl:attribute name="border-left-color">black</xsl:attribute>
        </xsl:when>
        <xsl:when test="@frame = 'rhs'">
          <xsl:attribute name="border-right-color">black</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="border-color">black</xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>
    <xsl:if test="@rules and (@rules = 'groups' or
                      @rules = 'rows' or
                      @rules = 'cols' or
                      @rules = 'all' and (not(@border or @frame) or 
                          @border = '0' or @frame and
                          not(@frame = 'box' or @frame = 'border')))">
      <xsl:attribute name="border-collapse">collapse</xsl:attribute>
      <xsl:if test="not(@border or @frame)">
        <xsl:attribute name="border-width">0px</xsl:attribute>
      </xsl:if>
    </xsl:if>
    <xsl:call-template name="process-common-attributes"/>
    <xsl:if test="(col|colgroup)[not(self::processing-instruction('se:choice'))]">
      <xsl:apply-templates select="(col|colgroup)[not(self::processing-instruction('se:choice'))]"/>
    </xsl:if>
    <xsl:if test="thead">
      <xsl:apply-templates select="thead"/>
    </xsl:if>
    <xsl:choose>
      <xsl:when test="tbody[not(self::processing-instruction('se:choice'))]">
        <xsl:apply-templates select="tbody"/>
      </xsl:when>
      <xsl:when test="tr[not(self::processing-instruction('se:choice'))]">
        <fo:table-body xsl:use-attribute-sets="xhtml-tbody">
          <xsl:apply-templates select="tr"/>
        </fo:table-body>
      </xsl:when>
    </xsl:choose>
    <!--xsl:if test="tfoot[not(self::processing-instruction('se:choice'))]">
      <xsl:apply-templates select="tfoot"/>
    </xsl:if-->
  </xsl:template>
  
  <xsl:template match="caption|CAPTION">
      <fo:block>
        <xsl:apply-templates/>
      </fo:block>
  </xsl:template>
  
  <xsl:template match="thead|THEAD">
    <fo:table-header xsl:use-attribute-sets="xhtml-thead">
      <xsl:call-template name="process-table-rowgroup"/>
    </fo:table-header>
  </xsl:template>
  
  <xsl:template match="tfoot|TFOOT">
    <fo:table-body xsl:use-attribute-sets="xhtml-tfoot">
      <xsl:call-template name="process-table-rowgroup"/>
    </fo:table-body>
  </xsl:template>
  
  <xsl:template match="tbody|TBODY">
    <fo:table-body xsl:use-attribute-sets="xhtml-tbody">
      <xsl:call-template name="process-table-rowgroup"/>
    </fo:table-body>
  </xsl:template>
  
  <xsl:template name="process-table-rowgroup">
    <xsl:if test="ancestor::table[1]/@rules = 'groups'">
      <xsl:attribute name="border-width">1px</xsl:attribute>
      <xsl:attribute name="border-color">#000000</xsl:attribute>
    </xsl:if>
    <xsl:call-template name="process-common-attributes-and-children"/>
  </xsl:template>

  <xsl:template match="col|COL" mode="draft">
    <fo:table-cell xsl:use-attribute-sets="xhtml-td" font-family="{$body.font.family}"
                   color="green" background-color="#e6e6e6">
      <xsl:text>&#x25CF;</xsl:text>
      <xsl:if test="@span &gt; 1">
        <fo:inline font-size="70%" baseline-shift="super">
          <xsl:value-of select="@span"/>
        </fo:inline>
      </xsl:if>
    </fo:table-cell>
    <xsl:if test="@span &gt; 1">
      <xsl:call-template name="process-draft-span">
        <xsl:with-param name="n" select="number(@span)-1"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:template>
  <xsl:template name="process-draft-span">  
    <xsl:param name="n"/>    
    <fo:table-cell xsl:use-attribute-sets="xhtml-td"  font-family="{$body.font.family}"
                   color="green" background-color="#e6e6e6">
      <xsl:choose>
        <xsl:when test="self::col">
          <xsl:text>&#x25CF;</xsl:text>
        </xsl:when>
        <xsl:when test="self::colgroup">
          <xsl:text>{}</xsl:text>
        </xsl:when>
      </xsl:choose>
      <fo:inline font-size="70%" baseline-shift="super">
        <xsl:value-of select="$n"/>
      </fo:inline>
    </fo:table-cell>
    <xsl:if test="$n &gt; 1">
      <xsl:call-template name="process-draft-span">
        <xsl:with-param name="n" select="number($n)-1"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:template>

  <xsl:template match="colgroup|COLGROUP" mode="draft">
    <fo:table-cell 
      xsl:use-attribute-sets="xhtml-td" 
      background-color="#e6e6e6" 
      font-family="{$body.font.family}">
      <xsl:text>{</xsl:text>
      <xsl:for-each select="col">
        <fo:inline color="green">
          <xsl:text>&#x25CF;</xsl:text>
        </fo:inline>
      </xsl:for-each>
      <xsl:text>}</xsl:text>
      <xsl:if test="@span &gt; 1">
        <fo:inline font-size="70%" baseline-shift="super">
          <xsl:value-of select="@span"/>
        </fo:inline>
      </xsl:if>
    </fo:table-cell>
    <xsl:if test="@span &gt; 1">
      <xsl:call-template name="process-draft-span">
        <xsl:with-param name="n" select="number(@span)-1"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:template>

  
  <xsl:template match="colgroup|COLGROUP">
    <fo:table-column xsl:use-attribute-sets="xhtml-table-column">
      <xsl:call-template name="process-table-column"/>
    </fo:table-column>
  </xsl:template>
  
  <xsl:template match="colgroup[col]|COLGROUP[col]">
    <xsl:for-each select="col">
      <fo:table-column xsl:use-attribute-sets="xhtml-table-column">
        <xsl:call-template name="process-table-column"/>
      </fo:table-column>
    </xsl:for-each>
  </xsl:template>
  
  <xsl:template match="col|COL">
    <fo:table-column xsl:use-attribute-sets="xhtml-table-column">
      <xsl:call-template name="process-table-column"/>
    </fo:table-column>
  </xsl:template>
  
  <xsl:template name="process-table-column">
    <xsl:if test="parent::colgroup">
      <xsl:call-template name="process-col-width">
        <xsl:with-param name="width" select="../@width"/>
      </xsl:call-template>
      <xsl:call-template name="process-cell-align">
        <xsl:with-param name="align" select="../@align"/>
      </xsl:call-template>
      <xsl:call-template name="process-cell-valign">
        <xsl:with-param name="valign" select="../@valign"/>
      </xsl:call-template>
    </xsl:if>
    <xsl:if test="@span">
      <xsl:attribute name="number-columns-repeated">
        <xsl:value-of select="@span"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:call-template name="process-col-width">
      <xsl:with-param name="width" select="@width"/>
    </xsl:call-template>
    <xsl:if test="ancestor::table[1]/@rules = 'cols'">
      <xsl:attribute name="border">1px</xsl:attribute>
      <xsl:attribute name="border-color">black</xsl:attribute>
    </xsl:if>
    <xsl:call-template name="process-common-attributes"/>
  </xsl:template>
  
  <xsl:template match="tr|TR">
    <fo:table-row xsl:use-attribute-sets="xhtml-tr">
      <xsl:call-template name="process-table-row"/>
    </fo:table-row>
  </xsl:template>
  
  <xsl:template match="tr[parent::table and th and not(td)]">
    <fo:table-row xsl:use-attribute-sets="xhtml-tr">
      <xsl:call-template name="process-table-row"/>
    </fo:table-row>
  </xsl:template>
  
  <xsl:template name="process-table-row">
    <xsl:choose>
      <xsl:when test="ancestor::table[1]/@rules = 'rows'">
        <xsl:attribute name="border">1px</xsl:attribute>
        <xsl:attribute name="border-color">black</xsl:attribute>
      </xsl:when>
      <xsl:when test="@border &gt; 0">
        <xsl:attribute name="border-width">
          <xsl:value-of select="@border"/>px</xsl:attribute>
        <xsl:attribute name="border-color">black</xsl:attribute>
      </xsl:when>
    </xsl:choose>
    <xsl:call-template name="process-common-attributes-and-children"/>
  </xsl:template>
  
  <xsl:template match="th|TH">
    <fo:table-cell xsl:use-attribute-sets="xhtml-th">
      <xsl:call-template name="process-table-cell"/>
    </fo:table-cell>
  </xsl:template>
  
  <xsl:template match="td|TD">
    <fo:table-cell xsl:use-attribute-sets="xhtml-td">
      <xsl:call-template name="process-table-cell"/>
    </fo:table-cell>
  </xsl:template>
  
  <xsl:template name="process-table-cell">
    <xsl:if test="@colspan">
      <xsl:attribute name="number-columns-spanned">
        <xsl:value-of select="@colspan"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:if test="@rowspan">
      <xsl:attribute name="number-rows-spanned">
        <xsl:value-of select="@rowspan"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:if test="not(ancestor::tfoot[1])">
      <xsl:for-each select="ancestor::table[1]">
        <xsl:if test="(@border or @rules) and (@rules = 'all' or
                      not(@rules) and not(@border = '0'))">
            <xsl:attribute name="border-width">
              <xsl:value-of select="@border"/>px</xsl:attribute>
            <xsl:attribute name="border-color">black</xsl:attribute>
        </xsl:if>
        <xsl:if test="@cellpadding">
          <xsl:attribute name="padding">
            <xsl:choose>
              <xsl:when test="contains(@cellpadding, '%')">
                <xsl:value-of select="@cellpadding"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="@cellpadding"/>px</xsl:otherwise>
            </xsl:choose>
          </xsl:attribute>
        </xsl:if>
      </xsl:for-each>
    </xsl:if>
    <xsl:for-each select="ancestor::tr[1]">
      <xsl:if test="@bgcolor">
        <xsl:attribute name="background-color">
          <xsl:value-of select="@bgcolor"/>
        </xsl:attribute>
      </xsl:if>
    </xsl:for-each>
    <xsl:if test="not(@align or ../@align or
                      ../parent::*[self::thead or self::tfoot or
                      self::tbody]/@align) and
                  ancestor::table[1]/*[self::col or
                      self::colgroup]/descendant-or-self::*/@align">
      <xsl:attribute name="text-align">from-table-column()</xsl:attribute>
    </xsl:if>
    <xsl:call-template name="process-common-attributes"/>
    <fo:block start-indent="3pt">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>
  
  <xsl:template name="process-col-width">
    <xsl:param name="width"/>
    <xsl:if test="$width and $width != '0*'">
      <xsl:attribute name="column-width">
        <xsl:choose>
          <xsl:when test="contains($width, '*')">
            <xsl:text>proportional-column-width(</xsl:text>
            <xsl:value-of select="substring-before($width, '*')"/>
            <xsl:text>)</xsl:text>
          </xsl:when>
          <xsl:when test="contains($width, '%')">
            <xsl:value-of select="$width"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$width"/>px</xsl:otherwise>
        </xsl:choose>
      </xsl:attribute>
    </xsl:if>
  </xsl:template>
  
  <xsl:template name="process-cell-align">
    <xsl:param name="align"/>
    <xsl:if test="$align">
      <xsl:attribute name="text-align">
        <xsl:choose>
          <xsl:when test="$align = 'char'">
            <xsl:choose>
              <xsl:when test="$align/../@char">
                <xsl:value-of select="$align/../@char"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="'.'"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$align"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:attribute>
    </xsl:if>
  </xsl:template>
  
  <xsl:template name="process-cell-valign">
  </xsl:template>
  
</xsl:stylesheet>
