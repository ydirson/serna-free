<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:doc="http://nwalsh.com/xsl/documentation/1.0"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:stbl="http://nwalsh.com/xslt/ext/com.nwalsh.saxon.Table"
                xmlns:xtbl="com.nwalsh.xalan.Table"
                xmlns:lxslt="http://xml.apache.org/xslt"
                exclude-result-prefixes="doc stbl xtbl lxslt"
                version='1.0'>

<!-- changed by wap, Mulberry technologies:

1. added parameter declarations and other parameterizations
2. added necessary pieces from other DocBook stylesheets
   to make a self-contained module

-->

<!-- start CALS/OASIS/DocBook table processing parameters,
     attribute sets and named templates
     (whatever was modified from the distribution; placed
     here for ease of access) -->

<xsl:param name="use.extensions" select="'0'"/>

<xsl:param name="table.border.padding" select="'2pt'"/>

<xsl:param name="table.border.color" select="$specialfacecolor"/>

<xsl:param name="table.border.style" select="'solid'"/>

<xsl:param name="table.border.thickness" select="'thin'"/>

<xsl:param name="table.box.thickness" select="'medium'"/>

<xsl:param name="table.entry.padding" select="'2pt'"/>

<xsl:param name="tablecolumns.extension" select="'1'"/>

<xsl:param name="default.table.width" select="''"/>

<!-- These attribute sets were modified from what's provided
     in DocBook -->
<xsl:attribute-set name="formal.object.properties">
  <xsl:attribute name="space-before.minimum">6pt</xsl:attribute>
  <xsl:attribute name="space-before.optimum">9pt</xsl:attribute>
  <xsl:attribute name="space-before.maximum">12pt</xsl:attribute>
  <xsl:attribute name="space-after.minimum">6pt</xsl:attribute>
  <xsl:attribute name="space-after.optimum">9pt</xsl:attribute>
  <xsl:attribute name="space-after.maximum">12pt</xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="formal.title.properties"
  use-attribute-sets="headercontrol">
  <xsl:attribute name="font-weight">bold</xsl:attribute>
  <xsl:attribute name="font-size">90%</xsl:attribute>
  <xsl:attribute name="hyphenate">false</xsl:attribute>
  <xsl:attribute name="space-after">3pt</xsl:attribute>
  <xsl:attribute name="keep-with-next.within-page">always</xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="table.properties">
  <xsl:attribute name="border-left-width">
    <xsl:value-of select="$table.box.thickness"/>
  </xsl:attribute>
  <xsl:attribute name="border-right-width">
    <xsl:value-of select="$table.box.thickness"/>
  </xsl:attribute>
  <xsl:attribute name="border-top-width">
    <xsl:value-of select="$table.box.thickness"/>
  </xsl:attribute>
  <xsl:attribute name="border-bottom-width">
    <xsl:value-of select="$table.box.thickness"/>
  </xsl:attribute>
  <xsl:attribute name="border-left-color">
    <xsl:value-of select="$table.border.color"/>
  </xsl:attribute>
  <xsl:attribute name="border-right-color">
    <xsl:value-of select="$table.border.color"/>
  </xsl:attribute>
  <xsl:attribute name="border-top-color">
    <xsl:value-of select="$table.border.color"/>
  </xsl:attribute>
  <xsl:attribute name="border-bottom-color">
    <xsl:value-of select="$table.border.color"/>
  </xsl:attribute>
</xsl:attribute-set>


<!-- end CALS/OASIS/DocBook table processing parameters -->


<!-- following parameter settings and named templates
     were copied from Norm's common.xsl, param.xsl,
     fo.xsl and pi.xsl
     (DocBook distribution) for local use -->

<xsl:template name="dbfo-attribute">
  <xsl:param name="pis" select="processing-instruction('dbfo')"/>
  <xsl:param name="attribute">filename</xsl:param>

  <xsl:call-template name="pi-attribute">
    <xsl:with-param name="pis" select="$pis"/>
    <xsl:with-param name="attribute" select="$attribute"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="anchor">
  <xsl:param name="node" select="."/>
  <xsl:param name="conditional" select="1"/>
  <xsl:variable name="id">
    <xsl:call-template name="object.id">
      <xsl:with-param name="object" select="$node"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:if test="$conditional = 0 or $node/@id">
    <xsl:attribute name="id"><xsl:value-of select="$id"/></xsl:attribute>
  </xsl:if>
</xsl:template>

<xsl:template name="pi-attribute">
  <xsl:param name="pis" select="processing-instruction('')"/>
  <xsl:param name="attribute">filename</xsl:param>
  <xsl:param name="count">1</xsl:param>

  <xsl:choose>
    <xsl:when test="$count>count($pis)">
      <!-- not found -->
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="pi">
        <xsl:value-of select="$pis[$count]"/>
      </xsl:variable>
      <xsl:choose>
        <xsl:when test="contains($pi,concat($attribute, '='))">
          <xsl:variable name="rest" select="substring-after($pi,concat($attribute,'='))"/>
          <xsl:variable name="quote" select="substring($rest,1,1)"/>
          <xsl:value-of select="substring-before(substring($rest,2),$quote)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="pi-attribute">
            <xsl:with-param name="pis" select="$pis"/>
            <xsl:with-param name="attribute" select="$attribute"/>
            <xsl:with-param name="count" select="$count + 1"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="object.id">
  <xsl:param name="object" select="."/>
  <xsl:choose>
    <xsl:when test="$object/@id">
      <xsl:value-of select="$object/@id"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="generate-id($object)"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="copy-string">
  <!-- returns 'count' copies of 'string' -->
  <xsl:param name="string"></xsl:param>
  <xsl:param name="count" select="0"/>
  <xsl:param name="result"></xsl:param>

  <xsl:choose>
    <xsl:when test="$count>0">
      <xsl:call-template name="copy-string">
        <xsl:with-param name="string" select="$string"/>
        <xsl:with-param name="count" select="$count - 1"/>
        <xsl:with-param name="result">
          <xsl:value-of select="$result"/>
          <xsl:value-of select="$string"/>
        </xsl:with-param>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$result"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- copied from fo/formal.xsl: -->

