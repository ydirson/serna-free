<?xml version='1.0' encoding='utf-8' ?>
<!DOCTYPE xsl:stylesheet [
<!ENTITY ldquo '&#x201C;' >
<!ENTITY rdquo '&#x201D;' >
<!ENTITY nbsp  '&#xA0;'   >
]>
<!-- ============================================================= -->
<!--  MODULE:    EXTREME.XSL                                       -->
<!--  VERSION:   1.3                                               -->
<!--  DATE:      August 2003                                       -->
<!--                                                               -->
<!-- ============================================================= -->

<!-- ============================================================= -->
<!-- SYSTEM:     EXTREME XML TO WEB                                -->
<!--             Mulberry Technologies, Inc.                       -->
<!--                                                               -->
<!-- PURPOSE:    Transform incoming valid XML documents from       -->
<!--             the extreme-author.dtd to html format             -->
<!--                                                               -->
<!--             Input file contains one document that represents  -->
<!--             a paper in the Extreme conference proceedings.    -->
<!--             A paper may reference external files for such     -->
<!--             things as figures and graphics.                   -->
<!--                                                               -->
<!--                                                               -->
<!-- CREATED FOR:GCA Extreme 2001 Conference Proceedings web site  -->
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
  htmlparams.xsl
    For setup of configuration (shared with indexing routines)
  table.xsl
    DocBook/OASIS -> html table conversion (thanks to Norman Walsh)

  RUNTIME PARAMETERS
  dirfile
    defaults to '../control/paperlist.xml', this is a pointer to
    a file listing all the Extreme papers, comprising metadata
    about them including information about which versions of the
    paper are available (and which links should be generated).     -->

<!-- ============================================================= -->


<!-- ============================================================= -->
<!--               VERSION HISTORY                                 -->
<!-- ============================================================= -->
<!--

     CHANGE LOG

 4.  CLEANUP AND MINOR TWEAKS                       v1.3  20030812
     Including replacing tables with proper divs, thereby
     moving much formatting code out to the CSS layer.
 3.  MODIFIED FOR EXTREME FINAL DESIGN              v1.2  20010911
 2.  MODIFIED FOR EXTREME PRE-FINAL                 v1.1  20010809
 1.  ORIGINAL VERSION                               v1.0  20010423
                                                                   -->

<!-- ============================================================= -->
<!--                    XSL STYLESHEET INVOCATION                  -->
<!-- ============================================================= -->


<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:import href="htmlparams.xsl"/>

<xsl:import href="table.xsl"/>
<!-- import the Norman Walsh OASIS (DocBook) table stylesheet      -->

<!-- GET THE NAME OF THE LIST OF ALL PAPER FILES                   -->

<xsl:param name="dirfile" select="'../control/paperlist.xml'" />

<xsl:strip-space elements="paper front author address bio
   abstract keywords body section subsec1 subsec2 subsec3
   rear acknowl bibliog bibitem lquote note figure caption
   randlist seqlist li deflist def.item def acronym.grp ftnote
   table thead tgroup row"/>

<xsl:preserve-space elements="sgml.block verbatim"/>

<xsl:output method="html" indent="yes" encoding="utf-8" />

<xsl:variable name="secnumbers" select="paper/@secnumbers"/>
<!-- the value should be an integer. 0 (the default) means
     no section numbering. An empty node set results if
     no DTD is available to provide a default; the tests
     (below) will account for this. -->

<xsl:variable name="footnotes" select="//ftnote"/>

<xsl:variable name="rootdir" select="'../../..'"/>

<!-- set parameters for Norman Walsh's table stylesheet: -->
  <xsl:variable name="default.table.width" select="'85%'"/>

  <xsl:variable name="saxon.extensions" select="0"/>
  <!-- running without extensions -->

  <xsl:variable name="saxon.tablecolumns" select="0"/>

<!-- [end parameters for table.xsl] -->

<!-- Variables for file information -->

<xsl:variable name="homepagefile" select="concat($rootdir, '/index.html')"/>

