<?xml version='1.0' encoding='utf-8'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format" version="1.0">

  <xsl:template match="*[namespace-uri()='http://www.w3.org/1999/XSL/Transform']" priority="-1">
    <xsl:param name="indent">0</xsl:param>
    <xsl:call-template name="elems">
      <xsl:with-param name="indent">
        <xsl:value-of select="$indent"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="*[namespace-uri()='http://www.w3.org/1999/XSL/Format']" priority="-1">
    <xsl:param name="indent">0</xsl:param>
    <xsl:call-template name="elems">
      <xsl:with-param name="indent">
        <xsl:value-of select="$indent"/>
      </xsl:with-param>
      <xsl:with-param name="prefix">fo</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="elems">
    <xsl:param name="indent">0</xsl:param>
    <xsl:param name="prefix">xsl</xsl:param>
    <fo:block font-family="{$xsl-font-family}" start-indent="{concat($indent, 'pt')}">
      <xsl:choose>
        <xsl:when test="self::xsl:stylesheet">
          <xsl:attribute name="white-space-treatment">preserve</xsl:attribute>
          <xsl:attribute name="white-space-collapse">false</xsl:attribute>
          <xsl:attribute name="linefeed-treatment">preserve</xsl:attribute>
        </xsl:when>
        <xsl:when test="self::xsl:template">
          <xsl:attribute name="font-size">
            <xsl:value-of select="$top-font-size"/>
          </xsl:attribute>
        </xsl:when>
        <xsl:when test="self::xsl:apply-templates">
          <xsl:attribute name="font-weight">bold</xsl:attribute>
        </xsl:when>
        <xsl:when test="self::xsl:text">
          <xsl:attribute name="white-space-treatment">preserve</xsl:attribute>
          <xsl:attribute name="white-space-collapse">false</xsl:attribute>
          <xsl:attribute name="linefeed-treatment">preserve</xsl:attribute>
          <xsl:attribute name="background-color">#e8e8e8</xsl:attribute>
          <xsl:attribute name="font-family">monospace</xsl:attribute>
          <xsl:attribute name="font-size">
            <xsl:value-of select="$xsl-font-size"/>
          </xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="font-size">
            <xsl:value-of select="$xsl-font-size"/>
          </xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:text>&lt;</xsl:text>
      <xsl:value-of select="$prefix"/>
      <xsl:text>:</xsl:text>
      <xsl:value-of select="local-name()"/>
      <xsl:text/>
      <xsl:apply-templates select="attribute::*"/>
      <xsl:if test="not(node())">
        <xsl:text>/</xsl:text>
      </xsl:if>
      <xsl:text>&gt;</xsl:text>
      <xsl:apply-templates>
        <xsl:with-param name="indent">
          <xsl:value-of select="$indent + $indent-shift"/>
        </xsl:with-param>
      </xsl:apply-templates>
      <xsl:if test="node()">
        <xsl:choose>
          <xsl:when test="self::xsl:text">
            <fo:inline>
              <xsl:call-template name="end-tag"/>
            </fo:inline>
          </xsl:when>
          <xsl:otherwise>
            <fo:block>
              <xsl:call-template name="end-tag">
                <xsl:with-param xsl:name="prefix">
                  <xsl:value-of xsl:select="$prefix"/>
                </xsl:with-param>
              </xsl:call-template>
            </fo:block>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
    </fo:block>
  </xsl:template>
  <xsl:template match="attribute::*[namespace-uri()='http://www.w3.org/1999/XSL/Transform' or namespace-uri()='http://www.w3.org/1999/XSL/Format']" priority="-1">
    <xsl:for-each select="@*">
      <fo:inline font-style="italic">
        <xsl:value-of select="local-name(.)"/>
        <xsl:text>=&quot;</xsl:text>
        <xsl:apply-templates/>
        <xsl:text>&quot; </xsl:text>
      </fo:inline>
    </xsl:for-each>
  </xsl:template>
  <xsl:template name="end-tag">
    <xsl:param name="prefix">xsl</xsl:param>
    <xsl:text>&lt;/</xsl:text>
    <xsl:value-of select="$prefix"/>
    <xsl:text>:</xsl:text>
    <xsl:value-of select="local-name()"/>
    <xsl:text>&gt;</xsl:text>
  </xsl:template>
  <!--xsl:template name="templateAttrs">
    <xsl:variable name="att-len" select="count(@*)"/>
    <xsl:for-each select="@*">
      <xsl:value-of select="local-name(.)"/>
      <xsl:text> = </xsl:text>
      <xsl:apply-templates/> 
      <xsl:if test="position() &lt; $att-len">
        <xsl:text>; </xsl:text>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="xsl:template">
    <fo:block 
      font-size="{$top-font-size}" 
      font-family="{$xsl-font-family}"
      start-indent="2pt"
      padding-before="1pt"
      background-color="#e8e8e8">
     <fo:block>
      <xsl:text>template </xsl:text>
      <xsl:for-each select="@match|@name">
        <xsl:value-of select="local-name(.)"/>
        <xsl:text>="</xsl:text>
        <xsl:value-of select="."/>
        <xsl:text>"</xsl:text>
      </xsl:for-each>

      <xsl:if test="attribute::*[not(self::match or self::name)]">
        <fo:block color="#f00000" font-size="{$xsl-attrs-font-size}">
          <xsl:call-template name="templateAttrs"/>
        </fo:block>
      </xsl:if>
     </fo:block> 

       <xsl:apply-templates>
         <xsl:with-param name="indent">
           <xsl:value-of select="$indent-shift"/>
         </xsl:with-param>
       </xsl:apply-templates>

    </fo:block>
  </xsl:template>

  <xsl:template match="xsl:apply-templates">
    <xsl:param name="indent">0</xsl:param>
    <fo:block 
      font-size="{$apply-font-size}" 
      font-family="{$xsl-font-family}"
      start-indent="{concat($indent, 'pt')}">
     <fo:block>
      <xsl:text>apply-templates </xsl:text>
      <xsl:if test="@select">
        <fo:inline font-style="italic" padding-right="3pt">
          <xsl:value-of select="local-name(@select)"/>
          <xsl:text>="</xsl:text>
          <xsl:apply-templates select="@select"/> 
          <xsl:text>"</xsl:text>
        </fo:inline>
      </xsl:if>
      <xsl:if test="attribute::*[not(self::select)]">
        <fo:block color="#f00000" font-size="{$xsl-attrs-font-size}">
          <xsl:call-template name="templateAttrs"/>
        </fo:block>
      </xsl:if>
     </fo:block>
     <xsl:apply-templates>
       <xsl:with-param name="indent">
         <xsl:value-of select="$indent + $indent-shift"/>
       </xsl:with-param>
     </xsl:apply-templates>
    </fo:block>
  </xsl:template--></xsl:stylesheet>

