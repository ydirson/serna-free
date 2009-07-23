<!-- ============================================================= -->
<!--  MODULE:    EXTREMETOC.HTML.XSL                               -->
<!--  VERSION:   1.3                                               -->
<!--  DATE:      August 2003                                       -->
<!--                                                               -->
<!-- ============================================================= -->

<!-- ============================================================= -->
<!-- SYSTEM:     EXTREME XML TO WEB                                -->
<!--             Mulberry Technologies, Inc.                       -->
<!--                                                               -->
<!-- PURPOSE:    Transform incoming valid XML documents from       -->
<!--             the extreme-author.dtd to html format;            -->
<!--             this stylesheet generates a page presenting       -->
<!--             just the paper's abstract and table of contents   -->
<!--             (linking to the full paper).                      -->
<!--                                                               -->
<!--             Input file contains one document that represents  -->
<!--             a paper in the Extreme conference proceedings.    -->
<!--             A paper may reference external files for such     -->
<!--             things as figures and graphics.                   -->
<!--                                                               -->
<!--                                                               -->
<!-- CREATED FOR:GCA Extreme 2001 Conference Proceedings web site  -->
<!--                                                               -->
<!-- CREATED BY: MULBERRY TECHNOLOGIES, INC.                       -->
<!--                                                               -->
<!-- ORIGINAL CREATION DATE:                                       -->
<!--             March 23, 2001                                    -->
<!--                                                               -->
<!-- CREATED BY: Wendell Piez
                 Paul Rosenberg                                    -->
<!--                                                               -->
<!-- UNDERLYING DTDs:                                              -->
<!--             extremepaper.xml.dtd                              -->
<!--                                                               -->
<!-- DEPENDENCIES:

  EXTERNAL STYLESHEET MODULES:
  extreme.html.xsl (which has dependencies of its own)
                                                                   -->

<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:import href="extreme.html.xsl"/>

<xsl:output method="xml" encoding="iso-8859-1" />

<xsl:template match="/">
  <html>
    <head>
      <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"></meta>
      <meta name="ROBOTS" content="NOINDEX, NOFOLLOW"></meta>
      <title>
        <xsl:value-of select="concat((paper/front/title | paperstub/title), ': Table of Contents')"/>
      </title>
      <base name="{concat($paperlabel, '-toc')}"/>
      <link rel="stylesheet" href="{concat($rootdir, '/',$cssfile)}" type="text/css"/>
    </head>
    <body>
      <xsl:apply-templates/>
    </body>
  </html>
</xsl:template>

<!--               TAKE JUST THE TOC TITLES AND THE ABSTRACT       -->
<xsl:template match="paper">
  <xsl:call-template name="masthead"/>
  <xsl:apply-templates select="front"/>
  <xsl:apply-templates select="body"/>
</xsl:template>

<xsl:template match="paperstub">
  <xsl:call-template name="masthead"/>
  <xsl:apply-templates select="title|subt|author|abstract"/>
</xsl:template>

<xsl:template match="front">
  <xsl:apply-templates select="title|subt|author|abstract"/>
</xsl:template>


<xsl:template match="front/title | paperstub/title">
  <a target="{$paperlabel}" href="{concat($paperlabel, '.html')}">
    <xsl:apply-imports/>
  </a>
</xsl:template>

<xsl:template match="abstract">
  <div class="abstract">
    <h5>Abstract</h5>
    <xsl:apply-templates/>
  </div>
</xsl:template>


<xsl:template match="body">
  <h4>
    <a target="{$paperlabel}" href="{concat($paperlabel, '.html')}">
      <xsl:apply-templates select="../front/title/node()"/>
    </a>
  </h4>
  <dl>
    <xsl:apply-templates select="section"/>
  </dl>
</xsl:template>

<!--               DEAL WITH SECTIONS                             -->
<xsl:template match="section|subsec1|subsec2|subsec3">
  <dt>
    <xsl:apply-templates select="title" mode="textonly"/>
  </dt>
  <xsl:if test="subsec1|subsec2|subsec3">
    <dl>
      <xsl:apply-templates select="subsec1|subsec2|subsec3"/>
    </dl>
  </xsl:if>
</xsl:template>

<xsl:template match="title" mode="textonly">
  <xsl:variable name="numstring">
    <xsl:number level="multiple" format="1.1.1.1" count="section|subsec1|subsec2|subsec3"/>
  </xsl:variable>
  <xsl:if test="$secnumbers = 1">
    <xsl:value-of select="concat($numstring, ' ')"/>
  </xsl:if>
  <a target="{$paperlabel}">
    <xsl:attribute name="href">
      <xsl:choose>
        <xsl:when test="../@id">
          <xsl:value-of select="concat($paperlabel, '.html#', ../@id)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="concat($paperlabel, '.html#t', $numstring)"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
    <xsl:apply-templates/>
  </a>
</xsl:template>

<xsl:template name="marksectitle">
</xsl:template>


</xsl:stylesheet>

<!-- ================== End Stylesheet =========================== -->
