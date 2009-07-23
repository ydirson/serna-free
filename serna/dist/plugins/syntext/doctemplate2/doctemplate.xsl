<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:t="http://www.syntext.com/Extensions/DocTemplate-1.0"
                xmlns:t2="http://www.syntext.com/Extensions/DocTemplate-2.0"
                version='1.0'>
  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/fonts.xsl"/>

  <xsl:output method="xml"/>
  <xsl:strip-space elements="*"/>
  <xsl:include href="simple_master_set.xsl"/>

  <xsl:variable name="indent-shift" select="'20'"/>
  <xsl:variable name="initial-indent" select="'20'"/>

  <!-- TODO: This should match to t:document-template -->

  <xsl:template match="t:document-template|t2:document-template">
    <fo:block
      font-size="12pt"
      start-indent="{$initial-indent}pt">
      <fo:block font-size="18pt"
        font-weight="bold" 
        padding-bottom="1em"
        text-align="center">
        <xsl:text>Syntext Serna Document Template Specification</xsl:text>
      </fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="*">
    <fo:block>
      <fo:inline color="#000000" font-weight="bold" font-style="italic">
        <xsl:text>&#x2022;&#160;</xsl:text>
        <xsl:value-of select="local-name(.)"/>
        <xsl:text>: </xsl:text>
      </fo:inline>
      <fo:inline color="#f00000"><xsl:text> </xsl:text><xsl:apply-templates/></fo:inline>
    </fo:block>
  </xsl:template>

  <xsl:template match="t:document-root|t2:content">
    <fo:block>
      <fo:inline font-weight="bold" font-style="italic">
        <xsl:text>&#x2022;&#160;</xsl:text>
        <xsl:value-of select="local-name(.)"/>
        <xsl:text> (Switch to tag mode for editing this field) : </xsl:text>
      </fo:inline>
      <xsl:apply-templates mode="root.mode"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="t2:document-skeleton">
    <fo:block>
      <fo:inline font-weight="bold" font-style="italic">
        <xsl:text>&#x2022;&#160;</xsl:text>
        <xsl:value-of select="local-name(.)"/>
      <xsl:text>:  </xsl:text>
      </fo:inline>
      <fo:block 
        start-indent="{concat(($initial-indent + $indent-shift), 'pt')}">
        <xsl:apply-templates/>
      </fo:block>
    </fo:block>
  </xsl:template>
  
  <xsl:template match="t:publish|t2:publish|t2:folding-rules">
    <fo:block>
      <fo:inline font-weight="bold" font-style="italic">
        <xsl:text>&#x2022;&#160;</xsl:text>
        <xsl:value-of select="local-name(.)"/>
      <xsl:text>:  </xsl:text>
      </fo:inline>
      <xsl:apply-templates mode="publish.mode"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="*" mode="publish.mode">
    <fo:block 
      start-indent="{concat(($initial-indent + $indent-shift), 'pt')}"
      font-weight="bold" 
      font-style="italic">
      <xsl:text>&#x2022;&#160;</xsl:text>
      <xsl:value-of select="local-name(.)"/>
      <xsl:text>:  </xsl:text>
      <fo:inline 
        font-weight="normal"
        font-style="normal" 
        color="#f00000"><xsl:text> </xsl:text><xsl:apply-templates/></fo:inline>
    </fo:block>
  </xsl:template>

  <xsl:template match="*" mode="root.mode">
    <xsl:param name="indent">0</xsl:param>
    <fo:block 
      start-indent="{concat(($initial-indent + $indent-shift + $indent), 'pt')}"
      color="#f00000"
      font-weight="bold" 
      font-style="italic">
      <xsl:apply-templates mode="root.mode">
        <xsl:with-param name="indent">
          <xsl:value-of select="$indent + $indent-shift"/>
        </xsl:with-param>
      </xsl:apply-templates>
    </fo:block>

  </xsl:template>

</xsl:stylesheet>
