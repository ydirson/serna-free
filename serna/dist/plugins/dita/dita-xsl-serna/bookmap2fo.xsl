<?xml version='1.0'?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:se="http://www.syntext.com/XSL/Format-1.0"
                xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
                extension-element-prefixes="dtm"
                version='1.0'>

  <xsl:attribute-set name="title-h1" >
    <xsl:attribute name="font-size">30</xsl:attribute>
    <xsl:attribute name="font-weight">bold</xsl:attribute>
  </xsl:attribute-set>

  <xsl:attribute-set name="title-h2" >
    <xsl:attribute name="font-size">20</xsl:attribute>
    <xsl:attribute name="start-indent">0.2in</xsl:attribute>
    <xsl:attribute name="font-weight">bold</xsl:attribute>
  </xsl:attribute-set>

  <xsl:attribute-set name="title-h3" >
    <xsl:attribute name="font-size">15</xsl:attribute>
    <xsl:attribute name="start-indent">0.2in</xsl:attribute>
    <xsl:attribute name="font-style">italic</xsl:attribute>
  </xsl:attribute-set>

  <dtm:doc dtm:elements="bookmap"
           dtm:status="finished"
           dtm:idref="bookmap"/>
  <xsl:template match="*[contains(@class,' bookmap/bookmap ')]"
                dtm:id="bookmap">
  <fo:block>
    <fo:block id="{generate-id()}">
      <xsl:if test="not(*[1])">
        <fo:block font-size="20pt" color="red">
            (Empty BookMap)
        </fo:block>
      </xsl:if>
      <xsl:apply-templates/>
    </fo:block>
  </fo:block>
  </xsl:template>

  <dtm:doc dtm:elements="booktitle" 
           dtm:status="finished"
           dtm:idref="bookmap.booktitle"/>
  <xsl:template match="*[contains(@class,' bookmap/booktitle ')]"
                dtm:id="bookmap.booktitle">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <dtm:doc dtm:elements="part"
           dtm:status="finished"
           dtm:idref="bookmap.part"/>
  <xsl:template match="*[contains(@class,' bookmap/part ')]"
                dtm:id="bookmap.part">
    <fo:block>
      <fo:block xsl:use-attribute-sets="title-h1">
        <xsl:call-template name="getString">
          <xsl:with-param name="stringName">
            <xsl:value-of select="local-name()"/>
          </xsl:with-param>
        </xsl:call-template>
        <xsl:number level="single" count="part" format="I. "/>
      </fo:block>
      <xsl:call-template name="process.topicref"/>
    </fo:block>
  </xsl:template>

  <dtm:doc dtm:elements="part"
           dtm:status="finished"
           dtm:idref="bookmap.part"/>
  <xsl:template match="*[contains(@class,' bookmap/part ')]/
                       *[contains(@class,' bookmap/chapter ')]"
                priority="10"
                dtm:id="bookmap.part">
    <fo:block>
      <fo:block xsl:use-attribute-sets="title-h2">
        <xsl:call-template name="getString">
          <xsl:with-param name="stringName">
            <xsl:value-of select="local-name()"/>
          </xsl:with-param>
        </xsl:call-template>
        <xsl:number level="single" count="part" format="I. "/>
        <xsl:number level="single" count="chapter" format="1. "/>
      </fo:block>
      <xsl:call-template name="process.topicref"/>
    </fo:block>
  </xsl:template>

  <dtm:doc dtm:elements="chapter"
           dtm:status="finished"
           dtm:idref="bookmap.chapter"/>
  <xsl:template match="*[contains(@class,' bookmap/chapter ')]"
                dtm:id="bookmap.chapter">
    <fo:block>
      <fo:block xsl:use-attribute-sets="title-h1">
        <xsl:call-template name="getString">
          <xsl:with-param name="stringName">
            <xsl:value-of select="local-name()"/>
          </xsl:with-param>
        </xsl:call-template>
        <xsl:number level="single" count="chapter" format="1. "/>
      </fo:block>
      <xsl:call-template name="process.topicref"/>
    </fo:block>
  </xsl:template>

  <dtm:doc dtm:elements="appendix" 
           dtm:status="finished"
           dtm:idref="bookmap.appendix"/>
  <xsl:template match="*[contains(@class,' bookmap/appendix ')]"
                dtm:id="bookmap.appendix">
    <fo:block>
      <fo:block xsl:use-attribute-sets="title-h1">
        <xsl:call-template name="getString">
          <xsl:with-param name="stringName">
            <xsl:value-of select="local-name()"/>
          </xsl:with-param>
        </xsl:call-template>
        <xsl:number level="single" count="appendix" format="A. "/>
      </fo:block>
      <xsl:call-template name="process.topicref"/>
    </fo:block>
  </xsl:template>

  <dtm:doc dtm:elements="booklibrary|booktitlealt"
           dtm:status="finished"
           dtm:idref="booklibrary"/>
  <xsl:template match="*[contains(@class,' bookmap/booklibrary ')]|
                       *[contains(@class,' bookmap/booktitlealt ')]"
                dtm:id="booklibrary">
    <fo:block xsl:use-attribute-sets="title-h3"
              font-weight="bold">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <dtm:doc dtm:elements="mainbooktitle"
           dtm:status="finished"
           dtm:idref="mainbooktitle"/>
  <xsl:template match="*[contains(@class,' bookmap/mainbooktitle ')]"
                dtm:id="mainbooktitle">
    <fo:block xsl:use-attribute-sets="title-h1"
              text-align="center">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <dtm:doc dtm:elements="frontmatter|backmatter"
           dtm:status="finished" 
           dtm:idref="bookmap.matters"/>
  <xsl:template match="*[contains(@class,' bookmap/frontmatter ')]|
                       *[contains(@class,' bookmap/backmatter ')]"
                dtm:id="bookmap.matters">
    <fo:block>
      <fo:block xsl:use-attribute-sets="title-h1">
        <xsl:call-template name="getString">
          <xsl:with-param name="stringName">
            <xsl:value-of select="local-name()"/>
          </xsl:with-param>
        </xsl:call-template>
      </fo:block>
      <xsl:call-template name="process.topicref"/>
    </fo:block>
  </xsl:template>

  <dtm:doc dtm:elements="preface|bookabstract|
                         booklists|colophon|
                         draftintro|notices|
                         dedication|amendments"
           dtm:status="finished"
           dtm:idref="bookmap.preface"/>
  <xsl:template match="*[contains(@class,' bookmap/preface ')]|
                       *[contains(@class,' bookmap/bookabstract ')]|
                       *[contains(@class,' bookmap/booklists ')]|
                       *[contains(@class,' bookmap/dedication ')]|
                       *[contains(@class,' bookmap/colophon ')]|
                       *[contains(@class,' bookmap/draftintro ')]|
                       *[contains(@class,' bookmap/notices ')]|
                       *[contains(@class,' bookmap/amendments ')]"
                dtm:id="bookmap.preface">
    <fo:block>
      <fo:block xsl:use-attribute-sets="title-h3">
        <xsl:call-template name="getString">
          <xsl:with-param name="stringName">
            <xsl:value-of select="local-name()"/>
          </xsl:with-param>
        </xsl:call-template>
      </fo:block>
      <xsl:call-template name="process.topicref"/>
    </fo:block>
  </xsl:template>

</xsl:stylesheet>