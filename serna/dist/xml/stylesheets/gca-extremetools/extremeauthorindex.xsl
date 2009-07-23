<?xml version="1.0" encoding="iso-8859-1"?>
<!-- ============================================================= -->
<!--  MODULE:    EXTREMEAUTHORINDEX.XSL                            -->
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
<!-- OUTPUT FILE: authors.html                                     -->
<!--                                                               -->
<!-- CREATED FOR:GCA Extreme 2001 Conference Proceedings web site  -->
<!-- CREATED BY: MULBERRY TECHNOLOGIES, INC.                       -->
<!--                                                               -->
<!-- ORIGINAL CREATION DATE:                                       -->
<!--             July 31, 2001                                     -->
<!--                                                               -->
<!-- CREATED BY: original design by Paul Rosenberg
                 grouping and enhancements by Wendell Piez         -->
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

 2.  2002 IMPROVEMENTS [wap]                        v1.3  2002
     Parameterized colors, improved layout, introduced CSS;
     changed graphics around

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

<xsl:key name="papers-by-author" match="paper" use="author/@key"/>
<!-- the author key is constructed in the source by concatenating
     fname, surname -->

<!--               SET UP THE HTML OUTPUT PAGE                     -->
<xsl:template match="/">
  <html>
    <head>
      <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"></meta>
      <meta name="ROBOTS" content="NOINDEX, NOFOLLOW"></meta>
      <title>Extreme Markup Languages Author Index</title>
      <link rel="stylesheet" href="{$cssfile}" type="text/css"/>
    </head>

    <body class="indexpage">

<!-- add the navigation links                                      -->
      <xsl:call-template name="navbar">
        <xsl:with-param name="control" select="'author'"/>
      </xsl:call-template>

<!-- add the masthead                                              -->
      <xsl:call-template name="header">
        <xsl:with-param name="thistitle" select="'Index to Authors'"/>
      </xsl:call-template>

<!-- add a key to explain the icons ... no, don't
      <xsl:call-template name="legend"/>                           -->

<!-- now we get our page title
      <h1><a name="listing-start"></a>Extreme papers by author</h1> -->

<!-- add a link to a more detailed explanation (to be put below) -->
      <xsl:call-template name="formatslink"/>

<!-- now, process the authors -->
      <xsl:apply-templates select="//author">
        <xsl:sort select="surname" />
      </xsl:apply-templates >

<!-- add an explanation of the formats                             -->
      <xsl:call-template name="formats"/>

<!-- add the key again ... no, don't
      <xsl:call-template name="legend"/>                           -->

<!-- add the navigation links to the bottom of the page as well    -->
      <xsl:call-template name="navbar">
        <xsl:with-param name="control" select="'author'"/>
      </xsl:call-template>

    </body>
  </html>
</xsl:template>

<xsl:template match="author">
  <xsl:variable name="authorkey" select="@key"/>
  <xsl:if test="count(key('papers-by-author', $authorkey)[1] | ..) = 1">
  <!-- only do the author for his/her first paper in the set
       (will group all others with it) -->
    <div class="listing">
      <h3 class="listhead">
         <xsl:apply-templates select="fname|surname"/>
      </h3>
      <xsl:for-each select="key('papers-by-author', $authorkey)">
      <!-- group all the papers by this author -->
        <xsl:sort select="conference"/>
        <xsl:sort select="sortingtitle"/>
        <!-- second level sorting by conference, then title -->
        <xsl:variable name="paperlabel" select="file"/>
        <div class="sublisting">
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
          <xsl:if test="author[not(@key=$authorkey)]">
            <!-- add other authors if there are any -->
            <xsl:text> (with </xsl:text>
            <xsl:for-each select="author[not(@key=$authorkey)]">
              <xsl:if test="(position() &gt; 1) and (position() &lt; last())">
                <xsl:text>, </xsl:text>
              </xsl:if>
              <xsl:if test="position() = last() and not(position() = 1)">
                <xsl:text> and </xsl:text>
              </xsl:if>
              <xsl:value-of select="fname"/>
              <xsl:text> </xsl:text>
              <xsl:value-of select="surname"/>
            </xsl:for-each>)<xsl:text/>
          </xsl:if>
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
      </xsl:for-each>
    </div>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>

<!-- =========  END EXTREMEAUTHORINDEX.XSL STYLESHEET  ==========  -->