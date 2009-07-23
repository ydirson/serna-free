<?xml version="1.0" encoding="UTF-8" ?>

<!--  map2fo.xsl
 | DITA map support for the demo set; extend as needed
 |
 | (C) Copyright IBM Corporation 2001, 2002, 2003. All Rights Reserved.
 | This file is part of the DITA package on IBM's developerWorks site.
 | See license.txt for disclaimers.
 +
 | updates:
 *-->

<xsl:transform version="1.0"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
                extension-element-prefixes="dtm"
                xmlns:se="http://www.syntext.com/XSL/Format-1.0"                
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xse="http://www.syntext.com/Extensions/XSLT-1.0">
                
                 
  <!-- Page setup - used by simple-master-set -->
  <xsl:param name="paper.type" select="'A4'"/>
  <xsl:param name="page.orientation" select="'portrait'"/>

  <xsl:param name="page.margin.bottom" select="'0cm'"/>
  <xsl:param name="page.margin.top" select="'0cm'"/>
  <xsl:param name="page.margin.inner">2cm</xsl:param>
  <xsl:param name="page.margin.outer">2cm</xsl:param>

  <xsl:param name="body.margin.bottom" select="'2cm'"/>
  <xsl:param name="body.margin.top" select="'2cm'"/>

  <xsl:param name="body.font.family" select="'Helvetica'"/>
  
  <xsl:include href="dita-page-setup.xsl"/>

  <!-- Whitespace stripping policy -->
  <xsl:strip-space elements="*"/>
  <xsl:preserve-space elements="pre p codeblock"/>

<!-- map elements -->
<dtm:doc dtm:status="testing" dtm:idref="page.root"/>
<xsl:template match="/" priority="10" dtm:id="page.root">
    <xsl:call-template name="dita-page-root"/>
</xsl:template>

<!-- make yet-unsupported elements show up, and be editable in the meantime -->
<dtm:doc dtm:status="testing" dtm:idref="unsupelems.showup"/>
<xsl:template match="*" dtm:id="unsupelems.showup">
  <fo:block background-color="yellow">
    <fo:inline font-weight="bold">[<xsl:value-of select="name()"/>] </fo:inline>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="map" dtm:status="finished" dtm:idref="map.map"/>
<xsl:template match="*[contains(@class,' map/map ')]" dtm:id="map.map">
  <fo:block>
    <!--fo:block color="red">DITA map doctype type</fo:block -->
    <fo:block id="{generate-id()}">
      <xsl:if test="not(*[1])">
          <fo:block font-size="14pt" color="red">
              (Empty DITA Map. Please insert topic references below)
          </fo:block>
      </xsl:if>
      <xsl:if test="@title">
        <fo:block xsl:use-attribute-sets="topictitle1" color="gray">
          <xsl:apply-templates select="@title"/>
        </fo:block>
      </xsl:if>
      <xsl:apply-templates/>
    </fo:block>
  </fo:block>
</xsl:template>

<xsl:template match="*[contains(@class,' map/map ')]/@title" dtm:id="map.title">
  <xsl:value-of select="."/>
</xsl:template>

<dtm:doc dtm:elements="navref" dtm:status="finished" dtm:idref="map.navref"/>
<xsl:template match="*[contains(@class,' map/navref ')]" dtm:id="map.navref">
  <fo:block font-weight="bold">
    <xsl:if test="normalize-space(@mapref) != ''">
        <xsl:value-of select="@mapref"/>
    </xsl:if>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="anchor" dtm:status="finished" dtm:idref="map.anchor"/>
