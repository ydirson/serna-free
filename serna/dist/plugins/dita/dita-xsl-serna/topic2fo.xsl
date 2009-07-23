<?xml version='1.0'?>

<!DOCTYPE xsl:transform [
<!-- entities for use in the generated output (must produce correctly in FO) -->
  <!ENTITY nbsp          "&#160;">

  <!ENTITY copyr         "&#xA9;">
  <!ENTITY trademark     "&#x2122;">
  <!ENTITY registered    "&#xAE;">

  <!ENTITY rbl           "&#160;">
  <!ENTITY quotedblleft  "&#x201C;">
  <!ENTITY quotedblright "&#x201D;">
  <!ENTITY bullet        "&#x2022;"><!--check these two for better assignments -->

]>

<!-- 13 Nov 2003 Don Day:
     This version of DITA FO stylesheets is based on the dita12 package FO stylesheet
     topic2fo.xsl, and has been revised specifically for the Serna FO-based
     editor.  

     For most authoring, use the omnibus dita2fo_shell.xsl as your main stylesheet.
     When authoring a specific DTD, its matching shell is appropriate, but the omnibus
     stylesheet supports ALL the standard infotypes by inclusion, so there is no
     practical difference one way or the other.

     Don Day, DITA Lead Architect
-->

<xsl:stylesheet version="1.0"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xse="http://syntext.com/Extensions/XSLT-1.0"
                xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
                extension-element-prefixes="dtm">
                
  <!-- Page setup - used by simple-master-set -->
  <xsl:param name="paper.type" select="'A4'"/>
  <xsl:param name="page.orientation" select="'portrait'"/>

  <xsl:param name="page.margin.bottom" select="'0cm'"/>
  <xsl:param name="page.margin.top" select="'0cm'"/>
  <xsl:param name="page.margin.inner">2cm</xsl:param>
  <xsl:param name="page.margin.outer">2cm</xsl:param>

  <xsl:param name="body.margin.bottom" select="'2cm'"/>
  <xsl:param name="body.margin.top" select="'2cm'"/>

  <xsl:param name="body.font.family" select="'Verdana, Geneva, sans-serif'"/>
  
  <xsl:include href="dita-page-setup.xsl"/>

  <!-- Whitespace stripping policy -->
  <xsl:strip-space elements="*"/>
  <xsl:preserve-space elements="pre p codeblock"/>

 <!-- Here we define default block properties. -->
 <xsl:attribute-set name="block.properties">
   <xsl:attribute name="border-bottom-width">0.5em</xsl:attribute>
 </xsl:attribute-set>

 <!-- Newline character (capture the native file newline) -->
<xsl:variable name="newline">
</xsl:variable>

<xsl:variable name="trace">no</xsl:variable>

<xsl:variable name="DITAEXT">dita</xsl:variable>
 
 <!-- ************ end of parameters setup *********** -->
<!-- Don Day:
     The hidden modules will be added back into the DITA authoring capability
     one by one. They need to be redone substantially from the print-oriented
     version because of very different processing issues between authoring views
     and final form.  From Serna's point of view, the issue is "How can you author
     something like metadata that is usually nulled out in print processing?"
-->

 <xsl:include href="dita-parms.xsl"/>
 <xsl:include href="dita-prolog.xsl"/>
 <xsl:include href="dita-titles.xsl"/>
 <xsl:include href="dita-elems.xsl"/>
 <xsl:include href="dita-lists.xsl"/>
 <xsl:include href="dita-links.xsl"/>
 <xsl:include href="dita-simpletable.xsl"/>
 <xsl:include href="dita-table.xsl"/>
 <xsl:include href="dita-subroutines.xsl"/>
 <xsl:include href="dita-conref.xsl"/>
<!--
 <xsl:include href="dita-diagnostics.xsl"/>
 <xsl:include href="dita-stubs.xsl"/>
 <xsl:include href="toc.xsl"/>
-->
 <xsl:include href="common/dita-utilities.xsl"/>
 <!--xsl:include href="dita-I18N.xsl"/-->




<!-- make yet-unsupported elements show up, and be editable in the meantime -->
<dtm:doc dtm:status="testing" dtm:idref="all.name"/>
<xsl:template match="*" dtm:id="all.name">
  <fo:block background-color="yellow">
    <fo:inline font-weight="bold">[<xsl:value-of select="name()"/>] </fo:inline>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="asterisk"/>
