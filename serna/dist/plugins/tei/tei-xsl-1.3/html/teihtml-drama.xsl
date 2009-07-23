<?xml version="1.0" encoding="utf-8"?>
<!-- 
TEI XSLT stylesheet family version 1.3
RCS: $Date: 2003/11/24 14:50:27 $, $Revision: 1.1 $, $Author: ilia $

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
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  >
<!-- elaborated by Nick  Nicholas <nicholas@uci.edu>, March 2001 -->
<xsl:template match="sp">
<dl>
 <dt>
   <xsl:if test="@id"><a name="{@id}"/></xsl:if>
   <xsl:apply-templates select="speaker"/>
 </dt>
<dd><xsl:apply-templates select="p | l | lg | seg | ab | stage"/></dd>
</dl>
</xsl:template>

<!-- paragraphs inside speeches do very little-->
 <xsl:template match="sp/p">
    <xsl:apply-templates/>
</xsl:template>


<xsl:template match="p/stage">
<em><xsl:apply-templates/></em>
</xsl:template>

<xsl:template match="stage">
<p><em>
      <xsl:apply-templates/>
</em></p>
</xsl:template>

<xsl:template match="castList">
<ul>
	<xsl:apply-templates/>
</ul>
</xsl:template>

<xsl:template match="castGroup">
<ul>
	<xsl:apply-templates/>
</ul>
</xsl:template>

<xsl:template match="castItem">
<li>
	<xsl:apply-templates/>
</li>
</xsl:template>

<xsl:template match="role">
<strong>
	<xsl:apply-templates/>
</strong>
</xsl:template>

<xsl:template match="roleDesc">
<blockquote>
	<xsl:apply-templates/>
</blockquote>
</xsl:template>

<xsl:template match="actor">
<em>
	<xsl:apply-templates/>
</em>
</xsl:template>

<xsl:template match="set">
<em>
	<xsl:apply-templates/>
</em>
</xsl:template>

<xsl:template match="view">
<em>
	<xsl:apply-templates/>
</em>
</xsl:template>

<xsl:template match="camera">
<em>
	<xsl:apply-templates/>
</em>
</xsl:template>

<xsl:template match="caption">
<em>
	<xsl:apply-templates/>
</em>
</xsl:template>

<xsl:template match="sound">
<em>
	<xsl:apply-templates/>
</em>
</xsl:template>

<xsl:template match="tech">
<em>
	<xsl:apply-templates/>
</em>
</xsl:template>


</xsl:stylesheet>