<xsl:template match="*[contains(@class,' map/anchor ')]" dtm:id="map.anchor">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<!-- this named template "paints" the topicref attributes in a standard layout -->
<dtm:doc dtm:status="testing" dtm:idref="topicref.data"/>
<xsl:template name="topicref-data" dtm:id="topicref.data">
    <xsl:param name="is-relcell" select='0'/>
  <xsl:variable name="sourcefile">
    <xsl:choose>
      <xsl:when test="contains(@href, '#')">
        <xsl:value-of select="substring-before(@href,'#')"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="@href"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="topicid"  select="substring-after(@href,'#')"/>
  <xsl:variable name="topicdoc2" 
                select="document($sourcefile,/)" 
                xse:document-mode="validate read-write"/>
  <xsl:variable name="topicdoc" select="($topicdoc2/*)"/>
    <xsl:choose>
     <xsl:when test="$SHOW-RESOLVED-TOPICREFS = 'yes'">
       <xsl:variable name="has-cycles">
         <xsl:call-template name="start-cycles-testing"/>
       </xsl:variable>
       <xsl:choose>
         <xsl:when test="$has-cycles">
           <xsl:variable name="error-message">
             <xsl:text>[cyclic reference: </xsl:text>
             <xsl:value-of select="@href"/>
             <xsl:text>]</xsl:text>
           </xsl:variable>
           <xsl:call-template name="show-conref-error">
             <xsl:with-param name="message" select="$error-message"/>
           </xsl:call-template>
         </xsl:when>
         <xsl:when test="$topicid">
           <xsl:choose>
             <xsl:when test="id($topicid, $topicdoc2)">
               <xsl:apply-templates select="id($topicid, $topicdoc2)"/>
             </xsl:when>
             <xsl:otherwise>
               <xsl:apply-templates select="id($topicid, $topicdoc)"/>
             </xsl:otherwise>
           </xsl:choose>
         </xsl:when>
         <xsl:otherwise>
           <xsl:if test="not($sourcefile='')">
             <xsl:apply-templates select="$topicdoc"/>
           </xsl:if>
         </xsl:otherwise>
       </xsl:choose>
     </xsl:when>
     <xsl:otherwise>
       <xsl:variable name="topicref-navtitle">
         <xsl:choose>
           <xsl:when test="@navtitle and (@locktitle='yes') and not(normalize-space(@navtitle)='')">
             <xsl:value-of select="@navtitle"/>
           </xsl:when>
           <xsl:when test="preceding-sibling::node()[1]
               [self::processing-instruction('docato-extra-info-title')]">
             <xsl:value-of select="preceding-sibling::node()[1]
               [self::processing-instruction('docato-extra-info-title')]"/>
           </xsl:when>
           <xsl:when test="$topicdoc">
             <xsl:choose>
               <xsl:when test="$topicid">
                 <xsl:variable name="topic" 
                               select="id($topicid, $topicdoc2)"/>
                 <xsl:choose>
                   <xsl:when test="$topic/*[contains(@class, 'topic/title')]">
                     <xsl:apply-templates select="$topic/*[contains(@class, 'topic/title')]"/>
                   </xsl:when>
                   <xsl:when test="$topic/@title">
                     <xsl:apply-templates select="$topic/@title"/>
                   </xsl:when>
                   <xsl:otherwise>
                     <xsl:text>(empty title)</xsl:text>
                   </xsl:otherwise>
                 </xsl:choose>
               </xsl:when>
               <xsl:otherwise>
                 <xsl:choose>
                   <xsl:when test="$topicdoc[contains(@class, ' map/map ')]">
                     <xsl:apply-templates select="$topicdoc/@title"/>
                   </xsl:when>
                   <xsl:otherwise>
                     <xsl:apply-templates select="($topicdoc/*[contains(@class, 'topic/title')]|$topicdoc/*/*[contains(@class, 'topic/title')])[1]|$topicdoc/*[contains(@class, 'map/map')]/@title"/>
                   </xsl:otherwise>
                 </xsl:choose>
               </xsl:otherwise>
             </xsl:choose>
           </xsl:when>
           <xsl:otherwise>
             <xsl:text>(empty title)</xsl:text>
           </xsl:otherwise>
         </xsl:choose>
       </xsl:variable>
       <xsl:choose>
        <xsl:when test="$is-relcell">
          <fo:block font-weight="bold">
            <xsl:value-of select="$topicref-navtitle"/>
          </fo:block>
          <fo:block color="blue" text-decoration="underline">
            <xsl:value-of select="@href"/>
          </fo:block>
         </xsl:when>
         <xsl:otherwise>
           <xsl:call-template name="topicref-table">
             <xsl:with-param name="navtitle" select="$topicref-navtitle"/>
             <xsl:with-param name="topicdoc" select="$topicdoc"/>
           </xsl:call-template>
         </xsl:otherwise>
       </xsl:choose> 
     </xsl:otherwise>
   </xsl:choose>
   <xsl:apply-templates select="*[not(self::processing-instruction('docato-extra-info-title'))]"/>
