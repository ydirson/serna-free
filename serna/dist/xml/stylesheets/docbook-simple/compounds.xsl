<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="set|book|part|chapter|appendix|section|
                       sect1|sect2|sect3|sect4|sect5|simplesect|
                       partintro|preface|reference|bibliodiv|
                       colophon|dedication|toc|tocchap|tocpart|
                       tocfront|tocback|tocentry|lot|lotentry">
    <xsl:call-template name="div"/>
  </xsl:template>

  <xsl:template match="toclevel1|toclevel2|toclevel3|toclevel4|toclevel5">
    <xsl:call-template name="div.decoration">
      <xsl:with-param name="start-indent" select="'2em'"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="set/title|book/title|reference/title|glossary/title|index/title">
    <xsl:call-template name="h1">
      <xsl:with-param name="content">
        <xsl:call-template name="center">
        </xsl:call-template>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="part/title">
    <xsl:call-template name="h1">
      <xsl:with-param name="content">
        <xsl:text>Part</xsl:text>
        <xsl:text> </xsl:text>
        <xsl:number level="single" count="part" format="I. "/>
        <xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="chapter/title">
    <xsl:call-template name="h2">
      <xsl:with-param name="content">
        <xsl:text>Chapter</xsl:text>
        <xsl:text> </xsl:text>
        <xsl:number level="single" count="chapter" format="1. "/>
        <xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="appendix/title">
    <xsl:call-template name="h2">
      <xsl:with-param name="content">
        <xsl:text>Appendix</xsl:text>
        <xsl:text> </xsl:text>
        <xsl:number level="single" count="appendix" format="I. "/>
        <xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="section/title|sect1/title|sect2/title|sect3/title|
                       sect4/title|sect5/title">
    <xsl:param name="level">
      <xsl:call-template name="section.level"/>
    </xsl:param>
    <xsl:param name="heading">
      <xsl:text>Section</xsl:text>
      <xsl:text> </xsl:text>
    </xsl:param>
    <xsl:variable name="title.content">
      <xsl:choose>
        <xsl:when test="$section.autolabel">
          <xsl:if test="not(ancestor::refentry)">
            <xsl:value-of select="$heading"/>
          </xsl:if>
          <xsl:choose>
            <xsl:when test="$section.label.includes.component.label">
              <xsl:choose>
                <xsl:when test="ancestor::appendix">
                  <xsl:number 
                    level="multiple" 
                    count="appendix|section|sect1|sect2|sect3|sect4|sect5|
                           refsect1|refsect2|refsect3"
                    format="A.1. "/>
                </xsl:when>
                <xsl:when test="ancestor::refentry">
                </xsl:when>
                <xsl:otherwise>
                  <xsl:number 
                    level="multiple" 
                    count="chapter|qandadiv|section|
                           sect1|sect2|sect3|sect4|sect5|
                           refsect1|refsect2|refsect3"
                    format="1. "/>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:when>
            <xsl:otherwise>
              <xsl:number 
                level="multiple" 
                count="qandadiv|section|sect1|sect2|sect3|sect4|sect5|
                       refsect1|refsect2|refsect3"
                format="1. "/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
      </xsl:choose>
      <xsl:apply-templates/>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="$level = 1">
        <xsl:call-template name="h3">
          <xsl:with-param name="content" select="$title.content"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="$level = 2">
        <xsl:call-template name="h4">
          <xsl:with-param name="content" select="$title.content"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="$level = 3">
        <xsl:call-template name="h5">
          <xsl:with-param name="content" select="$title.content"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="$level = 4">
        <xsl:call-template name="h6">
          <xsl:with-param name="content" select="$title.content"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:when test="$level = 5">
        <xsl:call-template name="h6">
          <xsl:with-param name="content" select="$title.content"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="h6">
          <xsl:with-param name="content">
            <xsl:text>NO SECTION LEVEL</xsl:text>
            <xsl:apply-templates/>
          </xsl:with-param>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="simplesect/title">
    <xsl:call-template name="h3"/>
  </xsl:template>

</xsl:stylesheet>
