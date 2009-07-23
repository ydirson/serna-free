<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:sf="http://www.syntext.com/Extensions/Functions"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:xse="http://www.syntext.com/Extensions/XSLT-1.0"
                extension-element-prefixes="xse"
                version='1.0'>
<!-- 

     Cals Table XSL Stylesheet for Syntext Serna.
     (c) 2004 by Syntext, Inc.

-->

  <xsl:attribute-set name="formal.title.properties">
    <xsl:attribute name="font-size">
      <xsl:value-of select="$body.font.master * 1.2"/>
      <xsl:text>pt</xsl:text>
    </xsl:attribute>
    <xsl:attribute name="padding-before">
      <xsl:value-of select="$body.font.master * 0.5"/><xsl:text>pt</xsl:text>
    </xsl:attribute>
  </xsl:attribute-set>

  <xsl:attribute-set name="formal.object.properties">
    <xsl:attribute name="padding-before">0em</xsl:attribute>
    <xsl:attribute name="padding-after">0em</xsl:attribute>
  </xsl:attribute-set>

  <xsl:attribute-set name="preamble.attributes">
    <xsl:attribute name="border-style">solid</xsl:attribute>
    <xsl:attribute name="border-top-width">1pt</xsl:attribute>
    <xsl:attribute name="border-left-width">1pt</xsl:attribute>    
    <xsl:attribute name="border-right-width">1pt</xsl:attribute>    
    <xsl:attribute name="border-bottom-width">1pt</xsl:attribute>
    <xsl:attribute name="border-top-color">#000000</xsl:attribute>
    <xsl:attribute name="border-bottom-color">#000000</xsl:attribute>
    <xsl:attribute name="border-left-color">#000000</xsl:attribute>
    <xsl:attribute name="border-right-color">#000000</xsl:attribute>
    <xsl:attribute name="background-color">#e0e0e0</xsl:attribute>
    <xsl:attribute name="font-size">10pt</xsl:attribute>
    <xsl:attribute name="padding-start">
      <xsl:value-of select="'2pc'"/>
    </xsl:attribute>
    <xsl:attribute name="padding-end">
      <xsl:value-of select="'2pc'"/>
    </xsl:attribute>
  </xsl:attribute-set>

  <xsl:attribute-set name="title.content.properties">
    <xsl:attribute name="font-family">
      <xsl:value-of select="$title.font.family"/>
    </xsl:attribute>
    <xsl:attribute name="font-weight">bold</xsl:attribute>
    <xsl:attribute name="text-align">center</xsl:attribute>
    <xsl:attribute name="margin-left">
      <xsl:value-of select="$title.margin.left"/>
    </xsl:attribute>
    <xsl:attribute name="margin-right">
      <xsl:value-of select="$title.margin.left"/>
    </xsl:attribute>
  </xsl:attribute-set>

  <xsl:param name="default.table.width" select="''"/>
  <xsl:param name="table.frame.border.color" select="'#000000'"/>
  <xsl:param name="table.frame.border.style" select="'solid'"/>

<!--Following 2 lines attrs are set to 0px only for workcard,
    which is not according to CALS standart.-->
  <xsl:param name="table.frame.border.thickness" select="'1px'"/>
  <xsl:param name="table.cell.border.thickness" select="'1px'"/>

  <xsl:param name="table.cell.border.color" select="'#000000'"/>
  <xsl:param name="table.cell.border.style" select="'solid'"/>

  <xsl:attribute-set name="table.cell.padding">
    <xsl:attribute name="padding-left">0pt</xsl:attribute>
    <xsl:attribute name="padding-right">0pt</xsl:attribute>
    <xsl:attribute name="padding-top">0pt</xsl:attribute>
    <xsl:attribute name="padding-bottom">0pt</xsl:attribute>
  </xsl:attribute-set>

  <xsl:param name="formal.title.placement">
    figure before
    example before
    equation before
    table before
    procedure before
  </xsl:param>

  <xsl:param name="show.preamble.editing" select="1"/>
  <xsl:param name="title.margin.left" select="'-2pc'"/>

  <xsl:template name="formal.object.heading">
    <xsl:param name="object" select="."/>
    <xsl:param name="placement" select="'before'"/>
    <xsl:if test="$object/title">
      <fo:block xsl:use-attribute-sets="formal.title.properties">
        <xsl:choose>
          <xsl:when test="$placement = 'before'">
            <xsl:attribute
                   name="keep-with-next.within-column">always</xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute
                   name="keep-with-previous.within-column">always</xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:apply-templates select="$object/title" mode="formal.title.mode">
          <xsl:with-param name="key" select="local-name($object)"/>
        </xsl:apply-templates>
      </fo:block>
    </xsl:if>
  </xsl:template>

  <xsl:template match="title" mode="formal.title.mode">
    <xsl:param name="key" select="''"/>
    <fo:block font-size="1em" font-weight="bold">
      <xsl:copy-of select="text()"/>
    </fo:block>
  </xsl:template>

