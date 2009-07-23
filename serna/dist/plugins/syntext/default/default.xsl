<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:xse="http://www.syntext.com/Extensions/XSLT-1.0"
                xmlns:se="http://syntext.com/XSL/Format-1.0"
                version='1.0'>

  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/fonts.xsl"/>
  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/common.xsl"/>
  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/layoutsetup.xsl"/>
  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/default-elements.xsl"/>
  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/page-sizes.xsl"/>

  <xsl:strip-space elements="*"/>

  <!-- Page parameters -->
  <xsl:param name="paper.type" select="'A4'"/>
  <xsl:param name="page.orientation" select="'portrait'"/>
  <xsl:param name="page.margin.bottom" select="'0.5in'"/>
  <xsl:param name="page.margin.inner">1in</xsl:param>
  <xsl:param name="page.margin.outer">1in</xsl:param>
  <xsl:param name="page.margin.top" select="'0.5in'"/>
  <xsl:param name="body.margin.bottom" select="'0.5in'"/>
  <xsl:param name="body.margin.top" select="'0.5in'"/>
  <xsl:param name="page.margin.left">1in</xsl:param>
  <xsl:param name="page.margin.right">1in</xsl:param>

  <!-- Font parameters -->
  <xsl:param name="body.font.size" select="concat($body.font.master,'pt')"/>
  <xsl:param name="attribute.font.size" select="concat(0.83 * $body.font.master,'pt')"/>

  <!-- Other parameters -->

  <xsl:variable name="default.indent.shift" select="'20'"/>

  <xsl:param name="show-attributes" select="'yes'" xse:type="string" 
    xse:annotation="One of 'yes', 'edit', 'no'"/>

  <xsl:param name="use-serna" 
    select="contains(system-property('xsl:vendor'), 'Syntext')"/>

  <xsl:attribute-set name="root">
    <xsl:attribute name="font-family">
      <xsl:value-of select="$body.font.family"/>
    </xsl:attribute>
    <xsl:attribute name="font-size"><xsl:value-of select="$body.font.size"/></xsl:attribute>
  </xsl:attribute-set>


  <!-- -->

  <xsl:template match="*">
    <xsl:param name="indent">0</xsl:param>
    <fo:block 
      font-size="{$body.font.size}" 
      font-family="{$body.font.family}"
      start-indent="{concat($indent, 'pt')}">
      <xsl:call-template name="attrs">
        <xsl:with-param name="fo" select="'block'"/>
      </xsl:call-template>
      <xsl:choose>
        <xsl:when test="text()">
          <xsl:apply-templates mode="inline"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:apply-templates>
            <xsl:with-param name="indent">
              <xsl:value-of select="$indent + $default.indent.shift"/>
            </xsl:with-param>
          </xsl:apply-templates>
        </xsl:otherwise>
      </xsl:choose>
    </fo:block>
  </xsl:template>

  <xsl:template match="*" mode="inline">
    <fo:inline font-style="italic" background-color="#f0f0f0">
      <xsl:call-template name="attrs">
        <xsl:with-param name="fo" select="'block'"/>
      </xsl:call-template>
      <xsl:apply-templates mode="inline"/>
    </fo:inline>
  </xsl:template>

  <xsl:template name="attrs">
    <xsl:param name="fo" select="'inline'"/>
      <xsl:if test="attribute::* and ($show-attributes = 'edit' or $show-attributes = 'yes')">
        <xsl:choose>
          <xsl:when test="$fo = 'block'">
            <fo:block color="#f00000" font-size="{$attribute.font.size}">
              <xsl:call-template name="attrs-core"/>
            </fo:block>
          </xsl:when>
          <xsl:otherwise>
            <fo:inline color="#f00000" font-size="{$attribute.font.size}">
              <xsl:call-template name="attrs-core"/>
            </fo:inline>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
  </xsl:template>

  <xsl:template name="attrs-core">
    <xsl:variable name="att-len" select="count(@*)"/>
    <xsl:choose>
      <xsl:when test="$show-attributes = 'yes'">
        <fo:inline font-style="italic"
          padding-right="3pt">
          <xsl:text>Attrs: </xsl:text>
        </fo:inline>
        <xsl:for-each select="@*">
          <xsl:value-of select="name()"/>
          <xsl:text> = </xsl:text>
          <xsl:value-of select="."/>
          <xsl:if test="position() &lt; $att-len">
            <xsl:text>; </xsl:text>
          </xsl:if>
        </xsl:for-each>
      </xsl:when>
      <xsl:when test="$show-attributes = 'edit' and $use-serna = 'yes'">
        <fo:inline font-style="italic"
          padding-right="3pt">
          <xsl:text>Attrs: </xsl:text>
        </fo:inline>
        <xsl:for-each select="@*">
          <xsl:call-template name="edit-attribute"/>
        </xsl:for-each>
      </xsl:when>
      <xsl:otherwise/>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="edit-attribute">
    <fo:inline>
      <xsl:value-of select="name(.)"/>
      <xsl:text>:</xsl:text>
      <xsl:choose>
        <xsl:when test="self::processing-instruction('se:enum-attr')">
          <se:combo-box width="1.5cm" value="{string()}" is-editable="true"/>
        </xsl:when>
        <xsl:otherwise>
          <se:line-edit width="1.5cm" value="{string()}"/>
        </xsl:otherwise>
      </xsl:choose>
    </fo:inline>
  </xsl:template>

</xsl:stylesheet>
