<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.1'
                xmlns:xslx="http://www.w3.org/1999/XSL/TransformAlias"
                xmlns:xb="http://www.syntext.com/XslBricks-1.0"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                exclude-result-prefixes="xb"
                >

  <xsl:output method="xml" indent="yes"/>
  <xsl:strip-space elements="*"/>
  <xsl:namespace-alias stylesheet-prefix="xslx" result-prefix="xsl"/>

  <xsl:param name="output">html</xsl:param>

  <xsl:template match="/*">
    <xslx:stylesheet version="1.0">

      <xsl:text>&#xA;&#xA;</xsl:text>
      <xsl:comment>
        <xsl:text>  This stylesheet was automatically generated.
      Do not edit it by hand!  </xsl:text>
      </xsl:comment>
      <xsl:text>&#xA;</xsl:text>
      <xsl:apply-templates/>

    </xslx:stylesheet>  
  </xsl:template>

  <xsl:template name="attrs">
    <xsl:for-each select="@*">
      <xsl:if test="namespace-uri() != 'http://www.syntext.com/XslBricks-1.0'">
        <xsl:attribute name="{name()}">
          <xsl:value-of select="."/>
        </xsl:attribute>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="text()">
    <xsl:value-of select="."/>
  </xsl:template>

  <xsl:template match="*" name="general">
    <xsl:param name="content"/>
    <xsl:element name="{name()}">
      <xsl:if test="@*">
        <xsl:call-template name="attrs"/>
      </xsl:if>
      <xsl:copy-of select="$content"/>
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>

  <xsl:template match="xsl:template">
    <xsl:variable name="name" select="@name">
    </xsl:variable>
    <xsl:variable name="content">
      <xsl:copy-of select="//xb:interface[@name=$name]/*"/>
    </xsl:variable>

    <xsl:if test="($output = 'html' and @xb:output = 'html')
                  or ($output = 'fo' and @xb:output = 'fo') or not(@xb:output)">
      <xsl:call-template name="general">
        <xsl:with-param name="content" select="$content"/>
      </xsl:call-template>
    </xsl:if>
    <xsl:text>&#xA;</xsl:text>
  </xsl:template>

  <xsl:template match="xb:interface">
  </xsl:template>

  <xsl:template match="xb:fo-attribute">
    <xsl:variable name="value">
      <xsl:choose>
        <xsl:when test="@value">
          <xsl:value-of select="@value"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>$</xsl:text>
          <xsl:value-of select="@name"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xslx:if>
      <xsl:attribute name="test">
        <xsl:value-of select="$value"/>
      </xsl:attribute>

      <xslx:attribute>
        <xsl:attribute name="name">
          <xsl:value-of select="@name"/>
        </xsl:attribute>
        <xslx:value-of>
          <xsl:attribute name="select">
            <xsl:value-of select="$value"/>
          </xsl:attribute>
        </xslx:value-of>
      </xslx:attribute>
    </xslx:if>
  </xsl:template>

  <xsl:template match="xb:html-attribute">
    <xsl:variable name="value">
      <xsl:choose>
        <xsl:when test="@value">
          <xsl:value-of select="@value"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>$</xsl:text>
          <xsl:value-of select="@name"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xslx:if>
      <xsl:attribute name="test">
        <xsl:value-of select="$value"/>
      </xsl:attribute>

      <xslx:text><xsl:value-of select="@name"/>: </xslx:text>
      <xslx:value-of>
        <xsl:attribute name="select">
          <xsl:value-of select="$value"/>
        </xsl:attribute>
      </xslx:value-of>
      <xslx:text>; </xslx:text>
    </xslx:if>
  </xsl:template>

  <xsl:template match="xb:html-process-content">
    <xslx:choose>
      <xslx:when test="$content">
        <xslx:copy-of select="$content"/>
      </xslx:when>
      <xslx:otherwise>
        <xslx:apply-templates/>
      </xslx:otherwise>
    </xslx:choose>
  </xsl:template>

  <xsl:template match="xb:fo-process-content">
    <xslx:choose>
      <xslx:when test="$content">
        <xslx:copy-of select="$content"/>
      </xslx:when>
      <xslx:otherwise>
        <xslx:call-template name="process-common-attributes-and-children"/>
      </xslx:otherwise>
    </xslx:choose>
  </xsl:template>

  <xsl:template match="xb:fo-process-content-wo-attributes">
    <xslx:choose>
      <xslx:when test="$content">
        <xslx:copy-of select="$content"/>
      </xslx:when>
      <xslx:otherwise>
        <xslx:apply-templates/>
      </xslx:otherwise>
    </xslx:choose>
  </xsl:template>

</xsl:stylesheet>
