<!-- 
     TEI XSL Stylesheet for Syntext Serna (c) 2003, Syntext Inc.

     The Stylesheet is based on Sebastian Rahtz/Oxford University 
     XSL TEI Stylesheet v1.3. See file SR-COPYING for 
     Sebastian Rahtz/Oxford University Copyright 
     information.
-->
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:fo="http://www.w3.org/1999/XSL/Format"
  xmlns:xse="http://www.syntext.com/Extensions/XSLT-1.0"
  version="1.0">

<!-- Words for I18N -->
<!-- TODO: move them out -->
<xsl:param name="authorWord">Author:</xsl:param>
<xsl:param name="biblioWords">Bibliography</xsl:param>
<xsl:param name="dateWord">Date:</xsl:param>
<xsl:param name="figureWord">Figure</xsl:param>
<xsl:param name="figureWords">Figures</xsl:param>
<xsl:param name="nextWord">Next</xsl:param>
<xsl:param name="previousWord">Previous</xsl:param>
<xsl:param name="revisedWord">revised</xsl:param>
<xsl:param name="tableWord">Table</xsl:param>
<xsl:param name="tableWords">Tables</xsl:param>
<xsl:param name="tocWords">Contents</xsl:param>
<xsl:param name="upWord">Up</xsl:param>
<xsl:template name="contentsWord">Contents</xsl:template>
<xsl:template name="contentsHeading">Sections in this document:</xsl:template>
<xsl:template name="searchWords">Search</xsl:template>
<xsl:param name="appendixWords">Appendix</xsl:param>


<!-- Parameterization. Lots more to do here -->
<xsl:param name="REQUEST"/>
<xsl:param name="STDOUT"/>

<!-- XSLT Params -->
  <!-- metainfo setup (teiHeader, figure/fileDesc, ...)-->
  <xsl:param name="showMetainfo" select="1" xse:type="numeric" 
    xse:annotation="Show draft areas?"/>
  <xsl:param name="numberHeadings" select="1" xse:type="numeric" 
    xse:annotation="Number headings"/>
  <xsl:param name="numberHeadingsDepth" select="9" xse:type="numeric" 
    xse:annotation="Maximal depth at which sections are numbered"/>
  <xsl:param name="numberBackHeadings" select="'A.1'" xse:type="string" 
    xse:annotation="Number headings in back matter"/>
  <xsl:param name="numberFrontHeadings" select="''" xse:type="string" 
    xse:annotation="Number headings in front matter"/>
  <xsl:param name="headingOutdent" select="'-3em'" xse:type="string" 
    xse:annotation="Indentation for div's 'head' elements"/>
  <xsl:param name="giColor" select="'black'" xse:type="string" 
    xse:annotation="Color for 'gi' element"/>
  <xsl:param name="identColor" select="'blue'" xse:type="string" 
    xse:annotation="Color for 'indent' element"/>
  <xsl:param name="parIndent" select="'1em'" xse:type="string" 
    xse:annotation="Indentation for the second and following para elements"/>
  <xsl:param name="parSkip" select="'0.5em'" xse:type="string" 
    xse:annotation="Padding between 'para' elements"/>
  <xsl:param name="div0Tocindent" select="'0in'" xse:type="string" 
    xse:annotation="Indentation for div0 TOC entries"/>
  <xsl:param name="div1Tocindent" select="'0.25in'" xse:type="string" 
    xse:annotation="Indentation for div1 TOC entries"/>
  <xsl:param name="div2Tocindent" select="'0.5in'" xse:type="string" 
    xse:annotation="Indentation for div2 TOC entries"/>
  <xsl:param name="div3Tocindent" select="'0.75in'" xse:type="string" 
    xse:annotation="Indentation for div3 TOC entries"/>
  <xsl:param name="div4Tocindent" select="'1in'" xse:type="string" 
    xse:annotation="Indentation for div4 TOC entries"/>
  <xsl:param name="tocFront" select="1" xse:type="numeric" 
    xse:annotation="Show TOC on the front matter?"/>
  <xsl:param name="tocBack" select="1" xse:type="numeric" 
    xse:annotation="Show TOC on the back matter?"/>  
  <xsl:param name="tableCaptionAlign" select="'center'" xse:type="string" 
    xse:annotation="Table caption alignment"/>
  <xsl:param name="tableAlign" select="'center'" xse:type="string" 
    xse:annotation="Table alignment"/>
  <xsl:param name="rowAlign" select="'left'" xse:type="string" 
    xse:annotation="Table row alignment"/>
  <xsl:param name="spaceBelowCaption" select="'4pt'" 
    xse:type="string" xse:annotation="Space below caption"/>
  <xsl:param name="spaceAroundTable" select="'8pt'" 
    xse:type="string" xse:annotation="Space around table"/>
  <xsl:param name="tableCellPadding" select="'2pt'" xse:type="string" 
    xse:annotation="Table cell padding"/>
  <xsl:param name="makeTableCaption" select="1" xse:type="numeric" 
    xse:annotation="Show table caption?"/>


