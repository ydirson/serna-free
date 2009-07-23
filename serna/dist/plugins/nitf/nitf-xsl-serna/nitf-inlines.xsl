<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="a">
    <fo:inline>
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="a[@href]">
    <fo:inline xsl:use-attribute-sets="a-link">
      <xsl:call-template name="process-a-link"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template name="process-a-link">
    <xsl:call-template name="process-common-attributes"/>
    <xsl:if test="@title">
      <xsl:attribute name="role">
        <xsl:value-of select="@title"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:apply-templates/>
  </xsl:template>
  
  <xsl:template match="em">
    <fo:inline xsl:use-attribute-sets="em">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="sub">
    <fo:inline xsl:use-attribute-sets="sub">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="sup">
    <fo:inline xsl:use-attribute-sets="sup">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="person">
    <fo:inline xsl:use-attribute-sets="b">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
   
  <xsl:template match="org">
    <fo:inline xsl:use-attribute-sets="a-link">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="q">
    <fo:inline xsl:use-attribute-sets="q">
      <xsl:call-template name="process-common-attributes"/>
          <xsl:text>"</xsl:text>
          <xsl:apply-templates/>
          <xsl:text>"</xsl:text>
    </fo:inline>
  </xsl:template>
   
  <xsl:template match="sublocation|city|state|region|country|alt-code|
                       rights.owner|rights.startdate|rights.enddate|
                       rights.agent|rights.geography|rights.type|rights.limitations">
    <fo:inline>
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="copyright">
    <fo:inline>
      <xsl:text>&#xA9;</xsl:text>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="copyright.year">
    <fo:inline>
      <xsl:text> </xsl:text>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="copyright.holder">
    <fo:inline>
      <xsl:text> </xsl:text>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="credit|chron|classifier|event|function|money|num|
                       object.title|postaddr|virtloc|lang|pronounce|
                       delivery.point|delivery.office|name.given|name.family|
                       numer|frac-sep|denom|postcode">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="frac">
    <fo:inline>
      <xsl:apply-templates select="numer"/>
      <xsl:choose>
        <xsl:when test="frac-sep">
          <xsl:apply-templates select="frac-sep"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>/</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="denom"/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="care.of">
    <fo:inline>
      <xsl:if test="node()">
        <xsl:text>(care of:</xsl:text>
        <xsl:apply-templates/>
        <xsl:text>)</xsl:text>
      </xsl:if>
    </fo:inline>
  </xsl:template>

  <xsl:template match="note/body.content">    
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>


  <xsl:template match="@*">    
    <fo:inline>
      <xsl:value-of select="."/>
      <!--xsl:apply-templates/-->
    </fo:inline>
  </xsl:template>

</xsl:stylesheet>