<xsl:template match="table|informaltable">
  <xsl:variable name="param.placement"
                select="substring-after(normalize-space($formal.title.placement),
                        concat(local-name(.), ' '))"/>
  <xsl:variable name="placement">
    <xsl:choose>
      <xsl:when test="contains($param.placement, ' ')">
        <xsl:value-of select="substring-before($param.placement, ' ')"/>
      </xsl:when>
      <xsl:when test="$param.placement = ''">before</xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$param.placement"/>
      </xsl:otherwise>                          
    </xsl:choose>
  </xsl:variable>
  <fo:block>
    <xsl:attribute name="span">
      <xsl:choose>
        <xsl:when test="@pgwide=1">all</xsl:when>
        <xsl:otherwise>none</xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>

    <fo:block xsl:use-attribute-sets="formal.object.properties">
      <xsl:if test="$placement = 'before' and not(self::informaltable)">
        <xsl:call-template name="formal.object.heading">
          <xsl:with-param name="placement" select="$placement"/>
        </xsl:call-template>
      </xsl:if>

      <xsl:apply-templates select="tgroup" mode="main"/>

      <xsl:if test="$placement != 'before' and not(self::informaltable)">
        <xsl:call-template name="formal.object.heading">
          <xsl:with-param name="placement" select="$placement"/>
        </xsl:call-template>
      </xsl:if>
    </fo:block>
  </fo:block>
</xsl:template>

<xsl:template match="tgroup" mode="main">
  <xsl:if test="$show.preamble.editing">
    <fo:block xsl:use-attribute-sets="preamble.attributes">
      <fo:block background-color="transparent"
        xsl:use-attribute-sets="title.content.properties formal.title.properties">
        <xsl:text>Draft area for "</xsl:text>
        <xsl:value-of select="local-name(.)"/>
        <xsl:text>" metainfo.</xsl:text>
        <xsl:if test="@cols"> 
          <xsl:text>Columns:</xsl:text>
          <xsl:value-of select="@cols"/>
        </xsl:if>
        <xsl:text>. (edit attributes of corresponding elements)</xsl:text>
      </fo:block>
      <xsl:apply-templates select="colspec[not(self::processing-instruction('se:choice'))]|
                                  spanspec[not(self::processing-instruction('se:choice'))]"/>
    </fo:block>
  </xsl:if>
  <fo:table border-collapse="collapse">
    <xsl:call-template name="table.frame"/>
    <xsl:if test="following-sibling::tgroup">
      <xsl:attribute name="border-bottom-width">0pt</xsl:attribute>
      <xsl:attribute name="border-bottom-style">none</xsl:attribute>
      <xsl:attribute name="padding-bottom">0pt</xsl:attribute>
      <xsl:attribute name="margin-bottom">0pt</xsl:attribute>
      <xsl:attribute name="space-after">0pt</xsl:attribute>
      <xsl:attribute name="space-after.minimum">0pt</xsl:attribute>
      <xsl:attribute name="space-after.optimum">0pt</xsl:attribute>
      <xsl:attribute name="space-after.maximum">0pt</xsl:attribute>
    </xsl:if>
    <xsl:if test="preceding-sibling::tgroup">
      <xsl:attribute name="border-top-width">0pt</xsl:attribute>
      <xsl:attribute name="border-top-style">none</xsl:attribute>
      <xsl:attribute name="padding-top">0pt</xsl:attribute>
      <xsl:attribute name="margin-top">0pt</xsl:attribute>
      <xsl:attribute name="space-before">0pt</xsl:attribute>
      <xsl:attribute name="space-before.minimum">0pt</xsl:attribute>
      <xsl:attribute name="space-before.optimum">0pt</xsl:attribute>
      <xsl:attribute name="space-before.maximum">0pt</xsl:attribute>
    </xsl:if>
    <xsl:if test="(colspec|thead/colspec|tfoot/colspec|tbody/colspec)[contains(@colwidth, '*')]">
      <xsl:attribute name="table-layout">fixed</xsl:attribute>
    </xsl:if>
    <xse:cals-table-group>
        <xsl:apply-templates select="."/>
    </xse:cals-table-group>
  </fo:table>