<!-- page setup -->
  <xsl:param name="paper.type" select="'A4'"/>
  <xsl:param name="page.orientation" select="'portrait'"/>
  <xsl:param name="body.margin.bottom" select="'24pt'"/>
  <xsl:param name="body.margin.top" select="'24pt'"/>
  <xsl:param name="page.margin.top" select="'1in'"/>
  <xsl:param name="page.margin.bottom" select="'1in'"/>
  <xsl:param name="page.margin.left" select="'1in'"/>
  <xsl:param name="page.margin.right" select="'1in'"/>

  <xsl:variable name="default.indent.shift" select="'20'"/>
  <xsl:param name="body.font.size" select="concat($body.font.master,'pt')"/>

  <xsl:param name="text-align">start</xsl:param>

  <xsl:attribute-set name="root">
    <xsl:attribute name="font-family">
      <xsl:value-of select="$body.font.family"/>
    </xsl:attribute>
    <xsl:attribute name="font-size"><xsl:value-of select="$body.font.size"/></xsl:attribute>
    <xsl:attribute name="text-align"><xsl:value-of select="$text-align"/></xsl:attribute>
  </xsl:attribute-set>

<xsl:param name="smallSize">
 <xsl:value-of select="$body.font.master * 0.9"/><xsl:text>pt</xsl:text>
</xsl:param>
<xsl:param name="flowMarginLeft"></xsl:param>
<xsl:param name="runSize">9pt</xsl:param>

<xsl:param name="useHeaderFrontMatter"/>

<xsl:attribute-set name="metainfoAttributes">
    <xsl:attribute name="border-style">solid</xsl:attribute>
    <xsl:attribute name="border-top-width">1pt</xsl:attribute>
    <xsl:attribute name="border-left-width">1pt</xsl:attribute>    
    <xsl:attribute name="border-right-width">1pt</xsl:attribute>    
    <xsl:attribute name="border-bottom-width">1pt</xsl:attribute>
    <xsl:attribute name="border-top-color">#000000</xsl:attribute>
    <xsl:attribute name="border-bottom-color">#000000</xsl:attribute>
    <xsl:attribute name="border-left-color">#000000</xsl:attribute>
    <xsl:attribute name="border-right-color">#000000</xsl:attribute>
    <xsl:attribute name="background-color">#e0e0e0</xsl:attribute>
    <xsl:attribute name="font-size">
      <xsl:value-of select="$body.font.master * 0.83"/><xsl:text>pt</xsl:text>
    </xsl:attribute>
</xsl:attribute-set>

<!--number headings-->
<xsl:param name="numberSpacer">
  <xsl:text> </xsl:text>
</xsl:param><!--&#160;-->
<xsl:param name="minimalCrossRef"/>

<!-- general H&J setup -->
<xsl:param name="hyphenate">true</xsl:param>
<xsl:param name="alignment">justify</xsl:param>

<xsl:param name="footnoteSize">
  <xsl:value-of select="$body.font.master * 0.66"/><xsl:text>pt</xsl:text>
</xsl:param>
<xsl:param name="footnotenumSize">
  <xsl:value-of select="$body.font.master * 0.58"/><xsl:text>pt</xsl:text>
</xsl:param>
<xsl:param name="authorSize">
  <xsl:value-of select="$body.font.master * 1.17"/><xsl:text>pt</xsl:text>
</xsl:param>
<xsl:param name="dateSize">
  <xsl:value-of select="$body.font.master * 1.17"/><xsl:text>pt</xsl:text>
</xsl:param>
<xsl:param name="titleSize">
  <xsl:value-of select="$body.font.master * 1.33"/><xsl:text>pt</xsl:text>
</xsl:param>
<xsl:param name="tocSize">
  <xsl:value-of select="$body.font.master * 1.33"/><xsl:text>pt</xsl:text>
</xsl:param>

<xsl:param name="activeLinebreaks"></xsl:param>
<xsl:param name="activePagebreaks"></xsl:param>