</xsl:template>

<xsl:template name="topicref-table">
  <xsl:param name="navtitle"/>
  <xsl:param name="topicdoc"/>
  <fo:table padding-top="1mm" border-width="0px">
    <fo:table-body border-width="0px">
      <fo:table-row border-width="0px">
        <fo:table-cell  border-width="0px">
          <fo:block font-weight="bold">
            <xsl:value-of select="$navtitle"/>
          </fo:block>
          <xsl:if test="$SHOW-MAP-DESCRIPTIONS='yes' 
                        and $topicdoc/shortdesc">
            <fo:block font-size="8pt" background-color="#CCFFCC"
                      start-indent="13pt" font-weight="bold">
              <xsl:value-of select="$topicdoc/shortdesc"/>
            </fo:block>
          </xsl:if>
        </fo:table-cell>
        <fo:table-cell border-width="0px">
          <fo:block text-align="right">
            <fo:inline color="blue" text-decoration="underline">
              <xsl:value-of select="@href"/>
            </fo:inline>
            <xsl:if test="@collection-type">
              <fo:inline color="green">
                <xsl:text>  (</xsl:text>
                <xsl:value-of select="@collection-type"/>
                <xsl:text>)</xsl:text>
              </fo:inline>
            </xsl:if>
            <xsl:if test="@linking">
              <fo:inline color="purple">
                <xsl:text>  (</xsl:text>
                <xsl:value-of select="@linking"/>
                <xsl:text>)</xsl:text>
              </fo:inline>
            </xsl:if>
            <xsl:apply-templates select="@type"/>
          </fo:block>
        </fo:table-cell>
      </fo:table-row>
    </fo:table-body>
  </fo:table>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="attribute.type"/>
<xsl:template match="@type" dtm:id="attribute.type">
  <fo:inline>
    <xsl:text> </xsl:text>
    <se:combo-box value="{string(.)}">
      <se:value>concept</se:value>
      <se:value>task</se:value>
      <se:value>reference</se:value>
      <se:value>topic</se:value>
      <se:value>section</se:value>
      <se:value>table</se:value>
      <se:value>fig</se:value>
      <se:value>li</se:value>
      <se:value>fn</se:value>
      <se:value></se:value>
    </se:combo-box>
  </fo:inline>
</xsl:template>

<!-- Because this template pulls content from a topic, we have to be prepared to process
     any possible content of a topic's shortdesc.
     The caller stylesheet already pulls in the domain processors. -->
<dtm:doc dtm:elements="shortdesc" dtm:status="finished" dtm:idref="topic.shortdesc"/>
<xsl:template match="*[contains(@class,' topic/shortdesc ')]" dtm:id="topic.shortdesc">
  <fo:block font-size="8pt"><!-- background-color="#CCFFCC"-->
  <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="keyword" dtm:status="finished" dtm:idref="topic.keyword"/>
<xsl:template match="*[contains(@class,' topic/keyword ')]" dtm:id="topic.keyword">
  <xsl:copy-of select="text()"/>
</xsl:template>

<dtm:doc dtm:elements="ph" dtm:status="finished" dtm:idref="topic.ph"/>
<xsl:template match="*[contains(@class,' topic/ph ')]" dtm:id="topic.ph">
  <fo:inline color="purple"><xsl:apply-templates/></fo:inline>
