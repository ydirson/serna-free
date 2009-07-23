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
<xsl:template match="figure" mode="header">
<xsl:if test="head">
  <p>
  <xsl:if test="$numberFigures">
     Figure <xsl:number level="any"/>.<xsl:text> </xsl:text>
  </xsl:if>
  <xsl:apply-templates select="head"/></p>
</xsl:if>
</xsl:template>

<xsl:template match="figure">

 <xsl:variable name="File">
  <xsl:choose> 
  <xsl:when test="@file">
   <xsl:value-of select="@file"/>
   <xsl:if test="not(contains(@file,'.'))">
      <xsl:value-of select="$graphicsSuffix"/>
   </xsl:if>
  </xsl:when>
  <xsl:when test="@url">
   <xsl:value-of select="@url"/>
   <xsl:if test="not(contains(@url,'.'))">
      <xsl:value-of select="$graphicsSuffix"/>
   </xsl:if>
  </xsl:when>
  <xsl:otherwise>
    <xsl:variable name="entity">
      <xsl:value-of select="unparsed-entity-uri(@entity)"/>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="starts-with($entity,'file:')">
        <xsl:value-of select="substring-after($entity,'file:')"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$entity"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:otherwise>
  </xsl:choose>
 </xsl:variable>

<xsl:if test="@id">
     <a name="{@id}"/>
</xsl:if>
<xsl:choose>
  <xsl:when test="$showFigures='true'">
    <span>
    <xsl:if test="@id">
      <xsl:attribute name="name"><xsl:value-of select="@id"/></xsl:attribute>
    </xsl:if>
   <xsl:if test="string-length(@rend) &gt;0">
    <xsl:attribute name="class"><xsl:value-of select="@rend"/></xsl:attribute>
   </xsl:if>
  <img src="{$graphicsPrefix}{$File}">
   <xsl:if test="@rend='inline'">
    <xsl:attribute name="border">0</xsl:attribute>
   </xsl:if>
   <xsl:if test="@width and not(contains(@width,'in'))">
    <xsl:attribute name="width"><xsl:value-of select="@width"/></xsl:attribute>
   </xsl:if>
   <xsl:if test="@height and not(contains(@height,'in'))">
    <xsl:attribute name="height"><xsl:value-of select="@height"/></xsl:attribute>
   </xsl:if>
   <xsl:attribute name="alt">
   <xsl:choose>
   <xsl:when test="figDesc">
        <xsl:value-of select="figDesc/text()"/>
   </xsl:when>
    <xsl:otherwise>
        <xsl:value-of select="head/text()"/>
    </xsl:otherwise>
     </xsl:choose>
     </xsl:attribute>
   <xsl:call-template name="imgHook"/>
 </img>
</span>
 </xsl:when>
 <xsl:otherwise>
   <hr/>
   <p>Figure <xsl:number level="any"/>
    file <xsl:value-of select="$File"/>
 <xsl:if test="figDesc">
  [<xsl:apply-templates select="figDesc/text()"/>]
 </xsl:if>
   </p>
   <hr/>
  </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="head"><p>
  <xsl:if test="$numberFigures" >
    Figure <xsl:number level="any"/>.<xsl:text> </xsl:text>
  </xsl:if>
  <xsl:apply-templates select="head"/>
   </p>
   </xsl:if>
</xsl:template>

<xsl:template name="imgHook"/>

<xsl:template match="figDesc"/>

</xsl:stylesheet>
