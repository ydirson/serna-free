<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="a|A">
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
  
  <xsl:template match="b|B">
    <fo:inline xsl:use-attribute-sets="b">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="strong|STRONG">
    <fo:inline xsl:use-attribute-sets="strong">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="strong//em|em//strong">
    <fo:inline xsl:use-attribute-sets="strong-em">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="i|I">
    <fo:inline xsl:use-attribute-sets="i">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="cite|CITE">
    <fo:inline xsl:use-attribute-sets="cite">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="em|EM">
    <fo:inline xsl:use-attribute-sets="em">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="var|VAR">
    <fo:inline xsl:use-attribute-sets="var">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="dfn|DFN">
    <fo:inline xsl:use-attribute-sets="dfn">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="tt|TT">
    <fo:inline xsl:use-attribute-sets="tt">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="code|CODE">
    <fo:inline xsl:use-attribute-sets="code">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="kbd|KBD">
    <fo:inline xsl:use-attribute-sets="kbd">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="samp|SAMP">
    <fo:inline xsl:use-attribute-sets="samp">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="big|BIG">
    <fo:inline xsl:use-attribute-sets="big">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="small|SMALL">
    <fo:inline xsl:use-attribute-sets="small">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="sub|SUB">
    <fo:inline xsl:use-attribute-sets="sub">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="sup|SUP">
    <fo:inline xsl:use-attribute-sets="sup">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="s|S">
    <fo:inline xsl:use-attribute-sets="s">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="strike|STRIKE">
    <fo:inline xsl:use-attribute-sets="strike">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="del|DEL">
    <fo:inline xsl:use-attribute-sets="del">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="u|U">
    <fo:inline xsl:use-attribute-sets="u">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="ins|INS">
    <fo:inline xsl:use-attribute-sets="ins">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="abbr|ABBR">
    <fo:inline xsl:use-attribute-sets="abbr">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="acronym|ACRONYM">
    <fo:inline xsl:use-attribute-sets="acronym">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="span|SPAN">
    <fo:inline>
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="font|FONT">
    <fo:inline>
      <xsl:call-template name="process-common-attributes-and-children"/>
      <xsl:if test="@color">
        <xsl:attribute name="color">
          <xsl:value-of select="@color"/>
        </xsl:attribute>
      </xsl:if>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="span[@dir]">
    <fo:inline>
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="span[@style and contains(@style, 'writing-mode')]">
    <fo:block alignment-baseline="central"
                         text-indent="0pt"
                         last-line-end-indent="0pt"
                         start-indent="0pt"
                         end-indent="0pt"
                         text-align="center"
                         text-align-last="center">
      <xsl:call-template name="process-common-attributes"/>
      <fo:block wrap-option="no-wrap" line-height="1">
        <xsl:apply-templates/>
      </fo:block>
    </fo:block>
  </xsl:template>
  
  <xsl:template match="bdo|BDO">
    <fo:inline>
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:inline>
  </xsl:template>
   
  <xsl:template match="q|Q">
    <fo:inline xsl:use-attribute-sets="q">
      <xsl:call-template name="process-common-attributes"/>
          <xsl:text>"</xsl:text>
          <xsl:apply-templates/>
          <xsl:text>"</xsl:text>
    </fo:inline>
  </xsl:template>
  
</xsl:stylesheet>
