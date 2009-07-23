<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:xb="http://www.syntext.com/XslBricks-1.0">

  <xb:interface name="para">
    <xsl:param name="content"/>
    <xsl:param name="class" select="local-name()"/>
  </xb:interface>

  <xsl:template name="para" xb:output="fo">
    <fo:block xsl:use-attribute-sets="p">
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="para" xb:output="html">
    <p class="{$class}">
      <xb:html-process-content/>
    </p>
  </xsl:template>


  <xb:interface name="pre">
    <xsl:param name="content"/>
    <xsl:param name="class" select="local-name()"/>
  </xb:interface>

  <xsl:template name="pre" xb:output="fo">
    <fo:block xsl:use-attribute-sets="pre">
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="pre" xb:output="html">
    <pre class="{$class}"><xb:html-process-content/></pre>
  </xsl:template>


  <xb:interface name="div">
    <xsl:param name="content"/>
    <xsl:param name="class" select="local-name()"/>
    <xsl:param name="background-color"></xsl:param>
  </xb:interface>

  <xsl:template name="div" xb:output="fo">
    <fo:block>
      <xb:fo-attribute name="background-color"/>
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="div" xb:output="html">
    <div class="{$class}">
      <xsl:attribute name="style">
        <xb:html-attribute name="background-color"/>
      </xsl:attribute>
      <xb:html-process-content/>
    </div>
  </xsl:template>

  <xb:interface name="para.decoration">
    <xsl:param name="content"/>
    <xsl:param name="class" select="local-name()"/>
    <xsl:param name="text-align"/>
    <xsl:param name="background-color"/>
    <xsl:param name="color"/>
    <xsl:param name="font-style"/>
    <xsl:param name="font-weight"/>
    <xsl:param name="font-family"/>
    <xsl:param name="font-size"/>
    <xsl:param name="text-decoration"/>
    <xsl:param name="padding"/>
    <xsl:param name="border-width">0</xsl:param>
    <xsl:param name="border-color">black</xsl:param>
    <xsl:param name="border-style">solid</xsl:param>
    <xsl:param name="start-indent"/>
    <xsl:param name="end-indent"/>
  </xb:interface>

  <xsl:template name="para.decoration" xb:output="fo">
    <fo:block xsl:use-attribute-sets="p">
      <xb:fo-attribute name="text-align"/>
      <xb:fo-attribute name="background-color"/>
      <xb:fo-attribute name="color"/>
      <xb:fo-attribute name="font-style"/>
      <xb:fo-attribute name="font-weight"/>
      <xb:fo-attribute name="font-family"/>
      <xb:fo-attribute name="font-size"/>
      <xb:fo-attribute name="text-decoration"/>
      <xb:fo-attribute name="padding"/>
      <xb:fo-attribute name="border-width"/>
      <xb:fo-attribute name="border-color"/>
      <xb:fo-attribute name="border-style"/>
      <xb:fo-attribute name="start-indent"/>
      <xb:fo-attribute name="end-indent"/>
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="para.decoration" xb:output="html">
    <p class="{$class}">
      <xsl:attribute name="style">
        <xb:html-attribute name="text-align"/>
        <xb:html-attribute name="background-color"/>
        <xb:html-attribute name="color"/>
        <xb:html-attribute name="font-style"/>
        <xb:html-attribute name="font-weight"/>
        <xb:html-attribute name="font-family"/>
        <xb:html-attribute name="font-size"/>
        <xb:html-attribute name="text-decoration"/>
        <xb:html-attribute name="padding"/>
        <xb:html-attribute name="border-width"/>
        <xb:html-attribute name="border-color"/>
        <xb:html-attribute name="border-style"/>
        <xb:html-attribute name="start-indent"/>
        <xb:html-attribute name="end-indent"/>
      </xsl:attribute>
      <xb:html-process-content/>
    </p>
  </xsl:template>


  <xb:interface name="pre.decoration">
    <xsl:param name="content"/>
    <xsl:param name="class" select="local-name()"/>
    <xsl:param name="text-align"/>
    <xsl:param name="background-color"/>
    <xsl:param name="color"/>
    <xsl:param name="font-style"/>
    <xsl:param name="font-weight"/>
    <xsl:param name="font-family"/>
    <xsl:param name="font-size"/>
    <xsl:param name="text-decoration"/>
    <xsl:param name="padding"/>
    <xsl:param name="border-width">0</xsl:param>
    <xsl:param name="border-color">black</xsl:param>
    <xsl:param name="border-style">solid</xsl:param>
    <xsl:param name="start-indent"/>
    <xsl:param name="end-indent"/>
  </xb:interface>
  
  <xsl:template name="pre.decoration" xb:output="fo">
    <fo:block xsl:use-attribute-sets="pre">
      <xb:fo-attribute name="text-align"/>
      <xb:fo-attribute name="background-color"/>
      <xb:fo-attribute name="color"/>
      <xb:fo-attribute name="font-style"/>
      <xb:fo-attribute name="font-weight"/>
      <xb:fo-attribute name="font-family"/>
      <xb:fo-attribute name="font-size"/>
      <xb:fo-attribute name="text-decoration"/>
      <xb:fo-attribute name="padding"/>
      <xb:fo-attribute name="border-width"/>
      <xb:fo-attribute name="border-color"/>
      <xb:fo-attribute name="border-style"/>
      <xb:fo-attribute name="start-indent"/>
      <xb:fo-attribute name="end-indent"/>
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="pre.decoration" xb:output="html">
    <pre class="{$class}">
      <xsl:attribute name="style">
        <xb:html-attribute name="text-align"/>
        <xb:html-attribute name="background-color"/>
        <xb:html-attribute name="color"/>
        <xb:html-attribute name="font-style"/>
        <xb:html-attribute name="font-weight"/>
        <xb:html-attribute name="font-family"/>
        <xb:html-attribute name="font-size"/>
        <xb:html-attribute name="text-decoration"/>
        <xb:html-attribute name="padding"/>
        <xb:html-attribute name="border-width"/>
        <xb:html-attribute name="border-color"/>
        <xb:html-attribute name="border-style"/>
        <xb:html-attribute name="start-indent"/>
        <xb:html-attribute name="end-indent"/>
      </xsl:attribute>
      <xb:html-process-content/>
    </pre>
  </xsl:template>


  <xb:interface name="div.decoration">
    <xsl:param name="content"/>
    <xsl:param name="class" select="local-name()"/>
    <xsl:param name="text-align"/>
    <xsl:param name="background-color"/>
    <xsl:param name="color"/>
    <xsl:param name="font-style"/>
    <xsl:param name="font-weight"/>
    <xsl:param name="font-family"/>
    <xsl:param name="font-size"/>
    <xsl:param name="text-decoration"/>
    <xsl:param name="padding"/>
    <xsl:param name="border-width">0</xsl:param>
    <xsl:param name="border-color">black</xsl:param>
    <xsl:param name="border-style">solid</xsl:param>
    <xsl:param name="start-indent"/>
    <xsl:param name="end-indent"/>
  </xb:interface>

  <xsl:template name="div.decoration" xb:output="fo">
    <fo:block>
      <xb:fo-attribute name="text-align"/>
      <xb:fo-attribute name="background-color"/>
      <xb:fo-attribute name="color"/>
      <xb:fo-attribute name="font-style"/>
      <xb:fo-attribute name="font-weight"/>
      <xb:fo-attribute name="font-family"/>
      <xb:fo-attribute name="font-size"/>
      <xb:fo-attribute name="text-decoration"/>
      <xb:fo-attribute name="padding"/>
      <xb:fo-attribute name="border-width"/>
      <xb:fo-attribute name="border-color"/>
      <xb:fo-attribute name="border-style"/>
      <xb:fo-attribute name="start-indent"/>
      <xb:fo-attribute name="end-indent"/>
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="div.decoration" xb:output="html">
    <div class="{$class}">
      <xsl:attribute name="style">
        <xb:html-attribute name="text-align"/>
        <xb:html-attribute name="background-color"/>
        <xb:html-attribute name="color"/>
        <xb:html-attribute name="font-style"/>
        <xb:html-attribute name="font-weight"/>
        <xb:html-attribute name="font-family"/>
        <xb:html-attribute name="font-size"/>
        <xb:html-attribute name="text-decoration"/>
        <xb:html-attribute name="padding"/>
        <xb:html-attribute name="border-width"/>
        <xb:html-attribute name="border-color"/>
        <xb:html-attribute name="border-style"/>
        <xb:html-attribute name="start-indent"/>
        <xb:html-attribute name="end-indent"/>
      </xsl:attribute>
      <xb:html-process-content/>
    </div>
  </xsl:template>

  <xb:interface name="h1">
    <xsl:param name="content"/>
    <xsl:param name="class" select="local-name()"/>
  </xb:interface>
 
  <xsl:template name="h1" xb:output="fo">
    <fo:block xsl:use-attribute-sets="h1">
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="h1" xb:output="html">
    <h1 class="{$class}">
      <xb:html-process-content/>
    </h1>
  </xsl:template>

  <xb:interface name="h2">
    <xsl:param name="content"/>
    <xsl:param name="class" select="local-name()"/>
  </xb:interface>
  
  <xsl:template name="h2" xb:output="fo">
    <fo:block xsl:use-attribute-sets="h2">
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="h2" xb:output="html">
    <h2 class="{$class}">
      <xb:html-process-content/>
    </h2>
  </xsl:template>

  <xb:interface name="h3">
    <xsl:param name="content"/>
    <xsl:param name="class" select="local-name()"/>
  </xb:interface>
  
  <xsl:template name="h3" xb:output="fo">
    <fo:block xsl:use-attribute-sets="h3">
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="h3" xb:output="html">
    <h3 class="{$class}">
      <xb:html-process-content/>
    </h3>
  </xsl:template>

  <xb:interface name="h4">
    <xsl:param name="content"/>
    <xsl:param name="class" select="local-name()"/>
  </xb:interface>
  
  <xsl:template name="h4" xb:output="fo">
    <fo:block xsl:use-attribute-sets="h4">
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="h4" xb:output="html">
    <h4 class="{$class}">
      <xb:html-process-content/>
    </h4>
  </xsl:template>


  <xb:interface name="h5">
    <xsl:param name="content"/>
    <xsl:param name="class" select="local-name()"/>
  </xb:interface>
  
  <xsl:template name="h5" xb:output="fo">
    <fo:block xsl:use-attribute-sets="h5" xb:output="fo">
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="h5" xb:output="html">
    <h5 class="{$class}">
      <xb:html-process-content/>
    </h5>
  </xsl:template>


  <xb:interface name="h6">
    <xsl:param name="content"/>
    <xsl:param name="class" select="local-name()"/>
  </xb:interface>
  
  <xsl:template name="h6" xb:output="fo">
    <fo:block xsl:use-attribute-sets="h6">
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="h6" xb:output="html">
    <h6 class="{$class}">
      <xb:html-process-content/>
    </h6>
  </xsl:template>


  <xb:interface name="image.block">
    <xsl:param name="url"></xsl:param>
    <xsl:param name="height"></xsl:param>
    <xsl:param name="width"></xsl:param>
  </xb:interface>

  <xsl:template name="image.block" xb:output="fo">
    <fo:block>
      <fo:external-graphic xsl:use-attribute-sets="img-link">
        <xb:fo-attribute name="content-height" value="$height"/>
        <xb:fo-attribute name="content-width" value="$width"/>
        <xsl:if test="$url">
          <xsl:attribute name="src">
            <xsl:text>url(</xsl:text>
            <xsl:value-of select="$url"/>
            <xsl:text>)</xsl:text>
          </xsl:attribute>
        </xsl:if>
      </fo:external-graphic>
    </fo:block>
  </xsl:template>

  <xsl:template name="image.block" xb:output="html">
    <div>
      <img src="{$url}">
        <xsl:if test="$height">
          <xsl:attribute name="height"><xsl:value-of select="$height"/></xsl:attribute>
        </xsl:if>
        <xsl:if test="$width">
          <xsl:attribute name="width"><xsl:value-of select="$width"/></xsl:attribute>
        </xsl:if>
      </img>
    </div>
  </xsl:template>

  <xb:interface name="blockquote">
    <xsl:param name="content"/>
    <xsl:param name="class" select="local-name()"/>
  </xb:interface>

  <xsl:template name="blockquote" xb:output="fo">
    <fo:block xsl:use-attribute-sets="blockquote">
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="blockquote" xb:output="html">
    <blockquote class="{$class}">
      <xb:html-process-content/>
    </blockquote>
  </xsl:template>

  <xb:interface name="center">
    <xsl:param name="content"/>
    <xsl:param name="class" select="local-name()"/>
  </xb:interface>
  
  <xsl:template name="center" xb:output="fo">
    <fo:block text-align="center">
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="center" xb:output="html">
    <center class="{$class}">
      <xb:html-process-content/>
    </center>
  </xsl:template>


  <xsl:template name="br" xb:output="fo">
    <fo:block xml:space="preserve"
              white-space-collapse="false">
      <xsl:call-template name="process-common-attributes"/>
      <xsl:text>&#xA;</xsl:text>
    </fo:block>
  </xsl:template>

  <xsl:template name="br" xb:output="html">
    <br/>
  </xsl:template>

</xsl:stylesheet>