<xsl:variable name="titleindexfile" select="concat($rootdir, '/titles.html')"/>

<xsl:variable name="authorindexfile" select="concat($rootdir, '/authors.html')"/>

<xsl:variable name="keywordindexfile" select="concat($rootdir, '/keywords.html')"/>

<xsl:variable name="thistitleauthor"
              select="concat(normalize-space(/paper/front/title | /paperstub/title), normalize-space(/paper/front/author/surname | /paperstub/author/surname))"/>

<xsl:variable name="papers"   select="document($dirfile)/paperlist/paper"/>

<xsl:variable name="thispaper"
              select="$papers[$thistitleauthor=concat(normalize-space(title), normalize-space(author/surname))]"/>

<xsl:variable name="paperpath"
              select="$thispaper/file" />

<xsl:variable name="paperlabel">
  <xsl:call-template name="trimpapername">
    <xsl:with-param name="papername" select="$paperpath"/>
  </xsl:call-template>
</xsl:variable>

<xsl:template name="trimpapername">
  <xsl:param name="papername" select="''"/>
  <xsl:choose>
    <xsl:when test="contains($papername, '/')">
      <xsl:call-template name="trimpapername">
        <xsl:with-param name="papername" select="substring-after($papername, '/')"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$papername"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:variable name="conference">
  <xsl:choose>
    <xsl:when test="/paper/front/conference">
      <xsl:apply-templates select="/paper/front/conference"/>
    </xsl:when>
    <xsl:otherwise>Extreme Markup Languages</xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<xsl:template name="masthead">
  <table class="masthead" width="100%">
    <tr><td>
      <h3 class="header">
        <xsl:value-of select="$conference"/>
        <xsl:text>: Proceedings</xsl:text>
      </h3>
    </td>
    <td align="right">
      <a href="{$homepagefile}" value="Proceedings Home">
        <xsl:choose>
          <xsl:when test="$conference='Extreme Markup Languages 2002'">
            <img src="{concat($rootdir, '/icons/',$logofile)}" align="right" border="0"/>
          </xsl:when>
          <xsl:otherwise>
            <i>Proceedings Home Page</i>
          </xsl:otherwise>
        </xsl:choose>
      </a>
    </td></tr>
  </table>
</xsl:template>

<xsl:template name="logocenter">
  <xsl:if test="$conference = 'Extreme Markup Languages 2002'">
    <table align="center" border="0">
      <tr><td>
        <a href="{$homepagefile}" value="Proceedings Home">
          <img src="{concat($rootdir, '/icons/',$logofile)}" border="0"/>
        </a>
      </td></tr>
    </table>
  </xsl:if>
</xsl:template>

<xsl:template name="indexlinkbar">
  <table width="100%" class="navbar" cellspacing="2">
    <tr>
      <td width="33%" align="center"><a href="{$authorindexfile}" value="Author Index">Papers&nbsp;by&nbsp;Author</a></td>
      <td width="34%" align="center"><a href="{$titleindexfile}" value="Title Index">Papers&nbsp;by&nbsp;Title</a> </td>
      <td width="33%" align="center"><a href="{$keywordindexfile}" value="Keyword Index">Papers&nbsp;by&nbsp;Keyword</a></td>
    </tr>
  </table>
</xsl:template>

