<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
   <xsl:param name="urlChunkPrefix">?ID=</xsl:param>
   <xsl:param name="REQUEST"/>
   <xsl:param name="ID"></xsl:param>
   <xsl:param name="css_override"></xsl:param>
   <xsl:param name="preferred_font">Helvetica</xsl:param>
   <xsl:param name="preferred_size">130%</xsl:param>
   <xsl:param name="preferred_color">#FFFF00</xsl:param>
   <xsl:param name="preferred_bgcolor">#000000</xsl:param>
   <xsl:param name="preferred_linkcolor">#0000FF</xsl:param>
   <xsl:param name="inputName"/>
   <xsl:param name="outputDir"/>
   <xsl:param name="splitLevel">-1</xsl:param>
   <xsl:param name="splitBackmatter">true</xsl:param>
   <xsl:param name="splitFrontmatter">true</xsl:param>
   <xsl:param name="sectionTopLink"/>
   <xsl:param name="useIDs">true</xsl:param>
   <xsl:param name="makingSlides"/>
   <xsl:param name="autoToc">true</xsl:param>
   <xsl:param name="tocDepth">5</xsl:param>
   <xsl:param name="subTocDepth">-1</xsl:param>
   <xsl:param name="tocFront">true</xsl:param>
   <xsl:param name="tocBack">true</xsl:param>
   <xsl:template name="logoPicture"><a target="_top" href="http://www.ox.ac.uk/"><img border="0" width="78" height="94"
    src="/images/ncrest.gif"
    alt="Oxford University"/></a></xsl:template>
   <xsl:param name="cssFile">/stylesheets/tei-oucs.css</xsl:param>
   <xsl:param name="cssSecondaryFile"></xsl:param>
   <xsl:param name="feedbackURL">mailto:feedback</xsl:param>
   <xsl:template name="feedbackWords">Feedback</xsl:template>
   <xsl:param name="homeLabel">Home</xsl:param>
   <xsl:param name="homeURL">http://www.oucs.ox.ac.uk/</xsl:param>
   <xsl:param name="homeWords">OUCS</xsl:param>
   <xsl:param name="institution">Oxford University Computing Services</xsl:param>
   <xsl:param name="department"/>
   <xsl:param name="parentURL">http://www.ox.ac.uk/</xsl:param>
   <xsl:param name="parentWords">Oxford University</xsl:param>
   <xsl:param name="searchURL">http://wwwsearch.ox.ac.uk/cgi-bin/oxunit?oucs</xsl:param>
   <xsl:template name="searchWords">Search</xsl:template>
   <xsl:param name="topNavigationPanel">true</xsl:param>
   <xsl:param name="bottomNavigationPanel">true</xsl:param>
   <xsl:param name="alignNavigationPanel">right</xsl:param>
   <xsl:param name="linkPanel">true</xsl:param>
   <xsl:template name="copyrightStatement"><a 
href="/documentation/copyright.xml">&#169;</a> University of Oxford.</xsl:template>
   <xsl:param name="leftLinks"/>
   <xsl:param name="rightLinks"/>
   <xsl:param name="linksWidth">15%</xsl:param>
   <xsl:param name="makeFrames"/>
   <xsl:param name="makePageTable"/>
   <xsl:param name="frameCols">200,*</xsl:param>
   <xsl:param name="frameAlternateURL"/>
   <xsl:template name="logoFramePicture"><a class="framelogo" target="_top" href="http://www.ox.ac.uk">
   <img src="http://www.oucs.ox.ac.uk/images/newcrest902.gif"
     vspace="5" width="90" height="107" border="0"
  alt="University Of Oxford"/></a></xsl:template>
   <xsl:param name="sectionUpLink"/>
   <xsl:template name="topLink"><p>[<a href="#TOP">Back to top</a>]</p></xsl:template>
   <xsl:param name="appendixWords">Appendix</xsl:param>
   <xsl:param name="tocWords">Contents</xsl:param>
   <xsl:param name="upWord">Up</xsl:param>
   <xsl:param name="nextWord">Next</xsl:param>
   <xsl:param name="previousWord">Previous</xsl:param>
   <xsl:template name="contentsWord">Contents</xsl:template>
   <xsl:param name="dateWord">Date:</xsl:param>
   <xsl:param name="authorWord">&#160;Author:</xsl:param>
   <xsl:param name="divOffset">2</xsl:param>
   <xsl:param name="numberParagraphs"></xsl:param>
   <xsl:param name="generateParagraphIDs">true</xsl:param>
   <xsl:param name="prenumberedHeadings"></xsl:param>
   <xsl:param name="numberSpacer"><xsl:text> </xsl:text></xsl:param><!--&#160;-->
   <xsl:param name="headingNumberSuffix">.<xsl:value-of 
