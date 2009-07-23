<?xml version='1.0'?>

<!-- DITA CALS table stylesheet. -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:xse="http://www.syntext.com/Extensions/XSLT-1.0"
                extension-element-prefixes="xse dtm"
                xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
                version='1.0'>

<!-- real parameters follow -->

<xsl:param name="default.table.width" select="'100%'"/>
<xsl:param name="table.frame.border.color" select="'#000000'"/>
<xsl:param name="table.frame.border.style" select="'solid'"/>
<xsl:param name="table.frame.border.thickness" select="'1px'"/>
<xsl:param name="table.cell.border.color" select="'#000000'"/>
<xsl:param name="table.cell.border.style" select="'solid'"/>
<xsl:param name="table.cell.border.thickness" select="'1px'"/>
<xsl:param name="table.thead.cell.color" select="'silver'"/>

<xsl:attribute-set name="table.cell.attributes">                
  <xsl:attribute name="padding-left">2pt</xsl:attribute>
  <xsl:attribute name="padding-right">2pt</xsl:attribute>
  <xsl:attribute name="padding-top">2pt</xsl:attribute>
  <xsl:attribute name="padding-bottom">2pt</xsl:attribute>
  <xsl:attribute name="start-indent">0</xsl:attribute>
</xsl:attribute-set>

<xsl:param name="show.preamble.editing" select="0"/>

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
  <xsl:attribute name="font-size">12pt</xsl:attribute>
</xsl:attribute-set>

<dtm:doc dtm:elements="table" dtm:status="finished" dtm:idref="topic.table"/>
<xsl:template match="*[contains(@class,' topic/table ')]" dtm:id="topic.table">
  <fo:block padding-bottom="1em">
    <xsl:attribute name="span">
      <xsl:choose>
        <xsl:when test="@pgwide=1">all</xsl:when>
        <xsl:otherwise>none</xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="tgroup" dtm:status="finished" dtm:idref="topic.tgroup"/>
<xsl:template match="*[contains(@class, ' topic/tgroup ')]" dtm:id="topic.tgroup">
  <xsl:if test="$show.preamble.editing">
    <fo:block xsl:use-attribute-sets="preamble.attributes">
      <xsl:apply-templates select="*[contains(@class, ' topic/colspec ')]"
                           mode="dita.calstable.colspec"/>
    </fo:block>
  </xsl:if>
  <fo:table border-collapse="collapse">
    <xsl:call-template name="dita.calstable.frame"/>
    <xsl:if test="following-sibling::*[contains(@class, ' topic/tgroup ')]">
      <xsl:attribute name="border-bottom-width">0pt</xsl:attribute>
      <xsl:attribute name="border-bottom-style">none</xsl:attribute>
      <xsl:attribute name="padding-bottom">0pt</xsl:attribute>
      <xsl:attribute name="margin-bottom">0pt</xsl:attribute>
      <xsl:attribute name="space-after">0pt</xsl:attribute>
      <xsl:attribute name="space-after.minimum">0pt</xsl:attribute>
      <xsl:attribute name="space-after.optimum">0pt</xsl:attribute>
      <xsl:attribute name="space-after.maximum">0pt</xsl:attribute>
    </xsl:if>
    <xsl:if test="preceding-sibling::*[contains(@class, ' topic/tgroup ')]">
      <xsl:attribute name="border-top-width">0pt</xsl:attribute>
      <xsl:attribute name="border-top-style">none</xsl:attribute>
      <xsl:attribute name="padding-top">0pt</xsl:attribute>
      <xsl:attribute name="margin-top">0pt</xsl:attribute>
      <xsl:attribute name="space-before">0pt</xsl:attribute>
      <xsl:attribute name="space-before.minimum">0pt</xsl:attribute>
      <xsl:attribute name="space-before.optimum">0pt</xsl:attribute>
      <xsl:attribute name="space-before.maximum">0pt</xsl:attribute>
    </xsl:if>
    <xsl:if test="*[contains(@colwidth, '*')]">
      <xsl:attribute name="table-layout">fixed</xsl:attribute>
    </xsl:if>
    <xse:cals-table-group>
      <xsl:call-template name="dita.calstable.tgroup"/>
    </xse:cals-table-group>
  </fo:table>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="all.calstable.colspec"/>