<xsl:template name="navbar">
  <table width="100%" class="navbar">
    <tr>
      <td width="50%" align="center">
        <a href="{concat($paperlabel, '-toc.html')}"
                 target="{concat($paperlabel, '-toc')}">View Abstract/Table&nbsp;of&nbsp;Contents (in its own window)</a>
      </td>
      <td width="50%" align="right">
        <xsl:if test="$thispaper/@full = 'yes'">
          <a class="icon" href="{concat($rootdir, '/xslfo-pdf/', $paperpath, '.pdf')}" target="{$paperlabel}">
            <img src="{concat($rootdir, '/icons/fopdficon.jpg')}" alt="Auto-generated PDF version" border="1"/>
          </a>&#160;&#160;<xsl:text/>
          <a class="icon" href="{concat($rootdir, '/xml/', $paperpath, '.xml')}" target="{$paperlabel}">
            <img src="{concat($rootdir, '/icons/xmlicon.jpg')}" alt="XML source" border="1"/>
          </a>&#160;&#160;<xsl:text/>
        </xsl:if>
        <xsl:if test="$thispaper/@typeset='yes'">
          <a class="icon" href="{concat($rootdir, '/typeset-pdf/', $paperpath, '.pdf')}" target="{$paperlabel}">
            <img src="{concat($rootdir, '/icons/tppdficon.jpg')}" alt="Typeset PDF" border="1"/>
          </a>
        </xsl:if>
        <xsl:if test="$thispaper/@authorpackage='yes'">
          <xsl:text/>&#160;&#160;<a class="icon" href="{concat($rootdir, '/onsite/', $paperpath, '.zip')}">
            <img src="{concat($rootdir, '/icons/siteicon.jpg')}" alt="Author's package" border="1"/>
          </a>
        </xsl:if>
      </td>
    </tr>
  </table>
  <!-- code for previous and next links was removed: check
       Proceedings stylesheets from 2002 or before to see it. -->
</xsl:template>

<xsl:template match="ftnote" mode="textonly"/>

<xsl:template match="paper" mode="linkstring">
  <!-- matches nodes in paper list document, not main paper;
       generates inline text to occur inside an anchor -->
  <b>
    <xsl:apply-templates select="title" mode="textonly"/>
    <xsl:for-each select="subt">
      <xsl:text>: </xsl:text>
      <xsl:apply-templates/>
    </xsl:for-each>
  </b>
  <xsl:text> (</xsl:text>
  <xsl:for-each select="author/surname">
    <xsl:if test="position() &gt; 1">
      <xsl:text>, </xsl:text>
    </xsl:if>
    <xsl:apply-templates/>
  </xsl:for-each>
  <xsl:text>)</xsl:text>
</xsl:template>

<!-- START GENERATING THE HTML PAGE                                -->
<xsl:template match="/">
  <html>
    <head>
      <meta name="ROBOTS" content="NOINDEX, NOFOLLOW"></meta>
      <title>
        <xsl:apply-templates select="/paper/front/title | /paperstub/title" mode="textonly"/>
      </title>
      <base name="{$paperlabel}"/>
      <link rel="stylesheet" href="{concat($rootdir, '/',$cssfile)}" type="text/css"/>
    </head>
    <body>

<!-- POP THE LINKS TO PAPER INDEXES UP TOP                         -->
      <xsl:call-template name="indexlinkbar"/>

<!-- ADD A CONSISTENT HEADER                                       -->
      <xsl:call-template name="masthead"/>

<!-- ADD THE NAVIGATION LINKS                                      -->
      <xsl:call-template name="navbar"/>

<!-- ...Now the content...                                         -->
      <xsl:apply-templates/>

<!-- ADD THE NAVIGATION LINKS TO THE BOTTOM OF THE PAGE AS WELL    -->
      <xsl:call-template name="indexlinkbar"/>
      <xsl:call-template name="logocenter"/>
    </body>
  </html>
</xsl:template>

<xsl:template match="paper">
  <xsl:apply-templates select="front|body"/>
  <xsl:call-template name="footnotes"/>
  <xsl:apply-templates select="rear"/>
  <hr/>
  <xsl:apply-templates select="front/title" mode="rear" />
  <xsl:apply-templates select="front/author" mode="rear" />
  <hr/>
</xsl:template>

<xsl:template match="paperstub">
  <xsl:apply-templates select="title | subt | author"/>
  <h3><i>This paper is not represented in the conference proceedings.</i></h3>
</xsl:template>

<xsl:template match="front">
  <xsl:apply-templates select="title|subt"/>
  <xsl:apply-templates select="author"/>
  <xsl:apply-templates select="keywords"/>
  <xsl:apply-templates select="abstract"/>
  <xsl:apply-templates select="copyright"/>
</xsl:template>

