<!-- style for OUCS userguides -->
<xsl:stylesheet
  xmlns:fotex="http://www.tug.org/fotex"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns:fo="http://www.w3.org/1999/XSL/Format">

<xsl:import href="tei.xsl"/>

<xsl:param name="giColor">black</xsl:param>
<xsl:param name="identColor">black</xsl:param>
<xsl:param name="titlePage">true</xsl:param>
<xsl:param name="parSkip">6pt</xsl:param>
<xsl:param name="parIndent">0em</xsl:param>
<xsl:param name="numberBackHeadings"></xsl:param>
<xsl:param name="numberFrontHeadings"></xsl:param>
<xsl:param name="tocFront"></xsl:param>
<xsl:param name="headingOutdent">-1in</xsl:param>
<xsl:param name="pageMarginLeft">144pt</xsl:param>
<xsl:param name="pageMarginRight">72pt</xsl:param>
<xsl:param name="pageMarginBottom">72pt</xsl:param>
<xsl:param name="pageMarginTop">72pt</xsl:param>
<xsl:param name="bodyMarginTop">12pt</xsl:param>
<xsl:param name="tocNumberSuffix"></xsl:param>
<xsl:param name="headingNumberSuffix"><xsl:text> </xsl:text></xsl:param>
<xsl:param name="xrefShowPage">true</xsl:param>
<xsl:param name="flowMarginLeft">1in</xsl:param>
<xsl:param name="activeLinebreaks">true</xsl:param>
<xsl:param name="listLeftIndent">10pt</xsl:param>
<xsl:param name="listNormalIndent">15pt</xsl:param>
<xsl:param name="listLeftGlossIndent">.5in</xsl:param>
<xsl:param name="institution">Oxford University Computing Services</xsl:param>
<xsl:param name="shortinstitution">OUCS</xsl:param>
<xsl:template name="divXRefHeading">
  <xsl:text>, </xsl:text>
  <fo:inline font-style="italic">
      <xsl:apply-templates mode="section" select="head"/>
  </fo:inline>
</xsl:template>

<xsl:param name="OUCS"></xsl:param>

<xsl:param name="forcePageMaster">twoside1nofirst</xsl:param>

<xsl:template match="titlePage"> 
  <fo:block text-align="center" space-after="4in"
     font-size="24pt" >
    <xsl:value-of select="$institution"/>
  </fo:block>
  <fo:block text-align="center" space-after="3.6in"
     font-size="28pt" >
    <xsl:apply-templates select="docTitle/titlePart[@type='main']"/>
  </fo:block>
  <fo:block break-after="page" text-align="center" 
     font-size="24pt" >
    <xsl:apply-templates select="docTitle/titlePart[@type='class']"/>
    <fo:leader rule-thickness="0pt"/>
    <xsl:apply-templates select="docTitle/titlePart[@type='code']"/>
  </fo:block>
</xsl:template>

<xsl:template name="setupDiv0">
 <xsl:attribute name="font-size">18pt</xsl:attribute>
 <xsl:attribute name="font-weight">bold</xsl:attribute>
 <xsl:attribute name="space-after">6pt</xsl:attribute>
 <xsl:attribute name="space-before.optimum">12pt</xsl:attribute>
 <xsl:if test="ancestor-or-self::body">
   <xsl:attribute name="text-indent">
     <xsl:value-of select="$headingOutdent"/></xsl:attribute>
 </xsl:if>
</xsl:template>

<xsl:template name="setupDiv1">
 <xsl:attribute name="font-size">14pt</xsl:attribute>
 <xsl:attribute name="font-weight">bold</xsl:attribute>
 <xsl:attribute name="space-after">3pt</xsl:attribute>
 <xsl:attribute name="space-before.optimum">9pt</xsl:attribute>
 <xsl:if test="ancestor-or-self::body">
   <xsl:attribute name="text-indent">
     <xsl:value-of select="$headingOutdent"/></xsl:attribute>
 </xsl:if>
</xsl:template>

<xsl:template name="setupDiv2">
 <xsl:attribute name="font-size">12pt</xsl:attribute>
 <xsl:attribute name="font-weight">bold</xsl:attribute>
 <xsl:attribute name="space-after">2pt</xsl:attribute>
 <xsl:attribute name="space-before.optimum">10pt</xsl:attribute>
 <xsl:attribute name="text-indent"><xsl:value-of select="$headingOutdent"/></xsl:attribute>
</xsl:template>

<xsl:template name="hookDefinepagemasters">
      <fo:simple-page-master
        master-name="front"
        page-width="{$pageWidth}"
        page-height="{$pageHeight}"
        margin-top="{$pageMarginTop}"
        margin-bottom="{$pageMarginBottom}"
        margin-left="72pt"
        margin-right="72pt">
        <fo:region-body 
		margin-bottom="{$bodyMarginBottom}"
		margin-top="{$bodyMarginTop}"/>
        <fo:region-after extent="{$regionAfterExtent}"/>
        <fo:region-before extent="{$regionBeforeExtent}"/>
      </fo:simple-page-master>
