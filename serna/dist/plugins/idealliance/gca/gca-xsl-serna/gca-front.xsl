<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="front">
    <fo:block font-family="{$serif.font.family}">
      <xsl:apply-templates mode="front"/>
      <xsl:if test="../body/section[not(self::processing-instruction('se:choice'))] or
                    ../rear[not(self::processing-instruction('se:choice'))]">
        <fo:block>
          <xsl:call-template name="contents"/>
        </fo:block>
      </xsl:if>
    </fo:block>
  </xsl:template>

  <xsl:template match="title|subt" mode="front">
    <fo:block xsl:use-attribute-sets="headercontrol">
      <xsl:apply-templates select="." mode="titlepage"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="comment()|processing-instruction()" mode="front">
    <fo:block>
      <xsl:apply-templates select="."/>
    </fo:block>
  </xsl:template>

  <xsl:template match="author|abstract" mode="front">
    <fo:block>
      <xsl:apply-templates select="." mode="titlepage"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="keyword[1]" mode="front">
    <fo:block start-indent="5in"
              padding-top="1pt">
      <fo:block text-align="center"
                font-size="12pt"
                font-weight="bold"
                font-style="italic">
        <xsl:text>Keywords:</xsl:text>
      </fo:block>
      <xsl:apply-templates select="."/>
    </fo:block>
  </xsl:template>

  <xsl:template match="keyword" mode="front">
    <fo:block start-indent="5in"
              padding-bottom="1pt">
      <xsl:apply-templates select="."/>
    </fo:block>
  </xsl:template>

  <xsl:template match="front/title" mode="titlepage">
    <fo:block text-align="center"
              font-size="24pt">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="front/subt" mode="titlepage">
    <fo:block text-align="center"
              font-size="{$body.font.size}"
              font-weight="normal"
              padding-top="1pt">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="author" mode="titlepage">
    <fo:block text-align="center" font-family="{$serif.font.family}">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="adress">
    <fo:inline>
      <xsl:apply-templates select="email"/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="affil" mode="titlepage">
    <fo:block font-style="italic">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="abstract">
    <fo:block font-size="11pt"
              line-height="11.5pt"
              padding-top="6pt"
              start-indent="0.5in"
              end-indent="0.5in"
              padding-right="6pt">
      <fo:block font-size="18pt"
                font-weight="bold"
                padding-bottom="6pt">Abstract</fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template name="contents">
    <fo:block>
      <fo:block font-size="18pt"
                font-weight="bold"               
                padding-top="6pt"
                padding-bottom="6pt">Table of Contents</fo:block>
      <fo:block>
        <xsl:apply-templates select="../body/section"  mode="toc"/>
      </fo:block>
      <xsl:apply-templates select="../rear" mode="toc"/>
    </fo:block>
  </xsl:template>

 <xsl:template match="keyword">
    <fo:block text-align="center"
              font-size="8pt">
      <xsl:apply-templates/>
      <xsl:if test="position() &lt; last()">
        <xsl:text>,</xsl:text>
      </xsl:if>
    </fo:block>                             
 </xsl:template>

<!-- TOC -->
  <xsl:template match="front/title" mode="toc">
    <fo:block font-size="18pt">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="front/subt" mode="toc">
    <fo:block font-size="16pt" font-style="italic">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="title" mode="toc">
    <xsl:if test="$secnumbers = 1">
      <xsl:number level="multiple" format="1.1.1.1." count="section|subsec1|subsec2|subsec3"/>
      <xsl:text> </xsl:text>
    </xsl:if>
    <fo:inline>
      <xsl:apply-templates mode="toc"/>
    </fo:inline>   
  </xsl:template>

  <xsl:template match="section | subsec1" mode="toc">
    <fo:block text-align="justify" padding-top="0pt" padding-bottom="0pt">
      <xsl:if test="title">
        <xsl:apply-templates select="title" mode="toc"/>
      </xsl:if>
      <xsl:if test="subsec1">
        <fo:block start-indent="{$indent}">
          <xsl:apply-templates select="subsec1|subsec2" mode="toc"/>
        </fo:block>
      </xsl:if>
    </fo:block>
  </xsl:template>

  <xsl:template match="acronym.grp" mode="toc">
    <fo:inline>
      <xsl:apply-templates select="acronym"/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="*" mode="toc">
    <fo:block>
      <xsl:apply-templates select="."/>
    </fo:block>
  </xsl:template>

  <xsl:template match="rear" mode="toc">
    <fo:block>
      <xsl:apply-templates mode="toc"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="bibliog" mode="toc">
    <fo:block>
      <xsl:text>Bibliography</xsl:text>
    </fo:block>
  </xsl:template>

  <xsl:template match="fnref|ftnote|xref|bibref|acknowl|subsec2|subsec3" mode="toc"/>

</xsl:stylesheet>
