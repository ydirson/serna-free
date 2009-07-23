<!-- 
TEI XSLT stylesheet family version 1.3
RCS: $Date: 2003/11/24 14:50:28 $, $Revision: 1.1 $, $Author: ilia $

XSL stylesheet to format TEI XML documents to HTML or XSL FO

 Copyright 1999-2002 Sebastian Rahtz/Oxford University  <sebastian.rahtz@oucs.ox.ac.uk>

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and any associated documentation files (the
 ``Software''), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
--> 
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  version="1.0">


<xsl:template name="noteID">
  <xsl:choose>
   <xsl:when test="@id">
    <xsl:value-of select="@id"/>
   </xsl:when>
   <xsl:when test="@n">
    <xsl:value-of select="@n"/>
   </xsl:when>
   <xsl:when test="ancestor::back">
    <xsl:number level="any"  count="note[@place='foot']" from="back"/>
   </xsl:when>
   <xsl:otherwise>
    <xsl:number level="any" count="note[@place='foot']" from="body"/>
   </xsl:otherwise>
   </xsl:choose>
</xsl:template>

<xsl:template match="note">

<xsl:choose>

 <xsl:when test="ancestor::bibl">
  (<xsl:apply-templates/>)
 </xsl:when>

 <xsl:when test="@place='inline'">
   <xsl:text> (</xsl:text>
    <xsl:apply-templates/>
   <xsl:text>)</xsl:text>
 </xsl:when>

 <xsl:when test="@place='display'">
   <blockquote>NOTE:
    <xsl:apply-templates/>
   </blockquote>
 </xsl:when>


 <xsl:when test="@place='foot'">
  <xsl:variable name="identifier">
    <xsl:call-template name="noteID"/>
  </xsl:variable>
  <xsl:choose>
   <xsl:when test="$footnoteFile">
    <a class="notelink" href="{$masterFile}-notes.html#{concat('Note',$identifier)}">
    <sup><xsl:value-of select="$identifier"/></sup></a>
   </xsl:when>
   <xsl:otherwise>
    <a class="notelink" href="#{concat('Note',$identifier)}">
    <sup><xsl:value-of select="$identifier"/></sup></a>
   </xsl:otherwise>
  </xsl:choose>
 </xsl:when>

 <xsl:otherwise>
   <xsl:text> [Note: </xsl:text>
    <xsl:apply-templates select="." mode="printnotes"/>
   <xsl:text>]</xsl:text>
 </xsl:otherwise>
</xsl:choose>
</xsl:template>


<!--div|div0|div1|div2|div3|div4|div5|div6-->

<xsl:template name="printNotes">
<xsl:choose>
<xsl:when test="$footnoteFile">
<xsl:variable name="BaseFile">
  <xsl:value-of select="$masterFile"/>
  <xsl:call-template name="addCorpusID"/>
</xsl:variable>

  <xsl:call-template name="outputChunk">
  <xsl:with-param name="ident">
    <xsl:value-of select="concat($BaseFile,'-notes')"/>
  </xsl:with-param>
  <xsl:with-param name="content">
    <xsl:call-template name="writeNotes"/>
  </xsl:with-param>
  </xsl:call-template>
</xsl:when>

<xsl:otherwise>
  <xsl:apply-templates select="text//note[@place='foot']" mode="printnotes"/>
</xsl:otherwise>

</xsl:choose>
</xsl:template>

<xsl:template name="printDivnotes">
 <xsl:variable name="ident">
   <xsl:apply-templates select="." mode="ident"/>
 </xsl:variable>
 <xsl:apply-templates select=".//note[@place='foot']" mode="printnotes">
    <xsl:with-param name="root" select="$ident"/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template match="note" mode="printnotes">
 <xsl:param name="root"/>
<xsl:if test="not(ancestor::bibl)">
<xsl:variable name="identifier">
<xsl:choose>
  <xsl:when test="@id">
    <xsl:value-of select="@id"/>
  </xsl:when>
  <xsl:when test="@n">
    <xsl:value-of select="@n"/>
  </xsl:when>
  <xsl:when test="ancestor::back">
    <xsl:number level="any"  count="note[@place='foot']" from="back"/>
  </xsl:when>
  <xsl:when test="ancestor::front">
    <xsl:number level="any"  count="note[@place='foot']" from="front"/>
  </xsl:when>
  <xsl:otherwise>
    <xsl:number level="any" count="note[@place='foot']"  from="body"/>
  </xsl:otherwise>
</xsl:choose>
</xsl:variable>
 <xsl:variable name="parent">
      <xsl:call-template name="locateParentdiv"/>
 </xsl:variable>
<xsl:if test="$parent=$root or $parent = '' or $footnoteFile">
 <xsl:if test="$verbose">
 <xsl:message>Note <xsl:value-of select="$identifier"/> with parent <xsl:value-of select="$parent"/> requested in <xsl:value-of select="$root"/></xsl:message>
 </xsl:if>
<p>
 <a name="{concat('Note',$identifier)}"><xsl:value-of select="$identifier"/>. </a>
 <xsl:apply-templates/>
</p>
</xsl:if>
</xsl:if>
</xsl:template>


<xsl:template name="writeNotes">
 <html><xsl:call-template name="addLangAtt"/> 
 <head>
 <title>Notes for
    <xsl:apply-templates select="descendant-or-self::text/front//docTitle//text()"/></title>
 <xsl:call-template name="includeCSS"/>
 </head>
 <body>
 <xsl:call-template name="bodyHook"/>
 <xsl:call-template name="bodyJavaScript"/>
 <xsl:call-template name="stdheader">
  <xsl:with-param name="title">
   <xsl:text>Notes for </xsl:text>
    <xsl:apply-templates select="descendant-or-self::text/front//docTitle//text()"/>
  </xsl:with-param>
 </xsl:call-template>

 <xsl:call-template name="processFootnotes"/>

 <xsl:call-template name="stdfooter">
       <xsl:with-param name="date">
         <xsl:choose>
          <xsl:when test="ancestor-or-self::TEI.2/teiHeader/revisionDesc//date[1]">
            <xsl:value-of select="ancestor-or-self::TEI.2/teiHeader/revisionDesc//date[1]"/>
          </xsl:when>
          <xsl:otherwise>
    	   <xsl:value-of select="ancestor-or-self::TEI.2//front//docDate"/>
          </xsl:otherwise>    
         </xsl:choose>
       </xsl:with-param>
       <xsl:with-param name="author">
         <xsl:apply-templates select="ancestor-or-self::TEI.2//front//docAuthor" mode="author"/>
       </xsl:with-param>
 </xsl:call-template>
 </body>
 </html>
</xsl:template>

<xsl:template name="processFootnotes">
  apply-templates select="text//note[@place='foot']" mode="printnotes"/>
</xsl:template>
</xsl:stylesheet>
