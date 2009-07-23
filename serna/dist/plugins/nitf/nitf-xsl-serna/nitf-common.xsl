<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">
  
  <xsl:template name="process-common-attributes-and-children">
    <xsl:call-template name="process-common-attributes"/>
    <xsl:apply-templates/>
  </xsl:template>
  
  <xsl:template name="process-common-attributes">   
    <xsl:choose>
      <xsl:when test="@xml:lang">
        <xsl:attribute name="xml:lang">
          <xsl:value-of select="@xml:lang"/>
        </xsl:attribute>
      </xsl:when>
      <xsl:when test="@lang">
        <xsl:attribute name="xml:lang">
          <xsl:value-of select="@lang"/>
        </xsl:attribute>
      </xsl:when>
    </xsl:choose>
    
    <xsl:choose>
      <xsl:when test="@id">
        <xsl:attribute name="id">
          <xsl:value-of select="@id"/>
        </xsl:attribute>
      </xsl:when>
      <xsl:when test="self::a/@name">
        <xsl:attribute name="id">
          <xsl:value-of select="@name"/>
        </xsl:attribute>
      </xsl:when>
    </xsl:choose>
    
    <xsl:if test="@align">
      <xsl:choose>
        <xsl:when test="self::caption">
        </xsl:when>
        <xsl:when test="self::img or self::object">
          <xsl:if test="@align = 'bottom' or @align = 'middle' or @align = 'top'">
            <xsl:attribute name="vertical-align">
              <xsl:value-of select="@align"/>
            </xsl:attribute>
          </xsl:if>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="process-cell-align">
            <xsl:with-param name="align" select="@align"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>
    <xsl:if test="@valign">
      <xsl:call-template name="process-cell-valign">
        <xsl:with-param name="valign" select="@valign"/>
      </xsl:call-template>
    </xsl:if>

    <xsl:if test="@bgcolor">
      <xsl:attribute name="background-color">
        <xsl:value-of select="@bgcolor"/>
      </xsl:attribute>
    </xsl:if>
    
    <xsl:if test="@style">
      <xsl:call-template name="process-style">
        <xsl:with-param name="style" select="@style"/>
      </xsl:call-template>
    </xsl:if>
    
  </xsl:template>
  
  <xsl:template name="process-style">
    <xsl:param name="style"/>
    <xsl:variable name="name"
                  select="normalize-space(substring-before($style, ':'))"/>
    <xsl:if test="$name">
      <xsl:variable name="value-and-rest"
                    select="normalize-space(substring-after($style, ':'))"/>
      <xsl:variable name="value">
        <xsl:choose>
          <xsl:when test="contains($value-and-rest, ';')">
            <xsl:value-of select="normalize-space(substring-before(
                                  $value-and-rest, ';'))"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$value-and-rest"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:variable>
      <xsl:choose>
        <xsl:when test="$name = 'width' and (self::col or self::colgroup)">
          <xsl:attribute name="column-width">
            <xsl:value-of select="$value"/>
          </xsl:attribute>
        </xsl:when>
        <xsl:when test="$name = 'vertical-align' and (
                                 self::table or self::caption or
                                 self::thead or self::tfoot or
                                 self::tbody or self::colgroup or
                                 self::col or self::tr or
                                 self::th or self::td)">
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="{$name}">
            <xsl:value-of select="$value"/>
          </xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:if>
    <xsl:variable name="rest"
                  select="normalize-space(substring-after($style, ';'))"/>
    <xsl:if test="$rest">
      <xsl:call-template name="process-style">
        <xsl:with-param name="style" select="$rest"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:template>
  
</xsl:stylesheet>