<xsl:template match="table">
  <xsl:variable name="id">
    <xsl:call-template name="object.id"/>
  </xsl:variable>
  <xsl:variable name="prop-columns"
    select=".//colspec[contains(@colwidth, '*')]"/>

  <fo:block>
    <xsl:attribute name="span">
      <xsl:choose>
        <xsl:when test="@pgwide=1">all</xsl:when>
        <xsl:otherwise>none</xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
    <xsl:if test="@orient='land'">
      <xsl:attribute name="reference-orientation">90</xsl:attribute>
    </xsl:if>

    <fo:table-and-caption id="{$id}"
      xsl:use-attribute-sets="formal.object.properties"
      >
      <!-- keep-together.within-column="1" -->
      <fo:table-caption>
        <fo:block xsl:use-attribute-sets="formal.title.properties">
          <!-- xsl:apply-templates select="." mode="object.title.markup"/ -->
          <xsl:call-template name="tabletitle"/>
        </fo:block>
      </fo:table-caption>
      <fo:table
        xsl:use-attribute-sets="table.properties">
        <!-- added table.properties to get control over table borders - wap -->
        <xsl:call-template name="table.frame"/>
        <xsl:if test="count($prop-columns) != 0">
          <xsl:attribute name="table-layout">fixed</xsl:attribute>
        </xsl:if>
        <xsl:apply-templates select="tgroup"/>
      </fo:table>
    </fo:table-and-caption>
  </fo:block>
</xsl:template>

<xsl:template match="table/title"></xsl:template>

<xsl:template name="table.frame">
  <xsl:variable name="frame">
    <xsl:choose>
      <xsl:when test="@frame">
        <xsl:value-of select="@frame"/>
      </xsl:when>
      <xsl:otherwise>all</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:choose>
    <xsl:when test="$frame='all'">
      <xsl:attribute name="border-left-style">solid</xsl:attribute>
      <xsl:attribute name="border-right-style">solid</xsl:attribute>
      <xsl:attribute name="border-top-style">solid</xsl:attribute>
      <xsl:attribute name="border-bottom-style">solid</xsl:attribute>
    </xsl:when>
    <xsl:when test="$frame='bottom'">
      <xsl:attribute name="border-left-style">none</xsl:attribute>
      <xsl:attribute name="border-right-style">none</xsl:attribute>
      <xsl:attribute name="border-top-style">none</xsl:attribute>
      <xsl:attribute name="border-bottom-style">solid</xsl:attribute>
    </xsl:when>
    <xsl:when test="$frame='sides'">
      <xsl:attribute name="border-left-style">solid</xsl:attribute>
      <xsl:attribute name="border-right-style">solid</xsl:attribute>
      <xsl:attribute name="border-top-style">none</xsl:attribute>
      <xsl:attribute name="border-bottom-style">none</xsl:attribute>
    </xsl:when>
    <xsl:when test="$frame='top'">
      <xsl:attribute name="border-left-style">none</xsl:attribute>
      <xsl:attribute name="border-right-style">none</xsl:attribute>
      <xsl:attribute name="border-top-style">solid</xsl:attribute>
      <xsl:attribute name="border-bottom-style">none</xsl:attribute>
    </xsl:when>
    <xsl:when test="$frame='topbot'">
      <xsl:attribute name="border-left-style">none</xsl:attribute>
      <xsl:attribute name="border-right-style">none</xsl:attribute>
      <xsl:attribute name="border-top-style">solid</xsl:attribute>
      <xsl:attribute name="border-bottom-style">solid</xsl:attribute>
    </xsl:when>
    <xsl:when test="$frame='none'">
      <xsl:attribute name="border-left-style">none</xsl:attribute>
      <xsl:attribute name="border-right-style">none</xsl:attribute>
      <xsl:attribute name="border-top-style">none</xsl:attribute>
      <xsl:attribute name="border-bottom-style">none</xsl:attribute>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>
        <xsl:text>Impossible frame on table: </xsl:text>
        <xsl:value-of select="$frame"/>
      </xsl:message>
      <xsl:attribute name="border-left-style">none</xsl:attribute>
      <xsl:attribute name="border-right-style">none</xsl:attribute>
      <xsl:attribute name="border-top-style">none</xsl:attribute>
      <xsl:attribute name="border-bottom-style">none</xsl:attribute>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<!-- ********************************************************************
     $Id: table.db-fo.xsl,v 1.1 2003/12/10 17:15:44 ego Exp $
     ********************************************************************

     This file is part of the XSL DocBook Stylesheet distribution.
     See ../README or http://nwalsh.com/docbook/xsl/ for copyright
     and other information.

     ******************************************************************** -->

<doc:reference xmlns="">
<referenceinfo>
<releaseinfo role="meta">
$Id: table.db-fo.xsl,v 1.1 2003/12/10 17:15:44 ego Exp $
</releaseinfo>
<author><surname>Walsh</surname>
<firstname>Norman</firstname></author>
<copyright><year>1999</year><year>2000</year>
<holder>Norman Walsh</holder>
</copyright>
</referenceinfo>
<title>Formatting Object Table Reference</title>

<partintro>
<section><title>Introduction</title>

<para>This is technical reference documentation for the DocBook XSL
Stylesheets; it documents (some of) the parameters, templates, and
other elements of the stylesheets.</para>

<para>This is not intended to be <quote>user</quote> documentation.
It is provided for developers writing customization layers for the
stylesheets, and for anyone who's interested in <quote>how it
works</quote>.</para>

<para>Although I am trying to be thorough, this documentation is known
to be incomplete. Don't forget to read the source, too :-)</para>
</section>
</partintro>
</doc:reference>

<!-- ==================================================================== -->

<lxslt:component prefix="xtbl"
                 functions="adjustColumnWidths"/>

<!-- ==================================================================== -->

