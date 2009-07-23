<!-- 
RCS: $Date: 2003/11/24 14:50:27 $, $Revision: 1.1 $, $Author: ilia $

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
  version="1.0">


<xsl:import href="teihtml.xsl"/>
<xsl:import href="teihtml-oucsdoc.xsl"/>

<xsl:param name="cssFile">/stylesheets/tei-oucs.css</xsl:param>
<xsl:param name="subTocDepth">0</xsl:param>
<xsl:param name="splitLevel">-1</xsl:param>
<xsl:param name="homeLabel">ACDT</xsl:param>
<xsl:param name="homeURL">/acdt</xsl:param>
<xsl:template name="searchWords">
  <img border="0" src="/images/search.gif"/>&#160;Search
</xsl:template>
<xsl:template name="feedbackWords">
  <img border="0" src="/images/feedback.gif"/>&#160;Feedback
</xsl:template>
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
 <li class="toc"><xsl:call-template name="header">
           <xsl:with-param name="toc" select="$pointer"/>
      </xsl:call-template>
 </li>
</xsl:if>
</xsl:template>

<xsl:template name="continuedToc">
   <xsl:if test="div|Exercise">
   <ul class="toc">
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
 <li><a class="toc" href="{$pointer}"><xsl:text>Exercise </xsl:text>
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
 <xsl:attribute name="border">1</xsl:attribute>
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

<xsl:template name="summaryToc">
	 <html> 
<xsl:comment>THIS IS A GENERATED FILE. DO NOT EDIT</xsl:comment>
	 <head>
	 <title><xsl:call-template name="generateDivtitle"/></title>
         <xsl:call-template name="headHook"/>
         <xsl:if test="not($cssFile = '')">
           <link rel="stylesheet" type="text/css" href="{$cssFile}"/>
         </xsl:if>
	 </head>
	 <body>
         <xsl:call-template name="bodyHook"/>
         <xsl:call-template name="bodyJavaScript"/>
         <a name="TOP"/>
         <div  class="teidiv">
  <p>Select headings on the left-hand side to see  
 more explanation of the links on the right.</p>
 <table  cellspacing="7">
  <thead><th nowrap="nowrap"/><th/></thead>
  <xsl:for-each select="//body/div">
<xsl:text>
</xsl:text> 
   <tr class="summaryline">
    <td class="summarycell" valign="top" align="right">
    <b><a class="nolink" targe="_top">
    <xsl:attribute name="href">
            <xsl:apply-templates mode="xrefheader" select="."/>
    </xsl:attribute>
    <xsl:value-of select="head"/></a></b>
    </td>
    <td  class="link" valign="top" >
       <xsl:for-each select=".//xref|.//xptr">
       <xsl:if test="position() &gt; 1">
           <xsl:text>&#160;</xsl:text>
           <img src="/images/dbluball.gif"/>
           <xsl:text> </xsl:text>
	</xsl:if>
        <span class="nowrap"><xsl:apply-templates select="."/></span>
       </xsl:for-each>
    </td>
   </tr>
  </xsl:for-each>
 </table>
        </div>
	</body>
	</html>

 </xsl:template>

</xsl:stylesheet>