<xsl:template match="front/title | paperstub/title">
  <h1>
    <xsl:if test="../@id">
      <a name="../@id"/>
    </xsl:if>
    <xsl:apply-templates/>
    <xsl:if test="$thispaper/@latebreaking='yes'">
      <i> [late breaking]</i>
    </xsl:if>
    <xsl:if test="$thispaper/@poster='yes'">
      <i> [poster]</i>
    </xsl:if>
  </h1>
</xsl:template>

<xsl:template match="front/subt | paperstub/subt">
  <h2>
    <xsl:apply-templates/>
  </h2>
</xsl:template>

<xsl:template match="front/title" mode="rear">
  <p class="footertitle">
    <xsl:if test="../@id">
      <a name="../@id"/>
    </xsl:if>
    <xsl:apply-templates/>
  </p>
</xsl:template>

<xsl:template match="subt"/>

<xsl:template match="author">
  <xsl:if test="preceding-sibling::author">
    <br class="br"/>
  </xsl:if>
  <address>
    <xsl:apply-templates select="fname"/>
    <xsl:text> </xsl:text>
    <xsl:apply-templates select="surname"/>
    <xsl:text> [</xsl:text>
    <xsl:apply-templates select="jobtitle"/>
    <xsl:if test="jobtitle">
      <xsl:text>; </xsl:text>
    </xsl:if>
    <xsl:apply-templates select="address"/>
    <xsl:text>]</xsl:text>
  </address>
</xsl:template>

<xsl:template match="author" mode="rear">
  <!--
  <xsl:if test="preceding-sibling::author">
    <br/>
  </xsl:if> -->
  <address>
    <xsl:apply-templates select="fname|surname" mode="rear"/>
    <xsl:text>[</xsl:text>
    <xsl:apply-templates select="jobtitle|address/affil|address/subaffil" mode="rear"/>
    <xsl:text>]</xsl:text>
    <xsl:apply-templates select="address/email" mode="rear"/>
  </address>
</xsl:template>

<xsl:template match="jobtitle|affil" mode="rear">
  <xsl:apply-templates/>
  <xsl:if test="following-sibling::address/affil or following-sibling::subaffil">
    <xsl:text>, </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="fname|surname" mode="rear">
  <xsl:apply-templates/>
  <xsl:text> </xsl:text>
</xsl:template>

<xsl:template match="email" mode="rear">
  <br class="br"/>
  <a href="mailto:{.}" class="mailto">
    <xsl:apply-templates/>
  </a>
</xsl:template>

<xsl:template match="jobtitle">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="address">
  <xsl:apply-templates select="affil|subaffil"/>
<!--  <xsl:apply-templates select="aline|city|state|province|cntry|postcode|phone|fax|email|web"/> -->
</xsl:template>

<xsl:template match="affil">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="aline|city|cntry|province">
  <xsl:text>, </xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="subaffil">
  <xsl:text>, </xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="state|postcode">
  <xsl:text> </xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="phone">
  <br class="br"/><xsl:text>tel: </xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="fax">
  <br class="br"/><xsl:text>fax: </xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="email">
  <br class="br"/>
  <a href="mailto:{.}">
    <xsl:apply-templates/>
  </a>
</xsl:template>

<xsl:template match="web">
  <br class="br"/>
  <a href="{.}" target="_blank">
    <xsl:apply-templates/>
  </a>
</xsl:template>

<xsl:template match="abstract"/>

<xsl:template match="keywords"/>

<xsl:template match="copyright">
  <h4><i>
    <xsl:apply-templates/>
  </i></h4>
</xsl:template>

<!-- BODY CONTENT ELEMENTS                                         -->

<xsl:template match="section|subsec1|subsec2|subsec3">
  <div class="local-name()">
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template name="marksectitle">
  <xsl:variable name="numstring">
    <xsl:number level="multiple" format="1.1.1.1" count="section|subsec1|subsec2|subsec3"/>
  </xsl:variable>
  <a>
    <xsl:attribute name="name">
      <xsl:choose>
        <xsl:when test="../@id">
          <xsl:value-of select="../@id"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="concat('t', $numstring)"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
  </a>
  <xsl:if test="$secnumbers = 1">
    <xsl:value-of select="concat($numstring, ' ')"/>
  </xsl:if>
