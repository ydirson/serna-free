<!DOCTYPE xsl:stylesheet [
<!ENTITY ldquo '&#x201C;' >
<!ENTITY rdquo '&#x201D;' >
<!ENTITY mdash '&#x2014;' >
]>

<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                >

<!-- For conversion of Extreme Markup Languages conference papers
     to FO for print.
     by Wendell Piez, Mulberry Technologies
     July, 2001

     Revisions made for 2003 processing made, July 2003
       * ripped out proprietary configurations (Saxon, FOP etc.)
       * made some top-level variables into parameters
         for tweaking at run time, if desired
       * tweaks to drive formatting a bit more consistently
         and conformantly

     Revisions made for 2002 processing made, June 2002
       * updated Saxon/FOP configuration
       * updated FO master-name attributes to master-reference
       * many tweaks and enhancements
       * installed Norm Walsh's DocBook OASIS tables XSL
-->

<!--xsl:import href="table.db-fo.xsl"/-->

<xsl:variable name="rootdir" select="'../..'"/>

<xsl:param name="blockbgcolor" select="'burlywood'"/>

<xsl:param name="mainbgcolor" select="'#FFFFFF'"/>

<xsl:param name="specialfacecolor" select="'darkred'"/>

<xsl:param name="extremelogo" select="concat($rootdir, '/icons/ex03.jpg')"/>

<xsl:param name="conference">
  <xsl:choose>
    <xsl:when test="paper/front/conference|gcapaper/front/conference">
      <xsl:value-of select="paper/front/conference|gcapaper/front/conference"/>
    </xsl:when>
    <xsl:otherwise>Extreme Markup Languages</xsl:otherwise>
  </xsl:choose>
</xsl:param>

<xsl:param name="venue"
  select="'Montr&#xE9;al, Qu&#xE9;bec'"/>

<xsl:param name="dates"
  select="'August 6-9, 2002'"/>

<xsl:param name="creditsblocks">
  <fo:block
    font-style="italic"
    font-size="85%">
    <xsl:text>This paper was formatted from XML source via XSL</xsl:text>
  </fo:block>
  <fo:block
    font-style="italic"
    font-size="85%">
    <xsl:text>by Mulberry Technologies, Inc.</xsl:text>
  </fo:block>
</xsl:param>

<xsl:output method="xml" indent="no"/>

<xsl:strip-space elements="gcapaper paper front author address bio
   abstract keywords body section subsec1 subsec2 subsec3
   rear acknowl bibliog bibitem lquote note figure caption
   randlist seqlist li deflist def.item def acronym.grp ftnote
   table thead tgroup row"/>

<xsl:variable name="secnumbers" select="/gcapaper/@secnumbers"/>
<!-- the value should be an integer. 0 (the default) means
     no section numbering. An empty node set results if
     no DTD is available to provide a default; the tests
     (below) will account for this. -->

<xsl:variable name="footnotes" select="//ftnote"/>

<xsl:variable name="authors" select="/gcapaper/front/author"/>

<!-- FO parameters -->

<xsl:param name="bodymargin" select="'1in'"/>

<xsl:param name="rightmargin" select="'0.5in'"/>

<xsl:param name="startmargin" select="'0.25in'"/>
<!-- This is the left margin for all text but the headers -->

<xsl:param name="headeroffset" select="'0.25in'"/>

<!-- <xsl:param name="mainfontsize" select="'10pt'"/>

<xsl:param name="mainlineheight" select="'12pt'"/>

<xsl:param name="smallfontsize" select="'9pt'"/>

<xsl:param name="smalllineheight" select="'11.5pt'"/>
 -->
<xsl:param name="mainfontsize" select="'9.5pt'"/>

<xsl:param name="mainlineheight" select="'11pt'"/>

<xsl:param name="smallfontsize" select="'8.5pt'"/>

<xsl:param name="smalllineheight" select="'11pt'"/>

<xsl:param name="mainindent" select="'15pt'"/>

<xsl:param name="sectiontopspace" select="'9pt'"/>

<xsl:param name="listtopspace" select="'12pt'"/>

<xsl:param name="extraindent" select="'24pt'"/>

<xsl:param name="hangingindent" select="concat('-', $mainindent)"/>

<xsl:param name="defoffset" select="'1.5in'"/>

<!-- end FO parameters -->


<xsl:template match="/">
  <fo:root>

<fo:layout-master-set>
  <fo:simple-page-master master-name="first"
     page-height="11in"
     page-width="8.5in"
     margin-top="0.5in"
     margin-bottom="0.5in"
     margin-left="0.5in"
     margin-right="0.5in">
    <fo:region-body region-name="main"
      display-align="center"
      margin-top="36pt"
      margin-bottom="0.75in"
      margin-left="{$bodymargin}"
      margin-right="{$bodymargin}"/>
    <fo:region-before
        extent="0.75in"/>
    <fo:region-after region-name="footer"
        extent="0.8in"/>
  </fo:simple-page-master>
  <fo:simple-page-master master-name="recto"
       page-height="11in"
       page-width="8.5in"
       margin-top="0.25in"
       margin-bottom="0.25in"
       margin-left="1in"
       margin-right="0.5in">
    <fo:region-body region-name="main"
       margin-top="48pt" margin-bottom="48pt"
       margin-left="{$bodymargin}"
       margin-right="{$rightmargin}"/>
    <fo:region-before region-name="rectoheader"
        extent="0.5in"/>
    <fo:region-after region-name="rectofooter"
        extent="24pt"/>
  </fo:simple-page-master>
  <fo:simple-page-master master-name="verso"
       page-height="11in"
       page-width="8.5in"
       margin-top="0.25in"
       margin-bottom="0.25in"
       margin-left="0.5in"
       margin-right="1in">
    <fo:region-body region-name="main"
       margin-top="48pt" margin-bottom="48pt"
       margin-left="{$bodymargin}"
       margin-right="{$rightmargin}"/>
    <fo:region-before region-name="versoheader"
        extent="0.5in"/>
    <fo:region-after region-name="versofooter"
        extent="24pt"/>
    </fo:simple-page-master>
    <fo:page-sequence-master master-name="title">
      <fo:repeatable-page-master-reference master-reference="first"/>
    </fo:page-sequence-master>
    <fo:page-sequence-master master-name="body">
      <fo:repeatable-page-master-alternatives>
        <fo:conditional-page-master-reference odd-or-even="odd"
          master-reference="recto"/>
        <fo:conditional-page-master-reference odd-or-even="even"
          master-reference="verso"/>
      </fo:repeatable-page-master-alternatives>
    </fo:page-sequence-master>
</fo:layout-master-set>

<xsl:apply-templates/>

  </fo:root>
</xsl:template>

