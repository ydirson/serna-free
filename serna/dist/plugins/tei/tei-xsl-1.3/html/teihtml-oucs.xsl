<!-- 
RCS: $Date: 2003/11/24 14:50:28 $, $Revision: 1.1 $, $Author: ilia $

XSL stylesheet to format TEI XML documents to HTML

 Copyright 1999-2001 Sebastian Rahtz/Oxford University  <sebastian.rahtz@oucs.ox.ac.uk>

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and any associated documentation files (the
 ``Software''), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be importd
 in all copies or substantial portions of the Software.
--> 
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  xmlns:html="http://www.w3.org/1999/xhtml"
  version="1.0">


<xsl:import href="teihtml.xsl"/>
<xsl:import href="teihtml-oucsdoc.xsl"/>

<xsl:param name="cssFile">/stylesheets/tei-oucs.css</xsl:param>
<xsl:param name="makePageTable">true</xsl:param>
<xsl:param name="subTocDepth">0</xsl:param>
<xsl:param name="splitLevel">0</xsl:param>
<xsl:param name="homeLabel">oucs</xsl:param>
<xsl:param name="homeURL">/</xsl:param>
<xsl:param name="linkPanel"></xsl:param>
<xsl:param name="linksWidth">10%</xsl:param>
<xsl:param name="bottomNavigationPanel">true</xsl:param>
<xsl:param name="autoToc"></xsl:param>
<xsl:param name="topNavigationPanel"></xsl:param>

<xsl:output encoding="iso-8859-1" method="html"/>

<xsl:template name="searchWords">
  <img border="0" src="/images/search.gif"/>&#160;Search
</xsl:template>
<xsl:param name="feedbackURL">/feedback/</xsl:param>
<xsl:template name="feedbackWords">Feedback</xsl:template>
<xsl:template match="xptr[@url]">
   <xsl:element name="{$fontURL}">
     <a class="xptr" href="{@url}">
     <xsl:call-template name="urltargets"/>
 <xsl:choose>
  <xsl:when test="starts-with(@url,'mailto:')">
     <xsl:value-of select="substring-after(@url,'mailto:')"/>
  </xsl:when>
  <xsl:when test="starts-with(@url,'http://')">
     <xsl:value-of select="substring-after(@url,'http://')"/>
  </xsl:when>
  <xsl:otherwise>
     <xsl:value-of select="@url"/>
  </xsl:otherwise>
  </xsl:choose>
  </a>
 </xsl:element>   
</xsl:template>


<!-- for Exercise division -->
<xsl:template match="Exercise" mode="header"> 
 <xsl:param name="minimal"/>
 <xsl:call-template name="header">
     <xsl:with-param name="minimal" select="$minimalCrossRef"/>
 </xsl:call-template>
</xsl:template>


<xsl:template match="Exercise" mode="maketoc">
 <xsl:param name="forcedepth"/>
<xsl:if test="head">
 <xsl:variable name="Depth">
 <xsl:choose>
  <xsl:when test="not($forcedepth='')">
     <xsl:value-of select="$forcedepth"/>
  </xsl:when>
  <xsl:otherwise>
     <xsl:value-of select="$tocDepth"/>
  </xsl:otherwise>
 </xsl:choose>
 </xsl:variable>
 <xsl:variable name="thislevel">
    <xsl:value-of select="count(ancestor::div)"/>
 </xsl:variable>
 <xsl:variable name="pointer">
      <xsl:apply-templates mode="xrefheader" select="."/>
 </xsl:variable>
 <li>
<xsl:if test="$cssFile"><xsl:attribute name="class">toc</xsl:attribute></xsl:if>
<xsl:call-template name="header">
           <xsl:with-param name="toc" select="$pointer"/>
      </xsl:call-template>
 </li>
</xsl:if>
</xsl:template>

<xsl:template name="continuedToc">
   <xsl:if test="div|Exercise">
   <ul>
<xsl:if test="$cssFile"><xsl:attribute name="class">toc</xsl:attribute></xsl:if>
   <xsl:apply-templates select="Exercise|div" mode="maketoc"/>
   </ul>
 </xsl:if>
 </xsl:template>