<!-- table of contents customization -->
<xsl:param name="headingNumberSuffix">. </xsl:param>
<xsl:param name="tocNumberSuffix">. </xsl:param>
<xsl:param name="tocStartPage">1</xsl:param>

<!-- list customization -->
<xsl:param name="listRightMargin">10pt</xsl:param>
<xsl:param name="listNormalIndent">15pt</xsl:param>
<xsl:param name="listLeftGlossIndent">0.5in</xsl:param>
<xsl:param name="listLeftGlossInnerIndent">0.25in</xsl:param>
<xsl:param name="listLeftIndent">15pt</xsl:param>
<xsl:param name="listItemsep">4pt</xsl:param>
<xsl:param name="listAbove-1">6pt</xsl:param>
<xsl:param name="listBelow-1">6pt</xsl:param>
<xsl:param name="listAbove-2">4pt</xsl:param>
<xsl:param name="listBelow-2">4pt</xsl:param>
<xsl:param name="listAbove-3">0pt</xsl:param>
<xsl:param name="listBelow-3">0pt</xsl:param>
<xsl:param name="listAbove-4">0pt</xsl:param>
<xsl:param name="listBelow-4">0pt</xsl:param>
<xsl:param name="listGlossSeparation">0.25in</xsl:param>
<xsl:param name="listGlossWidth">1in</xsl:param>
<xsl:param name="listLabelSeparation">0.08in</xsl:param>
<xsl:param name="listLabelWidth">0.3in</xsl:param>
<!--<xsl:param name="bulletOne">&#x2022;</xsl:param> -->
<xsl:param name="bulletOne">&#x2219;</xsl:param>
<xsl:param name="bulletTwo">&#x2013;</xsl:param>
<xsl:param name="bulletThree">&#x002A;</xsl:param>
<xsl:param name="bulletFour">&#x002B;</xsl:param>


<!-- example/quotation setup -->
<xsl:param name="exampleSize">
  <xsl:value-of select="$body.font.master * 0.8"/><xsl:text>pt</xsl:text>
</xsl:param>
<xsl:param name="exampleMargin">12pt</xsl:param>
<xsl:param name="exampleBefore">4pt</xsl:param>
<xsl:param name="exampleAfter">4pt</xsl:param>

<!-- do we want a separate title sheet -->
<xsl:param name="titlePage"></xsl:param>

<!-- style for OUCS -->
<xsl:param name="OUCS"></xsl:param>

<!-- use bookmarks option in hyperref -->
<xsl:param name="pdfBookmarks"></xsl:param>
<xsl:param name="divRunningheads"></xsl:param>

<!-- default language -->
<xsl:param name="language">en_US</xsl:param>

<!-- page layout -->
<xsl:param name="forcePageMaster">oneside1</xsl:param>
<xsl:param name="twoSided">false</xsl:param>
<xsl:param name="frontMulticolumns"></xsl:param>
<xsl:param name="bodyMulticolumns"></xsl:param>
<xsl:param name="backMulticolumns">oneside1</xsl:param>
<xsl:param name="columnCount">1</xsl:param>
<xsl:template name="hookDefinepagemasters"/>

<!-- running heads -->
<xsl:param name="sectionHeaders">true</xsl:param>

<!-- section headings -->
<xsl:template name="divXRefHeading">
<xsl:param name="x">
      <xsl:apply-templates mode="section" select="head"/>
</xsl:param>
  <xsl:text> (</xsl:text>
    <xsl:value-of select="normalize-space($x)"/>
  <xsl:text>)</xsl:text>
</xsl:template>

<xsl:template name="setupDiv0">
 <xsl:attribute name="font-size">
      <xsl:value-of select="$body.font.master * 1.5"/><xsl:text>pt</xsl:text>
 </xsl:attribute>
 <xsl:attribute name="text-align">left</xsl:attribute>
 <xsl:attribute name="font-weight">bold</xsl:attribute>
 <xsl:attribute name="padding-bottom">6pt</xsl:attribute>
 <xsl:attribute name="padding-top">12pt</xsl:attribute>
 <xsl:attribute name="start-indent"><xsl:value-of select="$headingOutdent"/></xsl:attribute>
</xsl:template>

<xsl:template name="setupDiv1">
 <xsl:attribute name="font-size">
      <xsl:value-of select="$body.font.master * 1.17"/><xsl:text>pt</xsl:text>
 </xsl:attribute>
 <xsl:attribute name="text-align">left</xsl:attribute>
 <xsl:attribute name="font-weight">bold</xsl:attribute>
 <xsl:attribute name="padding-bottom">3pt</xsl:attribute>
 <xsl:attribute name="padding-top">9pt</xsl:attribute>
 <xsl:attribute name="start-indent"><xsl:value-of select="$headingOutdent"/></xsl:attribute>