<xsl:template match="gcapaper|paper">
  <!-- First, the title page -->
  <fo:page-sequence master-reference="title">
    <fo:static-content flow-name="footer">
      <fo:block
        display-align="after"
        text-align="center">
        <fo:external-graphic
          overflow="visible"
          content-width="108pt"
          src="{$extremelogo}"/>
      </fo:block>
    </fo:static-content>
    <fo:flow flow-name="main"
        font-size="{$mainfontsize}"
        line-height="{$mainlineheight}"
        font-family="serif">
      <fo:block>
        <xsl:apply-templates select="front"/>
      </fo:block>
    </fo:flow>
  </fo:page-sequence>
  <fo:page-sequence master-reference="body"
      initial-page-number="1">
    <fo:static-content flow-name="rectoheader">
      <fo:block text-align="end"
        font-size="10pt"
        font-family="sans-serif"
        font-style="italic"
        line-height="14pt" >
        <xsl:apply-templates select="/gcapaper/front/title|/paper/front/title" mode="textonly"/>
      </fo:block>
    </fo:static-content>
    <fo:static-content flow-name="versoheader">
      <fo:block text-align="start"
        font-size="10pt"
        font-family="sans-serif"
        font-style="italic"
        line-height="14pt" >
        <xsl:call-template name="authorheader"/>
      </fo:block>
    </fo:static-content>
    <fo:static-content flow-name="rectofooter">
      <fo:block
        padding-before="6pt"
        xsl:use-attribute-sets="borderbefore">
        <fo:list-block
          provisional-distance-between-starts="2.75in"
          provisional-label-separation="0pt">
          <fo:list-item>
            <fo:list-item-label end-indent="label-end()">
              <fo:block
                font-size="10pt"
                font-family="sans-serif">
                <xsl:value-of select="$conference"/>
              </fo:block>
            </fo:list-item-label>
            <fo:list-item-body
              start-indent="body-start()">
              <fo:block text-align="end"
                font-size="10pt"
                font-family="sans-serif">
                <xsl:text>page </xsl:text>
                <fo:page-number/>
                <!-- <xsl:text> of </xsl:text>
                <fo:page-number-citation ref-id="footer"/>
                <xsl:text> </xsl:text> -->
              </fo:block>
            </fo:list-item-body>
          </fo:list-item>
        </fo:list-block>
      </fo:block>
    </fo:static-content>
    <fo:static-content flow-name="versofooter">
      <fo:block
        padding-before="6pt"
        xsl:use-attribute-sets="borderbefore">
        <fo:list-block
          provisional-distance-between-starts="4.75in"
          provisional-label-separation="0pt">
          <fo:list-item>
            <fo:list-item-label end-indent="label-end()">
              <fo:block
                font-size="10pt">
                <xsl:text>page </xsl:text>
                <fo:page-number/>
                <!-- <xsl:text> of </xsl:text>
                <fo:page-number-citation ref-id="footer"/>
                <xsl:text> </xsl:text> -->
              </fo:block>
            </fo:list-item-label>
            <fo:list-item-body
              start-indent="body-start()">
              <fo:block text-align="end"
                font-size="10pt"
                font-weight="bold">
                <xsl:value-of select="$conference"/>
              </fo:block>
            </fo:list-item-body>
          </fo:list-item>
        </fo:list-block>
      </fo:block>
    </fo:static-content>
    <fo:flow flow-name="main"
        font-size="{$mainfontsize}"
        line-height="{$mainlineheight}"
        font-family="serif">
      <fo:block>
        <xsl:apply-templates select="body"/>
        <xsl:call-template name="footnotes"/>
        <xsl:call-template name="rear"/>
      </fo:block>
    </fo:flow>
  </fo:page-sequence>
</xsl:template>

<xsl:attribute-set name="borderbefore">
  <xsl:attribute name="border-before-width">thin</xsl:attribute>
  <xsl:attribute name="border-before-style">solid</xsl:attribute>
  <xsl:attribute name="border-before-color">black</xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="borderafter">
  <xsl:attribute name="border-after-width">thin</xsl:attribute>
  <xsl:attribute name="border-after-style">solid</xsl:attribute>
  <xsl:attribute name="border-after-color">black</xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="boxborder">
  <xsl:attribute name="border-before-width">thin</xsl:attribute>
  <xsl:attribute name="border-before-style">solid</xsl:attribute>
  <xsl:attribute name="border-before-color">
    <xsl:value-of select="$specialfacecolor"/>
  </xsl:attribute>
  <xsl:attribute name="border-start-width">thin</xsl:attribute>
  <xsl:attribute name="border-start-style">solid</xsl:attribute>
  <xsl:attribute name="border-start-color">
    <xsl:value-of select="$specialfacecolor"/>
  </xsl:attribute>
  <xsl:attribute name="border-end-width">thin</xsl:attribute>
  <xsl:attribute name="border-end-style">solid</xsl:attribute>
  <xsl:attribute name="border-end-color">
    <xsl:value-of select="$specialfacecolor"/>
  </xsl:attribute>
  <xsl:attribute name="border-after-width">thin</xsl:attribute>
  <xsl:attribute name="border-after-style">solid</xsl:attribute>
  <xsl:attribute name="border-after-color">black</xsl:attribute>
</xsl:attribute-set>

<xsl:template match="ftnote" mode="textonly"/>

<!-- end page setup -->

<!-- front matter elements -->

<xsl:template match="front">
  <fo:block font-family="sans-serif">
    <xsl:call-template name="masthead"/>
    <fo:block xsl:use-attribute-sets="headercontrol">
      <xsl:apply-templates select="title|subt" mode="titlepage"/>
    </fo:block>
    <xsl:call-template name="authorblock"/>
    <xsl:apply-templates select="keywords"/>
    <xsl:apply-templates select="abstract"/>
    <xsl:call-template name="contents"/>
  </fo:block>
</xsl:template>

<xsl:attribute-set name="borderafter">
  <xsl:attribute name="border-after-width">thin</xsl:attribute>
  <xsl:attribute name="border-after-style">solid</xsl:attribute>
  <xsl:attribute name="border-after-color">black</xsl:attribute>
</xsl:attribute-set>