</xsl:template>

<dtm:doc dtm:elements="q" dtm:status="finished" dtm:idref="topic.q"/>
<xsl:template match="*[contains(@class,' topic/q ')]" dtm:id="topic.q">
  <fo:inline>"<xsl:apply-templates/>"</fo:inline>
</xsl:template>

<!-- <dtm:doc dtm:elements="xref" dtm:status="finished" dtm:idref="topic.xref"/>
<xsl:template match="*[contains(@class,' topic/xref ')]" dtm:id="topic.xref">
  <fo:inline color="blue" text-decoration="underlined"><xsl:apply-templates/></fo:inline>
</xsl:template> -->

<!-- these nested contexts create indention for topicref data -->
<dtm:doc dtm:elements="topicref|abbrevlist|bibliolist|booklist|figurelist|
                       tablelist|glossarylist|indexlist|toc|trademarklist|
                       draftintro|bookabstract|notices|dedication|colophon|
                       amendments" dtm:status="finished" dtm:idref="map.topicref"/>
<xsl:template match="*[contains(@class,' map/topicref ')]"
              name="process.topicref" dtm:id="map.topicref">
  <xsl:choose>
    <xsl:when test="string(@href) and (@format='dita' or @format='DITA'
                    or @format='ditamap' or @format='DITAMAP'
                    or not(@format) or string-length(@format)=0)">
      <xsl:choose>
        <xsl:when test="not(@href or @conref)">
          <fo:block>
            <xsl:apply-templates/>
          </fo:block>
        </xsl:when>
        <xsl:when test="ancestor::*[contains(@class, ' map/relcell ')]">
          <fo:block>
            <xsl:call-template name="topicref-data">
              <xsl:with-param name="is-relcell" select="1"/>
              <xsl:with-param name="refs-queue" select="$queue"/>
            </xsl:call-template>
          </fo:block>
        </xsl:when>
        <xsl:otherwise>
          <fo:block end-indent="6pt"
                    start-indent="{concat($basic-start-indent, '+', 
                                   count(ancestor::*) - 1, 'em')}">
            <xsl:call-template name="topicref-data">
              <xsl:with-param name="refs-queue" select="$queue"/>
            </xsl:call-template>
          </fo:block>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:choose>
        <xsl:when test="string(@href)">
          <xsl:call-template name="topicref-table">
            <xsl:with-param name="navtitle">
              <xsl:choose>
                <xsl:when test="@navtitle and (@locktitle='yes')">
                  <xsl:value-of select="@navtitle"/>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:value-of select="'(empty title)'"/>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:with-param>
          </xsl:call-template>
          <xsl:if test="count(*)">
            <fo:block start-indent="{concat($basic-start-indent, '+', 
                                     count(ancestor::*) - 1, 'em')}">
              <xsl:apply-templates>
                <xsl:with-param name="refs-queue" select="$queue"/>
              </xsl:apply-templates>
            </fo:block>
          </xsl:if>
        </xsl:when>
        <xsl:otherwise>
          <fo:block end-indent="6pt"
                    start-indent="{concat($basic-start-indent, '+', 
                                   count(ancestor::*) - 1, 'em')}">
            <xsl:apply-templates>
              <xsl:with-param name="refs-queue" select="$queue"/>
            </xsl:apply-templates>
          </fo:block>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<dtm:doc dtm:elements="topicmeta" dtm:status="finished" dtm:idref="map.topicmeta.content map.topicmeta map.topicmeta.content.content map.topicmeta.content.content.content"/>
<xsl:template match="*[contains(@class,' map/topicmeta ')]" dtm:id="map.topicmeta">
  <xsl:if test="'no' = $HIDE-MAP-TOPICMETA">
     <fo:block background-color="#a0e0ff"	    
               start-indent="{concat($basic-start-indent, '+', 
                             count(ancestor::*) - 1, 'em')}">
      <xsl:attribute name="end-indent">6pt</xsl:attribute>
      <fo:inline font-weight="bold"><xsl:value-of select="name()"/>: </fo:inline>
      <xsl:apply-templates/>
     </fo:block>
  </xsl:if>
