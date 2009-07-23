<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:xb="http://www.syntext.com/XslBricks-1.0">

  <xb:interface name="ul">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="ul" xb:output="fo">
    <fo:list-block xsl:use-attribute-sets="ul">
      <xb:fo-process-content/>
    </fo:list-block>
  </xsl:template>

  <xsl:template name="ul" xb:output="html">
    <ul>
      <xb:html-process-content/>
    </ul>
  </xsl:template>


  <xb:interface name="ul.nested">
    <xsl:param name="content"/>
  </xb:interface>
  
  <xsl:template name="ul.nested" xb:output="fo"> <!-- li//ul -->
    <fo:list-block xsl:use-attribute-sets="ul-nested">
      <xb:fo-process-content/>
    </fo:list-block>
  </xsl:template>

  <xsl:template name="ul.nested" xb:output="html"> <!-- li//ul -->
    <ul>
      <xb:html-process-content/>
    </ul>
  </xsl:template>

  <xb:interface name="ol">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="ol" xb:output="fo">
    <fo:list-block xsl:use-attribute-sets="ol">
      <xb:fo-process-content/>
    </fo:list-block>
  </xsl:template>
  
  <xsl:template name="ol" xb:output="html">
    <ol>
      <xb:html-process-content/>
    </ol>
  </xsl:template>

  <xb:interface name="ol.nested">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="ol.nested" xb:output="fo"> <!-- li//ol -->
    <fo:list-block xsl:use-attribute-sets="ol-nested">
      <xb:fo-process-content/>
    </fo:list-block>
  </xsl:template>

  <xsl:template name="ol.nested" xb:output="html"> <!-- li//ol -->
    <ol>
      <xb:html-process-content/>
    </ol>
  </xsl:template>
  
  <xsl:template name="ul-li" xb:output="fo"> <!-- ul/li -->
    <fo:list-item xsl:use-attribute-sets="ul-li">
      <xsl:call-template name="process-ul-li"/>
    </fo:list-item>
  </xsl:template>
    
  <xsl:template name="ul-li" xb:output="html"> <!-- ul/li -->
    <li>
      <xsl:apply-templates/>
    </li>
  </xsl:template>

  <xsl:template name="ol-li" xb:output="fo"> <!-- ol/li -->
    <fo:list-item xsl:use-attribute-sets="ol-li">
      <xsl:call-template name="process-ol-li"/>
    </fo:list-item>
  </xsl:template>

  <xsl:template name="ol-li" xb:output="html"> <!-- ol/li -->
    <li>
      <xsl:apply-templates/>
    </li>
  </xsl:template>

  <xb:interface name="dl">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="dl" xb:output="fo">
    <fo:block xsl:use-attribute-sets="dl">
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="dl" xb:output="html">
    <dl>
      <xb:html-process-content/>
    </dl>
  </xsl:template>

  <xb:interface name="dt">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="dt" xb:output="fo">
    <fo:block xsl:use-attribute-sets="dt">
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="dt" xb:output="html">
    <dt>
      <xb:html-process-content/>
    </dt>
  </xsl:template>


  <xb:interface name="dd">
    <xsl:param name="content"/>
  </xb:interface>

  <xsl:template name="dd" xb:output="fo">
    <fo:block xsl:use-attribute-sets="dd">
      <xb:fo-process-content/>
    </fo:block>
  </xsl:template>

  <xsl:template name="dd" xb:output="html">
    <dd>
      <xb:html-process-content/>
    </dd>
  </xsl:template>

  <xsl:template name="process-ol-li" xb:output="fo">
    <xsl:call-template name="process-common-attributes"/>
    <fo:list-item-label end-indent="label-end()"
                        text-align="end" wrap-option="no-wrap">
      <fo:block>
        <xsl:variable name="depth" select="count(ancestor::ol)" />
        <xsl:choose>
          <xsl:when test="$depth = 1">
            <fo:inline xsl:use-attribute-sets="ol-label-1">
              <xsl:number format="1."/>
            </fo:inline>
          </xsl:when>
          <xsl:when test="$depth = 2">
            <fo:inline xsl:use-attribute-sets="ol-label-2">
              <xsl:number format="a."/>
            </fo:inline>
          </xsl:when>
          <xsl:otherwise>
            <fo:inline xsl:use-attribute-sets="ol-label-3">
              <xsl:number format="i."/>
            </fo:inline>
          </xsl:otherwise>
        </xsl:choose>
      </fo:block>
    </fo:list-item-label>
    <fo:list-item-body start-indent="body-start()">
      <fo:block>
        <xsl:apply-templates/>
      </fo:block>
    </fo:list-item-body>
  </xsl:template>

  <xsl:template name="process-ul-li" xb:output="fo">
    <xsl:call-template name="process-common-attributes"/>
    <fo:list-item-label end-indent="label-end()"
                        text-align="end" wrap-option="no-wrap">
      <fo:block>
        <xsl:variable name="depth" select="count(ancestor::ul)" />
        <xsl:choose>
          <xsl:when test="$depth = 1">
            <fo:inline xsl:use-attribute-sets="ul-label-1">
              <xsl:value-of select="$ul.label-1"/>
            </fo:inline>
          </xsl:when>
          <xsl:when test="$depth = 2">
            <fo:inline xsl:use-attribute-sets="ul-label-2">
              <xsl:value-of select="$ul.label-2"/>
            </fo:inline>
          </xsl:when>
          <xsl:otherwise>
            <fo:inline xsl:use-attribute-sets="ul-label-3">
              <xsl:value-of select="$ul.label-3"/>
            </fo:inline>
          </xsl:otherwise>
        </xsl:choose>
      </fo:block>
    </fo:list-item-label>
    <fo:list-item-body start-indent="body-start()">
      <fo:block>
        <xsl:apply-templates/>
      </fo:block>
    </fo:list-item-body>
  </xsl:template>

</xsl:stylesheet>