</xsl:template>

<xsl:template match="section/title">
  <h2>
    <xsl:call-template name="marksectitle"/>
    <xsl:apply-templates/>
  </h2>
</xsl:template>

<xsl:template match="subsec1/title">
  <h3>
    <xsl:call-template name="marksectitle"/>
    <xsl:apply-templates/>
  </h3>
</xsl:template>

<xsl:template match="subsec2/title">
  <h4>
    <xsl:call-template name="marksectitle"/>
    <xsl:apply-templates/>
  </h4>
</xsl:template>

<xsl:template match="subsec3/title">
  <h5>
    <xsl:call-template name="marksectitle"/>
    <xsl:apply-templates/>
  </h5>
</xsl:template>

<xsl:template match="para">
  <p>
    <xsl:apply-templates/>
  </p>
</xsl:template>

<xsl:template match="note">
  <div class="note">
    <h5>NOTE: </h5>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="figure">
  <div class="figure">
    <xsl:if test="@id">
      <a name="{@id}"/>
    </xsl:if>
    <h5>Figure <xsl:number count="figure" level="any"/>
      <xsl:if test="title">
        <xsl:text>: </xsl:text>
      </xsl:if>
      <xsl:apply-templates select="title"/>
    </h5>
    <xsl:apply-templates select="graphic|verbatim|para" mode="figure"/>
    <xsl:apply-templates select="caption"/>
  </div>
</xsl:template>

<xsl:template match="caption">
  <div style="font-size:85%">
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="verbatim" mode="figure">
  <pre>
    <xsl:apply-templates/>
  </pre>
</xsl:template>

<xsl:template match="para" mode="figure">
  <p>
    <xsl:apply-templates/>
  </p>
</xsl:template>


<xsl:template match="graphic" mode="figure">
  <xsl:variable name="absfilename">
    <xsl:value-of select="unparsed-entity-uri(@figname)"/>
  </xsl:variable>
  <xsl:variable name="relfilename">
    <xsl:call-template name="trimURI">
      <xsl:with-param name="URIstring" select="$absfilename"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="$relfilename">
      <img src="{$relfilename}" border="0"/>
      <h5>[Link to <a href="{$relfilename}" target="{$relfilename}">open this graphic in a separate page</a>]</h5>
    </xsl:when>
    <xsl:otherwise>
      <h5>
        <i>(Please declare graphic entity)</i>
      </h5>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="lquote">
  <div class="lquote">
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template name="trimURI">
  <!-- trims a URI to just what follows the final '/' character
       (i.e. should be just a relative path to a file in the same subdirectory) -->
  <xsl:param name="URIstring" select="''"/>
  <xsl:choose>
    <xsl:when test="contains($URIstring, '/')">
      <xsl:call-template name="trimURI">
        <xsl:with-param name="URIstring" select="substring-after($URIstring, '/')"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$URIstring"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="randlist">
  <xsl:choose>
    <xsl:when test="@style='simple'">
      <dl>
        <xsl:apply-templates/>
      </dl>
    </xsl:when>
    <xsl:otherwise>
      <ul>
        <xsl:if test="@style='dashed'">
          <!-- dashed lists get squares ... too bad -->
          <xsl:attribute name="type">
            <xsl:text>square</xsl:text>
          </xsl:attribute>
        </xsl:if>
        <xsl:apply-templates/>
      </ul>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="seqlist">
  <ol>
    <xsl:attribute name="type">
      <xsl:choose>
        <xsl:when test="ualpha">A</xsl:when>
        <xsl:when test="uroman">I</xsl:when>
        <xsl:when test="lalpha">a</xsl:when>
        <xsl:when test="lroman">i</xsl:when>
        <xsl:otherwise>1</xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
    <xsl:apply-templates/>
  </ol>
</xsl:template>