select="$numberSpacer"/></xsl:param>
   <xsl:param name="numberHeadingsDepth">9</xsl:param>
   <xsl:param name="fontURL">span</xsl:param>
   <xsl:param name="tableAlign">left</xsl:param>
   <xsl:param name="cellAlign">left</xsl:param>
   <xsl:param name="showTitleAuthor"/>
   <xsl:param name="footnoteFile"/>
   <xsl:param name="minimalCrossRef"/>
   <xsl:param name="graphicsPrefix"/>
   <xsl:param name="graphicsSuffix">.png</xsl:param>
   <xsl:param name="showFigures">true</xsl:param>
   <xsl:param name="numberFigures">true</xsl:param>
   <xsl:template name="bodyHook"/>
   <xsl:template name="bodyJavaScript"/>
   <xsl:template name="headHook"/>
   <xsl:param name="verbose"/>
   <xsl:param name="downPicture">http://www.oucs.ox.ac.uk/images/down.gif</xsl:param>
   <xsl:param name="useHeaderFrontMatter"/>
   <xsl:param name="outputEncoding">iso-8859-1</xsl:param>

   <xsl:param name="rendSeparator" select="';'"/>

   <xsl:param name="numberHeadings">true</xsl:param>
   <xsl:param name="numberFrontHeadings"/>
   <xsl:param name="numberBackHeadings">true</xsl:param>

   <xsl:template name="numberBackDiv">
     <xsl:param name="minimal"/>
     <xsl:if test="not($numberBackHeadings='')">
        <xsl:number format="A.1.1.1.1.1" 
         level="multiple" count="div|div0|div1|div2|div3|div4|div5|div6"/>
      <xsl:if test="not($minimal)"><xsl:value-of select="$numberSpacer"/>
</xsl:if>
    </xsl:if>
   </xsl:template>

   <xsl:template name="numberFrontDiv">
     <xsl:param name="minimal"/>
     <xsl:if test="not($numberFrontHeadings='')">
         <xsl:number level="multiple" 
                     count="div|div0|div1|div2|div3|div4|div5|div6"/>
        <xsl:if test="not($minimal)"><xsl:value-of select="$numberSpacer"/>
</xsl:if>
      </xsl:if>
   </xsl:template>

   <xsl:template name="numberBodyDiv">
     <xsl:param name="minimal"/>
     <xsl:if test="not($numberHeadings='')">
         <xsl:number level="multiple"
                     count="div|div0|div1|div2|div3|div4|div5|div6"/>
     </xsl:if>
   </xsl:template>

   <xsl:param name="teixslHome">http://www.oucs.ox.ac.uk/stylesheets/</xsl:param> 
   <xsl:param name="noframeWords">No Frames</xsl:param>

   <xsl:template name="singleFileLabel">For Printing</xsl:template>

   <xsl:template name="searchbox"/>
   <xsl:param name="rawIE"/>
<!--
   <xsl:template match="divGen[@type='tsmnews']">
      <hr/>
      <a href="/news/">News</a>
   </xsl:template> -->
<xsl:template name="preAddress"/>
   <xsl:template name="startDivHook"/>
   <xsl:param name="preQuote">&#x2018;</xsl:param>
   <xsl:param name="postQuote">&#x2019;</xsl:param>
</xsl:stylesheet>