<xsl:template match="asterisk" dtm:id="asterisk">
<fo:block background-color="yellow">
    &lt;<xsl:value-of select="name()"/><xsl:text> </xsl:text>
    <xsl:for-each select="@*">
       <xsl:value-of select="name(@*)"/>="<xsl:value-of select="."/>"
    </xsl:for-each>&gt;
    <xsl:apply-templates/>
    &lt;/<xsl:value-of select="name()"/>&gt;
</fo:block>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="asterisk2"/>
<xsl:template match="asterisk2" dtm:id="asterisk2">
    <xsl:apply-templates/>
 </xsl:template>



<!-- null out some things relocated by modes -->


<!-- set up the root topic here -->
<!-- ( should we not presume this is the document element? ) -->
 
 <dtm:doc dtm:elements="topic|reference|task|concept|glossentry" dtm:status="finished" dtm:idref="topic.toplevel"/>
 <xsl:template match="*[contains(@class,' topic/topic ')]" dtm:id="topic.toplevel"><!-- mode="toplevel"-->
   <xsl:param name="conrefs-queue"/>
   <fo:block white-space-collapse="true">
     <!--fo:inline font-weight="bold">HERE!!!</fo:inline-->
     <!--xsl:apply-templates select="title" mode="doc.title"/-->
     <!-- Make a TOC.  -->
     <!--xsl:call-template name="make.toc"/-->
     <!-- Process the rest of the document.  -->
     <xsl:apply-templates>
       <xsl:with-param name="conrefs-queue" select="$conrefs-queue"/>
     </xsl:apply-templates>
   </fo:block>
 </xsl:template>

<!-- =============== start of contextual topic titles ================= -->

<!-- NESTED TOPIC CONTEXTS (child topics get a fo:block wrapper and fall through) -->
<dtm:doc dtm:elements="topic/topic" dtm:status="finished" dtm:idref="topic.contexttitle.firstlevel"/>
<xsl:template match="*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]" priority="2" dtm:id="topic.contexttitle.firstlevel">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="topic/topic/topic" dtm:status="finished" dtm:idref="topic.contexttitle.secondlevel"/>
<xsl:template match="*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]" priority="3" dtm:id="topic.contexttitle.secondlevel">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="topic/topic/topic/topic" dtm:status="finished" dtm:idref="topic.contexttitle.thirdlevel"/>
<xsl:template match="*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]" priority="4" dtm:id="topic.contexttitle.thirdlevel">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="topic/topic/topic/topic/topic" dtm:status="finished" dtm:idref="topic.contexttitle.forthlevel"/>
<xsl:template match="*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]" priority="5" dtm:id="topic.contexttitle.forthlevel">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="topic/topic/topic/topic/topic/topic" dtm:status="finished" dtm:idref="topic.contexttitle.fivthlevel"/>
<xsl:template match="*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]" priority="6" dtm:id="topic.contexttitle.fivthlevel">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>



<!-- =============== end of contextual topic titles ================= -->




<!-- ============== modes (out of line processing) ================ -->

<!-- provide fall through processing for title/desc pulled in table or figure contexts -->
<dtm:doc dtm:elements="title" dtm:status="finished" dtm:idref="topic.title.exhibittitle"/>
<xsl:template match="*[contains(@class,' topic/title ')]" mode="exhibittitle" dtm:id="topic.title.exhibittitle">
  <xsl:apply-templates/>
</xsl:template>

<dtm:doc dtm:elements="desc" dtm:status="finished" dtm:idref="topic.desc.exhibitdesc"/>
<xsl:template match="*[contains(@class,' topic/desc ')]" mode="exhibitdesc" dtm:id="topic.desc.exhibitdesc">
  <xsl:apply-templates/>
</xsl:template>

<!-- ============ end of modes (out of line processing) ============== -->


<!--  "FORMAT" MACROS  -->
<!--
 | These macros support globally-defined formatting constants for
 | document content.  Some elements have attributes that permit local
 | control of formatting; such logic is part of the pertinent template rule.
 +-->