<xsl:template match="li">
  <xsl:choose>
    <xsl:when test="parent::randlist[@style='simple']">
      <dd>
        <xsl:apply-templates/>
      </dd>
    </xsl:when>
    <xsl:otherwise>
      <li>
        <xsl:apply-templates/>
      </li>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="li/para">
  <xsl:apply-templates/>
  <xsl:if test="following-sibling::*">
    <br class="br"/>
  </xsl:if>
</xsl:template>

<xsl:template match="deflist">
  <xsl:apply-templates select="title"/>
    <table border="0" cellpadding="8" class="deflist">
      <xsl:if test="term.heading|def.heading">
        <tr>
          <th>
            <xsl:apply-templates select="term.heading"/>
          </th>
          <th>
            <xsl:apply-templates select="def.heading"/>
          </th>
        </tr>
      </xsl:if>
      <xsl:apply-templates select="def.item"/>
    </table>
</xsl:template>

<xsl:template match="deflist/title">
  <h4>
    <xsl:apply-templates/>
  </h4>
</xsl:template>

<xsl:template match="def.item">
  <tr>
    <xsl:apply-templates/>
  </tr>
</xsl:template>

<xsl:template match="def.term|def">
  <td>
    <xsl:apply-templates/>
  </td>
</xsl:template>

<!-- Inline elements -->

<xsl:template match="expansion">
  <!-- we don't need to match acronym.grp or acronym, just
       pass them through -->
  <xsl:text> [</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>]</xsl:text>
</xsl:template>

<xsl:template match="bibref">
  <b><span style="font-size:85%"><a href="#{@refloc}" name="from{@refloc}">
    <xsl:text>[</xsl:text>
    <xsl:value-of select="id(@refloc)/bib"/>
    <xsl:text>]</xsl:text>
  </a></span></b>
</xsl:template>

<xsl:template match="inline.graphic">
  <xsl:variable name="absfilename">
    <xsl:value-of select="unparsed-entity-uri(@figname)"/>
  </xsl:variable>
  <xsl:variable name="relfilename">
    <xsl:call-template name="trimURI">
      <xsl:with-param name="URIstring" select="$absfilename"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="$relfilename">
      <img src="{$relfilename}" border="0"/>
      <h5>[Link to <a href="{$relfilename}" target="{$relfilename}">open this graphic in a separate page</a>]</h5>
    </xsl:when>
    <xsl:otherwise>
      <b>
        <i>(Please declare graphic entity)</i>
      </b>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="cit">
 <i>
   <xsl:apply-templates/>
 </i>
</xsl:template>

<xsl:template match="fnref">
  <xsl:variable name="ftnote" select="id(@refloc)"/>
  <xsl:variable name="noteno">
    <xsl:for-each select="$ftnote">
      <xsl:number level="any"/>
    </xsl:for-each>
  </xsl:variable>
  <sup>
    <span class="highlight">
      <a href="#to{generate-id($ftnote)}">
        <xsl:value-of select="$noteno"/>
      </a>
    </span>
  </sup>
</xsl:template>

<xsl:template match="ftnote">
  <xsl:variable name="noteno">
    <xsl:number level="any"/>
  </xsl:variable>
  <sup>
    <span class="highlight">
      <a href="#to{generate-id()}" name="from{generate-id()}">
          <xsl:value-of select="$noteno"/>
        </a>
    </span>
  </sup>
</xsl:template>

<xsl:template match="ftnote" mode="notes">
  <xsl:variable name="noteno">
    <xsl:number level="any" format="1."/>
  </xsl:variable>
  <tr>
    <td valign="top" width="10%" align="right">
    <a href="#from{generate-id()}" name="to{generate-id()}">
      <b><xsl:value-of select="$noteno"/></b>
    </a>
    </td>
    <td>
      <xsl:apply-templates/>
    </td>
  </tr>
</xsl:template>

<xsl:template match="sgml">
  <tt class="code">
    <xsl:apply-templates/>
  </tt>
</xsl:template>

<xsl:template match="sgml.block|verbatim">
  <div class="codeblock">
    <pre>
      <xsl:apply-templates/>
    </pre>
  </div>