<xsl:template name="empty.table.cell">
  <xsl:param name="colnum" select="0"/>

  <xsl:variable name="frame" select="ancestor::tgroup/parent::*/@frame"/>

  <xsl:variable name="rowsep">
    <xsl:call-template name="inherited.table.attribute">
      <xsl:with-param name="entry" select="NOT-AN-ELEMENT-NAME"/>
      <xsl:with-param name="colnum" select="$colnum"/>
      <xsl:with-param name="attribute" select="'rowsep'"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:variable name="colsep">
    <xsl:call-template name="inherited.table.attribute">
      <xsl:with-param name="entry" select="NOT-AN-ELEMENT-NAME"/>
      <xsl:with-param name="colnum" select="$colnum"/>
      <xsl:with-param name="attribute" select="'colsep'"/>
    </xsl:call-template>
  </xsl:variable>

  <fo:table-cell text-align="center"
                 display-align="center"
                 padding="{$table.border.padding}">
    <xsl:choose>
      <xsl:when test="$frame='all'">
        <xsl:call-template name="border">
          <xsl:with-param name="side" select="'right'"/>
          <xsl:with-param name="padding" select="1"/>
        </xsl:call-template>
        <xsl:call-template name="border">
          <xsl:with-param name="side" select="'bottom'"/>
          <xsl:with-param name="padding" select="1"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:if test="$colsep &gt; 0">
          <xsl:call-template name="border">
            <xsl:with-param name="side" select="'right'"/>
            <xsl:with-param name="padding" select="1"/>
          </xsl:call-template>
        </xsl:if>
        <xsl:if test="$rowsep &gt; 0">
          <xsl:call-template name="border">
            <xsl:with-param name="side" select="'bottom'"/>
            <xsl:with-param name="padding" select="1"/>
          </xsl:call-template>
        </xsl:if>
      </xsl:otherwise>
    </xsl:choose>
    <fo:block/> <!-- fo:table-cell should not be empty -->
  </fo:table-cell>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template name="border">
  <xsl:param name="side" select="'left'"/>
  <xsl:param name="padding" select="0"/>

  <xsl:attribute name="border-{$side}-width">
    <xsl:value-of select="$table.border.thickness"/>
  </xsl:attribute>
  <xsl:attribute name="border-{$side}-style">
    <xsl:value-of select="$table.border.style"/>
  </xsl:attribute>
  <xsl:attribute name="border-{$side}-color">
    <xsl:value-of select="$table.border.color"/>
  </xsl:attribute>
  <xsl:if test="$padding != 0">
    <xsl:attribute name="padding-{$side}">
      <xsl:value-of select="$table.border.padding"/>
    </xsl:attribute>
  </xsl:if>
</xsl:template>

<!-- ==================================================================== -->

<xsl:template match="tgroup">
  <xsl:variable name="explicit.table.width">
    <xsl:call-template name="dbfo-attribute">
      <xsl:with-param name="pis"
                      select="../processing-instruction('dbfo')"/>
      <xsl:with-param name="attribute" select="'table-width'"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:variable name="table.width">
    <xsl:choose>
      <xsl:when test="$explicit.table.width != ''">
        <xsl:value-of select="$explicit.table.width"/>
      </xsl:when>
      <xsl:when test="$default.table.width = ''">
        <xsl:text>100%</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$default.table.width"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="colspecs">
    <xsl:choose>
      <xsl:when test="$use.extensions != 0
                      and $tablecolumns.extension != 0">
        <xsl:call-template name="generate.colgroup.raw">
          <xsl:with-param name="cols" select="@cols"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="generate.colgroup">
          <xsl:with-param name="cols" select="@cols"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:attribute name="width">
    <xsl:value-of select="$table.width"/>
  </xsl:attribute>

  <xsl:choose>
    <xsl:when test="$use.extensions != 0
                    and $tablecolumns.extension != 0">
      <xsl:choose>
        <xsl:when test="function-available('stbl:adjustColumnWidths')">
          <xsl:copy-of select="stbl:adjustColumnWidths($colspecs)"/>
        </xsl:when>
        <xsl:when test="function-available('xtbl:adjustColumnWidths')">
          <xsl:copy-of select="xtbl:adjustColumnWidths($colspecs)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:message terminate="yes">
            <xsl:text>No adjustColumnWidths function available.</xsl:text>
          </xsl:message>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:copy-of select="$colspecs"/>
    </xsl:otherwise>
  </xsl:choose>

  <xsl:apply-templates select="thead"/>
  <xsl:apply-templates select="tfoot"/>
  <xsl:apply-templates select="tbody"/>
</xsl:template>

<xsl:template match="colspec"></xsl:template>

<xsl:template match="spanspec"></xsl:template>

<xsl:template match="thead">
  <xsl:variable name="tgroup" select="parent::*"/>

  <fo:table-header
    background-color="{$blockbgcolor}">
    <xsl:apply-templates select="row[1]">
      <xsl:with-param name="spans">
        <xsl:call-template name="blank.spans">
          <xsl:with-param name="cols" select="../@cols"/>
        </xsl:call-template>
      </xsl:with-param>
    </xsl:apply-templates>
  </fo:table-header>
</xsl:template>

<xsl:template match="tfoot">
  <xsl:variable name="tgroup" select="parent::*"/>

  <fo:table-footer>
    <xsl:apply-templates select="row[1]">
      <xsl:with-param name="spans">
        <xsl:call-template name="blank.spans">
          <xsl:with-param name="cols" select="../@cols"/>
        </xsl:call-template>
      </xsl:with-param>
    </xsl:apply-templates>

  </fo:table-footer>
</xsl:template>

<xsl:template match="tbody">
  <xsl:variable name="tgroup" select="parent::*"/>

  <fo:table-body>
    <xsl:apply-templates select="row[1]">
      <xsl:with-param name="spans">
        <xsl:call-template name="blank.spans">
          <xsl:with-param name="cols" select="../@cols"/>
        </xsl:call-template>
      </xsl:with-param>
    </xsl:apply-templates>
  </fo:table-body>
</xsl:template>

<xsl:template match="row">
  <xsl:param name="spans"/>

  <fo:table-row>
    <xsl:call-template name="anchor"/>

    <xsl:apply-templates select="entry[1]">
      <xsl:with-param name="spans" select="$spans"/>
    </xsl:apply-templates>
  </fo:table-row>

  <xsl:if test="following-sibling::row">
    <xsl:variable name="nextspans">
      <xsl:apply-templates select="entry[1]" mode="span">
        <xsl:with-param name="spans" select="$spans"/>
      </xsl:apply-templates>
    </xsl:variable>

    <xsl:apply-templates select="following-sibling::row[1]">
      <xsl:with-param name="spans" select="$nextspans"/>
    </xsl:apply-templates>
  </xsl:if>
</xsl:template>

