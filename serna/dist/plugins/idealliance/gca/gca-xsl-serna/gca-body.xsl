<!DOCTYPE xsl:stylesheet [
  <!ENTITY ldquo '&#x201C;' >
  <!ENTITY rdquo '&#x201D;' >
  <!ENTITY mdash '&#x2014;' >
]>

<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="body">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template name="sectionhead">
    <xsl:param name="headsize" select="'100%'"/>
    <xsl:param name="headstyle" select="'normal'"/>
    <fo:block id="{generate-id(..)}"           
           font-size="{$headsize}"
           font-style="{$headstyle}"           
           xsl:use-attribute-sets="headercontrol">
      <xsl:if test="$secnumbers = 1">
        <xsl:number level="multiple" format="1.1.1.1. " count="section|subsec1|subsec2|subsec3"/>
      </xsl:if>
      <xsl:apply-templates/>
    </fo:block>    
  </xsl:template>

  <xsl:template match="section|subsec1|subsec2|subsec3">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="section/title">
    <fo:block>
      <xsl:call-template name="sectionhead">
        <xsl:with-param name="headsize">16pt</xsl:with-param>
      </xsl:call-template>
    </fo:block>
  </xsl:template>

  <xsl:template match="subsec1/title">
    <fo:block>
      <xsl:call-template name="sectionhead">
        <xsl:with-param name="headsize">14.1pt</xsl:with-param>
      </xsl:call-template>
    </fo:block>
  </xsl:template>

  <xsl:template match="subsec2/title">
    <fo:block>
      <xsl:call-template name="sectionhead">
        <xsl:with-param name="headsize">12.1pt</xsl:with-param>
      </xsl:call-template>
    </fo:block>
  </xsl:template>

  <xsl:template match="subsec3/title">
    <fo:block>
      <xsl:call-template name="sectionhead">
        <xsl:with-param name="headsize">12.1pt</xsl:with-param>
        <xsl:with-param name="headstyle">italic</xsl:with-param>
      </xsl:call-template>
    </fo:block>
  </xsl:template>

  <xsl:template match="note|lquote">
    <fo:block font-size="12pt"
              xsl:use-attribute-sets="indentedblock">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="para">
    <fo:block padding-top="6pt">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="li/para|note/para|def/para|lquote/para|figure/para|caption/para">
    <fo:block text-indent="0pt">
      <xsl:if test="preceding-sibling::*">
        <xsl:attribute name="padding-top">4pt</xsl:attribute>
      </xsl:if>
      <xsl:if test="parent::note and not(preceding-sibling::*)">
        <xsl:attribute name="start-indent">20pt</xsl:attribute>
        <fo:block font-size="18pt" padding-bottom="3pt">Note</fo:block>
      </xsl:if>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="randlist|seqlist">
    <fo:block padding-top="6pt" border-bottom-width="0px">
      <xsl:if test="parent::li">
        <xsl:attribute name="padding-top">0pt</xsl:attribute>
      </xsl:if>
      <xsl:apply-templates select="title"/>
      <xsl:if test="*[not(self::title) and not(self::processing-instruction('se:choice'))]">
        <fo:list-block padding-bottom="6pt"
                       provisional-distance-between-starts="{$list-indent}"
                       provisional-label-separation="12pt">
          <xsl:apply-templates select="*[not(self::title)]"/>
        </fo:list-block>
      </xsl:if>
    </fo:block>
  </xsl:template>

  <xsl:template match="seqlist/title|randlist/title">
    <fo:block font-weight="bold" xsl:use-attribute-sets="headercontrol"
              padding-top="9pt"  border-bottom-width="0px">
      <xsl:if test="../parent::li">
        <xsl:attribute name="padding-top">0pt</xsl:attribute>
      </xsl:if>

      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="li">
    <fo:list-item padding-top="2pt">
      <fo:list-item-label end-indent="label-end()">
        <fo:block text-align="end">
          <xsl:choose>
            <xsl:when test="parent::randlist">
              <xsl:choose>
                <xsl:when test="../@style='bulleted'">&#x2022;</xsl:when>
                <xsl:when test="../@style='dashed'">-</xsl:when>
                <xsl:otherwise/>
              </xsl:choose>
            </xsl:when>
            <xsl:when test="parent::seqlist">
              <xsl:choose>
                <xsl:when test="../@number='ualpha'">
                  <xsl:number format="A."/>
                </xsl:when>
                <xsl:when test="../@number='uroman'">
                  <xsl:number format="I."/>
                </xsl:when>
                <xsl:when test="../@number='lalpha'">
                  <xsl:number format="a."/>
                </xsl:when>
                <xsl:when test="../@number='lroman'">
                  <xsl:number format="i."/>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:number format="1."/>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:when>
            <xsl:otherwise/>
          </xsl:choose>
        </fo:block>
      </fo:list-item-label>
      <fo:list-item-body start-indent="body-start()">
        <xsl:apply-templates/>
      </fo:list-item-body>
    </fo:list-item>
  </xsl:template>

  <xsl:template match="deflist/title">
    <fo:block padding-left="{$list-offset}"
              padding-bottom="6pt"
              xsl:use-attribute-sets="headercontrol">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="deflist">
    <fo:block padding-top="6pt">
      <xsl:apply-templates select="title"/>
      <xsl:if test="*[not(self::title) and not(self::processing-instruction('se:choice'))]">
        <fo:list-block padding-bottom="6pt"
                     provisional-distance-between-starts="{$list-offset}"
                     provisional-label-separation="9pt">
          <xsl:if test="term.heading">
            <fo:list-item padding-top="3pt"
                        font-weight="bold">
              <xsl:apply-templates select="term.heading"/>
              <xsl:if test="def.heading[not(self::processing-instruction('se:choice'))]">
                <xsl:apply-templates select="def.heading"/>
              </xsl:if>
            </fo:list-item>
          </xsl:if>
          <xsl:apply-templates select="def.item"/>
        </fo:list-block>
      </xsl:if>
    </fo:block>
  </xsl:template>

  <xsl:template match="def.item">
    <fo:list-item padding-bottom="6pt">
      <xsl:apply-templates/>
    </fo:list-item>
  </xsl:template>

  <xsl:template match="term.heading|def.term">
    <fo:list-item-label end-indent="label-end()">
      <fo:block text-align="start">
        <xsl:apply-templates/>
      </fo:block>
    </fo:list-item-label>
  </xsl:template>

  <xsl:template match="def.heading">
    <fo:list-item-body start-indent="body-start()">
      <fo:block>
        <xsl:apply-templates/>
      </fo:block>
    </fo:list-item-body>
  </xsl:template>

  <xsl:template match="def">
    <fo:list-item-body start-indent="body-start()">
      <xsl:apply-templates/>
    </fo:list-item-body>
  </xsl:template>

  <xsl:template match="figure">
    <!-- the default is to float a figure; if its @anchor attribute
         is set to 'fixed' it will match the next template -->    
    <fo:block xsl:use-attribute-sets="borderbefore borderafter"              
              font-size="8.5pt"
              font-family="{$serif.font.family}">
      <xsl:if test="graphic[not(self::processing-instruction('se:choice'))] |
                    para[not(self::processing-instruction('se:choice'))] |
                    code.block[not(self::processing-instruction('se:choice'))]">
        <xsl:apply-templates select="graphic|para|code.block"/>
      </xsl:if>
      <fo:block font-size="10pt"
                font-weight="bold"
                text-align="center"
                xsl:use-attribute-sets="headercontrol">
          <xsl:text>Figure</xsl:text>
          <fo:inline start-indent="2pt">
            <xsl:number level="multiple" format="1.1.1.1." count="section|subsec1|subsec2|subsec3"/>
            <xsl:number format="1."/>
          </fo:inline>
          <xsl:apply-templates select="title"/>
      </fo:block>
      <xsl:if test="figcaption[not(self::processing-instruction('se:choice'))]">
        <fo:block font-size="10pt"
                  text-align="center"
                  xsl:use-attribute-sets="headercontrol">
          <xsl:apply-templates select="figcaption"/>
        </fo:block>
      </xsl:if>
    </fo:block>
    <!--fo:block padding-top="6pt"/-->
  </xsl:template>

  <xsl:template match="figure/title|figure/figcaption">
    <fo:inline>
      <xsl:text> </xsl:text>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="caption">
    <fo:block font-size="11pt">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="graphic">
    <xsl:variable name="filename">
      <xsl:choose>
        <xsl:when test="@figname">
          <xsl:value-of select="unparsed-entity-uri(@figname)"/>
        </xsl:when>
        <!-- this fallback is for preprocessed files in which
             the graphic file reference has been resolved and
             placed on a "file" attribute -->
        <xsl:when test="@href">
          <xsl:value-of select="@href"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="@file"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <fo:block padding-top="9pt"
              padding-bottom="9pt" text-align="center">
      <fo:external-graphic src="url({$filename})" max-width="336pt">
        <xsl:for-each select="@scale">
          <xsl:attribute name="content-width">
            <xsl:value-of select="."/>
          </xsl:attribute>
        </xsl:for-each>
      </fo:external-graphic>
    </fo:block>
  </xsl:template>

  <xsl:template match="inline.graphic">
    <fo:inline>
      <xsl:if test="@href">
        <fo:external-graphic>
          <xsl:attribute name="src">
            <xsl:text>url(</xsl:text>
            <xsl:value-of select="@href"/>
            <xsl:text>)</xsl:text>
          </xsl:attribute>
        </fo:external-graphic>
      </xsl:if>
    </fo:inline>
  </xsl:template>

  <xsl:template name="tabletitle">
    <xsl:text>Table </xsl:text>
    <xsl:number level="multiple" format="1.1.1.1." count="section|subsec1|subsec2|subsec3"/>
    <xsl:number format="1. "/>
    <xsl:apply-templates select="caption"/>
  </xsl:template>

  <xsl:template match="table/caption">
    <fo:inline>
      <xsl:text> </xsl:text>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>


  <xsl:template match="bibref">
    <fo:inline>
      <xsl:if test="@refloc">
        <xsl:text>[</xsl:text>
        <xsl:value-of select="id(@refloc)/bib"/>
        <xsl:text>]</xsl:text>
      </xsl:if>
    </fo:inline>
  </xsl:template>

  <xsl:template match="xref">
    <xsl:variable name="ref">
      <xsl:apply-templates select="id(@refloc)" mode="xreftext">
        <xsl:with-param name="type" select="@type"/>
        <xsl:with-param name="caller" select="."/>
      </xsl:apply-templates>
    </xsl:variable>
    <fo:inline>
      <xsl:value-of select="$ref"/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="section|subsec1|subsec2|subsec3" mode="xreftext">
    <xsl:param name="type" select="false()"/>
    <xsl:param name="caller" select="."/>
    <fo:inline color="{$special.face.color}">
      <xsl:choose>
        <xsl:when test="$type='title' or not(/gcapaper/@secnumbers='1')">
          <xsl:text>&ldquo;</xsl:text>
          <xsl:apply-templates select="title" mode="inline"/>
          <xsl:text>&rdquo;</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:number level="multiple" 
                      format="1.1.1.1" 
                      count="section|subsec1|subsec2|subsec3"/>
        </xsl:otherwise>
      </xsl:choose>
    </fo:inline>
  </xsl:template>

  <xsl:template match="title" mode="inline">
    <fo:inline>
      <xsl:value-of select="."/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="figure|table" mode="xreftext">
    <xsl:param name="type" select="false()"/>
    <xsl:param name="caller" select="."/>
    <fo:inline color="{$special.face.color}">
      <xsl:choose>
        <xsl:when test="$type='title'">
          <xsl:text>&ldquo;</xsl:text>
          <xsl:choose>
            <xsl:when test="self::table">
              <xsl:value-of select="caption"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="title"/>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:text>&rdquo;</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:number level="multiple" format="1.1.1.1." count="section|subsec1|subsec2|subsec3"/>
          <xsl:number format="1."/>
        </xsl:otherwise>
      </xsl:choose>
    </fo:inline>
  </xsl:template>

  <xsl:template match="ftnote">
    <xsl:variable name="noteno">
      <xsl:number/>
    </xsl:variable>
    <fo:inline baseline-shift="3pt"
               font-size="8pt">
      <xsl:value-of select="$noteno"/>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="ftnote/para">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="fnref">
    <xsl:variable name="noteno">
      <xsl:for-each select="id(@refloc)">
        <xsl:number/>
      </xsl:for-each>
    </xsl:variable>
    <fo:inline font-size="8pt" baseline-shift="3pt">
      <xsl:value-of select="$noteno"/>
    </fo:inline>
  </xsl:template>

  <!-- named template and moded templates for footnotes as end notes -->

  <xsl:template name="footnotes">
    <xsl:if test="$footnotes">
      <fo:block id="footnotes"
                font-size="16pt"
                xsl:use-attribute-sets="borderbefore headercontrol"
                padding-top="6pt">
        <xsl:text>Notes</xsl:text>
      </fo:block>
      <fo:list-block start-indent="0pt"
                     end-indent="{$indent}"
                     padding-top="4pt"
                     provisional-distance-between-starts="{$list-indent}"
                     provisional-label-separation="6pt">
        <xsl:apply-templates select="$footnotes" mode="notes"/>
      </fo:list-block>
    </xsl:if>
  </xsl:template>

  <xsl:template match="ftnote" mode="notes">
    <xsl:variable name="noteno">
      <xsl:number format="1."/>
    </xsl:variable>
    <fo:list-item padding-bottom="6pt">
      <fo:list-item-label end-indent="label-end()">
        <fo:block text-align="end">
          <xsl:value-of select="$noteno"/>
        </fo:block>
      </fo:list-item-label>
      <fo:list-item-body start-indent="body-start()">
        <xsl:apply-templates/>
      </fo:list-item-body>
    </fo:list-item>
  </xsl:template>

  <xsl:template match="code.block">
    <fo:block 
              font-family="{$monospace.font.family}"
              font-size="10pt"
              background-color="#e0e0e0"
              padding-top="6pt"
              start-indent="4pt"
              white-space-treatment='preserve'
              white-space-collapse='false'
              linefeed-treatment="preserve"
              text-align="start">
        <xsl:apply-templates/>
    </fo:block>
  </xsl:template>


  <xsl:template match="code">
    <fo:inline 
              font-family="{$monospace.font.family}"
              font-size="10pt"
              white-space-treatment='preserve'
              white-space-collapse='false'
              text-align="start">
        <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="code.block/b">
    <fo:inline font-weight="bold"
               white-space-treatment='preserve'
               white-space-collapse='false'
               linefeed-treatment="preserve">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="b">
    <fo:inline font-weight="bold">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="code.block/i">
    <fo:inline font-style="italic"
               white-space-treatment='preserve'
               white-space-collapse='false'
               linefeed-treatment="preserve">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="i">
    <fo:inline font-style="italic">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="a">
    <fo:inline color="blue">
      <xsl:if test="@href">
        <xsl:value-of select="@href"/>
      </xsl:if>
    </fo:inline>
  </xsl:template>


  <xsl:template match="*" mode="glossary">
    <xsl:apply-templates select="*[not(self::processing-instruction('se:choice'))]" 
                         mode="glossary"/>
  </xsl:template>

  <xsl:template match="text()" mode="glossary"/>

  <xsl:template match="acronym.grp" mode="glossary">
    <fo:list-item padding-top="3pt">
      <fo:list-item-label end-indent="label-end()">
        <xsl:apply-templates select="acronym" mode="gloss"/>
      </fo:list-item-label>
      <fo:list-item-body start-indent="body-start()">
        <xsl:apply-templates select="expansion" mode="gloss"/>      
      </fo:list-item-body>
    </fo:list-item>
  </xsl:template>

  <xsl:template match="acronym|expansion" mode="gloss">
    <fo:block>
        <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

</xsl:stylesheet>