</xsl:template>

<!-- Following is commented to use template from "dita-prolog.xsl"  -->
<!--xsl:template match="*[contains(@class,' map/topicmeta ')]/*" dtm:id="map.topicmeta.content">
  <fo:block start-indent="{$basic-start-indent} + 1em">
    <xsl:attribute name="end-indent">6pt</xsl:attribute>
    <fo:inline font-weight="bold"><xsl:value-of select="name()"/>: </fo:inline>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template-->

<xsl:template match="*[contains(@class,' map/topicmeta ')]/*/*" dtm:id="map.topicmeta.content.content">
  <fo:block start-indent="{$basic-start-indent} + 2em">
    <xsl:attribute name="end-indent">6pt</xsl:attribute>
    <fo:inline font-weight="bold"><xsl:value-of select="name()"/>: </fo:inline>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="*[contains(@class,' map/topicmeta ')]/*/*/*" dtm:id="map.topicmeta.content.content.content">
  <fo:block start-indent="{$basic-start-indent} + 3em">
    <xsl:attribute name="end-indent">6pt</xsl:attribute>
    <fo:inline font-weight="bold"><xsl:value-of select="name()"/>: </fo:inline>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="linktext" dtm:status="finished" dtm:idref="map.linktext"/>
<xsl:template match="*[contains(@class,' map/linktext ')]" dtm:id="map.linktext">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="mapgroup/topichead|mapgroup-d/topichead" dtm:status="finished" dtm:idref="map.topicref.topichead"/>
<xsl:template match="*[contains(@class,' map/topicref mapgroup/topichead ') or 
    contains(@class,' map/topicref mapgroup-d/topichead ')]" priority="2" dtm:id="map.topicref.topichead">
  <fo:block>
    <xsl:if test="parent::*[contains(@class,' map/topicref mapgroup/topichead ') or 
                  contains(@class,' map/topicref mapgroup-d/topichead ')]">
      <xsl:attribute name="start-indent">
        <xsl:value-of select="$basic-start-indent"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:if test="not($SHOW-RESOLVED-TOPICREFS = 'yes')">
      <fo:inline font-weight="bold">
         <xsl:value-of select="@navtitle"/>
      </fo:inline>
    </xsl:if>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="mapgroup/topicgroup|mapgroup-d/topicgroup" dtm:status="finished" dtm:idref="map.topicref.topicgroup"/>
<xsl:template match="*[contains(@class,' map/topicref mapgroup/topicgroup ') or 
                       contains(@class,' map/topicref mapgroup-d/topicgroup') ]" dtm:id="map.topicref.topicgroup">
  <fo:block>
    <xsl:apply-templates 
      select="*[not(self::processing-instruction('docato-extra-info-title'))]"/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="searchtitle" dtm:status="finished" dtm:idref="map.searchtitle"/>
<xsl:template match="*[contains(@class,' map/searchtitle ')]" dtm:id="map.searchtitle">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="shortdesc" dtm:status="finished" dtm:idref="map.shortdesc"/>
<xsl:template match="*[contains(@class,' map/shortdesc ')]" dtm:id="map.shortdesc">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="reltable" dtm:status="finished" dtm:idref="map.reltable"/>
<xsl:template match="*[contains(@class,' map/reltable ')]" dtm:id="map.reltable">
<fo:block padding-top="1em">
  <xsl:apply-templates select="*[local-name(.) = 'topicmeta']"/>
  <fo:table xsl:use-attribute-sets="table.data frameall">
    <fo:table-body>
      <xsl:call-template name="gen-dflt-data-hdr"/>
      <xsl:apply-templates select="*[local-name(.) != 'topicmeta']"/>
    </fo:table-body>
  </fo:table>
</fo:block>
</xsl:template>