<xsl:template match="entry" name="entry">
  <xsl:param name="col" select="1"/>
  <xsl:param name="spans"/>

  <xsl:variable name="row" select="parent::row"/>
  <xsl:variable name="group" select="$row/parent::*[1]"/>
  <xsl:variable name="frame" select="ancestor::tgroup/parent::*/@frame"/>

  <xsl:variable name="empty.cell" select="count(node()) = 0"/>

  <xsl:variable name="named.colnum">
    <xsl:call-template name="entry.colnum"/>
  </xsl:variable>

  <xsl:variable name="entry.colnum">
    <xsl:choose>
      <xsl:when test="$named.colnum &gt; 0">
        <xsl:value-of select="$named.colnum"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$col"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="entry.colspan">
    <xsl:choose>
      <xsl:when test="@spanname or @namest">
        <xsl:call-template name="calculate.colspan"/>
      </xsl:when>
      <xsl:otherwise>1</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="following.spans">
    <xsl:call-template name="calculate.following.spans">
      <xsl:with-param name="colspan" select="$entry.colspan"/>
      <xsl:with-param name="spans" select="$spans"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:variable name="rowsep">
    <xsl:call-template name="inherited.table.attribute">
      <xsl:with-param name="entry" select="."/>
      <xsl:with-param name="colnum" select="$entry.colnum"/>
      <xsl:with-param name="attribute" select="'rowsep'"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:variable name="colsep">
    <xsl:call-template name="inherited.table.attribute">
      <xsl:with-param name="entry" select="."/>
      <xsl:with-param name="colnum" select="$entry.colnum"/>
      <xsl:with-param name="attribute" select="'colsep'"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:variable name="valign">
    <xsl:call-template name="inherited.table.attribute">
      <xsl:with-param name="entry" select="."/>
      <xsl:with-param name="colnum" select="$entry.colnum"/>
      <xsl:with-param name="attribute" select="'valign'"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:variable name="align">
    <xsl:call-template name="inherited.table.attribute">
      <xsl:with-param name="entry" select="."/>
      <xsl:with-param name="colnum" select="$entry.colnum"/>
      <xsl:with-param name="attribute" select="'align'"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:variable name="char">
    <xsl:call-template name="inherited.table.attribute">
      <xsl:with-param name="entry" select="."/>
      <xsl:with-param name="colnum" select="$entry.colnum"/>
      <xsl:with-param name="attribute" select="'char'"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:variable name="charoff">
    <xsl:call-template name="inherited.table.attribute">
      <xsl:with-param name="entry" select="."/>
      <xsl:with-param name="colnum" select="$entry.colnum"/>
      <xsl:with-param name="attribute" select="'charoff'"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:choose>
    <xsl:when test="$spans != '' and not(starts-with($spans,'0:'))">
      <xsl:call-template name="entry">
        <xsl:with-param name="col" select="$col+1"/>
        <xsl:with-param name="spans" select="substring-after($spans,':')"/>
      </xsl:call-template>
    </xsl:when>

    <xsl:when test="$entry.colnum &gt; $col">
      <xsl:call-template name="empty.table.cell">
        <xsl:with-param name="colnum" select="$col"/>
      </xsl:call-template>
      <xsl:call-template name="entry">
        <xsl:with-param name="col" select="$col+1"/>
        <xsl:with-param name="spans" select="substring-after($spans,':')"/>
      </xsl:call-template>
    </xsl:when>

    <xsl:otherwise>
      <fo:table-cell padding="{$table.border.padding}">
        <xsl:call-template name="anchor"/>

        <xsl:choose>
          <xsl:when test="$frame='all'">
            <xsl:call-template name="border">
              <xsl:with-param name="side" select="'right'"/>
              <xsl:with-param name="padding" select="1"/>
            </xsl:call-template>
            <xsl:call-template name="border">
              <xsl:with-param name="side" select="'bottom'"/>
              <xsl:with-param name="padding" select="1"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:otherwise>
            <xsl:if test="$colsep &gt; 0">
              <xsl:call-template name="border">
                <xsl:with-param name="side" select="'right'"/>
                <xsl:with-param name="padding" select="1"/>
              </xsl:call-template>
            </xsl:if>
            <xsl:if test="$rowsep &gt; 0">
              <xsl:call-template name="border">
                <xsl:with-param name="side" select="'bottom'"/>
                <xsl:with-param name="padding" select="1"/>
              </xsl:call-template>
            </xsl:if>
          </xsl:otherwise>
        </xsl:choose>

        <xsl:if test="@morerows">
          <xsl:attribute name="number-rows-spanned">
            <xsl:value-of select="@morerows+1"/>
          </xsl:attribute>
        </xsl:if>

        <xsl:if test="$entry.colspan &gt; 1">
          <xsl:attribute name="number-columns-spanned">
            <xsl:value-of select="$entry.colspan"/>
          </xsl:attribute>
        </xsl:if>

        <xsl:if test="$valign != ''">
          <xsl:attribute name="display-align">
            <xsl:choose>
              <xsl:when test="$valign='top'">before</xsl:when>
              <xsl:when test="$valign='middle'">center</xsl:when>
              <xsl:when test="$valign='bottom'">after</xsl:when>
              <xsl:otherwise>
                <xsl:message>
                  <xsl:text>Unexpected valign value: </xsl:text>
                  <xsl:value-of select="$valign"/>
                  <xsl:text>, center used.</xsl:text>
                </xsl:message>
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

<!--
        <xsl:if test="@charoff">
          <xsl:attribute name="charoff">
            <xsl:value-of select="@charoff"/>
          </xsl:attribute>
        </xsl:if>