</xsl:template>

<xsl:template match="front">
  <fo:page-sequence
	force-page-count="end-on-even"
	hyphenate="{$hyphenate}"
	language="{$language}"
	master-name="front"
    >
   <fo:static-content flow-name="xsl-region-before-right">
     <fo:block/>
   </fo:static-content>
   <fo:static-content flow-name="xsl-region-before-left">
     <fo:block/>
   </fo:static-content>
   <fo:static-content flow-name="xsl-region-after-right">
     <fo:block/>
   </fo:static-content>
   <fo:static-content flow-name="xsl-region-after-left">
     <fo:block/>
   </fo:static-content>
   <fo:static-content flow-name="xsl-region-after">
     <fo:block/>
   </fo:static-content>
   <fo:static-content flow-name="xsl-region-before">
     <fo:block/>
   </fo:static-content>

   <fo:flow flow-name="xsl-region-body">
      <xsl:apply-templates select="titlePage"/>
      <xsl:apply-templates select="div"/>
      <fo:block break-before="page">
        <xsl:call-template name="mainTOC"/>
      </fo:block>
     <fo:block space-before="20pt">
         <fo:inline font-weight="bold">Author: </fo:inline>
         <xsl:apply-templates select="titlePage/docAuthor/text()"/>
     </fo:block>
     <fo:block space-before="12pt">
         <fo:inline font-weight="bold">Revision History: </fo:inline>
         <xsl:apply-templates select="titlePage/docDate/text()"/>
     </fo:block>
     <fo:block space-before="20pt">
        <fo:block text-align="center">&#169; 
        <xsl:value-of select="$institution"/>
     </fo:block>
     <fo:block  padding-start="1in" padding-end="1in"
text-align="justify">Although formal copyright is
reserved, members of academic institutions may republish the material
in the document subject to due acknowledgement of the
source.</fo:block>
         
     </fo:block>
 <xsl:apply-templates/>
  </fo:flow>
 </fo:page-sequence>
</xsl:template>

<xsl:template name="footerRight">
<fo:inline>
  <xsl:value-of select="/TEI.2/text/front/titlePage/docDate"/>
</fo:inline>
<fo:leader rule-thickness="0pt"/>
              <fo:page-number/>
<fo:leader rule-thickness="0pt"/>
<fo:inline><xsl:value-of select="$shortinstitution"/></fo:inline>
</xsl:template>

<xsl:template name="footerLeft">
           <fo:inline><xsl:value-of select="$shortinstitution"/></fo:inline>
           <fo:leader rule-thickness="0pt"/>
              <fo:page-number/>
           <fo:leader rule-thickness="0pt"/>
           <fo:inline>
		<xsl:value-of select="/TEI.2/text/front/titlePage/docDate"/>
           </fo:inline>
</xsl:template>

<xsl:template name="headerRight">
           <fo:inline>
		<xsl:value-of select="/TEI.2/text/front/titlePage/docTitle/titlePart[@type='code']"/>
           </fo:inline>
           <fo:leader rule-thickness="0pt"/>
           <fo:inline>
		<xsl:value-of select="/TEI.2/text/front/titlePage/docTitle/titlePart[@type='main']"/>
           </fo:inline>
</xsl:template>

<xsl:template name="headerLeft">
           <fo:inline>
		<xsl:value-of select="/TEI.2/text/front/titlePage/docTitle/titlePart[@type='main']"/>
           </fo:inline>
           <fo:leader rule-thickness="0pt"/>
           <fo:inline>
		<xsl:value-of select="/TEI.2/text/front/titlePage/docTitle/titlePart[@type='code']"/>
           </fo:inline>
</xsl:template>

<xsl:template name="headers-footers-twoside">
  <xsl:param name="runodd"/>
  <xsl:param name="runeven"/>
  <fo:static-content flow-name="xsl-region-after-right">
          <fo:block  text-align="justify" font-size="{$bodySize}">
             <xsl:call-template name="footerRight"/>
          </fo:block>
  </fo:static-content>
  <fo:static-content flow-name="xsl-region-after-left">
          <fo:block  text-align="justify" font-size="{$bodySize}">
             <xsl:call-template name="footerLeft"/>
          </fo:block>
  </fo:static-content>
  <fo:static-content flow-name="xsl-region-before-right">
          <fo:block text-align="justify" font-size="{$bodySize}">
             <xsl:call-template name="headerRight"/>
          </fo:block>
  </fo:static-content>
  <fo:static-content flow-name="xsl-region-before-left">
          <fo:block text-align="justify" font-size="{$bodySize}">
             <xsl:call-template name="headerLeft"/>
          </fo:block>
  </fo:static-content>
</xsl:template>