</xsl:template>

<xsl:template match="figure/verbatim|figure/sgml.block">
  <pre>
    <xsl:apply-templates/>
  </pre>
</xsl:template>

<xsl:template match="xref">
  <a href="#{@refloc}">
    <xsl:apply-templates select="id(@refloc)" mode="xreftext">
      <xsl:with-param name="type" select="@type"/>
    </xsl:apply-templates>
  </a>
</xsl:template>

<xsl:template match="section|subsec1|subsec2|subsec3" mode="xreftext">
  <xsl:param name="type" select="'title'"/>
  <xsl:choose>
    <xsl:when test="$type='title' or not(/paper/@secnumbers='1')">
      <xsl:text>&ldquo;</xsl:text>
        <xsl:apply-templates select="title" mode="inline"/>
      <xsl:text>&rdquo;</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <!-- xsl:text>Section </xsl:text -->
      <xsl:number level="multiple" format="1.1.1.1" count="section|subsec1|subsec2|subsec3"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="figure" mode="xreftext">
  <xsl:param name="type" select="'title'"/>
  <xsl:choose>
    <xsl:when test="$type='title' or not(/paper/@secnumbers='1')">
      <xsl:text>&ldquo;</xsl:text>
      <xsl:apply-templates select="title" mode="inline"/>
      <xsl:text>&rdquo;</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <!-- xsl:text>Figure </xsl:text -->
      <xsl:number level="any"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="highlight[@style='bold']">
  <b>
    <xsl:apply-templates/>
  </b>
</xsl:template>

<xsl:template match="highlight[@style='ital']">
  <i>
    <xsl:apply-templates/>
  </i>
</xsl:template>

<xsl:template match="highlight[@style='under']">
  <u>
    <xsl:apply-templates/>
  </u>
</xsl:template>

<xsl:template match="highlight[@style='roman']">
  <span style="font-style:normal; font-weight:normal">
    <xsl:apply-templates/>
  </span>
</xsl:template>

<xsl:template match="highlight[@style='bital']">
  <i>
    <b>
      <xsl:apply-templates/>
    </b>
  </i>
</xsl:template>

<xsl:template match="sub">
  <sub>
    <xsl:apply-templates/>
  </sub>
</xsl:template>

<xsl:template match="super">
  <sup>
    <xsl:apply-templates/>
  </sup>
</xsl:template>

<!-- inline mode allows processing of titles and such in line -->

<xsl:template match="title" mode="inline">
  <xsl:apply-templates/>
</xsl:template>

<!-- Back matter and bibliography elements -->

<xsl:template name="footnotes">
  <xsl:if test="$footnotes">
    <h3 class="footnotes">Notes</h3>
    <table class="footnotes">
      <xsl:apply-templates select="$footnotes" mode="notes"/>
    </table>
  </xsl:if>
</xsl:template>

<xsl:template match="rear">
  <hr/>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="acknowl">
  <h3><i>
    <xsl:text>Acknowledgments</xsl:text>
  </i></h3>
  <xsl:apply-templates/>
  <hr/>
</xsl:template>

<xsl:template match="bibliog">
  <h3><i>
    <xsl:text>Bibliography</xsl:text>
  </i></h3>
  <xsl:apply-templates>
    <xsl:sort select="bib"/>
  </xsl:apply-templates>
  <hr/>
</xsl:template>

<xsl:template match="bibitem">
  <p>
    <xsl:apply-templates/>
  </p>
</xsl:template>

<xsl:template match="bib">
  <b><a name="{../@id}">
    <xsl:if test="//bibref/@refloc=../@id">
      <xsl:attribute name="href">
        <xsl:value-of select="concat('#from', ../@id)"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:text>[</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>] </xsl:text>
  </a></b>
</xsl:template>

<xsl:template match="pub">
  <xsl:text> </xsl:text>
  <xsl:apply-templates/>
</xsl:template>

</xsl:stylesheet>

<!-- ==============  END EXTREME.XSL STYLESHEET  ================  -->
