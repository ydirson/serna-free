<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:xb="http://www.syntext.com/XslBricks-1.0">

  <xb:interface name="inline">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="inline" xb:output="fo">
    <fo:inline>
      <xb:fo-process-content/>
    </fo:inline>
  </xsl:template>

  <xsl:template name="inline" xb:output="html">
    <xb:html-process-content/>
  </xsl:template>
  

  <xb:interface name="italic.inline">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="italic.inline" xb:output="fo">
    <fo:inline font-style="italic">
      <xb:fo-process-content/>
    </fo:inline>
  </xsl:template>

  <xsl:template name="italic.inline" xb:output="html">
    <i>
      <xb:html-process-content/>
    </i>
  </xsl:template>


  <xb:interface name="bold.inline">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="bold.inline" xb:output="fo">
    <fo:inline font-weight="bold">
      <xb:fo-process-content/>
    </fo:inline>
  </xsl:template>

  <xsl:template name="bold.inline" xb:output="html">
    <b><xb:html-process-content/></b>
  </xsl:template>


  <xb:interface name="bold-italic.inline">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="bold-italic.inline" xb:output="fo">
    <fo:inline font-style="italic" font-weight="bold">
      <xb:fo-process-content/>
    </fo:inline>
  </xsl:template>

  <xsl:template name="bold-italic.inline" xb:output="html">
    <b><i><xb:html-process-content/></i></b>
  </xsl:template>


  <xb:interface name="blue-underline.inline">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="blue-underline.inline" xb:output="fo">
    <fo:inline color="blue" text-decoration="underline">
      <xb:fo-process-content/>
    </fo:inline>
  </xsl:template>

  <xsl:template name="blue-underline.inline" xb:output="html">
    <font style="color: blue; text-decoration: underline;">
      <xb:html-process-content/>
    </font>
  </xsl:template>


  <xb:interface name="monospace.inline">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="monospace.inline" xb:output="fo">
    <fo:inline xsl:use-attribute-sets="code">
      <xb:fo-process-content/>
    </fo:inline>
  </xsl:template>

  <xsl:template name="monospace.inline" xb:output="html">
    <code>
      <xb:html-process-content/>
    </code>
  </xsl:template>


  <xb:interface name="small.inline">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="small.inline" xb:output="fo">
    <fo:inline xsl:use-attribute-sets="small">
      <xb:fo-process-content/>
    </fo:inline>
  </xsl:template>

  <xsl:template name="small.inline" xb:output="html">
    <small>
      <xb:html-process-content/>
    </small>
  </xsl:template>


  <xb:interface name="big.inline">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="big.inline" xb:output="fo">
    <fo:inline xsl:use-attribute-sets="big">
      <xb:fo-process-content/>
    </fo:inline>
  </xsl:template>

  <xsl:template name="big.inline" xb:output="html">
    <big>
      <xb:html-process-content/>
    </big>
  </xsl:template>


  <xb:interface name="strike.inline">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="strike.inline" xb:output="fo">
    <fo:inline xsl:use-attribute-sets="s">
      <xb:fo-process-content/>
    </fo:inline>
  </xsl:template>

  <xsl:template name="strike.inline" xb:output="html">
    <s>
      <xb:html-process-content/>
    </s>
  </xsl:template>


  <xb:interface name="underline.inline">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="underline.inline" xb:output="fo">
    <fo:inline xsl:use-attribute-sets="u">
      <xb:fo-process-content/>
    </fo:inline>
  </xsl:template>

  <xsl:template name="underline.inline" xb:output="html">
    <u>
      <xb:html-process-content/>
    </u>
  </xsl:template>


  <xb:interface name="super.inline">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="super.inline" xb:output="fo">
    <fo:inline xsl:use-attribute-sets="sup">
      <xb:fo-process-content/>
    </fo:inline>
  </xsl:template>

  <xsl:template name="super.inline" xb:output="html">
    <sup>
      <xb:html-process-content/>
    </sup>
  </xsl:template>


  <xb:interface name="sub.inline">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="sub.inline" xb:output="fo">
    <fo:inline xsl:use-attribute-sets="sub">
      <xb:fo-process-content/>
    </fo:inline>
  </xsl:template>

  <xsl:template name="sub.inline" xb:output="html">
    <sub>
      <xb:html-process-content/>
    </sub>
  </xsl:template>


  <xb:interface name="quote.inline">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="quote.inline" xb:output="fo">
    <fo:inline xsl:use-attribute-sets="q">
      <xsl:call-template name="process-common-attributes"/>
      <xsl:text>"</xsl:text>
      <xb:fo-process-content-wo-attributes/>
      <xsl:text>"</xsl:text>
    </fo:inline>
  </xsl:template>

  <xsl:template name="quote.inline" xb:output="html">
    <q>
      <xb:html-process-content/>
    </q>
  </xsl:template>


  <xb:interface name="image.inline">
    <xsl:param name="url"/>
  </xb:interface>

  <xsl:template name="image.inline" xb:output="fo">
    <fo:external-graphic xsl:use-attribute-sets="img-link">
      <xsl:if test="$url">
        <xsl:attribute name="src">
          <xsl:text>url(</xsl:text>
          <xsl:value-of select="$url"/>
          <xsl:text>)</xsl:text>
        </xsl:attribute>
      </xsl:if>
    </fo:external-graphic>
  </xsl:template>

  <xsl:template name="image.inline" xb:output="html">
    <img src="{$url}"/>
  </xsl:template>


  <xb:interface name="inline.decoration">
    <xsl:param name="content"/>
    <xsl:param name="background-color"></xsl:param>
    <xsl:param name="color"></xsl:param>
    <xsl:param name="vertical"></xsl:param>
    <xsl:param name="font-style"></xsl:param>
    <xsl:param name="font-weight"></xsl:param>
    <xsl:param name="font-family"></xsl:param>
    <xsl:param name="font-size"></xsl:param>
    <xsl:param name="text-decoration"></xsl:param>
    <xsl:param name="border-width">0</xsl:param>
    <xsl:param name="border-color">black</xsl:param>
    <xsl:param name="border-style">solid</xsl:param>
    <xsl:param name="padding"/>
  </xb:interface>

  <xsl:template name="inline.decoration" xb:output="fo">
    <fo:inline>
      <xb:fo-attribute name="background-color"/>
      <xb:fo-attribute name="color"/>
      <xb:fo-attribute name="baseline-shift" value="$vertical"/>
      <xb:fo-attribute name="font-style"/>
      <xb:fo-attribute name="font-weight"/>
      <xb:fo-attribute name="font-family"/>
      <xb:fo-attribute name="font-size"/>
      <xb:fo-attribute name="text-decoration"/>
      <xb:fo-attribute name="padding"/>
      <xb:fo-attribute name="border-width"/>
      <xb:fo-attribute name="border-color"/>
      <xb:fo-attribute name="border-style"/>
      <xb:fo-process-content/>
    </fo:inline>
  </xsl:template>

  <xsl:template name="inline.decoration" xb:output="html">
    <font>
      <xsl:attribute name="style">
        <xb:html-attribute name="background-color"/>
        <xb:html-attribute name="color"/>
        <xb:html-attribute name="vertical-align" value="$vertical"/>
        <xb:html-attribute name="font-style"/>
        <xb:html-attribute name="font-weight"/>
        <xb:html-attribute name="font-family"/>
        <xb:html-attribute name="font-size"/>
        <xb:html-attribute name="text-decoration"/>
        <xb:html-attribute name="border-width"/>
        <xb:html-attribute name="border-color"/>
        <xb:html-attribute name="border-style"/>
        <xb:html-attribute name="padding"/>
      </xsl:attribute>
      <xb:html-process-content/>
    </font>
  </xsl:template>

</xsl:stylesheet>