<xsl:template match="*" mode="dita.calstable.colspec" dtm:id="all.calstable.colspec">
  <fo:block white-space-treatment='preserve' white-space-collapse='false'>
    <xsl:text>COLSPEC: </xsl:text>
    <fo:inline font-style="italic">
      <xsl:for-each select="@*">
        <xsl:value-of select="concat(local-name(.), '=', ., '   ')"/>
      </xsl:for-each>
    </fo:inline>
  </fo:block>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="calstable.frame"/>
<xsl:template name="dita.calstable.frame" dtm:id="calstable.frame">
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

<!-- Note: this template name is predefined, and used by XSLT CALS ext. -->
<dtm:doc dtm:status="testing" dtm:idref="calstable.cell.empty"/>
<xsl:template name="cals-table-empty-cell" dtm:id="calstable.cell.empty">
  <xsl:variable name="rowsep" select="xse:cals-attribute('rowsep', '1')"/>
  <xsl:variable name="colsep" select="xse:cals-attribute('colsep', '1')"/>
  <xsl:variable name="cols"   select="xse:cals-attribute('cols',   '1')"/>
  <xsl:variable name="colnum" select="xse:cals-attribute('cals:colnum')"/>
  
  <fo:table-cell text-align="center" display-align="center"
                 xsl:use-attribute-sets="table.cell.attributes">
      <xsl:if test="parent::*[contains(@class, ' topic/thead ')
                    or contains(@class, ' topic/tfoot ')]">
          <xsl:attribute name="background-color">
            <xsl:value-of select="$table.thead.cell.color"/>
          </xsl:attribute>
      </xsl:if>
    <xsl:if test="$rowsep &gt; 0">
      <xsl:call-template name="dita.calstable.border">
        <xsl:with-param name="side" select="'bottom'"/>
      </xsl:call-template>
    </xsl:if>
    <xsl:if test="$colsep &gt; 0 and $colnum &lt; $cols">
      <xsl:call-template name="dita.calstable.border">
        <xsl:with-param name="side" select="'right'"/>
      </xsl:call-template>
    </xsl:if>
    <!-- fo:table-cell should not be empty -->
    <fo:block>
        <xsl:text> </xsl:text>
    </fo:block>
  </fo:table-cell>
</xsl:template>

<!-- ==================================================================== -->
<dtm:doc dtm:status="testing" dtm:idref="calstable.border"/>
<xsl:template name="dita.calstable.border" dtm:id="calstable.border">
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
<dtm:doc dtm:status="testing" dtm:idref="calstable.tgroup"/>
<xsl:template name="dita.calstable.tgroup" dtm:id="calstable.tgroup">
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
      <xsl:value-of select="$default.table.width"/>
    </xsl:attribute>
  </xsl:if>

  <xsl:call-template name="dita.calstable.generate.colgroup">
    <xsl:with-param name="cols" select="$cols"/>
  </xsl:call-template>
  
  <xsl:apply-templates/>

</xsl:template>

<dtm:doc dtm:elements="thead" dtm:status="finished" dtm:idref="topic.thead"/>
<xsl:template match="*[contains(@class, ' topic/thead ')]" dtm:id="topic.thead">
  <fo:table-header>
    <xsl:apply-templates/>
  </fo:table-header>
</xsl:template>

<dtm:doc dtm:elements="tfoot" dtm:status="testing" dtm:idref="topic.tfoot"/>
<xsl:template match="*[contains(@class, ' topic/tfoot ')]" dtm:id="topic.tfoot">
  <fo:table-footer>
    <xsl:apply-templates/>
  </fo:table-footer>
</xsl:template>

<dtm:doc dtm:elements="tbody" dtm:status="finished" dtm:idref="topic.tbody"/>
<xsl:template match="*[contains(@class, ' topic/tbody ')]" dtm:id="topic.tbody">
  <fo:table-body  start-indent="0pt">
    <xsl:apply-templates/>
  </fo:table-body>
</xsl:template>

<dtm:doc dtm:elements="row" dtm:status="finished" dtm:idref="topic.row"/>
<xsl:template match="*[contains(@class, ' topic/row ')]" dtm:id="topic.row">
  <!-- Build current row with the incoming mnemonic row in "span" -->
  <fo:table-row>
    <xse:cals-table-row>
      <xsl:apply-templates/>
    </xse:cals-table-row> 
  </fo:table-row>
</xsl:template>