-->

        <fo:block>
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

          <!--
          <xsl:text>(</xsl:text>
          <xsl:value-of select="$rowsep"/>
          <xsl:text>,</xsl:text>
          <xsl:value-of select="$colsep"/>
          <xsl:text>)</xsl:text>
          -->
          <xsl:choose>
            <xsl:when test="$empty.cell">
              <xsl:text>&#160;</xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:apply-templates/>
            </xsl:otherwise>
          </xsl:choose>
        </fo:block>
      </fo:table-cell>

      <xsl:choose>
        <xsl:when test="following-sibling::entry">
          <xsl:apply-templates select="following-sibling::entry[1]">
            <xsl:with-param name="col" select="$col+$entry.colspan"/>
            <xsl:with-param name="spans" select="$following.spans"/>
          </xsl:apply-templates>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="finaltd">
            <xsl:with-param name="spans" select="$following.spans"/>
            <xsl:with-param name="col" select="$col+$entry.colspan"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="entry" name="sentry" mode="span">
  <xsl:param name="col" select="1"/>
  <xsl:param name="spans"/>

  <xsl:variable name="entry.colnum">
    <xsl:call-template name="entry.colnum"/>
  </xsl:variable>

  <xsl:variable name="entry.colspan">
    <xsl:choose>
      <xsl:when test="@spanname or @namest">
        <xsl:call-template name="calculate.colspan"/>
      </xsl:when>
      <xsl:otherwise>1</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="following.spans">
    <xsl:call-template name="calculate.following.spans">
      <xsl:with-param name="colspan" select="$entry.colspan"/>
      <xsl:with-param name="spans" select="$spans"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:choose>
    <xsl:when test="$spans != '' and not(starts-with($spans,'0:'))">
      <xsl:value-of select="substring-before($spans,':')-1"/>
      <xsl:text>:</xsl:text>
      <xsl:call-template name="sentry">
        <xsl:with-param name="col" select="$col+1"/>
        <xsl:with-param name="spans" select="substring-after($spans,':')"/>
      </xsl:call-template>
    </xsl:when>

    <xsl:when test="$entry.colnum &gt; $col">
      <xsl:text>0:</xsl:text>
      <xsl:call-template name="sentry">
        <xsl:with-param name="col" select="$col+$entry.colspan"/>
        <xsl:with-param name="spans" select="$following.spans"/>
      </xsl:call-template>
    </xsl:when>

    <xsl:otherwise>
      <xsl:call-template name="copy-string">
        <xsl:with-param name="count" select="$entry.colspan"/>
        <xsl:with-param name="string">
          <xsl:choose>
            <xsl:when test="@morerows">
              <xsl:value-of select="@morerows"/>
            </xsl:when>
            <xsl:otherwise>0</xsl:otherwise>
          </xsl:choose>
          <xsl:text>:</xsl:text>
        </xsl:with-param>
      </xsl:call-template>

      <xsl:choose>
        <xsl:when test="following-sibling::entry">
          <xsl:apply-templates select="following-sibling::entry[1]"
                               mode="span">
            <xsl:with-param name="col" select="$col+$entry.colspan"/>
            <xsl:with-param name="spans" select="$following.spans"/>
          </xsl:apply-templates>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="sfinaltd">
            <xsl:with-param name="spans" select="$following.spans"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="generate.colgroup.raw">
  <xsl:param name="cols" select="1"/>
  <xsl:param name="count" select="1"/>

  <xsl:choose>
    <xsl:when test="$count>$cols"></xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="generate.col.raw">
        <xsl:with-param name="countcol" select="$count"/>
      </xsl:call-template>
      <xsl:call-template name="generate.colgroup.raw">
        <xsl:with-param name="cols" select="$cols"/>
        <xsl:with-param name="count" select="$count+1"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="generate.colgroup">
  <xsl:param name="cols" select="1"/>
  <xsl:param name="count" select="1"/>

  <xsl:choose>
    <xsl:when test="$count>$cols"></xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="generate.col">
        <xsl:with-param name="countcol" select="$count"/>
      </xsl:call-template>
      <xsl:call-template name="generate.colgroup">
        <xsl:with-param name="cols" select="$cols"/>
        <xsl:with-param name="count" select="$count+1"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="generate.col.raw">
  <!-- generate the table-column for column countcol -->
  <xsl:param name="countcol">1</xsl:param>
  <xsl:param name="colspecs" select="./colspec"/>
  <xsl:param name="count">1</xsl:param>
  <xsl:param name="colnum">1</xsl:param>

  <xsl:choose>
    <xsl:when test="$count>count($colspecs)">
      <fo:table-column column-number="{$countcol}"/>
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
            <xsl:attribute name="column-width">
              <xsl:value-of select="$colspec.colwidth"/>
            </xsl:attribute>
          </fo:table-column>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="generate.col.raw">
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

<xsl:template name="generate.col">
  <!-- generate the table-column for column countcol -->
  <xsl:param name="countcol">1</xsl:param>
  <xsl:param name="colspecs" select="./colspec"/>
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

<doc:template name="calc.column.width" xmlns="">
<refpurpose>Calculate an XSL FO table column width specification from a
CALS table column width specification.</refpurpose>

<refdescription>
<para>CALS expresses table column widths in the following basic
forms:</para>

<itemizedlist>
<listitem>
<para><emphasis>99.99units</emphasis>, a fixed length specifier.</para>
</listitem>
<listitem>
<para><emphasis>99.99</emphasis>, a fixed length specifier without any units.</para>
</listitem>
<listitem>
<para><emphasis>99.99*</emphasis>, a relative length specifier.</para>
</listitem>
<listitem>
<para><emphasis>99.99*+99.99units</emphasis>, a combination of both.</para>
</listitem>
</itemizedlist>

<para>The CALS units are points (pt), picas (pi), centimeters (cm),
millimeters (mm), and inches (in). These are the same units as XSL,
except that XSL abbreviates picas "pc" instead of "pi". If a length
specifier has no units, the CALS default unit (pt) is assumed.</para>

<para>Relative length specifiers are represented in XSL with the
proportional-column-width() function.</para>

<para>Here are some examples:</para>

<itemizedlist>
<listitem>
<para>"36pt" becomes "36pt"</para>
</listitem>
<listitem>
<para>"3pi" becomes "3pc"</para>
</listitem>
<listitem>
<para>"36" becomes "36pt"</para>
</listitem>
<listitem>
<para>"3*" becomes "proportional-column-width(3)"</para>
</listitem>
<listitem>
<para>"3*+2pi" becomes "proportional-column-width(3)+2pc"</para>
</listitem>
<listitem>
<para>"1*+2" becomes "proportional-column-width(1)+2pt"</para>
</listitem>
</itemizedlist>
</refdescription>

<refparameter>
<variablelist>
<varlistentry><term>colwidth</term>
<listitem>
<para>The CALS column width specification.</para>
</listitem>
</varlistentry>
</variablelist>
</refparameter>

<refreturn>
<para>The XSL column width specification.</para>
</refreturn>
</doc:template>