</xsl:template>

<xsl:template name="setupDiv2">
 <xsl:attribute name="font-size">
      <xsl:value-of select="$body.font.master"/><xsl:text>pt</xsl:text>
 </xsl:attribute>
 <xsl:attribute name="text-align">left</xsl:attribute>
 <xsl:attribute name="font-weight">bold</xsl:attribute>
 <xsl:attribute name="font-style">italic</xsl:attribute>
 <xsl:attribute name="padding-bottom">2pt</xsl:attribute>
 <xsl:attribute name="padding-top">4pt</xsl:attribute>
 <xsl:attribute name="start-indent"><xsl:value-of select="$headingOutdent"/></xsl:attribute>
</xsl:template>

<xsl:template name="setupDiv3">
 <xsl:attribute name="font-size">
      <xsl:value-of select="$body.font.master * 0.83"/><xsl:text>pt</xsl:text>
 </xsl:attribute>
 <xsl:attribute name="text-align">left</xsl:attribute>
 <xsl:attribute name="font-style">italic</xsl:attribute>
 <xsl:attribute name="padding-bottom">0pt</xsl:attribute>
 <xsl:attribute name="padding-top">4pt</xsl:attribute>
 <xsl:attribute name="start-indent"><xsl:value-of select="$headingOutdent"/></xsl:attribute>
</xsl:template>

<xsl:template name="setupDiv4">
 <xsl:attribute name="font-size">
      <xsl:value-of select="$body.font.master * 0.83"/><xsl:text>pt</xsl:text>
 </xsl:attribute>
 <xsl:attribute name="padding-top">4pt</xsl:attribute>
 <xsl:attribute name="start-indent"><xsl:value-of select="$headingOutdent"/></xsl:attribute>
</xsl:template>

<xsl:template name="blockStartHook"/>

<!-- pictures and graphics -->
<!-- allows for pictures in a subdirectory -->
<xsl:param name="graphicsPrefix"></xsl:param>
<xsl:param name="graphicsSuffix">.png</xsl:param>
<xsl:param name="autoScaleFigures"></xsl:param>
<xsl:param name="captionInlinefigures"></xsl:param>
<xsl:param name="xrefShowTitle"></xsl:param>
<xsl:param name="xrefShowHead"></xsl:param>
<xsl:param name="xrefShowPage"></xsl:param>

<xsl:template name="showXrefURL">
  <xsl:param name="dest"/>
</xsl:template>

<xsl:param name="readColSpecFile"></xsl:param>

<!-- figures -->
<xsl:template name="figureCaptionstyle">
  <xsl:attribute name="text-align">center</xsl:attribute>
  <xsl:attribute name="font-style">italic</xsl:attribute>
  <xsl:attribute name="end-indent">
   <xsl:value-of select="$exampleMargin"/>
  </xsl:attribute>
  <xsl:attribute name="start-indent">
   <xsl:value-of select="$exampleMargin"/>
  </xsl:attribute>
</xsl:template>

<!-- bibliography customization -->
<xsl:param name="biblSize">
     <xsl:value-of select="$body.font.master * 1.33"/><xsl:text>pt</xsl:text>
</xsl:param>
<xsl:param name="indentBibl">1em</xsl:param>
<xsl:param name="spaceBeforeBibl">4pt</xsl:param>
<xsl:param name="spaceAfterBibl">0pt</xsl:param>

<!-- table customization -->
<xsl:param name="inlineTables"></xsl:param>
<xsl:template name="tableCaptionstyle">
  <xsl:attribute name="text-align">center</xsl:attribute>
  <xsl:attribute name="font-style">italic</xsl:attribute>
  <xsl:attribute name="end-indent">
   <xsl:value-of select="$exampleMargin"/>
  </xsl:attribute>
  <xsl:attribute name="padding-left">
   <xsl:value-of select="$exampleMargin"/>
  </xsl:attribute>
        <xsl:attribute name="padding-top">
          <xsl:value-of select="$spaceAroundTable"/>
        </xsl:attribute>
  <xsl:attribute name="padding-bottom">
          <xsl:value-of select="$spaceBelowCaption"/>
  </xsl:attribute>
  <xsl:attribute name="keep-with-next">always</xsl:attribute>

</xsl:template>

<xsl:param name="tableSize">
  <xsl:value-of select="$body.font.master * 0.9"/><xsl:text>pt</xsl:text>
</xsl:param>

</xsl:stylesheet>