<xsl:template match="Exercise" mode="xrefheader">
  <xsl:variable name="ident">
   <xsl:apply-templates select="." mode="ident"/>
  </xsl:variable>
  <xsl:variable name="depth">
   <xsl:apply-templates select="." mode="depth"/>
  </xsl:variable>
  <xsl:variable name="Hash">
   <xsl:if test="$makeFrames and not($STDOUT='true')">
    <xsl:value-of select="$masterFile"/>
    <xsl:if test="ancestor::teiCorpus.2">
     <xsl:text>-</xsl:text>
      <xsl:choose>
      <xsl:when test="@id"><xsl:value-of select="@id"/></xsl:when> 
      <xsl:otherwise><xsl:number/></xsl:otherwise>
     </xsl:choose>
    </xsl:if>
    <xsl:text>.html</xsl:text>
   </xsl:if>
   <xsl:text>#</xsl:text>
  </xsl:variable>
  <xsl:choose>
   <xsl:when test="$STDOUT='true'">
       <xsl:value-of select="concat($Hash,$ident)"/>
   </xsl:when>
   <xsl:when test="ancestor::back and not($splitBackmatter)">
       <xsl:value-of select="concat($Hash,$ident)"/>
   </xsl:when>
   <xsl:when test="ancestor::front and not($splitFrontmatter)">
       <xsl:value-of select="concat($Hash,$ident)"/>
   </xsl:when>
   <xsl:when test="$splitLevel= -1">
    <xsl:value-of select="concat($Hash,$ident)"/>
   </xsl:when>
   <xsl:when test="$depth &lt;= $splitLevel">
     <xsl:value-of select="concat($ident,'.html')"/>
   </xsl:when>
   <xsl:otherwise>
   <xsl:variable name="parent">
      <xsl:call-template name="locateParentdiv"/>
   </xsl:variable>
   <xsl:value-of select="concat(concat($parent,'.html#'),$ident)"/>
   </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="Exercise" mode="maketoc" priority="999">
 <xsl:variable name="pointer">
      <xsl:apply-templates mode="xrefheader" select="."/>
 </xsl:variable>
 <li><a href="{$pointer}">
<xsl:if test="$cssFile"><xsl:attribute name="class">toc</xsl:attribute></xsl:if>
<xsl:text>Exercise </xsl:text>
       <xsl:number level="any" count="Exercise"/>
       <xsl:text>. </xsl:text>
<xsl:apply-templates select="head" mode="plain"/></a>
 </li>
</xsl:template>


<xsl:template match="Exercise">
 <xsl:variable name="pointer">
      <xsl:apply-templates mode="ident" select="."/>
 </xsl:variable>
<hr/>
<table class="exercise">
<tr>
<td>
 <b><a name="{$pointer}">Exercise</a><xsl:text> </xsl:text>
       <xsl:number level="any" count="Exercise"/>
       <xsl:text>. </xsl:text>
<xsl:apply-templates select="head" mode="plain"/></b></td>
</tr>
<tr><td><em>Description</em></td></tr>
<tr><td><xsl:apply-templates/></td></tr>
</table>       
<hr/>
</xsl:template>

<xsl:template match="Exercise/head"/>


<xsl:template name="imgHook">
 <xsl:attribute name="align">center</xsl:attribute>
 <xsl:if test="not(@rend='inline')">
  <xsl:attribute name="border">1</xsl:attribute>
</xsl:if>
 <xsl:attribute name="alt"><xsl:value-of select="head"/></xsl:attribute>
</xsl:template>


<xsl:template match="q[@rend='literal']">
 <tt>
 <xsl:apply-templates/>
 </tt>
</xsl:template>

<xsl:template match="item" mode="runin">
  <xsl:apply-templates/> / 
</xsl:template>

<xsl:template match="divGen[@type='summary']">
 <xsl:if test="not($STDOUT='true')">
  <xsl:call-template name="outputChunk">
    <xsl:with-param name="ident" select="'summaryToc'"/>
   <xsl:with-param name="content">
     <xsl:call-template name="summaryToc"/>
   </xsl:with-param>
   </xsl:call-template>
 </xsl:if>