<xsl:template name="calc.column.width">
  <xsl:param name="colwidth">1*</xsl:param>

  <!-- Ok, the colwidth could have any one of the following forms: -->
  <!--        1*       = proportional width -->
  <!--     1unit       = 1.0 units wide -->
  <!--         1       = 1pt wide -->
  <!--  1*+1unit       = proportional width + some fixed width -->
  <!--      1*+1       = proportional width + some fixed width -->

  <!-- If it has a proportional width, translate it to XSL -->
  <xsl:if test="contains($colwidth, '*')">
    <xsl:text>proportional-column-width(</xsl:text>
    <xsl:value-of select="substring-before($colwidth, '*')"/>
    <xsl:text>)</xsl:text>
  </xsl:if>

  <!-- Now grab the non-proportional part of the specification -->
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

  <!-- Ok, now the width-units could have any one of the following forms: -->
  <!--                 = <empty string> -->
  <!--     1unit       = 1.0 units wide -->
  <!--         1       = 1pt wide -->
  <!-- with an optional leading sign -->

  <!-- Grab the width part by blanking out the units part and discarding -->
  <!-- whitespace. It's not pretty, but it works. -->
  <xsl:variable name="width"
       select="normalize-space(translate($width-units,
                                         '+-0123456789.abcdefghijklmnopqrstuvwxyz',
                                         '+-0123456789.'))"/>

  <!-- Grab the units part by blanking out the width part and discarding -->
  <!-- whitespace. It's not pretty, but it works. -->
  <xsl:variable name="units"
       select="normalize-space(translate($width-units,
                                         'abcdefghijklmnopqrstuvwxyz+-0123456789.',
                                         'abcdefghijklmnopqrstuvwxyz'))"/>

  <!-- Output the width -->
  <xsl:value-of select="$width"/>

  <!-- Output the units, translated appropriately -->
  <xsl:choose>
    <xsl:when test="$units = 'pi'">pc</xsl:when>
    <xsl:when test="$units = '' and $width != ''">pt</xsl:when>
    <xsl:otherwise><xsl:value-of select="$units"/></xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- ==================================================================== -->

<!--
Here follow the templates from the DocBook common.xsl
stylesheet component that are called for table processing ... -->

<xsl:template name="blank.spans">
  <xsl:param name="cols" select="1"/>
  <xsl:if test="$cols &gt; 0">
    <xsl:text>0:</xsl:text>
    <xsl:call-template name="blank.spans">
      <xsl:with-param name="cols" select="$cols - 1"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template name="inherited.table.attribute">
  <xsl:param name="entry" select="."/>
  <xsl:param name="colnum" select="0"/>
  <xsl:param name="attribute" select="'colsep'"/>

  <xsl:variable name="entry.value">
    <xsl:call-template name="get-attribute">
      <xsl:with-param name="element" select="$entry"/>
      <xsl:with-param name="attribute" select="$attribute"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:variable name="row.value">
    <xsl:call-template name="get-attribute">
      <xsl:with-param name="element" select="$entry/ancestor::row[1]"/>
      <xsl:with-param name="attribute" select="$attribute"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:variable name="span.value">
    <xsl:if test="$entry/@spanname">
      <xsl:variable name="spanname" select="$entry/@spanname"/>
      <xsl:variable name="spanspec"
                    select="$entry/ancestor::tgroup/spanspec[@spanname=$spanname]"/>
      <xsl:variable name="span.colspec"
                    select="$entry/ancestor::tgroup/colspec[@colname=$spanspec/@namest]"/>

      <xsl:variable name="spanspec.value">
        <xsl:call-template name="get-attribute">
          <xsl:with-param name="element" select="$spanspec"/>
          <xsl:with-param name="attribute" select="$attribute"/>
        </xsl:call-template>
      </xsl:variable>

      <xsl:variable name="scolspec.value">
        <xsl:call-template name="get-attribute">
          <xsl:with-param name="element" select="$span.colspec"/>
          <xsl:with-param name="attribute" select="$attribute"/>
        </xsl:call-template>
      </xsl:variable>

      <xsl:choose>
        <xsl:when test="$spanspec.value != ''">
          <xsl:value-of select="$spanspec.value"/>
        </xsl:when>
        <xsl:when test="$scolspec.value != ''">
          <xsl:value-of select="$scolspec.value"/>
        </xsl:when>
        <xsl:otherwise></xsl:otherwise>
      </xsl:choose>
    </xsl:if>
  </xsl:variable>

  <xsl:variable name="namest.value">
    <xsl:if test="$entry/@namest">
      <xsl:variable name="namest" select="$entry/@namest"/>
      <xsl:variable name="colspec"
                    select="$entry/ancestor::tgroup/colspec[@colname=$namest]"/>

      <xsl:variable name="namest.value">
        <xsl:call-template name="get-attribute">
          <xsl:with-param name="element" select="$colspec"/>
          <xsl:with-param name="attribute" select="$attribute"/>
        </xsl:call-template>
      </xsl:variable>

      <xsl:choose>
        <xsl:when test="$namest.value">
          <xsl:value-of select="$namest.value"/>
        </xsl:when>
        <xsl:otherwise></xsl:otherwise>
      </xsl:choose>
    </xsl:if>
  </xsl:variable>

  <xsl:variable name="tgroup.value">
    <xsl:call-template name="get-attribute">
      <xsl:with-param name="element" select="$entry/ancestor::tgroup[1]"/>
      <xsl:with-param name="attribute" select="$attribute"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:choose>
    <xsl:when test="$entry.value != ''">
      <xsl:value-of select="$entry.value"/>
    </xsl:when>
    <xsl:when test="$row.value != ''">
      <xsl:value-of select="$row.value"/>
    </xsl:when>
    <xsl:when test="$span.value != ''">
      <xsl:value-of select="$span.value"/>
    </xsl:when>
    <xsl:when test="$namest.value != ''">
      <xsl:value-of select="$namest.value"/>
    </xsl:when>
    <xsl:when test="$colnum &gt; 0">
      <xsl:variable name="calc.colvalue">
        <xsl:call-template name="colnum.colspec">
          <xsl:with-param name="colnum" select="$colnum"/>
          <xsl:with-param name="attribute" select="$attribute"/>
        </xsl:call-template>
      </xsl:variable>
      <xsl:choose>
        <xsl:when test="$calc.colvalue != ''">
          <xsl:value-of select="$calc.colvalue"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$tgroup.value"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$tgroup.value"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="entry.colnum">
  <xsl:param name="entry" select="."/>

  <xsl:choose>
    <xsl:when test="$entry/@spanname">
      <xsl:variable name="spanname" select="$entry/@spanname"/>
      <xsl:variable name="spanspec"
                    select="$entry/ancestor::tgroup/spanspec[@spanname=$spanname]"/>
      <xsl:variable name="colspec"
                    select="$entry/ancestor::tgroup/colspec[@colname=$spanspec/@namest]"/>
      <xsl:call-template name="colspec.colnum">
        <xsl:with-param name="colspec" select="$colspec"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="$entry/@colname">
      <xsl:variable name="colname" select="$entry/@colname"/>
      <xsl:variable name="colspec"
                    select="$entry/ancestor::tgroup/colspec[@colname=$colname]"/>
      <xsl:call-template name="colspec.colnum">
        <xsl:with-param name="colspec" select="$colspec"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="$entry/@namest">
      <xsl:variable name="namest" select="$entry/@namest"/>
      <xsl:variable name="colspec"
                    select="$entry/ancestor::tgroup/colspec[@colname=$namest]"/>
      <xsl:call-template name="colspec.colnum">
        <xsl:with-param name="colspec" select="$colspec"/>
      </xsl:call-template>
    </xsl:when>
    <!-- no idea, return 0 -->
    <xsl:otherwise>0</xsl:otherwise>
  </xsl:choose>