</xsl:template>

<xsl:template match="colspec|spanspec">
  <fo:block white-space-treatment='preserve' white-space-collapse='false'>
    <xsl:value-of select="concat(translate(local-name(.), 
        'colspean', 'COLSPEAN'), ': ')"/>
      <fo:inline font-style="italic">
        <xsl:for-each select="@*">
            <xsl:value-of select="concat(local-name(.), '=', ., '   ')"/>
        </xsl:for-each>
      </fo:inline>
  </fo:block>
</xsl:template>

<xsl:template name="table.frame">
  <xsl:variable name="frame">
    <xsl:choose>
      <xsl:when test="../@frame">
        <xsl:value-of select="../@frame"/>
      </xsl:when>
      <xsl:otherwise>all</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="$frame='all'">
      <xsl:attribute name="border-left-style">
        <xsl:value-of select="$table.frame.border.style"/>
      </xsl:attribute>
      <xsl:attribute name="border-right-style">
        <xsl:value-of select="$table.frame.border.style"/>
      </xsl:attribute>
      <xsl:attribute name="border-top-style">
        <xsl:value-of select="$table.frame.border.style"/>
      </xsl:attribute>
      <xsl:attribute name="border-bottom-style">
        <xsl:value-of select="$table.frame.border.style"/>
      </xsl:attribute>
      <xsl:attribute name="border-left-width">
        <xsl:value-of select="$table.frame.border.thickness"/>
      </xsl:attribute>
      <xsl:attribute name="border-right-width">
        <xsl:value-of select="$table.frame.border.thickness"/>
      </xsl:attribute>
      <xsl:attribute name="border-top-width">
        <xsl:value-of select="$table.frame.border.thickness"/>
      </xsl:attribute>
      <xsl:attribute name="border-bottom-width">
        <xsl:value-of select="$table.frame.border.thickness"/>
      </xsl:attribute>
      <xsl:attribute name="border-left-color">
        <xsl:value-of select="$table.frame.border.color"/>
      </xsl:attribute>
      <xsl:attribute name="border-right-color">
        <xsl:value-of select="$table.frame.border.color"/>
      </xsl:attribute>
      <xsl:attribute name="border-top-color">
        <xsl:value-of select="$table.frame.border.color"/>
      </xsl:attribute>
      <xsl:attribute name="border-bottom-color">
        <xsl:value-of select="$table.frame.border.color"/>
      </xsl:attribute>
    </xsl:when>
    <xsl:when test="$frame='bottom'">
      <xsl:attribute name="border-left-style">none</xsl:attribute>
      <xsl:attribute name="border-right-style">none</xsl:attribute>
      <xsl:attribute name="border-top-style">none</xsl:attribute>
      <xsl:attribute name="border-bottom-style">
        <xsl:value-of select="$table.frame.border.style"/>
      </xsl:attribute>
      <xsl:attribute name="border-bottom-width">
        <xsl:value-of select="$table.frame.border.thickness"/>
      </xsl:attribute>
      <xsl:attribute name="border-bottom-color">
        <xsl:value-of select="$table.frame.border.color"/>
      </xsl:attribute>
    </xsl:when>
    <xsl:when test="$frame='sides'">
      <xsl:attribute name="border-left-style">
        <xsl:value-of select="$table.frame.border.style"/>
      </xsl:attribute>
      <xsl:attribute name="border-right-style">
        <xsl:value-of select="$table.frame.border.style"/>
      </xsl:attribute>
      <xsl:attribute name="border-top-style">none</xsl:attribute>
      <xsl:attribute name="border-bottom-style">none</xsl:attribute>
      <xsl:attribute name="border-left-width">
        <xsl:value-of select="$table.frame.border.thickness"/>
      </xsl:attribute>
      <xsl:attribute name="border-right-width">
        <xsl:value-of select="$table.frame.border.thickness"/>
      </xsl:attribute>
      <xsl:attribute name="border-left-color">
        <xsl:value-of select="$table.frame.border.color"/>
      </xsl:attribute>
      <xsl:attribute name="border-right-color">
        <xsl:value-of select="$table.frame.border.color"/>
      </xsl:attribute>
    </xsl:when>
    <xsl:when test="$frame='top'">
      <xsl:attribute name="border-left-style">none</xsl:attribute>
      <xsl:attribute name="border-right-style">none</xsl:attribute>
      <xsl:attribute name="border-top-style">
        <xsl:value-of select="$table.frame.border.style"/>
      </xsl:attribute>
      <xsl:attribute name="border-bottom-style">none</xsl:attribute>
      <xsl:attribute name="border-top-width">
        <xsl:value-of select="$table.frame.border.thickness"/>
      </xsl:attribute>
      <xsl:attribute name="border-top-color">
        <xsl:value-of select="$table.frame.border.color"/>
      </xsl:attribute>
    </xsl:when>
    <xsl:when test="$frame='topbot'">
      <xsl:attribute name="border-left-style">none</xsl:attribute>
      <xsl:attribute name="border-right-style">none</xsl:attribute>
      <xsl:attribute name="border-top-style">
        <xsl:value-of select="$table.frame.border.style"/>
      </xsl:attribute>
      <xsl:attribute name="border-bottom-style">
        <xsl:value-of select="$table.frame.border.style"/>
      </xsl:attribute>
      <xsl:attribute name="border-top-width">
        <xsl:value-of select="$table.frame.border.thickness"/>
      </xsl:attribute>
      <xsl:attribute name="border-bottom-width">
        <xsl:value-of select="$table.frame.border.thickness"/>
      </xsl:attribute>
      <xsl:attribute name="border-top-color">
        <xsl:value-of select="$table.frame.border.color"/>
      </xsl:attribute>
      <xsl:attribute name="border-bottom-color">
        <xsl:value-of select="$table.frame.border.color"/>
      </xsl:attribute>
    </xsl:when>
    <xsl:when test="$frame='none'">
      <xsl:attribute name="border-left-style">none</xsl:attribute>
      <xsl:attribute name="border-right-style">none</xsl:attribute>
      <xsl:attribute name="border-top-style">none</xsl:attribute>
      <xsl:attribute name="border-bottom-style">none</xsl:attribute>
    </xsl:when>
    <xsl:otherwise>
      <xsl:attribute name="border-left-style">none</xsl:attribute>
      <xsl:attribute name="border-right-style">none</xsl:attribute>
      <xsl:attribute name="border-top-style">none</xsl:attribute>
      <xsl:attribute name="border-bottom-style">none</xsl:attribute>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template name="border">
  <xsl:param name="side" select="'left'"/>

  <xsl:attribute name="border-{$side}-width">
    <xsl:value-of select="$table.cell.border.thickness"/>
  </xsl:attribute>
  <xsl:attribute name="border-{$side}-style">
    <xsl:value-of select="$table.cell.border.style"/>
  </xsl:attribute>
  <xsl:attribute name="border-{$side}-color">
    <xsl:value-of select="$table.cell.border.color"/>
  </xsl:attribute>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="tgroup" name="tgroup">
  <xsl:variable name="cols">
    <xsl:variable name="ncols" select='number(@cols)'/>
    <xsl:choose>
        <xsl:when test="$ncols = 'NaN' or (floor($ncols) - $ncols != 0)
            or $ncols &lt; 1 or $ncols &gt; 100">
            <!--xsl:message>Bad COLS attribute value</xsl:message -->
            <xsl:text>1</xsl:text>
        </xsl:when>
        <xsl:otherwise>
            <xsl:value-of select="$ncols"/>
        </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
          
  <xsl:if test="position() = 1">
    <!-- If this is the first tgroup, output the width attribute for the -->
    <!-- surrounding fo:table. (If this isn't the first tgroup, trying   -->
    <!-- to output the attribute will cause an error.)                   -->
    <xsl:attribute name="width">
      <xsl:choose>
        <xsl:when test="$default.table.width = ''">
          <xsl:text>100%</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$default.table.width"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
  </xsl:if>

  <xsl:call-template name="generate.colgroup">
    <xsl:with-param name="cols" select="$cols"/>
  </xsl:call-template>
  
  <xsl:apply-templates select="thead|tbody|tfoot"/>