</xsl:template>


<xsl:template name="searchbox">
  <form method="get" action="http://wwwsearch.ox.ac.uk/cgi-bin/oxford">
 <input type="hidden" name="restrict"
value=".oucs.ox.ac.uk|hfs.ox.ac.uk|news.ox.ac.uk|.hcu.ox.ac.uk|http://info.ox.ac.uk/bnc/|.natcorp.ox.ac.uk|.humbul.ac.uk"/>
 <input type="hidden" name="config" value="oxford"/>
 <input type="text" size="20" name="words" value=" "/>
 <br/><input type="submit" value="Search OUCS web"
       style="color: #ffffff; background-color: #000066"/>
</form>
</xsl:template>

<xsl:template name="singleFileLabel">
  <!--Single&#160;file&#160;for&#160;printing-->
  <img alt="Single file for printing" src="/images/print.gif" border="0"/>
</xsl:template>


<xsl:template match="xptr[@type='course']">
 <xsl:variable name="code" select="substring(@to,1,2)"/>
 <xsl:apply-templates select="document('../courses/course.xml')/courses/course[@id=$code]/title" mode="use"/>
 <xsl:variable name="software" 
          select="document(concat(concat('../courses/',$code),'.xml'))/course/software"/>
  <xsl:if test="not($software = '')">
   <xsl:text> (</xsl:text>
   <xsl:value-of select="$software"/>
   <xsl:text>)</xsl:text>
  </xsl:if>
 (course code <a href="{$code}.xml"><xsl:value-of select="$code"/></a>).
 <xsl:variable name="dates" select="document('../courses/course_pres.xml')/presentations/pres[@pid=$code]"/>
 <xsl:choose>
  <xsl:when test="$dates">
  </xsl:when>
  <xsl:otherwise>
  <i>The course is not available this term.</i>
  </xsl:otherwise>
 </xsl:choose>
</xsl:template>

<xsl:template match="item/p[1]">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template name="generateSubTitle">
  <xsl:variable name="authority">
    <xsl:value-of select="/TEI.2/teiHeader/fileDesc/publicationStmt/authority"/>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="$authority=''"/>
    <xsl:when test="$authority='OUCS'"/>
    <xsl:when test="$authority='NSMS'">
      <h3 class="subtitle">Network Systems Management Services</h3>
    </xsl:when>
    <xsl:when test="$authority='INTERNAL'">
      <h3 class="subtitle">OUCS Internal Document</h3>
    </xsl:when> 
 </xsl:choose>

</xsl:template>

<xsl:template name="unknownRend">
 <xsl:param name="rend"/>
 <xsl:param name="rest"/>
   <span class="{$rend}">
     <xsl:call-template name="applyRend">
      <xsl:with-param name="parms" select="$rest"/>
     </xsl:call-template>
   </span>
 </xsl:template>

<xsl:template match="q[@rend='display']">
 <p class="blockquote">
  <xsl:apply-templates/>
 </p>
</xsl:template>

   <xsl:template name="logoFramePicture">
     <span class="oucstitle"><a class="frametoc" 
      href="/">Oxford University<br/>
     Computing Services</a><br/></span>
<a class="framelogo" target="_top" href="http://www.ox.ac.uk">
   <img src="/images/newcrest902.gif"
     vspace="5" width="90" height="107" border="0"
  alt="University Of Oxford"/></a>
</xsl:template>

<xsl:template name="leftHandFrame">
  <xsl:param name="currentID"/>
          <xsl:call-template name="logoFramePicture"/>
        <br/>
        <xsl:text>
</xsl:text>
       <a class="frametoc" 
          href="http://wwwsearch.ox.ac.uk/cgi-bin/oxunit?oucs">Full 
         search</a>
     <br/>
        <xsl:text>
</xsl:text>
       <a target="_top" href="{$feedbackURL}" class="frametoc">
        <xsl:call-template name="feedbackWords"/></a>
        <br/>