</xsl:template>

<doc:template name="entry.colnum" xmlns="">
<refpurpose>Determine the column number in which a given entry occurs</refpurpose>
<refdescription>
<para>If an <sgmltag>entry</sgmltag> has a
<sgmltag class="attribute">colname</sgmltag> or
<sgmltag class="attribute">namest</sgmltag> attribute, this template
will determine the number of the column in which the entry should occur.
For other <sgmltag>entry</sgmltag>s, nothing is returned.</para>
</refdescription>
<refparameter>
<variablelist>
<varlistentry><term>entry</term>
<listitem>
<para>The <sgmltag>entry</sgmltag>-element which is to be tested.</para>
</listitem>
</varlistentry>
</variablelist>
</refparameter>

<refreturn>
<para>This template returns the column number if it can be determined,
or 0 (the empty string)</para>
</refreturn>
</doc:template>

<xsl:template name="calculate.colspan">
  <xsl:param name="entry" select="."/>
  <xsl:variable name="spanname" select="$entry/@spanname"/>
  <xsl:variable name="spanspec"
                select="$entry/ancestor::tgroup/spanspec[@spanname=$spanname]"/>

  <xsl:variable name="namest">
    <xsl:choose>
      <xsl:when test="@spanname">
        <xsl:value-of select="$spanspec/@namest"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$entry/@namest"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="nameend">
    <xsl:choose>
      <xsl:when test="@spanname">
        <xsl:value-of select="$spanspec/@nameend"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$entry/@nameend"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="scol">
    <xsl:call-template name="colspec.colnum">
      <xsl:with-param name="colspec"
                      select="$entry/ancestor::tgroup/colspec[@colname=$namest]"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:variable name="ecol">
    <xsl:call-template name="colspec.colnum">
      <xsl:with-param name="colspec"
                      select="$entry/ancestor::tgroup/colspec[@colname=$nameend]"/>
    </xsl:call-template>
  </xsl:variable>

  <xsl:choose>
    <xsl:when test="$namest != '' and $nameend != ''">
      <xsl:choose>
        <xsl:when test="$ecol &gt;= $scol">
          <xsl:value-of select="$ecol - $scol + 1"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$scol - $ecol + 1"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>1</xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="calculate.following.spans">
  <xsl:param name="colspan" select="1"/>
  <xsl:param name="spans" select="''"/>

  <xsl:choose>
    <xsl:when test="$colspan &gt; 0">
      <xsl:call-template name="calculate.following.spans">
        <xsl:with-param name="colspan" select="$colspan - 1"/>
        <xsl:with-param name="spans" select="substring-after($spans,':')"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$spans"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="finaltd">
  <xsl:param name="spans"/>
  <xsl:param name="col" select="0"/>

  <xsl:if test="$spans != ''">
    <xsl:choose>
      <xsl:when test="starts-with($spans,'0:')">
        <xsl:call-template name="empty.table.cell">
          <xsl:with-param name="colnum" select="$col"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise></xsl:otherwise>
    </xsl:choose>

    <xsl:call-template name="finaltd">
      <xsl:with-param name="spans" select="substring-after($spans,':')"/>
      <xsl:with-param name="col" select="$col+1"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template name="sfinaltd">
  <xsl:param name="spans"/>

  <xsl:if test="$spans != ''">
    <xsl:choose>
      <xsl:when test="starts-with($spans,'0:')">0:</xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="substring-before($spans,':')-1"/>
        <xsl:text>:</xsl:text>
      </xsl:otherwise>
    </xsl:choose>

    <xsl:call-template name="sfinaltd">
      <xsl:with-param name="spans" select="substring-after($spans,':')"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template name="colspec.colnum">
  <xsl:param name="colspec" select="."/>
  <xsl:choose>
    <xsl:when test="$colspec/@colnum">
      <xsl:value-of select="$colspec/@colnum"/>
    </xsl:when>
    <xsl:when test="$colspec/preceding-sibling::colspec">
      <xsl:variable name="prec.colspec.colnum">
        <xsl:call-template name="colspec.colnum">
          <xsl:with-param name="colspec"
                          select="$colspec/preceding-sibling::colspec[1]"/>
        </xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="$prec.colspec.colnum + 1"/>
    </xsl:when>
    <xsl:otherwise>1</xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="get-attribute">
  <xsl:param name="element" select="."/>
  <xsl:param name="attribute" select="''"/>

  <xsl:for-each select="$element/@*">
    <xsl:if test="local-name(.) = $attribute">
      <xsl:value-of select="."/>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

<xsl:template name="colnum.colspec">
  <xsl:param name="colnum" select="0"/>
  <xsl:param name="attribute" select="'colname'"/>
  <xsl:param name="colspecs" select="ancestor::tgroup/colspec"/>
  <xsl:param name="count" select="1"/>

  <xsl:choose>
    <xsl:when test="not($colspecs) or $count &gt; $colnum">
      <!-- nop -->
    </xsl:when>
    <xsl:when test="$colspecs[1]/@colnum">
      <xsl:choose>
        <xsl:when test="$colspecs[1]/@colnum = $colnum">
          <xsl:call-template name="get-attribute">
            <xsl:with-param name="element" select="$colspecs[1]"/>
            <xsl:with-param name="attribute" select="$attribute"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="colnum.colspec">
            <xsl:with-param name="colnum" select="$colnum"/>
            <xsl:with-param name="attribute" select="$attribute"/>
            <xsl:with-param name="colspecs"
                            select="$colspecs[position()&gt;1]"/>
            <xsl:with-param name="count"
                            select="$colspecs[1]/@colnum+1"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:choose>
        <xsl:when test="$count = $colnum">
          <xsl:call-template name="get-attribute">
            <xsl:with-param name="element" select="$colspecs[1]"/>
            <xsl:with-param name="attribute" select="$attribute"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="colnum.colspec">
            <xsl:with-param name="colnum" select="$colnum"/>
            <xsl:with-param name="attribute" select="$attribute"/>
            <xsl:with-param name="colspecs"
                            select="$colspecs[position()&gt;1]"/>
            <xsl:with-param name="count" select="$count+1"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>