<xsl:template mode="toc" match="Exercise"  priority="999">
  <fo:block>
      <xsl:attribute name="text-indent">
       <xsl:value-of select="$div2Tocindent"/>
      </xsl:attribute>
   <xsl:text>Exercise </xsl:text>
   <xsl:number level="any"  count="Exercise"/>
   <xsl:text> </xsl:text>
   <xsl:text>&#x2003;</xsl:text>
   <fo:inline>
     <xsl:apply-templates mode="section" select="head"/>
   </fo:inline>
    <fo:leader rule-thickness="0pt"/>
    <fo:inline color="{$linkColor}"><fo:page-number-citation>
      <xsl:attribute name="ref-id">
        <xsl:call-template name="idLabel"/>
      </xsl:attribute>
    </fo:page-number-citation></fo:inline> 
  </fo:block>
</xsl:template>


<xsl:template match="Exercise">
  <fo:block keep-with-next.within-page="always">
  <xsl:variable name="divid">
        <xsl:call-template name="idLabel"/> 
  </xsl:variable>
  <xsl:attribute name="id">
   <xsl:value-of select="$divid"/>
  </xsl:attribute>
  <xsl:attribute name="text-align">start</xsl:attribute>
  <xsl:attribute name="font-family">
       <xsl:value-of select="$divFont"/></xsl:attribute>
  <xsl:call-template name="setupDiv2"/>
   <xsl:text>Exercise </xsl:text>
   <xsl:number level="any"  count="Exercise"/>
   <xsl:text> </xsl:text>
   <xsl:text>&#x2003;</xsl:text>
   <fo:inline>
     <xsl:apply-templates mode="section" select="head"/>
   </fo:inline>
  </fo:block>
  <fo:block><fo:inline font-style="italic">Description</fo:inline></fo:block>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="Exercise/head"/>

<xsl:template name="tocBits">
        <xsl:if test="$tocFront='true'">
          <xsl:for-each select="ancestor::text/front//div">
            <xsl:apply-templates mode="toc" select="(.)"/>
          </xsl:for-each>
        </xsl:if>
        <xsl:for-each select="ancestor::text/body">
          <xsl:for-each select=".//Exercise|.//div">
            <xsl:apply-templates mode="toc" select="(.)"/>
        </xsl:for-each>
      </xsl:for-each>
        <xsl:if test="$tocBack='true'">
          <xsl:for-each select="ancestor::text/back//div">
            <xsl:apply-templates mode="toc" select="(.)"/>
          </xsl:for-each>
        </xsl:if>
</xsl:template>


<!-- special code additions -->


<xsl:template match="Menu">
  <fo:inline font-family="{$typewriterFont}"><xsl:text>[</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>]</xsl:text></fo:inline>
</xsl:template>

<xsl:template match="Key">
  <fo:inline font-family="{$typewriterFont}">&lt;<xsl:apply-templates/>&gt;</fo:inline>
</xsl:template>

<xsl:template match="Code">
  <fo:inline font-family="{$typewriterFont}">
  <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<xsl:template match="Input">
  <fo:inline font-family="{$typewriterFont}">
  <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<xsl:template match="Output">
  <fo:inline font-family="{$typewriterFont}">
  <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<xsl:template match="Screen">
    <fo:block font-family="{$typewriterFont}" 
	white-space-collapse="false" 
	wrap-option="no-wrap" 
	text-indent="0em"
	start-indent="{$exampleMargin}"
	text-align="start"
	font-size="{$exampleSize}"
	space-before.optimum="4pt"
	space-after.optimum="4pt"
	>
       <xsl:if test="not($flowMarginLeft='')">
        <xsl:attribute name="padding-start">
         <xsl:value-of select="$exampleMargin"/>
        </xsl:attribute>
       </xsl:if>
      <xsl:value-of select="translate(.,' -','&#160;&#8208;')"/>
    </fo:block>
</xsl:template>

<xsl:template match="Program">
    <fo:block font-family="{$typewriterFont}" 
	white-space-collapse="false" 
	wrap-option="no-wrap" 
	text-indent="0em"
	start-indent="{$exampleMargin}"
	text-align="start"
	font-size="{$exampleSize}"
	space-before.optimum="4pt"
	space-after.optimum="4pt"
	>
       <xsl:if test="not($flowMarginLeft='')">
        <xsl:attribute name="padding-start">
         <xsl:value-of select="$exampleMargin"/>
        </xsl:attribute>
       </xsl:if>
      <xsl:value-of select="translate(.,' -','&#160;&#8208;')"/>
    </fo:block>
</xsl:template>

<xsl:template match="Prompt">
  <fo:inline font-family="{$typewriterFont}">
  <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<xsl:template match="Field">
  <fo:inline font-family="{$typewriterFont}">
  <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<xsl:template match="Link">
  <fo:inline font-family="{$typewriterFont}">
  <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<xsl:template match="Filespec">
  <fo:inline font-family="{$typewriterFont}">
  <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<xsl:template match="Button">
  <fo:inline font-family="{$typewriterFont}">
  <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<xsl:template match="Value">
  <fo:inline font-family="{$typewriterFont}">
  <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<xsl:template match="Keyword">
  <fo:inline font-family="{$typewriterFont}">
  <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

</xsl:stylesheet>