<dtm:doc dtm:elements="relheader" dtm:status="finished" dtm:idref="map.relheader"/>
<xsl:template match="*[contains(@class,' map/relheader ')]" dtm:id="map.relheader">
  <fo:table-row>
    <xsl:apply-templates/>
  </fo:table-row>
</xsl:template>

<dtm:doc dtm:elements="relcolspec" dtm:status="finished" dtm:idref="map.relcolspec"/>
<xsl:template match="*[contains(@class,' map/relcolspec ')]" dtm:id="map.relcolspec">
  <fo:table-cell start-indent="2pt" background-color="gray" padding="2pt" xsl:use-attribute-sets="frameall">
    <fo:block>
      <xsl:apply-templates select="*[local-name(.) = 'topicmeta']"/>
    </fo:block>
    <fo:block color="white" text-align="center" font-weight="bold">
      <xsl:value-of select="@type"/>
      <xsl:apply-templates select="*[local-name(.) != 'topicmeta']"/>
    </fo:block>
  </fo:table-cell>
</xsl:template>

<dtm:doc dtm:elements="relrow" dtm:status="finished" dtm:idref="map.relrow"/>
<xsl:template match="*[contains(@class,' map/relrow ')]" dtm:id="map.relrow">
  <fo:table-row>
    <xsl:apply-templates/>
  </fo:table-row>
</xsl:template>

<dtm:doc dtm:elements="relcell" dtm:status="finished" dtm:idref="map.relcell"/>
<xsl:template match="*[contains(@class,' map/relcell ')]" dtm:id="map.relcell">
  <fo:table-cell start-indent="2pt" background-color="#fafafa" padding="2pt" xsl:use-attribute-sets="frameall">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </fo:table-cell>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="positioning-controls.add"/>
<xsl:template name="proc-ing" dtm:id="positioning-controls.add">
  <xsl:if test="$do-place-ing = 'yes'"> <!-- set in a global variable, as with label placement, etc. -->
    <fo:external-graphic src="url(image/tip-ing.jpg)"/> <!-- this should be an xsl:choose with the approved list and a selection method-->
    <!-- add any other required positioning controls, if needed, but must be valid in the location
         from which the call to this template was made -->
    &#160;
  </xsl:if>
</xsl:template>

<!--  NAMED TEMPLATES (call by name, only)  -->
<dtm:doc dtm:status="testing" dtm:idref="attribute.label.generate"/>
<xsl:template name="gen-att-label" dtm:id="attribute.label.generate">
  <xsl:if test="@spectitle">
    <fo:block margin-bottom="0" font-weight="bold">
    <xsl:value-of select="@spectitle"/></fo:block>
  </xsl:if>
</xsl:template>

<!-- generate null filler if the phrase is evidently empty -->
<dtm:doc dtm:status="testing" dtm:idref="phrase.nullfiller.generate"/>
<xsl:template name="apply-for-phrases" dtm:id="phrase.nullfiller.generate">
  <xsl:choose>
    <xsl:when test="not(text()[normalize-space(.)] | *)"><!--xsl:comment>null</xsl:comment--></xsl:when>
    <xsl:otherwise><xsl:apply-templates/></xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- =================== start of diagnostics  ====================== -->
<!-- reenable this by including dita-diagnostics.xsl -->
<dtm:doc dtm:status="testing" dtm:idref="output-message"/>
<xsl:template name="output-message" dtm:id="output-message"/>

<!-- =================== end of diagnostics  ====================== -->

<dtm:doc dtm:elements="map" 
         dtm:status="finished" 
         dtm:idref="map.cycles.text"/>