<!--

<xsl:template name="calculate.rowsep">
  <xsl:param name="entry" select="."/>
  <xsl:param name="colnum" select="0"/>

  <xsl:call-template name="inherited.table.attribute">
    <xsl:with-param name="entry" select="$entry"/>
    <xsl:with-param name="colnum" select="$colnum"/>
    <xsl:with-param name="attribute" select="'rowsep'"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="X.calculate.rowsep">
  <xsl:param name="entry" select="."/>
  <xsl:param name="colnum" select="0"/>

<!- -
  <xsl:text>[</xsl:text>
  <xsl:value-of select="$colnum"/>
  <xsl:text>]</xsl:text>
- - >

  <xsl:choose>
    <xsl:when test="$entry/ancestor::table/@frame = 'all'
                    or $entry/ancestor::informaltable/@frame = 'all'">
      <xsl:value-of select="1"/>
    </xsl:when>
    <xsl:when test="$entry/@rowsep">
      <xsl:value-of select="$entry/@rowsep"/>
    </xsl:when>
    <xsl:when test="$entry/ancestor::row[1]/@rowsep">
      <xsl:value-of select="$entry/ancestor::row[1]/@rowsep"/>
    </xsl:when>
    <xsl:when test="$entry/@spanname">
      <xsl:variable name="spanname" select="$entry/@spanname"/>
      <xsl:variable name="spanspec"
                    select="$entry/ancestor::tgroup/spanspec[@spanname=$spanname]"/>
      <xsl:variable name="span.colspec"
                    select="$entry/ancestor::tgroup/colspec[@colname=$spanspec/@namest]"/>
      <xsl:choose>
        <xsl:when test="$spanspec/@rowsep">
          <xsl:value-of select="$spanspec/@rowsep"/>
        </xsl:when>
        <xsl:when test="$span.colspec/@rowsep">
          <xsl:value-of select="$span.colspec/@rowsep"/>
        </xsl:when>
        <xsl:otherwise>0</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="$entry/@namest">
      <xsl:variable name="namest" select="$entry/@namest"/>
      <xsl:variable name="colspec"
                    select="$entry/ancestor::tgroup/colspec[@colname=$namest]"/>
      <xsl:choose>
        <xsl:when test="$colspec/@rowsep">
          <xsl:value-of select="$colspec/@rowsep"/>
        </xsl:when>
        <xsl:otherwise>0</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="$colnum &gt; 0">
      <xsl:variable name="calc.rowsep">
        <xsl:call-template name="colnum.colspec">
          <xsl:with-param name="colnum" select="$colnum"/>
          <xsl:with-param name="attribute" select="'rowsep'"/>
        </xsl:call-template>
      </xsl:variable>
      <xsl:choose>
        <xsl:when test="$calc.rowsep != ''">
          <xsl:value-of select="$calc.rowsep"/>
        </xsl:when>
        <xsl:otherwise>0</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>0</xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="calculate.colsep">
  <xsl:param name="entry" select="."/>
  <xsl:param name="colnum" select="0"/>

  <xsl:call-template name="inherited.table.attribute">
    <xsl:with-param name="entry" select="$entry"/>
    <xsl:with-param name="colnum" select="$colnum"/>
    <xsl:with-param name="attribute" select="'colsep'"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="X.calculate.colsep">
  <xsl:param name="entry" select="."/>
  <xsl:param name="colnum" select="0"/>

<!- -
  <xsl:text>[</xsl:text>
  <xsl:value-of select="$colnum"/>
  <xsl:text>]</xsl:text>
- - >

  <xsl:choose>
    <xsl:when test="$entry/ancestor::table/@frame = 'all'
                    or $entry/ancestor::informaltable/@frame = 'all'">
      <xsl:value-of select="1"/>
    </xsl:when>
    <xsl:when test="$entry/@colsep">
      <xsl:value-of select="$entry/@colsep"/>
    </xsl:when>
    <xsl:when test="$entry/ancestor::row[1]/@colsep">
      <xsl:value-of select="$entry/ancestor::row[1]/@colsep"/>
    </xsl:when>
    <xsl:when test="$entry/@spanname">
      <xsl:variable name="spanname" select="$entry/@spanname"/>
      <xsl:variable name="spanspec"
                    select="$entry/ancestor::tgroup/spanspec[@spanname=$spanname]"/>
      <xsl:variable name="span.colspec"
                    select="$entry/ancestor::tgroup/colspec[@colname=$spanspec/@namest]"/>
      <xsl:choose>
        <xsl:when test="$spanspec/@colsep">
          <xsl:value-of select="$spanspec/@colsep"/>
        </xsl:when>
        <xsl:when test="$span.colspec/@colsep">
          <xsl:value-of select="$span.colspec/@colsep"/>
        </xsl:when>
        <xsl:otherwise>0</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="$entry/@namest">
      <xsl:variable name="namest" select="$entry/@namest"/>
      <xsl:variable name="colspec"
                    select="$entry/ancestor::tgroup/colspec[@colname=$namest]"/>
      <xsl:choose>
        <xsl:when test="$colspec/@colsep">
          <xsl:value-of select="$colspec/@colsep"/>
        </xsl:when>
        <xsl:otherwise>0</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="$colnum &gt; 0">
      <xsl:variable name="calc.colsep">
        <xsl:call-template name="colnum.colspec">
          <xsl:with-param name="colnum" select="$colnum"/>
          <xsl:with-param name="attribute" select="'colsep'"/>
        </xsl:call-template>
      </xsl:variable>
      <xsl:choose>
        <xsl:when test="$calc.colsep != ''">
          <xsl:value-of select="$calc.colsep"/>
        </xsl:when>
        <xsl:otherwise>0</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>0</xsl:otherwise>
  </xsl:choose>
</xsl:template>

-->

</xsl:stylesheet>