<xsl:template match="front/title" mode="titlepage">
  <fo:block
    text-align="center"
    font-size="200%"
    line-height="120%"
    space-before="20pt">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="front/subt" mode="titlepage">
  <fo:block
    text-align="center"
    font-size="180%"
    font-style="italic"
    line-height="120%">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template name="authorblock">
  <fo:table
    width="100%"
    space-before="15pt"
    padding-before="10pt"
    space-after="15pt"
    padding-after="10pt"
    font-size="10pt"
    line-height="11.5pt"
    xsl:use-attribute-sets="borderbefore borderafter">
    <fo:table-body>
      <xsl:for-each select="author[boolean(position() mod 2)]">
        <fo:table-row>
          <fo:table-cell
            padding-before="3pt"
            padding-after="3pt"
            padding-end="0.25in"
            margin-left="0in"
            text-align="left">
            <xsl:if test="not(following-sibling::author[1])">
              <xsl:attribute name="number-columns-spanned">2</xsl:attribute>
            </xsl:if>
            <xsl:apply-templates select="." mode="titlepage"/>
          </fo:table-cell>
          <xsl:for-each select="following-sibling::author[1]">
            <fo:table-cell
              padding-before="3pt"
              padding-after="3pt"
              padding-start="0.25in"
              margin-right="0in"
              text-align="right">
              <xsl:apply-templates select="." mode="titlepage"/>
            </fo:table-cell>
          </xsl:for-each>
        </fo:table-row>
      </xsl:for-each>
    </fo:table-body>
  </fo:table>
</xsl:template>

<xsl:template match="author" mode="titlepage">
  <fo:block>
    <xsl:apply-templates select="fname"/>
    <xsl:text>&#160;</xsl:text>
    <xsl:apply-templates select="surname"/>
  </fo:block>
  <xsl:apply-templates select="address/affil" mode="titlepage"/>
</xsl:template>

<xsl:template match="affil" mode="titlepage">
  <fo:block font-style="italic">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="keywords"/>

<!-- xsl:template match="keywords" are being suppressed ... >
  <fo:list-block
    provisional-distance-between-starts="{$defoffset}"
    provisional-label-separation="9pt"
    space-before.minimum="{$sectiontopspace}">
    <fo:list-item
      space-after="6pt">
      <fo:list-item-label
        end-indent="label-end()"
        font-style="italic">
        <fo:block
          text-align="end"
          font-size="12pt"
          font-weight="bold">
          <xsl:text>Keywords</xsl:text>
        </fo:block>
      </fo:list-item-label>
      <fo:list-item-body
        start-indent="body-start()">
        <fo:block>
          <xsl:for-each select="*">
            <xsl:apply-templates/>
            <xsl:if test="position() &lt; last()"> &mdash; </xsl:if>
          </xsl:for-each>
        </fo:block>
      </fo:list-item-body>
    </fo:list-item>
  </fo:list-block>
</xsl:template -->

<xsl:template match="abstract">
  <fo:block
    space-before="{$sectiontopspace}"
    font-size="9pt"
    line-height="11.5pt"
    background-color="{$blockbgcolor}"
    padding-start="6pt"
    padding-end="6pt"
    padding-before="6pt"
    padding-after="6pt"
    margin-left="0.5in"
    margin-right="0.5in" >
    <fo:block
      keep-with-next="always"
      font-size="12pt"
      font-weight="bold"
      font-style="italic"
      space-after="6pt">Abstract</fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template name="masthead">
  <fo:block
    background-color="{$blockbgcolor}"
    font-family="sans-serif">
    <fo:list-block
      provisional-distance-between-starts="3.75in"
      provisional-label-separation="9pt"
      margin-left="4pt"
      margin-right="4pt"
      margin-top="6pt"
      margin-bottom="6pt">
      <fo:list-item
        font-weight="bold">
        <fo:list-item-label
          end-indent="label-end()">
          <fo:block
            font-size="140%"
            line-height="120%"
            font-style="italic">
            <xsl:value-of select="$conference"/>
          </fo:block>
        </fo:list-item-label>
        <fo:list-item-body
          font-size="10pt"
          start-indent="body-start()">
          <fo:block text-align="end">
            <xsl:value-of select="$venue"/>
          </fo:block>
          <fo:block text-align="end">
            <xsl:value-of select="$dates"/>
          </fo:block>
        </fo:list-item-body>
      </fo:list-item>
    </fo:list-block>
  </fo:block>
</xsl:template>

<xsl:template name="contents">
<!-- context node is <front> -->
  <fo:block
    break-before="page"
    space-before.minimum="1in"
    font-size="10pt"
    font-family="serif"
    line-height="13pt"
    padding-before="6pt"
    padding-after="6pt" >
    <!-- had:
    keep-together.within-page="always"
    -->
    <fo:block xsl:use-attribute-sets="headercontrol">
      <xsl:apply-templates select="title|subt" mode="toc"/>
    </fo:block>
    <fo:block
      keep-with-next="always"
      font-family="sans-serif"
      font-size="12pt"
      font-weight="bold"
      font-style="italic"
      space-after="6pt">Table of Contents</fo:block>
    <xsl:apply-templates select="../body/section"  mode="toc"/>
    <xsl:if test="$footnotes">
      <fo:block
        margin-left="{$mainindent}"
        text-indent="{concat('-', $mainindent)}"
        text-align-last="justify">
        <xsl:text>Footnotes</xsl:text>
        <fo:leader leader-pattern="dots"/>
        <fo:page-number-citation ref-id="footnotes"/>
      </fo:block>
    </xsl:if>
    <xsl:apply-templates select="../rear" mode="toc"/>
    <fo:block
      margin-left="{$mainindent}"
      text-indent="{concat('-', $mainindent)}"
      text-align-last="justify">
      <xsl:text>The Author</xsl:text>
      <xsl:if test="count($authors) > 1">s</xsl:if>
      <fo:leader leader-pattern="dots"/>
      <fo:page-number-citation ref-id="authors"/>
    </fo:block>
  </fo:block>
</xsl:template>

<xsl:template match="front/title" mode="toc">
  <fo:block
    font-size="160%"
    line-height="140%">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="front/subt" mode="toc">
  <fo:block
    font-size="140%"
    line-height="120%"
    font-style="italic">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="title" mode="toc">
  <xsl:if test="$secnumbers = 1">
    <xsl:number level="multiple" format="1.1.1.1" count="section|subsec1|subsec2|subsec3"/>
    <xsl:text> </xsl:text>
  </xsl:if>
  <fo:basic-link internal-destination="{generate-id(..)}">
    <xsl:apply-templates mode="toc"/>
  </fo:basic-link>
  <fo:leader leader-pattern="dots"/>
  <fo:page-number-citation ref-id="{generate-id(..)}"/>
</xsl:template>

<xsl:template match="section | subsec1 | subsec2 | subsec3" mode="toc">
  <fo:block
    margin-left="{$mainindent}"
    text-indent="{concat('-', $mainindent)}"
    text-align-last="justify">
    <xsl:apply-templates select="title | subsec1 | subsec2 | subsec3" mode="toc"/>
  </fo:block>
</xsl:template>

<xsl:template match="fnref | ftnote | xref | bibref" mode="toc"/>

<xsl:template match="acronym.grp" mode="toc">
  <xsl:apply-templates select="acronym"/>
