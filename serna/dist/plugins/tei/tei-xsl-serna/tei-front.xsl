<!-- 
     TEI XSL Stylesheet for Syntext Serna (c) 2003, Syntext Inc.

     The Stylesheet is based on Sebastian Rahtz/Oxford University 
     XSL TEI Stylesheet v1.3. See file SR-COPYING for 
     Sebastian Rahtz/Oxford University Copyright 
     information.
-->
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="teiHeader">
    <xsl:apply-templates select="." mode="metainfo"/>
  </xsl:template>

  <xsl:template match="teiHeader|teiHeader/fileDesc|teiHeader/encodingDesc|
                       teiHeader/profileDesc|teiHeader/revisionDesc"
    mode="metainfo">
    <xsl:if test="$showMetainfo">
      <fo:block xsl:use-attribute-sets="metainfoAttributes">
        <fo:block>
          <xsl:call-template name="setupDiv1"/>
          <xsl:attribute name="text-align">center</xsl:attribute>
          <xsl:text>Draft area for "</xsl:text>
          <xsl:value-of select="local-name()"/>
          <xsl:text>"</xsl:text>
        </fo:block>
        <xsl:apply-templates mode="metainfo"/>
      </fo:block>
    </xsl:if>
  </xsl:template>
 
  <xsl:template match="titleStmt|editionStmt|publicationStmt|seriesStmt|
                       notesStmt|extent|sourceDesc|title|author|sponsor|
                       funder|principal|respStmt|resp|name|edition|respStmt|
                       publisher|distributor|authority|pubPlace|address|idno|
                       availability|date|bibl|biblFull|listBibl|projectDesc|
                       samplingDecl|editorialDecl|tagsDecl|refsDecl|classDecl|
                       normalization|quotation|hyphenation|segmentation|
                       interpretation|rendition|tagUsage|taxonomy|bibl|
                       category|catDesc|creation|langUsage|language|textClass|
                       keywords|classCode|catRef|change|item" mode="metainfo">
    <xsl:param name="indent">0</xsl:param>
    <fo:block
      font-size="{$body.font.size}"
      font-family="{$body.font.family}"
      start-indent="{$indent}pt">
      <fo:inline font-weight="bold">
        <xsl:text>&#x2022;&#160;</xsl:text>
        <xsl:value-of select="local-name()"/>
        <xsl:text>: </xsl:text>
      </fo:inline>
      <xsl:apply-templates mode="metainfo">
        <xsl:with-param name="indent">
          <xsl:value-of select="$indent + $default.indent.shift"/>
        </xsl:with-param>
      </xsl:apply-templates>
    </fo:block>
  </xsl:template>

  <xsl:template match="*" mode="metainfo">
    <xsl:apply-templates select="."/>
  </xsl:template>

  <xsl:template match="front">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="back">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template name="textTitle">
    <fo:block>
      <xsl:apply-templates select="front"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="docTitle">
    <fo:block text-align="left" font-size="{$titleSize}" >
      <xsl:if test="ancestor::group/text/front">
        <xsl:attribute name="id">
          <xsl:choose>
            <xsl:when test="ancestor::text/@id">
              <xsl:value-of select="translate(ancestor::text/@id,'_','-')"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="generate-id()"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:attribute>
      </xsl:if>
      <fo:inline font-weight="bold">
        <xsl:apply-templates select="titlePart"/>
      </fo:inline>
    </fo:block>
  </xsl:template>

  <xsl:template match="docAuthor" mode="heading">
    <fo:block>
      <xsl:if test="node() and preceding-sibling::docAuthor">
        <xsl:choose>
          <xsl:when test="not(following-sibling::docAuthor)">
            <xsl:text> and</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>,</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:if>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="docAuthor">
    <fo:block font-size="{$authorSize}">
      <fo:inline font-style="italic">
        <xsl:apply-templates/>
      </fo:inline>
    </fo:block>
  </xsl:template>

  <xsl:template match="docDate">
    <fo:block font-size="{$dateSize}">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="titlePart">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="titlePage">
    <fo:block text-align="center">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="titlePage/titlePart[@type='main']">
    <fo:block font-size="{$titleSize}"
            padding-bottom="8pt"
            text-align="center">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="titlePage/titlePart[@type='sub']">
    <fo:block font-size="{$authorSize}"
            padding-bottom="8pt"
            text-align="center">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="div/docDate">
    <xsl:if test="$showMetainfo">
      <fo:block xsl:use-attribute-sets="metainfoAttributes">
        <fo:block>
          <xsl:call-template name="setupDiv1"/>
          <xsl:attribute name="text-align">center</xsl:attribute>
       TEI "div/docDate" Metainfo
        </fo:block>
        <xsl:apply-templates mode="metainfo"/>
      </fo:block>
    </xsl:if>
  </xsl:template>

  <xsl:template match="div/docAuthor">
    <xsl:if test="$showMetainfo">
      <fo:block xsl:use-attribute-sets="metainfoAttributes">
        <fo:block>
          <xsl:call-template name="setupDiv1"/>
          <xsl:attribute name="text-align">center</xsl:attribute>
       TEI "div/docAuthor" Metainfo
        </fo:block>
        <xsl:apply-templates mode="metainfo"/>
      </fo:block>
    </xsl:if>
  </xsl:template>

  <xsl:template match="div/docTitle">
    <xsl:if test="$showMetainfo">
      <fo:block xsl:use-attribute-sets="metainfoAttributes">
        <fo:block>
          <xsl:call-template name="setupDiv1"/>
          <xsl:attribute name="text-align">center</xsl:attribute>
       TEI "div/docTitle" Metainfo
        </fo:block>
        <xsl:apply-templates mode="metainfo"/>
      </fo:block>
    </xsl:if>
  </xsl:template>


  <xsl:template match="docImprint">
    <xsl:if test="$showMetainfo">
      <fo:block xsl:use-attribute-sets="metainfoAttributes">
        <fo:block>
          <xsl:call-template name="setupDiv1"/>
          <xsl:attribute name="text-align">center</xsl:attribute>
       TEI "div/docImprint" Metainfo
        </fo:block>
        <xsl:apply-templates mode="metainfo"/>
      </fo:block>
    </xsl:if>
  </xsl:template>

</xsl:stylesheet>