<xsl:template match="*[contains(@class,' map/map ')]"
              mode="cycles.test" 
              dtm:id="map.cycles.test">
  <xsl:param name="refs-queue"/>
  <xsl:variable name="id">
    <xsl:value-of select="generate-id()"/>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="contains($refs-queue, $id)">
      <xsl:value-of select="true()"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="queue" select="concat($refs-queue, '/', $id)"/>
      <xsl:apply-templates mode="cycles.test">
        <xsl:with-param name="refs-queue" select="$queue"/>
      </xsl:apply-templates>      
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="*[contains(@class,' map/topicref ')]"
              mode="cycles.test"
              dtm:id="topicref.cycles.test">
  <xsl:param name="refs-queue"/>
  <xsl:variable name="sourcefile">
    <xsl:choose>
      <xsl:when test="contains(@href, '#')">
        <xsl:value-of select="substring-before(@href,'#')"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="@href"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="topicid"  select="substring-after(@href,'#')"/>
  <xsl:variable name="topicdoc2" 
                select="document($sourcefile,/)" 
                xse:document-mode="validate read-write"/>
  <xsl:variable name="topicdoc" select="($topicdoc2/*)"/>
  <xsl:variable name="id">
    <xsl:value-of select="generate-id()"/>
  </xsl:variable>
  <xsl:if test="contains($refs-queue, $id)">
    <xsl:value-of select="true()"/>
  </xsl:if>
  <xsl:variable name="queue" select="concat($refs-queue, '/', $id)"/>
  <xsl:call-template name="apply-testing-templates">
    <xsl:with-param name="sourcefile" select="$sourcefile"/>
    <xsl:with-param name="topicid" select="$topicid"/>
    <xsl:with-param name="topicdoc" select="$topicdoc"/>
    <xsl:with-param name="topicdoc2" select="$topicdoc2"/>
    <xsl:with-param name="refs-queue" select="$queue"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="start-cycles-testing">
  <xsl:variable name="sourcefile">
    <xsl:choose>
      <xsl:when test="contains(@href, '#')">
        <xsl:value-of select="substring-before(@href,'#')"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="@href"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="topicid"  select="substring-after(@href,'#')"/>
  <xsl:variable name="topicdoc2" 
                select="document($sourcefile,/)" 
                xse:document-mode="validate read-write"/>
  <xsl:variable name="topicdoc" select="($topicdoc2/*)"/>
  <xsl:variable name="id">
    <xsl:value-of select="generate-id()"/>
  </xsl:variable>
  <xsl:variable name="parent-id">
    <xsl:value-of select="generate-id()"/>
  </xsl:variable>
  <xsl:variable name="refs-queue" select="concat($parent-id, '/', $id, '/')"/>
  <xsl:call-template name="apply-testing-templates">
    <xsl:with-param name="sourcefile" select="$sourcefile"/>
    <xsl:with-param name="topicid" select="$topicid"/>
    <xsl:with-param name="topicdoc" select="$topicdoc"/>
    <xsl:with-param name="topicdoc2" select="$topicdoc2"/>
    <xsl:with-param name="refs-queue" select="$refs-queue"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="apply-testing-templates">
  <xsl:param name="sourcefile"/>
  <xsl:param name="topicid"/>
  <xsl:param name="topicdoc"/>
  <xsl:param name="topicdoc2"/>
  <xsl:param name="refs-queue"/>
  <xsl:choose>
     <xsl:when test="$topicid">
       <xsl:choose>
         <xsl:when test="id($topicid, $topicdoc2)">
           <xsl:apply-templates select="id($topicid, $topicdoc2)" mode="cycles.test">
             <xsl:with-param name="refs-queue" select="$refs-queue"/>
           </xsl:apply-templates>
        </xsl:when>
        <xsl:otherwise>
          <xsl:apply-templates select="id($topicid, $topicdoc)" mode="cycles.test">
            <xsl:with-param name="refs-queue" select="$refs-queue"/>
          </xsl:apply-templates>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:if test="not($sourcefile='')">
        <xsl:apply-templates select="$topicdoc" mode="cycles.test">
          <xsl:with-param name="refs-queue" select="$refs-queue"/>
        </xsl:apply-templates>
      </xsl:if>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="*" mode="cycles.test"/>

</xsl:transform>
