<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:strip-space elements="*"/>
  <xsl:preserve-space elements="pre pre.decoration"/>

  <!-- Blocks -->

  <xsl:template match="doc">
    <xsl:call-template name="div"/>
  </xsl:template>

  <xsl:template match="div">
    <xsl:call-template name="div"/>
  </xsl:template>

  <xsl:template match="title">
    <xsl:call-template name="h1">
      <xsl:with-param name="content">
        <xsl:call-template name="inline.decoration">
          <xsl:with-param name="text-decoration"  select="'underline'"/>
          <xsl:with-param name="color"  select="'red'"/>
        </xsl:call-template>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="h1">
    <xsl:call-template name="h1"/>
  </xsl:template>

  <xsl:template match="h2">
    <xsl:call-template name="h2"/>
  </xsl:template>

  <xsl:template match="h3">
    <xsl:call-template name="h3"/>
  </xsl:template>

  <xsl:template match="h4">
    <xsl:call-template name="h4"/>
  </xsl:template>

  <xsl:template match="h5">
    <xsl:call-template name="h5"/>
  </xsl:template>

  <xsl:template match="h6">
    <xsl:call-template name="h6"/>
  </xsl:template>

  <xsl:template match="para">
    <xsl:call-template name="para"/>
  </xsl:template>


  <!-- Inlines -->

  <xsl:template match="inline">
    <xsl:call-template name="inline"/>
  </xsl:template>

  <xsl:template match="italic.inline">
    <xsl:call-template name="italic.inline"/>
  </xsl:template>

  <xsl:template match="bold.inline">
    <xsl:call-template name="bold.inline"/>
  </xsl:template>

  <xsl:template match="bold-italic.inline">
    <xsl:call-template name="bold-italic.inline"/>
  </xsl:template>

  <xsl:template match="blue-underline.inline">
    <xsl:call-template name="blue-underline.inline"/>
  </xsl:template>

  <xsl:template match="monospace.inline">
    <xsl:call-template name="monospace.inline">
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="small.inline">
    <xsl:call-template name="small.inline">
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="big.inline">
    <xsl:call-template name="big.inline">
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="strike.inline">
    <xsl:call-template name="strike.inline">
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="underline.inline">
    <xsl:call-template name="underline.inline">
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="super.inline">
    <xsl:call-template name="super.inline">
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="sub.inline">
    <xsl:call-template name="sub.inline">
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="quote.inline">
    <xsl:call-template name="quote.inline">
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="image.inline">
    <xsl:call-template name="image.inline">
      <xsl:with-param name="url" select="@filename"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="ul">
    <xsl:call-template name="ul"/>
  </xsl:template>

  <xsl:template match="ol">
    <xsl:call-template name="ol"/>
  </xsl:template>

  <xsl:template match="li/ul">
    <xsl:call-template name="ul.nested"/>
  </xsl:template>

  <xsl:template match="li/ol">
    <xsl:call-template name="ol.nested"/>
  </xsl:template>

  <xsl:template match="ul/li">
    <xsl:call-template name="ul-li"/>
  </xsl:template>

  <xsl:template match="ol/li">
    <xsl:call-template name="ol-li"/>
  </xsl:template>

  <xsl:template match="dl">
    <xsl:call-template name="dl"/>
  </xsl:template>

  <xsl:template match="dd">
    <xsl:call-template name="dd"/>
  </xsl:template>

  <xsl:template match="dt">
    <xsl:call-template name="dt"/>
  </xsl:template>

  <xsl:template match="image.block">
    <xsl:call-template name="image.block">
      <xsl:with-param name="url" select="@filename"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="table">
    <xsl:call-template name="simple-table">
      <xsl:with-param name="rows" select="row"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="row">
    <xsl:call-template name="simple-row">
      <xsl:with-param name="cells" select="cell"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="cell">
    <xsl:variable name="cnt">
      <xsl:for-each select="parent::row[1]">
        <xsl:number/>     
      </xsl:for-each>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="$cnt mod 2">
        <xsl:call-template name="simple-cell">
          <xsl:with-param name="span" select="@span"/>
          <xsl:with-param name="text-align"       select="'right'"/>
          <xsl:with-param name="background-color" select="'gray'"/>
          <xsl:with-param name="padding"          select="'0px'"/>
          <xsl:with-param name="border-width"     select="'2px'"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="simple-cell">
          <xsl:with-param name="span" select="@span"/>
          <xsl:with-param name="text-align"       select="'center'"/>
          <xsl:with-param name="background-color" select="'white'"/>
          <xsl:with-param name="color"            select="'blue'"/>
          <xsl:with-param name="font-style"       select="'normal'"/>
          <xsl:with-param name="font-weight"      select="'normal'"/>
          <xsl:with-param name="font-family"      select="'Courier'"/>
          <xsl:with-param name="font-size"        select="'12pt'"/>
          <xsl:with-param name="text-decoration"  select="'none'"/>
          <xsl:with-param name="padding"          select="'0px'"/>
          <xsl:with-param name="border-width"     select="'0px'"/>
          <xsl:with-param name="border-color"     select="'white'"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>  
  </xsl:template>

  <xsl:template match="div.decoration">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="text-align"            select="'center'"/>
      <xsl:with-param name="background-color" select="'blue'"/>
      <xsl:with-param name="color"            select="'black'"/>
      <xsl:with-param name="font-style"       select="'normal'"/>
      <xsl:with-param name="font-weight"      select="'normal'"/>
      <xsl:with-param name="font-family"      select="'Times'"/>
      <xsl:with-param name="font-size"        select="'16pt'"/>
      <xsl:with-param name="text-decoration"  select="'none'"/>
      <xsl:with-param name="border-width"           select="'2px'"/>
      <xsl:with-param name="padding"          select="'1em'"/>
      <xsl:with-param name="start-indent"     select="'0px'"/>
      <xsl:with-param name="end-indent"       select="'0px'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="para.decoration">
    <xsl:call-template name="para.decoration">
      <xsl:with-param name="text-align"            select="'right'"/>
      <xsl:with-param name="background-color" select="'white'"/>
      <xsl:with-param name="color"            select="'black'"/>
      <xsl:with-param name="font-style"       select="'normal'"/>
      <xsl:with-param name="font-weight"      select="'normal'"/>
      <xsl:with-param name="font-family"      select="'Arial'"/>
      <xsl:with-param name="font-size"        select="'14pt'"/>
      <xsl:with-param name="text-decoration"  select="'underline'"/>
      <xsl:with-param name="border-width"           select="'0pt'"/>
      <xsl:with-param name="padding"          select="'0px'"/>
      <xsl:with-param name="start-indent"     select="'20pt'"/>
      <xsl:with-param name="end-indent"       select="'20pt'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="pre.decoration">
    <xsl:call-template name="pre.decoration">
      <xsl:with-param name="text-align"            select="'left'"/>
      <xsl:with-param name="background-color" select="'gray'"/>
      <xsl:with-param name="color"            select="'yellow'"/>
      <xsl:with-param name="font-style"       select="'normal'"/>
      <xsl:with-param name="font-weight"      select="'normal'"/>
      <xsl:with-param name="font-family"      select="'Courier'"/>
      <xsl:with-param name="font-size"        select="'12pt'"/>
      <xsl:with-param name="text-decoration"  select="'none'"/>
      <xsl:with-param name="border-width"           select="'0px'"/>
      <xsl:with-param name="padding"          select="'0px'"/>
      <xsl:with-param name="start-indent"     select="'20pt'"/>
      <xsl:with-param name="end-indent"       select="'0px'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="pre">
    <xsl:call-template name="pre"/>
  </xsl:template>

  <xsl:template match="blockquote">
    <xsl:call-template name="blockquote"/>
  </xsl:template>

  <xsl:template match="center">
    <xsl:call-template name="center"/>
  </xsl:template>

  <xsl:template match="br">
    <xsl:call-template name="br"/>
  </xsl:template>

  <xsl:template match="inline.decoration">
    <xsl:call-template name="inline.decoration">
      <xsl:with-param name="text-align"            select="'left'"/>
      <xsl:with-param name="background-color" select="'white'"/>
      <xsl:with-param name="font-style"       select="@style"/>
      <xsl:with-param name="font-weight"      select="@weight"/>
      <xsl:with-param name="border-width"           select="'0px'"/>
      <xsl:with-param name="padding"          select="'0pt'"/>
    </xsl:call-template>
  </xsl:template>
 
</xsl:stylesheet>
