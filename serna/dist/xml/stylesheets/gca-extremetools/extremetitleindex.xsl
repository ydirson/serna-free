<?xml version="1.0" encoding="iso-8859-1"?>
<!-- ============================================================= -->
<!--  MODULE:    EXTREMETITLEINDEX.XSL                             -->
<!--  VERSION:   1.3                                               -->
<!--  DATE:      June-July, 2002                                   -->
<!--                                                               -->
<!-- ============================================================= -->

<!-- ============================================================= -->
<!-- SYSTEM:     EXTREME XML TO WEB                                -->
<!--             Mulberry Technologies, Inc.                       -->
<!--                                                               -->
<!-- PURPOSE:    Transform incoming valid XML documents to make    -->
<!--             an index for Extreme conference papers.           -->
<!--                                                               -->
<!--             The stylesheet uses a file of paper names in      -->
<!--             xml format as input to locate each paper to be    -->
<!--             processed.                                        -->
<!--                                                               -->
<!--                                                               -->
<!-- INPUT FILES: paperlist.xml (created by dirlist2paperlist.xsl) -->
<!--                                                               -->
<!-- OUTPUT FILE: extremetitleindex.xsl                            -->
<!--                                                               -->
<!-- CREATED FOR:GCA Extreme 2001 Conference Proceedings web site  -->
<!-- CREATED BY: MULBERRY TECHNOLOGIES, INC.                       -->
<!--                                                               -->
<!-- ORIGINAL CREATION DATE:                                       -->
<!--             July 31, 2001                                     -->
<!--                                                               -->
<!-- CREATED BY: derived from extremeauthorindex.xsl
                 by Paul Rosenberg and Wendell Piez                -->
<!--                                                               -->
<!-- UNDERLYING DTDs:                                              -->
<!--             extremepaper.xml.dtd                              -->
<!--                                                               -->
<!-- ============================================================= -->


<!-- ============================================================= -->
<!--               VERSION HISTORY                                 -->
<!-- ============================================================= -->
<!--

     CHANGE LOG

 2.  Added grouping by year [wap, August 2003)

 1.  ORIGINAL VERSION                               v1.1  20010731
                                                                   -->

<!-- ============================================================= -->
<!--                    XSL STYLESHEET INVOCATION                  -->
<!-- ============================================================= -->
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:import href="htmlparams.xsl"/>

<xsl:import href="index.core.xsl"/>

<xsl:output method="html" indent="yes" encoding="iso-8859-1" />

<xsl:key name="papers-by-conference" match="paper" use="conference"/>

<!--               SET UP THE HTML OUTPUT PAGE                     -->
<xsl:template match="/">
  <html>
    <head>
      <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"></meta>
      <meta name="ROBOTS" content="NOINDEX, NOFOLLOW"></meta>
      <title>Extreme Markup Languages Title Index</title>
      <link rel="stylesheet" href="{$cssfile}" type="text/css"/>
    </head>

    <body class="indexpage">

<!-- add the navigation links                                      -->
      <xsl:call-template name="navbar">
        <xsl:with-param name="control" select="'title'"/>
      </xsl:call-template>

<!-- add the masthead                                              -->
      <xsl:call-template name="header">
        <xsl:with-param name="thistitle" select="'Title Index'"/>
      </xsl:call-template>

<!-- add a key to explain the icons ... no, don't
      <xsl:call-template name="legend"/>                           -->

<!-- now we get our page title
      <h1><a name="listing-start"></a>Extreme papers by title</h1> -->

<!-- add a link to a more detailed explanation (to be put below) -->
      <xsl:call-template name="formatslink"/>

<!-- now, process the papers -->
      <xsl:for-each select="//paper">
        <xsl:sort select="conference" order="descending"/>
        <xsl:if test="count(.|key('papers-by-conference',conference)[1])=1">
          <xsl:apply-templates select="conference" mode="listpapers"/>
        </xsl:if>
      </xsl:for-each>

<!-- add an explanation of the formats                             -->
      <xsl:call-template name="formats"/>

<!-- add the key again ... no, don't
      <xsl:call-template name="legend"/>                           -->

<!-- add the navigation links to the bottom of the page as well    -->
      <xsl:call-template name="navbar">
        <xsl:with-param name="control" select="'title'"/>
      </xsl:call-template>

    </body>
  </html>
</xsl:template>

<xsl:template match="paper">
  <xsl:variable name="paperlabel" select="file"/>

  <div class="listing">
    <h3 class="listhead">
      <!-- grab the paper title and authors, and link to the abstract 
           page unless the paper was 2001 or 2002 (in which case no
           abstract is linked) and there is no full paper -->
      <xsl:choose>
        <xsl:when test="(contains(conference,'2002') or contains(conference,'2001')) and not(@full='yes')">
            <xsl:apply-templates select="title" mode="textonly"/>
        </xsl:when>
        <xsl:otherwise>
          <a href="html/{$paperlabel}-toc.html" target="{$paperlabel}">
            <xsl:apply-templates select="title" mode="textonly"/>
          </a>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:apply-templates select="fname|surname"/>
    </h3>
    <div>
      <xsl:text>By </xsl:text>
      <xsl:for-each select="author">
        <xsl:if test="(position() &gt; 1) and (position() &lt; last())">
          <xsl:text>, </xsl:text>
        </xsl:if>
        <xsl:if test="position() = last() and not(position() = 1)">
          <xsl:text> and </xsl:text>
        </xsl:if>
        <xsl:value-of select="fname"/>
        <xsl:text> </xsl:text>
        <xsl:value-of select="surname"/>
      </xsl:for-each>
      <xsl:text/>.<br/>Presented at <xsl:text/>
      <xsl:apply-templates select="conference"/>
      <xsl:text>.</xsl:text><br/>
      <xsl:call-template name="buttonbar">
        <xsl:with-param name="paperlabel" select="$paperlabel"/>
        <xsl:with-param name="authorpackage" select="@authorpackage"/>
        <xsl:with-param name="fullpaper" select="@full"/>
        <xsl:with-param name="typeset" select="@typeset"/>
      </xsl:call-template>
    </div>
  </div>
</xsl:template>

<xsl:template match="conference" mode="listpapers">
  <h1><xsl:apply-templates/></h1>
  <xsl:apply-templates select="key('papers-by-conference',.)">
    <xsl:sort select="sortingtitle"/>
  </xsl:apply-templates >
</xsl:template>

</xsl:stylesheet>

<!-- =========  END EXTREMETITLEINDEX.XSL STYLESHEET  ==========  -->