</xsl:template>

<xsl:template match="*" mode="toc">
  <!-- if an element is called in this mode without a template,
       fall back into regular processing -->
  <xsl:apply-templates select="."/>
</xsl:template>

<xsl:template match="rear" mode="toc">
  <xsl:apply-templates mode="toc"/>
</xsl:template>

<xsl:template match="acknowl" mode="toc">
  <fo:block
    margin-left="{$mainindent}"
    text-indent="{concat('-', $mainindent)}"
    text-align-last="justify">
    <xsl:text>Acknowledgements</xsl:text>
    <fo:leader leader-pattern="dots"/>
    <fo:page-number-citation ref-id="acknowl"/>
  </fo:block>
</xsl:template>

<xsl:template match="bibliog" mode="toc">
  <fo:block
    margin-left="{$mainindent}"
    text-indent="{concat('-', $mainindent)}"
    text-align-last="justify">
    <xsl:text>Bibliography</xsl:text>
    <fo:leader leader-pattern="dots"/>
    <fo:page-number-citation ref-id="bibliog"/>
  </fo:block>
</xsl:template>


<!-- end front matter elements -->

<!-- body elements -->

<xsl:template match="body">
  <xsl:call-template name="bodyheader"/>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template name="bodyheader">
  <fo:block xsl:use-attribute-sets="headercontrol">
    <xsl:apply-templates select="../front/title|../front/subt"/>
  </fo:block>
  <xsl:call-template name="authorline"/>
</xsl:template>

<xsl:template match="front/title">
  <fo:block
    id="{generate-id()}"
    font-size="200%"
    line-height="120%">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="front/subt">
  <fo:block
    id="{generate-id()}"
    font-size="160%"
    font-style="italic"
    line-height="120%">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template name="authorline">
  <fo:block
    font-family="sans-serif"
    font-size="125%"
    line-height="125%"
    font-style="italic">
    <xsl:for-each select="../front/author">
      <xsl:if test="not(position()=1) and not(position()=last() and position()=2)">
        <xsl:text>,</xsl:text>
      </xsl:if>
      <xsl:if test="not(position()=1) and position()=last()">
        <xsl:text> and</xsl:text>
      </xsl:if>
      <xsl:if test="not(position()=1)">
        <xsl:text> </xsl:text>
      </xsl:if>
      <xsl:apply-templates select="fname"/>
      <xsl:text>&#160;</xsl:text>
      <xsl:apply-templates select="surname"/>
    </xsl:for-each>
    <fo:footnote>
      <fo:inline/>
      <fo:footnote-body>
        <fo:block
          font-family="serif"
          font-size="75%"
          line-height="100%"
          font-style="normal"
          margin-left="-{$bodymargin}">
        <fo:block>
          <fo:leader
            leader-length="2in"
            rule-thickness="0.2mm"
            leader-pattern="rule"
            rule-style="solid"/>
        </fo:block>
        <fo:block
          margin-top="3pt">
          <xsl:value-of select="/gcapaper/front/copyright|/paper/front/copyright"/>
        </fo:block>
        </fo:block>
      </fo:footnote-body>
    </fo:footnote>

  </fo:block>
</xsl:template>
<!-- Header elements -->

<xsl:attribute-set name="headercontrol">
  <xsl:attribute name="font-family">sans-serif</xsl:attribute>
  <xsl:attribute name="font-weight">bold</xsl:attribute>
  <xsl:attribute name="keep-with-next.within-page">always</xsl:attribute>
  <xsl:attribute name="space-before.optimum">
    <xsl:value-of select="$sectiontopspace"/>
  </xsl:attribute>
  <xsl:attribute name="space-before.minimum">
    <xsl:value-of select="$sectiontopspace"/>
  </xsl:attribute>
  <xsl:attribute name="space-after.optimum">6pt</xsl:attribute>
  <xsl:attribute name="space-after.minimum">6pt</xsl:attribute>
  <xsl:attribute name="space-after.maximum">9pt</xsl:attribute>
  <xsl:attribute name="color">
    <xsl:value-of select="$specialfacecolor"/>
  </xsl:attribute>
</xsl:attribute-set>

<xsl:template name="sectionhead">
  <xsl:param name="headsize" select="'100%'"/>
  <xsl:param name="headstyle" select="'normal'"/>
  <fo:list-block
    id="{generate-id(..)}"
    margin-left="{concat('-', $headeroffset)}"
    provisional-distance-between-starts="{$headeroffset}"
    provisional-label-separation="6pt"
    font-size="{$headsize}"
    line-height="{$headsize}"
    font-style="{$headstyle}"
    xsl:use-attribute-sets="headercontrol">
    <xsl:attribute name="space-before">
      <xsl:choose>
        <!-- extra whitespace if there's nothing before the first section -->
        <xsl:when test="not(../preceding-sibling::*)">12pt</xsl:when>
        <xsl:otherwise>6pt</xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
    <fo:list-item keep-with-next.within-page="always">
      <fo:list-item-label
        end-indent="label-end()">
        <xsl:call-template name="marksectitle"/>
      </fo:list-item-label>
      <fo:list-item-body
        start-indent="body-start()">
        <fo:block
          margin-left="{$mainindent}"
          text-indent="{$hangingindent}">
          <fo:wrapper font-style="normal">
            <xsl:if test="$secnumbers = 1">
              <xsl:number level="multiple" format="1.1.1.1 " count="section|subsec1|subsec2|subsec3"/>
            </xsl:if>
          </fo:wrapper>
          <xsl:apply-templates/>
        </fo:block>
      </fo:list-item-body>
    </fo:list-item>
  </fo:list-block>
</xsl:template>

<xsl:template name="marksectitle">
    <fo:block
      text-align="end"
      font-style="normal">
      <xsl:choose>
        <xsl:when test="parent::section">
          <xsl:text>&#xA7;</xsl:text><!-- a "section" sign -->
        </xsl:when>
        <xsl:when test="parent::subsec1"/>
        <!-- (having decided not to place markers on lower-level headers) -->
        <xsl:otherwise/>
      </xsl:choose>
      <!-- <xsl:text>&#x2756;</xsl:text> an x-diamond (font-family="ZapfDingbats") -->
      <!-- <xsl:text>&#x25C6;</xsl:text> a black diamond -->
      <!-- <xsl:text>&#x27A4;</xsl:text> a right-pointing arrow -->
      <!-- <xsl:text>&#x25D7;</xsl:text> a black half-circle -->
      <!-- <xsl:text>&#xB7;</xsl:text> a bullet -->
      <!-- <xsl:text>&#x275A;</xsl:text> a lefthand block -->
      <!-- <xsl:text>&#xB6;</xsl:text> a pilcrow -->
    </fo:block>
</xsl:template>