</xsl:template>

<xsl:template match="thead">
  <fo:table-header start-indent="0pt">
    <xsl:apply-templates select="row"/>
  </fo:table-header>
</xsl:template>

<xsl:template match="tfoot">
  <fo:table-footer start-indent="0pt">
    <xsl:apply-templates select="row"/>
  </fo:table-footer>
</xsl:template>

<xsl:template match="tbody">
  <fo:table-body start-indent="0pt">
    <xsl:apply-templates select="row"/>
  </fo:table-body>
</xsl:template>

<xsl:template match="row">
  <!-- Build current row with the incoming mnemonic row in "span" -->
  <fo:table-row>  
      <xse:cals-table-row>
        <xsl:choose>
          <xsl:when test="entry|entrytbl">
            <xsl:apply-templates select="entry|entrytbl"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:call-template name="entry"/>
          </xsl:otherwise>
        </xsl:choose>
      </xse:cals-table-row> 
  </fo:table-row>
</xsl:template>

<xsl:template match="entry|entrytbl" name="entry">
  <xse:cals-table-cell>
    <xsl:variable name="rowsep" select="xse:cals-attribute('rowsep', '1')"/>
    <xsl:variable name="colsep" select="xse:cals-attribute('colsep', '1')"/>
    <xsl:variable name="valign" select="xse:cals-attribute('valign', '')"/>
    <xsl:variable name="align"  select="xse:cals-attribute('align', '')"/>
    <xsl:variable name="char"   select="xse:cals-attribute('char', '')"/>
    <xsl:variable name="colspan" select="xse:cals-attribute('cals:colspan')"/>
    <xsl:variable name="cols">
      <xsl:variable name="ncols" select='number(ancestor::tgroup/@cols)'/>
      <xsl:choose>
          <xsl:when test="$ncols = 'NaN' or (floor($ncols) - $ncols != 0)
              or $ncols &lt; 1 or $ncols &gt; 100">
              <xsl:text>1</xsl:text>
          </xsl:when>
          <xsl:otherwise>
              <xsl:value-of select="$ncols"/>
          </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
  
    <fo:table-cell xsl:use-attribute-sets="table.cell.padding">
      <xsl:if test="$rowsep &gt; 0">
        <xsl:call-template name="border">
          <xsl:with-param name="side" select="'bottom'"/>
        </xsl:call-template>
      </xsl:if>

      <xsl:if test="$colsep &gt; 0 and
              xse:cals-attribute('cals:colnum') &lt; $cols">
        <xsl:call-template name="border">
          <xsl:with-param name="side" select="'right'"/>
        </xsl:call-template>
      </xsl:if>

      <xsl:if test="$colspan &gt; 1">
        <xsl:attribute name="number-columns-spanned">
            <xsl:value-of select="$colspan"/>
        </xsl:attribute>
      </xsl:if>
    
      <xsl:if test="@morerows">
        <xsl:attribute name="number-rows-spanned">
          <xsl:value-of select="@morerows+1"/>
        </xsl:attribute>
      </xsl:if>

      <xsl:if test="$valign != ''">
        <xsl:attribute name="display-align">
          <xsl:choose>
            <xsl:when test="$valign='top'">before</xsl:when>
            <xsl:when test="$valign='middle'">center</xsl:when>
            <xsl:when test="$valign='bottom'">after</xsl:when>
            <xsl:otherwise>
              <xsl:text>center</xsl:text>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:attribute>
      </xsl:if>

      <xsl:if test="$align != ''">
        <xsl:attribute name="text-align">
          <xsl:value-of select="$align"/>
        </xsl:attribute>
      </xsl:if>

      <xsl:if test="$char != ''">
        <xsl:attribute name="text-align">
          <xsl:value-of select="$char"/>
        </xsl:attribute>
      </xsl:if>

      <fo:block>
        <!-- highlight this entry? -->
        <xsl:if test="ancestor::thead">
          <xsl:attribute name="font-weight">bold</xsl:attribute>
        </xsl:if>

        <!-- are we missing any indexterms? -->
        <xsl:if test="not(preceding-sibling::entry)
                      and not(parent::row/preceding-sibling::row)">
          <!-- this is the first entry of the first row -->
          <xsl:if test="ancestor::thead or
                        (ancestor::tbody
                         and not(ancestor::tbody/preceding-sibling::thead
                                 or ancestor::tbody/preceding-sibling::tbody))">
            <!-- of the thead or the first tbody -->
            <xsl:apply-templates select="ancestor::tgroup/preceding-sibling::indexterm"/>
          </xsl:if>
        </xsl:if>

        <xsl:choose>
          <!-- Generate whitespace if no children -->
          <xsl:when test="not(node())">
            <xsl:text>&#160;</xsl:text>
          </xsl:when>

          <!-- Generate table if it is entrytbl -->
          <xsl:when test="self::entrytbl">
            <fo:table border-collapse="collapse">
              <xsl:if test="(colspec|thead/colspec|tbody/colspec)[contains(@colwidth, '*')]">
                <xsl:attribute name="table-layout">fixed</xsl:attribute>
              </xsl:if>
              <xsl:call-template name="tgroup"/>
            </fo:table>
          </xsl:when>

          <!-- Otherwise build the content -->
          <xsl:otherwise>
            <xsl:choose>
              <xsl:when test="not(parent::row/preceding-sibling::row) and text">
                <xsl:apply-templates mode="table-head"/>
              </xsl:when>
              <xsl:otherwise><xsl:apply-templates mode="table"/></xsl:otherwise>
            </xsl:choose>
          </xsl:otherwise>
        </xsl:choose>
      </fo:block>
    </fo:table-cell>
  </xse:cals-table-cell>