<dtm:doc dtm:elements="entry" dtm:status="finished" dtm:idref="topic.entry"/>
<xsl:template match="*[contains(@class, ' topic/entry ')]" dtm:id="topic.entry">
  <xse:cals-table-cell>
    <xsl:variable name="rowsep" select="xse:cals-attribute('rowsep', '1')"/>
    <xsl:variable name="colsep" select="xse:cals-attribute('colsep', '1')"/>
    <xsl:variable name="cols"   select="xse:cals-attribute('cols',   '1')"/>
    <xsl:variable name="valign" select="xse:cals-attribute('valign', '')"/>
    <xsl:variable name="align"  select="xse:cals-attribute('align', '')"/>
    <xsl:variable name="char"   select="xse:cals-attribute('char', '')"/>
    <xsl:variable name="colspan" select="xse:cals-attribute('cals:colspan')"/>
  
    <fo:table-cell xsl:use-attribute-sets="table.cell.attributes">
      <xsl:if test="parent::*/parent::*[contains(@class, ' topic/thead ')
                    or contains(@class, ' topic/tfoot ')]">
          <xsl:attribute name="background-color">
            <xsl:value-of select="$table.thead.cell.color"/>
          </xsl:attribute>
      </xsl:if>
      <xsl:if test="$rowsep &gt; 0">
        <xsl:call-template name="dita.calstable.border">
          <xsl:with-param name="side" select="'bottom'"/>
        </xsl:call-template>
      </xsl:if>

      <xsl:if test="$colsep &gt; 0 and
              xse:cals-attribute('cals:colnum') &lt; $cols">
        <xsl:call-template name="dita.calstable.border">
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
        <xsl:if test="parent::*/parent::*[contains(@class, ' topic/thead' )]">
          <xsl:attribute name="font-weight">bold</xsl:attribute>
        </xsl:if>
        
        <xsl:choose>
          <!-- Generate whitespace if no children -->
          <xsl:when test="not(node())">
            <xsl:text>&#160;</xsl:text>
          </xsl:when>
          <!-- Otherwise build the content -->
          <xsl:otherwise>
            <xsl:apply-templates/>
          </xsl:otherwise>
        </xsl:choose>
      </fo:block>
    </fo:table-cell>
  </xse:cals-table-cell>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="calstable.colgroup.generate"/>
<xsl:template name="dita.calstable.generate.colgroup" dtm:id="calstable.colgroup.generate">
  <xsl:param name="cols" select="1"/>
  <xsl:param name="count" select="1"/>

  <xsl:choose>
    <xsl:when test="$count>$cols"></xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="dita.calstable.generate.col">
        <xsl:with-param name="countcol" select="$count"/>
        <xsl:with-param name="colspecs" select="colspec"/>
      </xsl:call-template>
      <xsl:call-template name="dita.calstable.generate.colgroup">
        <xsl:with-param name="cols" select="$cols"/>
        <xsl:with-param name="count" select="$count+1"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="calstable.col.generate"/>
<xsl:template name="dita.calstable.generate.col" dtm:id="calstable.col.generate">
  <!-- generate the table-column for column countcol -->
  <xsl:param name="countcol">1</xsl:param>
  <xsl:param name="colspecs"/>
  <xsl:param name="count">1</xsl:param>
  <xsl:param name="colnum">1</xsl:param>

  <xsl:choose>
    <xsl:when test="$count>count($colspecs)">
      <fo:table-column column-number="{$countcol}">
        <xsl:variable name="colwidth">
          <xsl:call-template name="dita.calstable.calc.column.width"/>
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
              <xsl:call-template name="dita.calstable.calc.column.width">
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
          <xsl:call-template name="dita.calstable.generate.col">
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

<dtm:doc dtm:status="testing" dtm:idref="calstable.colwidth.calculate"/>
<xsl:template name="dita.calstable.calc.column.width" dtm:id="calstable.colwidth.calculate">
  <xsl:param name="colwidth">1*</xsl:param>

  <xsl:if test="contains($colwidth, '*')">
    <xsl:text>proportional-column-width(</xsl:text>
    <xsl:value-of select="substring-before($colwidth, '*')"/>
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

<dtm:doc dtm:elements="colspec" dtm:status="finished" dtm:idref="topic.colspec"/>
<xsl:template match="*[contains(@class, ' topic/colspec ')]" dtm:id="topic.colspec"/>

</xsl:stylesheet>