<xsl:template match="section/title">
  <xsl:call-template name="sectionhead">
    <xsl:with-param name="headsize">125%</xsl:with-param>
  </xsl:call-template>
</xsl:template>

<xsl:template match="subsec1/title">
  <xsl:call-template name="sectionhead">
    <xsl:with-param name="headsize">110%</xsl:with-param>
    <xsl:with-param name="headstyle">italic</xsl:with-param>
  </xsl:call-template>
</xsl:template>

<xsl:template match="subsec2/title">
  <xsl:call-template name="sectionhead">
  </xsl:call-template>
</xsl:template>

<xsl:template match="subsec3/title">
  <xsl:call-template name="sectionhead">
    <xsl:with-param name="headsize">90%</xsl:with-param>
    <xsl:with-param name="headstyle">italic</xsl:with-param>
  </xsl:call-template>
</xsl:template>

<!-- end header elements -->

<!-- regular body elements -->

<xsl:attribute-set name="indentedblock">
  <xsl:attribute name="margin-left">
    <xsl:value-of select="$mainindent"/>
  </xsl:attribute>
  <xsl:attribute name="margin-right">
    <xsl:value-of select="$mainindent"/>
  </xsl:attribute>
  <xsl:attribute name="padding-start">6pt</xsl:attribute>
  <xsl:attribute name="padding-end">6pt</xsl:attribute>
  <xsl:attribute name="space-before">6pt</xsl:attribute>
  <xsl:attribute name="space-after">6pt</xsl:attribute>
</xsl:attribute-set>