<xsl:if test="text/body/div">
         <a class="frametoc">
          <xsl:attribute name="href">
            <xsl:value-of select="concat($masterFile,$standardSuffix)"/>
            <xsl:text>?style=printable</xsl:text>
          </xsl:attribute>
          <xsl:call-template name="singleFileLabel"/></a> 
</xsl:if>
          <hr/>
     <xsl:choose>
      <xsl:when test="$currentID=''">
         <xsl:call-template name="linkListContents">
           <xsl:with-param name="style" select="'frametoc'"/>
         </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="count(key('IDS',$currentID))&gt;0">
             <xsl:for-each select="key('IDS',$currentID)">  
               <xsl:call-template name="linkListContents">
                 <xsl:with-param name="style" select="'frametoc'"/>
               </xsl:call-template>
             </xsl:for-each>
          </xsl:when>
          <xsl:otherwise>
           <xsl:apply-templates select="descendant::text" mode="xpath">
               <xsl:with-param name="xpath" select="$currentID" />
               <xsl:with-param name="action" select="'toclist'" />
           </xsl:apply-templates>
        </xsl:otherwise>
       </xsl:choose>
      </xsl:otherwise>
     </xsl:choose>
</xsl:template>

<!--
<xsl:template match="html:*">
     <xsl:element name="{local-name()}">
       <xsl:copy-of select="@*"/>
       <xsl:apply-templates/>
     </xsl:element>
</xsl:template>
-->
<xsl:template match="formerrors">
    <xsl:apply-templates select="..//error"/>
</xsl:template>

<xsl:template match="error">
  <br/>
   <span class="form_error"><xsl:value-of select="."/></span>
</xsl:template>

<xsl:template match="textfield">
    <input 
        type="text"
        name="{@name|name}" 
        value="{@value|value}" 
        size="{@width|width}" 
        maxlength="{@maxlength|maxlength}" />
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="password">
    <input 
        type="password"
        name="{@name|name}" 
        value="{@value|value}" 
        size="{@width|width}" 
        maxlength="{@maxlength|maxlength}" />
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="checkbox">
    <input
        type="checkbox"
        name="{@name|name}"
        value="{@value|value}" />
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="submit_button">
    <input
        type="submit"
        name="{@name|name}"
        value="{@value|value}" />
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="hidden">
    <input
        type="hidden"
        name="{@name|name}"
        value="{@value|value}" />
</xsl:template>

<xsl:template match="options/option">
  <option value="{@value|value}">
    <xsl:if test="selected[. = 'selected'] | @selected[. = 'selected']">
      <xsl:attribute name="selected">selected</xsl:attribute>
    </xsl:if>
    <xsl:value-of select="@text|text"/>
  </option>
</xsl:template>

<xsl:template match="single_select">
    <select name="{@name|name}">
        <xsl:apply-templates select="options/option"/>
    </select>
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="textarea">
    <textarea name="{@name|name}" cols="{@cols|cols}" rows="{@rows|rows}">
    <xsl:if test="@wrap|wrap"><xsl:attribute name="wrap">physical</xsl:attribute></xsl:if>
    <xsl:value-of select="@value|value"/>
    </textarea> <br />
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="form">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xptr[@doc and @rend='expand']">
  <xsl:variable name="url">
    <xsl:value-of select="unparsed-entity-uri(@doc)"/>
  </xsl:variable>
  <a>
   <xsl:attribute name="class">
   <xsl:choose>
     <xsl:when test="@rend"><xsl:value-of select="@rend"/></xsl:when>
     <xsl:otherwise>quicklink</xsl:otherwise>     
   </xsl:choose>
   </xsl:attribute>
 <xsl:attribute name="href">
      <xsl:value-of select="$url"/>
</xsl:attribute>
   <xsl:element name="{$fontURL}">
  <xsl:choose>
    <xsl:when test="starts-with($url,'http:')">
     <xsl:value-of select="$url"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>http://www.oucs.ox.ac.uk</xsl:text>
      <xsl:value-of select="$url"/>
    </xsl:otherwise>
  </xsl:choose>
   </xsl:element>
</a>
</xsl:template>

</xsl:stylesheet>