</xsl:template>

<xsl:template name="generate.colgroup">
  <xsl:param name="cols" select="1"/>
  <xsl:param name="count" select="1"/>

  <xsl:choose>
    <xsl:when test="$count>$cols"></xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="generate.col">
        <xsl:with-param name="countcol" select="$count"/>
        <xsl:with-param name="colspecs" select="colspec"/>
      </xsl:call-template>
      <xsl:call-template name="generate.colgroup">
        <xsl:with-param name="cols" select="$cols"/>
        <xsl:with-param name="count" select="$count+1"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="generate.col">
  <!-- generate the table-column for column countcol -->
  <xsl:param name="countcol">1</xsl:param>
  <xsl:param name="colspecs"/>
  <xsl:param name="count">1</xsl:param>
  <xsl:param name="colnum">1</xsl:param>

  <xsl:choose>
    <xsl:when test="$count>count($colspecs)">
      <fo:table-column column-number="{$countcol}">
        <xsl:variable name="colwidth">
          <xsl:call-template name="calc.column.width"/>
        </xsl:variable>
        <xsl:if test="$colwidth != 'proportional-column-width(1)'">
          <xsl:attribute name="column-width">
            <xsl:value-of select="$colwidth"/>
          </xsl:attribute>
        </xsl:if>
      </fo:table-column>
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="colspec" select="$colspecs[$count=position()]"/>

      <xsl:variable name="colspec.colnum">
        <xsl:choose>
          <xsl:when test="$colspec/@colnum">
            <xsl:value-of select="$colspec/@colnum"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$colnum"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:variable>

      <xsl:variable name="colspec.colwidth">
        <xsl:choose>
          <xsl:when test="$colspec/@colwidth">
            <xsl:value-of select="$colspec/@colwidth"/>
          </xsl:when>
          <xsl:otherwise>1*</xsl:otherwise>
        </xsl:choose>
      </xsl:variable>

      <xsl:choose>
        <xsl:when test="$colspec.colnum=$countcol">
          <fo:table-column column-number="{$countcol}">
            <xsl:variable name="colwidth">
              <xsl:call-template name="calc.column.width">
                <xsl:with-param name="colwidth">
                  <xsl:value-of select="$colspec.colwidth"/>
                </xsl:with-param>
              </xsl:call-template>
            </xsl:variable>
            <xsl:if test="$colwidth != 'proportional-column-width(1)'">
              <xsl:attribute name="column-width">
                <xsl:value-of select="$colwidth"/>
              </xsl:attribute>
            </xsl:if>
          </fo:table-column>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="generate.col">
            <xsl:with-param name="countcol" select="$countcol"/>
            <xsl:with-param name="colspecs" select="$colspecs"/>
            <xsl:with-param name="count" select="$count+1"/>
            <xsl:with-param name="colnum">
              <xsl:choose>
                <xsl:when test="$colspec/@colnum">
                  <xsl:value-of select="$colspec/@colnum + 1"/>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:value-of select="$colnum + 1"/>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:with-param>
           </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="calc.column.width">
  <xsl:param name="colwidth">1*</xsl:param>

  <xsl:if test="contains($colwidth, '*')">
    <xsl:text>proportional-column-width(</xsl:text>
    <xsl:choose>
      <xsl:when test="'*' = $colwidth">1</xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="substring-before($colwidth, '*')"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>)</xsl:text>
  </xsl:if>

  <xsl:variable name="width-units">
    <xsl:choose>
      <xsl:when test="contains($colwidth, '*')">
        <xsl:value-of
             select="normalize-space(substring-after($colwidth, '*'))"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="normalize-space($colwidth)"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="width"
       select="normalize-space(translate($width-units,
                                         '+-0123456789.abcdefghijklmnopqrstuvwxyz',
                                         '+-0123456789.'))"/>

  <xsl:variable name="units"
       select="normalize-space(translate($width-units,
                                         'abcdefghijklmnopqrstuvwxyz+-0123456789.',
                                         'abcdefghijklmnopqrstuvwxyz'))"/>

  <xsl:value-of select="$width"/>

  <xsl:choose>
    <xsl:when test="$units = 'pi'">pc</xsl:when>
    <xsl:when test="$units = '' and $width != ''">pt</xsl:when>
    <xsl:otherwise><xsl:value-of select="$units"/></xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="cals-table-empty-cell">
  <xsl:variable name="rowsep" select="xse:cals-attribute('rowsep', '1')"/>
  <xsl:variable name="colsep" select="xse:cals-attribute('colsep', '1')"/>
  <xsl:variable name="colnum" select="xse:cals-attribute('cals:colnum')"/>
  
  <fo:table-cell text-align="center"
                 display-align="center"
                 xsl:use-attribute-sets="table.cell.padding">
    <xsl:if test="$rowsep &gt; 0">
      <xsl:call-template name="border">
        <xsl:with-param name="side" select="'bottom'"/>
      </xsl:call-template>
    </xsl:if>

    <xsl:if test="$colsep &gt; 0 and $colnum &lt; ancestor::tgroup/@cols">
      <xsl:call-template name="border">
        <xsl:with-param name="side" select="'right'"/>
      </xsl:call-template>
    </xsl:if>
    <!-- fo:table-cell should not be empty -->
    <fo:block>
        <xsl:text> </xsl:text>
    </fo:block>
  </fo:table-cell>
</xsl:template>

<xsl:template match="*" mode="table">
  <xsl:choose>
    <xsl:when test="self::text and para">
      <fo:block>
        <xsl:apply-templates mode="table"/>
      </fo:block>
    </xsl:when>
    <xsl:otherwise><xsl:apply-templates select="."/></xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="text" mode="table-head">
  <fo:block>
    <xsl:choose>
      <xsl:when test="para">
        <xsl:apply-templates mode="table"/>
      </xsl:when>
      <xsl:otherwise><xsl:apply-templates/></xsl:otherwise>
    </xsl:choose>
  </fo:block>
</xsl:template>

<xsl:template match="para" mode="table">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

</xsl:stylesheet>