<dtm:doc dtm:status="testing" dtm:idref="macros.placewidth"/>
<xsl:template name="place-tbl-width" dtm:id="macros.placewidth">
<xsl:variable name="twidth-fixed">100%</xsl:variable>
  <xsl:if test="$twidth-fixed != ''">
    <xsl:attribute name="width"><xsl:value-of select="$twidth-fixed"/></xsl:attribute>
  </xsl:if>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="macros.addposcontrols"/>
<xsl:template name="proc-ing" dtm:id="macros.addposcontrols">
  <xsl:if test="$do-place-ing = 'yes'"> <!-- set in a global variable, as with label placement, etc. -->
    <fo:external-graphic src="url(image/tip-ing.jpg)"/> <!-- this should be an xsl:choose with the approved list and a selection method-->
    <!-- add any other required positioning controls, if needed, but must be valid in the location
         from which the call to this template was made -->
    &nbsp;
  </xsl:if>
</xsl:template>

<!-- =================== end of processors  ====================== -->




<!--  NAMED TEMPLATES (call by name, only)  -->
<dtm:doc dtm:status="testing" dtm:idref="label.generate"/>
<xsl:template name="gen-att-label" dtm:id="label.generate">
  <xsl:if test="@spectitle">
    <fo:block margin-bottom="0" font-weight="bold">
    <xsl:value-of select="@spectitle"/></fo:block>
  </xsl:if>
</xsl:template>


<!-- named templates that can be used anywhere -->

<!-- this replaces newlines with the BR element, forcing non-concatenation even in flow contexts -->
<dtm:doc dtm:status="testing" dtm:idref="replace-br"/>
<xsl:template name="br-replace" dtm:id="replace-br">
  <xsl:param name="word"/>
<!-- capture an actual newline within the xsl:text element -->
  <xsl:variable name="cr"><xsl:text>
</xsl:text></xsl:variable>
  <xsl:choose>
    <xsl:when test="contains($word,$cr)">
       <xsl:value-of select="substring-before($word,$cr)"/>
       <!--br class="br"/-->
       <xsl:call-template name="br-replace">
         <xsl:with-param name="word" select="substring-after($word,$cr)"/>
       </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$word"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- diagnostic: call this to generate a path-like view of an element's ancestry! -->
<dtm:doc dtm:status="testing" dtm:idref="path.generate"/>
<xsl:template name="breadcrumbs" dtm:id="path.generate">
<xsl:variable name="full-path">
  <xsl:for-each select="ancestor-or-self::*">
    <xsl:value-of select="concat('/',name())"/>
  </xsl:for-each>
</xsl:variable>
<fo:block font-weight="bold"><xsl:value-of select="$full-path"/></fo:block>
</xsl:template>


<!-- the following named templates generate inline content for the delivery context -->


<!-- generate null filler if the phrase is evidently empty -->
<dtm:doc dtm:status="testing" dtm:idref="phrase.fill"/>
<xsl:template name="apply-for-phrases" dtm:id="phrase.fill">
  <xsl:choose>
    <xsl:when test="not(text()[normalize-space(.)] | *)"><!--xsl:comment>null</xsl:comment--></xsl:when>
    <xsl:otherwise><xsl:apply-templates/></xsl:otherwise>
  </xsl:choose>
</xsl:template>




<!-- render any contained footnotes as endnotes.  Links back to reference point -->
<!-- TBD: test to see if the footnote is in a draft-comment or required-cleanup context -->
<!--      if so, need to filter them out! -->
<!-- Basic reuse issue: if ANY addressable content within these elements IS referenced, it will
      be pulled and instanced. -->

<!-- not included:
-  gen-endnotes
-  gen-toc
-  gen-ptoc
-  gen-sect-ptoc
as these involved a lot of browser-specific stuff to clean up for FO
-->  

<!-- =================== start of diagnostics  ====================== -->
<!-- reenable this by including dita-diagnostics.xsl -->
<dtm:doc dtm:status="testing" dtm:idref="output-message"/> 
<xsl:template name="output-message" dtm:id="output-message"/>

<!-- =================== end of diagnostics  ====================== -->

<!-- adding in elements one by one -->

<dtm:doc dtm:elements="no-topic-nesting" dtm:status="finished" dtm:idref="no-topic-nesting"/>
<xsl:template match="*[contains(@class, ' topic/no-topic-nesting ')]" dtm:id="no-topic-nesting">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

</xsl:stylesheet>