<xsl:template match="note|lquote">
  <fo:block
    font-size="95%"
    xsl:use-attribute-sets="indentedblock">
     <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="para">
  <fo:block>
    <!-- initial paragraphs might get extra space before (but don't) -->
    <xsl:attribute name="space-before">
      <xsl:choose>
        <xsl:when test="not(preceding-sibling::para)">6pt</xsl:when>
        <xsl:otherwise>6pt</xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
    <!-- initial paragraphs might get an indent, but don't
    <xsl:attribute name="text-indent">
      <xsl:choose>
        <xsl:when test="not(preceding-sibling::para)">
          <xsl:value-of select="$extraindent"/>
        </xsl:when>
        <xsl:otherwise>0in</xsl:otherwise>
      </xsl:choose>
    </xsl:attribute> -->
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="li/para|note/para|def/para|lquote/para|figure/para|caption/para|ftnote/para">
  <!-- inside certain block elements, paras never have space before -->
  <fo:block text-indent="0pt">
    <xsl:if test="preceding-sibling::*">
      <xsl:attribute name="space-before">4pt</xsl:attribute>
    </xsl:if>
    <xsl:if test="preceding-sibling::* and not(following-sibling::*)">
      <!-- stops there from being an orphan para -->
      <xsl:attribute name="keep-with-previous">always</xsl:attribute>
    </xsl:if>
    <xsl:if test="parent::note and not(preceding-sibling::*)">
      <fo:wrapper font-style="italic"
        font-size="90%">NOTE: </fo:wrapper>
    </xsl:if>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="randlist|seqlist">
  <xsl:apply-templates select="title"/>
  <fo:list-block
    margin-left="0pt"
    space-before="6pt"
    space-after="6pt"
    provisional-distance-between-starts="{$extraindent}"
    provisional-label-separation="6pt">
    <!-- margin-right="{$mainindent}" -->
    <xsl:apply-templates select="*[not(self::title)]"/>
  </fo:list-block>
</xsl:template>

<xsl:template match="seqlist/title | randlist/title">
  <fo:block
      font-weight="bold"
      xsl:use-attribute-sets="headercontrol">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>


<xsl:template match="li">
  <fo:list-item padding-before="4pt">
    <xsl:if test="not(preceding-sibling::li)">
      <xsl:attribute name="keep-with-next.within-page">always</xsl:attribute>
    </xsl:if>
    <xsl:if test="not(following-sibling::li)">
      <xsl:attribute name="keep-with-previous.within-page">always</xsl:attribute>
    </xsl:if>
    <fo:list-item-label end-indent="label-end()">
      <fo:block text-align="end">
        <xsl:choose>
          <xsl:when test="parent::randlist">
            <xsl:choose>
              <xsl:when test="../@style='bulleted'">&#x2022;</xsl:when>
              <xsl:when test="../@style='dashed'">-</xsl:when>
              <xsl:otherwise/> <!-- otherwise is simple: no label -->
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
              <xsl:otherwise> <!-- otherwise is arabic (default) -->
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

<xsl:template match="sgml.block|verbatim">
  <fo:block
    white-space="pre"
    linefeed-treatment="preserve"
    font-family="monospace"
    padding-before="6pt"
    padding-after="3pt"
    font-size="85%"
    line-height="100%"
    xsl:use-attribute-sets="indentedblock">
      <xsl:attribute name="margin-left">
        <xsl:choose>
          <xsl:when test="parent::figure">
            <!-- had allowed extra whitespace, but lines come out too short -->
            <xsl:text>0pt</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$mainindent"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:attribute>
      <xsl:attribute name="margin-right">
        <xsl:choose>
          <xsl:when test="parent::figure">
            <xsl:text>6pt</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>-</xsl:text>
            <xsl:value-of select="$rightmargin"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:attribute>
      <xsl:attribute name="background-color">
        <xsl:choose>
          <xsl:when test="parent::figure">
            <xsl:value-of select="$mainbgcolor"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$blockbgcolor"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:attribute>
     <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="deflist/title">
  <fo:block
      margin-left="{$defoffset}"
      space-after="6pt"
      xsl:use-attribute-sets="headercontrol">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="deflist">
  <fo:block space-before="6pt">
    <xsl:apply-templates select="title"/>
    <fo:list-block
      space-after="6pt"
      provisional-distance-between-starts="{$defoffset}"
      provisional-label-separation="9pt">
      <xsl:if test="term.heading">
        <fo:list-item
          space-before="3pt"
          font-weight="bold">
            <xsl:apply-templates select="term.heading"/>
          <fo:list-item-body
            start-indent="body-start()">
            <xsl:apply-templates select="def.heading"/>
          </fo:list-item-body>
        </fo:list-item>
      </xsl:if>
      <xsl:apply-templates select="def.item"/>
    </fo:list-block>
</fo:block>
</xsl:template>

<xsl:template match="def.item">
  <fo:list-item
    space-after="6pt">
    <xsl:if test="not(preceding-sibling::def.item)">
      <xsl:attribute name="keep-with-next.within-page">always</xsl:attribute>
    </xsl:if>
    <xsl:if test="not(following-sibling::def.item)">
      <xsl:attribute name="keep-with-previous.within-page">always</xsl:attribute>
    </xsl:if>
    <xsl:apply-templates/>
  </fo:list-item>
</xsl:template>

<xsl:template match="term.heading|def.term">
  <fo:list-item-label
    end-indent="label-end()">
    <fo:block text-align="end">
      <xsl:apply-templates/>
    </fo:block>
  </fo:list-item-label>
</xsl:template>

<xsl:template match="def.heading">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="def">
  <fo:list-item-body
    start-indent="body-start()">
    <xsl:apply-templates/>
  </fo:list-item-body>
</xsl:template>

<!-- inline elements -->

<xsl:template match="acronym">
  <fo:wrapper font-variant="small-caps">
    <xsl:apply-templates/>
  </fo:wrapper>
</xsl:template>

<xsl:template match="expansion">
  <xsl:text> [</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>]</xsl:text>
</xsl:template>

<xsl:template match="cit">
 <fo:wrapper font-style="italic">
   <xsl:apply-templates/>
 </fo:wrapper>
</xsl:template>

<xsl:template match="sgml">
  <fo:wrapper
    font-size="95%"
    font-family="monospace">
    <xsl:apply-templates/>
  </fo:wrapper>
</xsl:template>

<xsl:template match="highlight[@style='bold']">
  <fo:wrapper font-weight="bold" font-size="95%">
    <xsl:apply-templates/>
  </fo:wrapper>
</xsl:template>

<xsl:template match="highlight[@style='ital']">
  <fo:wrapper font-style="italic">
    <xsl:apply-templates/>
  </fo:wrapper>
</xsl:template>

<xsl:template match="highlight[@style='under']">
  <fo:wrapper text-decoration="underline">
    <xsl:apply-templates/>
  </fo:wrapper>
</xsl:template>

<xsl:template match="highlight[@style='rom']">
  <fo:wrapper font-style="normal" font-weight="normal">
    <xsl:apply-templates/>
  </fo:wrapper>
</xsl:template>

<xsl:template match="highlight[@style='bital']">
  <fo:wrapper font-style="italic" font-weight="bold">
    <xsl:apply-templates/>
  </fo:wrapper>
</xsl:template>

<xsl:template match="sub">
  <fo:wrapper font-size="70%"
    baseline-shift="sub">
    <xsl:apply-templates/>
  </fo:wrapper>
</xsl:template>

<xsl:template match="super">
  <fo:wrapper baseline-shift="super"
             font-size="70%">
    <!-- vertical-align maybe should be baseline-shift,
         but FOP 0.19 doesn't do it -->
    <xsl:apply-templates/>
  </fo:wrapper>
</xsl:template>

<xsl:template match="sym">
  <fo:wrapper font-family="'Lucida Sans Unicode'" font-style="normal">
    <xsl:apply-templates/>
  </fo:wrapper>
</xsl:template>

<xsl:template match="figure">
  <!-- the default is to float a figure; if its @anchor attribute
       is set to 'fixed' it will match the next template -->
  <fo:float
    id="{generate-id()}"
  >
    <xsl:attribute name="float">
      <xsl:choose>
        <xsl:when test="@anchor='fixed'">none</xsl:when>
        <xsl:otherwise>before</xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>

  <fo:block
    xsl:use-attribute-sets="borderbefore borderafter"
    margin-top="{$sectiontopspace}"
    margin-bottom="{$sectiontopspace}"
    padding="6pt"
    font-size="{$smallfontsize}"
    line-height="{$smalllineheight}"
    font-family="serif"
>
    <!-- had:
    keep-together.within-page="always"
    background-color="{$blockbgcolor}"
    padding="6pt"
    space-before="6pt"
    space-after="6pt"
    margin-bottom="36pt"
    padding-left="6pt"
    padding-top="6pt"
    padding-bottom="6pt"
    -->
    <fo:block
      font-size="85%"
      font-weight="bold"
      space-before="6pt"
      keep-with-next.within-page="always"
      xsl:use-attribute-sets="headercontrol">
      <xsl:text/>Figure <xsl:number level="any"/>
      <xsl:apply-templates select="title"/>
    </fo:block>
    <xsl:apply-templates select="graphic|verbatim|para"/>
    <xsl:apply-templates select="caption"/>
  </fo:block>
  <fo:block space-before="6pt"/>
  <!-- a workaround because the formatters aren't doing all the white space -->
  </fo:float>
</xsl:template>

<xsl:template match="figure/title">
  <xsl:text>: </xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="caption">
  <fo:block
    font-size="85%">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="graphic">
  <xsl:variable name="filename">
    <xsl:choose>
      <xsl:when test="@figname">
        <xsl:value-of select="unparsed-entity-uri(@figname)"/>
      </xsl:when>
      <xsl:when test="@href">
        <xsl:value-of select="@href"/>
      </xsl:when>
    <!-- this fallback is for preprocessed files in which
         the graphic file reference has been resolved and
         placed on a "file" attribute -->
      <xsl:otherwise>
        <xsl:value-of select="@file"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <fo:block
    margin-top="{$sectiontopspace}"
    margin-bottom="{$sectiontopspace}">
    <fo:external-graphic src="{$filename}" max-width="336pt">
      <xsl:for-each select="@scale">
        <xsl:attribute name="content-width">
          <xsl:value-of select="."/>
        </xsl:attribute>
      </xsl:for-each>
    </fo:external-graphic>
  </fo:block>
</xsl:template>

<xsl:template match="inline.graphic">
  <xsl:variable name="filename">
    <xsl:value-of select="unparsed-entity-uri(@figname)"/>
    <!-- <xsl:call-template name="makefilename">
      <xsl:with-param name="filename" select="unparsed-entity-uri(@figname)"/>
    </xsl:call-template> -->
  </xsl:variable>
  <fo:wrapper>
    <!--  <xsl:value-of select="@figname"/> ... <xsl:text/>
    <xsl:value-of select="unparsed-entity-uri(@figname)"/> ... <xsl:text/>
    <xsl:value-of select="$filename"/> ... <xsl:text/> -->
    <fo:external-graphic src="{$filename}" background-color="white"/>
  </fo:wrapper>
</xsl:template>

<!--
<xsl:template match="inline.graphic">
  <b>
    <xsl:text>[Graphic entity </xsl:text>
    <xsl:value-of select="@figname"/>
    <xsl:if test="unparsed-entity-uri(@figname)">
      <xsl:text/>(<xsl:value-of select="unparsed-entity-uri(@figname)"/>)<xsl:text/>
    </xsl:if>
    <xsl:text>]</xsl:text>
  </b>
</xsl:template>
-->


<xsl:template name="makefilename">
  <xsl:param name="filename" select="''"/>
  <!-- commented the old code out since we don't have to do
       the filename massaging: now it just returns the parameter -->
  <xsl:value-of select="$filename"/>

<!-- we may not need this stuff any more -->
  <!-- takes a .gif filename, makes a .jpg filename -->
  <!-- because of the strictness of Xalan/FOP's handling of URI syntax,
       a horrible recursion must take place in order to strip the name down
       to just the relative URI -->
  <xsl:choose>
    <xsl:when test="contains($filename, '/')">
    <xsl:call-template name="makefilename">
      <xsl:with-param name="filename" select="substring-after($filename, '/')"/>
    </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="concat('file:../graphics/', substring($filename, 1, (string-length($filename) - 3)), 'jpg')"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<xsl:template name="tabletitle">
  <!-- Generates a table title, including its number -->
    <xsl:text/>Table <xsl:number level="any"/>
    <xsl:if test="title">
      <xsl:text>: </xsl:text>
    </xsl:if>
    <xsl:apply-templates select="title" mode="inline"/>
</xsl:template>

<!-- Table processing happens in the imported module
     table.db-fo.xsl (DocBook / OASIS table module
     by Norman Walsh) -->

<!-- cross-references, footnotes and other such beasties -->

<!-- We'll use keys instead of IDs just in case we need
     DTD-free processing (which we do, sometimes) -->

<xsl:key name="reftargets" match="*[@id]" use="@id"/>

<xsl:key name="bibitems-by-id" match="bibitem" use="@id"/>

<xsl:key name="footnotes-by-id" match="ftnote" use="@id"/>
<!-- could be using reftargets for all these, but this is cleaner
     (they will fail if link is broken; we can write exception-
      handling code if we want) -->


<xsl:template match="bibref">
  <fo:wrapper>
    <xsl:text>[</xsl:text>
    <xsl:value-of select="key('bibitems-by-id', @refloc)/bib"/>
    <xsl:text>]</xsl:text>
  </fo:wrapper>
</xsl:template>

<xsl:template match="xref">
  <xsl:apply-templates select="key('reftargets', @refloc)" mode="xreftext">
    <xsl:with-param name="type" select="@type"/>
    <xsl:with-param name="caller" select="."/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template match="section|subsec1|subsec2|subsec3" mode="xreftext">
  <xsl:param name="type" select="false()"/>
  <xsl:param name="caller" select="."/>
  <fo:basic-link internal-destination="{generate-id()}" color="{$specialfacecolor}">
    <xsl:choose>
      <xsl:when test="$type='title' or not(/gcapaper/@secnumbers='1' or /paper/@secnumbers='1')">
        <xsl:text>&ldquo;</xsl:text>
        <xsl:apply-templates select="title" mode="inline"/>
        <xsl:text>&rdquo;</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <!-- xsl:choose>
          <xsl:when test="string($caller)">
            <xsl:value-of select="$caller"/>
          </xsl:when>
          <xsl:otherwise>Section </xsl:otherwise>
        </xsl:choose -->
        <xsl:number level="multiple" format="1.1.1.1" count="section|subsec1|subsec2|subsec3"/>
      </xsl:otherwise>
    </xsl:choose>
    <!-- <xsl:call-template name="makerefstring">
      <xsl:with-param name="caller" select="$caller"/>
    </xsl:call-template> -->
  </fo:basic-link>
</xsl:template>

<xsl:template name="makerefstring">
  <!-- generates the string [above] or [below] depending on whether the
       $caller parameter is before or after the context node
       $caller = the xref
       the context node = its target -->
  <xsl:param name="caller" select="."/>
  <!-- super-fancy code to insert "below" or "above" is suppressed
  <xsl:choose>
    <xsl:when test="count($caller|preceding::*|ancestor::*) = count(preceding::*|ancestor::*)">
      <!- - the link is before its target - ->
      <xsl:text> [below]</xsl:text>
    </xsl:when>
    <xsl:when test="count($caller|following::*|descendant::*) = count(following::*|descendant::*)">
      <!- - the link is after its target - ->
      <xsl:text> [above]</xsl:text>
    </xsl:when>
    <xsl:otherwise/><!- - otherwise the xref is to itself - ->
  </xsl:choose>
  -->
</xsl:template>

<xsl:template match="title" mode="inline">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="figure" mode="xreftext">
  <xsl:param name="type" select="false()"/>
  <xsl:param name="caller" select="."/>
  <fo:basic-link internal-destination="{generate-id()}" color="{$specialfacecolor}">
    <xsl:choose>
      <xsl:when test="$type='title'">
        <xsl:text>&ldquo;</xsl:text>
        <xsl:value-of select="title"/>
        <xsl:text>&rdquo;</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <!-- xsl:choose>
          <xsl:when test="string($caller)">
            <xsl:value-of select="$caller"/>
          </xsl:when>
          <xsl:otherwise>Figure </xsl:otherwise>
        </xsl:choose -->
        <xsl:number level="any"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:call-template name="makerefstring">
      <xsl:with-param name="caller" select="$caller"/>
    </xsl:call-template>
  </fo:basic-link>
</xsl:template>

<xsl:template match="ftnote">
  <xsl:variable name="noteno">
    <xsl:number level="any"/>
  </xsl:variable>
  <fo:inline baseline-shift="25%"
             font-size="60%"
             line-height="60%">
    <xsl:value-of select="$noteno"/>
  </fo:inline>
  <!-- fo:footnote-body>
    <fo:list-block
      provisional-distance-between-starts="{$extraindent}"
      provisional-label-separation="6pt">
      <fo:list-item>
        <fo:list-item-label end-indent="label-end()">
          <fo:block text-align="end">
            <xsl:value-of select="$noteno"/>
          </fo:block>
        </fo:list-item-label>
        <fo:list-item-body
          start-indent="body-start()">
          <xsl:apply-templates/>
        </fo:list-item-body>
      </fo:list-item>
    </fo:list-block>
  </fo:footnote-body -->
</xsl:template>

<xsl:template match="fnref">
  <xsl:variable name="noteno">
    <xsl:for-each select="key('footnotes-by-id', @refloc)">
      <xsl:number level="any"/>
    </xsl:for-each>
  </xsl:variable>
  <fo:inline font-size="60%"
    baseline-shift="25%">
    <xsl:value-of select="$noteno"/>
  </fo:inline>
</xsl:template>

<!-- named template and moded templates for footnotes as end notes -->

<xsl:template name="footnotes">
  <xsl:if test="$footnotes">
    <fo:block
        id="footnotes"
        font-size="125%"
        xsl:use-attribute-sets="borderbefore headercontrol"
        padding-before="6pt"
        space-before.optimum="{$sectiontopspace}">
      <xsl:text>Notes</xsl:text>
    </fo:block>
    <fo:list-block
        margin-left="0pt"
        margin-right="{$mainindent}"
        space-before="4pt"
        space-after="4pt"
        provisional-distance-between-starts="{$extraindent}"
        provisional-label-separation="6pt">
      <xsl:apply-templates select="$footnotes" mode="notes"/>
    </fo:list-block>
  </xsl:if>
</xsl:template>

<xsl:template match="ftnote" mode="notes">
  <xsl:variable name="noteno">
    <xsl:number level="any" format="1."/>
  </xsl:variable>
  <fo:list-item space-after="6pt">
    <fo:list-item-label end-indent="label-end()">
      <fo:block text-align="end">
        <xsl:value-of select="$noteno"/>
      </fo:block>
    </fo:list-item-label>
    <fo:list-item-body
      start-indent="body-start()">
      <xsl:apply-templates/>
    </fo:list-item-body>
  </fo:list-item>
</xsl:template>

<xsl:template name="rear">
  <xsl:apply-templates select="rear"/>
  <xsl:call-template name="authorinfo"/>
  <xsl:call-template name="documentfooter"/>
</xsl:template>

<xsl:template match="acknowl">
  <fo:block
      id="acknowl"
      font-size="125%"
      padding-before="6pt"
      space-before.optimum="{$sectiontopspace}"
      xsl:use-attribute-sets="borderbefore headercontrol">
    <xsl:text>Acknowledgements</xsl:text>
  </fo:block>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="bibliog">
  <fo:block
      id="bibliog"
      font-size="125%"
      xsl:use-attribute-sets="borderbefore headercontrol"
      padding-before="6pt"
      space-before.optimum="{$sectiontopspace}">
    <xsl:text>Bibliography</xsl:text>
  </fo:block>
  <xsl:apply-templates>
    <xsl:sort select="bib"/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template match="bibitem">
  <fo:block
    keep-together="always"
    space-before="6pt"
    margin-left="{$mainindent}"
    text-indent="{$hangingindent}">
    <xsl:if test="position()=1">
      <xsl:attribute name="keep-with-next.within-page">always</xsl:attribute>
    </xsl:if>
    <xsl:if test="position()=last()">
      <xsl:attribute name="keep-with-previous.within-page">always</xsl:attribute>
    </xsl:if>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="bib">
  <fo:inline font-weight="bold">
    <xsl:text>[</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>] </xsl:text>
  </fo:inline>
</xsl:template>

<xsl:template match="pub">
  <xsl:text> </xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="email">
  <fo:wrapper font-style="italic">
    <fo:basic-link external-destination="mailto:{.}" color="{$specialfacecolor}">
      <xsl:apply-templates/>
    </fo:basic-link>
  </fo:wrapper>
</xsl:template>

<xsl:template match="web">
  <fo:wrapper font-weight="bold" font-size="90%">
    <fo:basic-link external-destination="{.}" color="{$specialfacecolor}">
      <xsl:apply-templates/>
    </fo:basic-link>
  </fo:wrapper>
</xsl:template>

<xsl:template name="authorinfo">
  <fo:block
      id="authors"
      font-size="125%"
      xsl:use-attribute-sets="borderbefore headercontrol"
      padding-before="6pt"
      space-before.optimum="{$sectiontopspace}">
    <xsl:text>The Author</xsl:text>
    <xsl:if test="count($authors) &gt; 1">s</xsl:if>
  </fo:block>
  <xsl:apply-templates select="$authors" mode="authorinfo"/>
</xsl:template>

<xsl:template match="author" mode="authorinfo">
  <fo:block
      keep-together.within-page="always"
      font-size="10pt"
      font-family="sans-serif"
      line-height="11.5pt"
      padding-before="6pt"
      padding-after="6pt">
    <fo:block
      font-weight="bold">
    <xsl:apply-templates select="fname"/>
    <xsl:text> </xsl:text>
    <xsl:apply-templates select="surname"/>
    </fo:block>
    <xsl:apply-templates select="jobtitle|address|bio"/>
  </fo:block>
</xsl:template>

<xsl:template match="jobtitle">
  <fo:block
    font-style="italic">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="address">
  <fo:block
    font-style="italic">
    <xsl:apply-templates select="affil|subaffil"/>
  </fo:block>
  <!-- it would be nice to have tighter display of addresses, but the loose
       content model can throw anything at all at us -->
  <xsl:apply-templates
    select="aline|city|state|province|cntry|postcode|phone|fax|email|web"/>
</xsl:template>

<xsl:template match="affil">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="subaffil">
  <xsl:text/>, <xsl:apply-templates/>
</xsl:template>

<xsl:template match="aline|city|cntry|province|state|postcode">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="phone">
  <fo:block>
    <xsl:text>tel: </xsl:text>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="fax">
  <fo:block>
    <xsl:text>fax: </xsl:text>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="address/email">
  <fo:block font-size="90%">
    <fo:basic-link external-destination="mailto:{.}" color="{$specialfacecolor}">
      <xsl:apply-templates/>
    </fo:basic-link>
  </fo:block>
</xsl:template>

<xsl:template match="address/web">
  <fo:block font-size="90%">
    <!-- font-weight="bold"> -->
    <fo:basic-link external-destination="{.}" color="{$specialfacecolor}">
      <xsl:apply-templates/>
    </fo:basic-link>
  </fo:block>
</xsl:template>

<xsl:template match="bio">
  <fo:block
    font-family="serif">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template name="documentfooter">
  <fo:block id="footer"
    background-color="{$blockbgcolor}"
    space-before.maximum="36pt"
    space-before.optimum="24pt"
    font-family="sans-serif"
    keep-together.within-page="always"
    keep-with-previous.within-page="always">
    <!-- Apparently gratuitous block keeps background from "spilling over" -->
    <fo:block
      padding-before="6pt"
      padding-after="6pt"
      padding-start="6pt"
      padding-end="6pt"
      text-align="center">
      <fo:block
        font-weight="bold">
        <xsl:value-of select="$conference"/>
      </fo:block>
      <fo:block
        font-size="85%">
        <xsl:value-of select="$venue"/>
        <xsl:text/>, <xsl:value-of select="$dates"/>
      </fo:block>
      <xsl:copy-of select="$creditsblocks"/>
    </fo:block>
  </fo:block>
</xsl:template>


<xsl:template name="authorheader">
  <xsl:for-each select="/paper/front/author|/gcapaper/front/author">
    <xsl:if test="not(position()=1) and not(position()=last() and position()=2)">
      <xsl:text>,</xsl:text>
    </xsl:if>
    <xsl:if test="not(position()=1) and position()=last()">
      <xsl:text> and</xsl:text>
    </xsl:if>
    <xsl:if test="not(position()=1)">
      <xsl:text> </xsl:text>
    </xsl:if>
    <xsl:apply-templates select="surname"/>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
