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

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

<xsl:template name="generateTitle">
 <xsl:choose>
   <xsl:when test="$useHeaderFrontMatter='true' and ancestor-or-self::TEI.2/text/front//docTitle">
     <xsl:apply-templates 
       select="ancestor-or-self::TEI.2/text/front//docTitle"/>
     </xsl:when>
   <xsl:otherwise>
    <xsl:apply-templates 
      select="ancestor-or-self::TEI.2/teiHeader/fileDesc/titleStmt/title"
      mode="htmlheader"/>
   </xsl:otherwise>
</xsl:choose>
</xsl:template>


<xsl:template name="generateDate">
  <xsl:param name="showRev">true</xsl:param>
<xsl:variable name="realdate">
 <xsl:choose>
   <xsl:when test="$useHeaderFrontMatter='true' and ancestor-or-self::TEI.2/text/front//docDate">
  <xsl:apply-templates 
    select="ancestor-or-self::TEI.2/text/front//docDate" 
    mode="date"/>
  </xsl:when>
  <xsl:when test="ancestor-or-self::TEI.2/teiHeader/fileDesc/editionStmt/descendant::date">
  <xsl:apply-templates select="ancestor-or-self::TEI.2/teiHeader/fileDesc/editionStmt/descendant::date[1]"/>
    </xsl:when>
  </xsl:choose>
</xsl:variable>

<xsl:variable name="revdate">
<xsl:apply-templates 
 select="ancestor-or-self::TEI.2/teiHeader/revisionDesc/descendant::date[1]"/>
</xsl:variable>
<xsl:value-of select="$dateWord"/><xsl:text> </xsl:text>
<xsl:if test="not($realdate = '')">
  <xsl:value-of select="$realdate"/>
</xsl:if>


<xsl:if test="$showRev='true' and not($revdate = '') and not ($revdate='&#36;Date$')">
 (revised <xsl:choose>
  <xsl:when test="starts-with($revdate,'$Date:')"> <!-- it's RCS -->
    <xsl:value-of select="substring($revdate,16,2)"/>
    <xsl:text>/</xsl:text>
    <xsl:value-of select="substring($revdate,13,2)"/>
    <xsl:text>/</xsl:text>
    <xsl:value-of select="substring($revdate,8,4)"/> 
  </xsl:when>
  <xsl:otherwise>
   <xsl:value-of select="$revdate"/>    
  </xsl:otherwise>
 </xsl:choose>
 <xsl:text>)</xsl:text></xsl:if>

</xsl:template>

<xsl:template name="generateAuthor">
 <xsl:choose>
   <xsl:when test="$useHeaderFrontMatter='true' and ancestor-or-self::TEI.2/text/front//docAuthor">
     <xsl:apply-templates select="ancestor-or-self::TEI.2/text/front//docAuthor[1]"  mode="author"/>
  </xsl:when>
  <xsl:when test="ancestor-or-self::TEI.2/teiHeader/fileDesc/titleStmt/author">
  <xsl:apply-templates select="ancestor-or-self::TEI.2/teiHeader/fileDesc/titleStmt/author"/>
    </xsl:when>
    <xsl:when test="ancestor-or-self::TEI.2/text/front//docAuthor">
      <xsl:apply-templates select="ancestor-or-self::TEI.2/text/front//docAuthor[1]" mode="author"/>
  </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template name="generateAuthorList">
<xsl:variable name="realauthor">
  <xsl:call-template name="generateAuthor"/>
</xsl:variable>
<xsl:variable name="revauthor">
<xsl:apply-templates 
select="ancestor-or-self::TEI.2/teiHeader/revisionDesc/change[1]/respStmt/name/text()"/>
</xsl:variable>
 <xsl:value-of select="$authorWord"/><xsl:text> </xsl:text>
<xsl:if test="not($realauthor = '')">
 <xsl:value-of select="$realauthor"/>
</xsl:if>
<xsl:if test="not($revauthor = '') and not(normalize-space($revauthor)='&#36;Author$')">
 (revised <xsl:choose>
  <xsl:when test="starts-with($revauthor,'$Author:')"> <!-- it's RCS -->
    <xsl:value-of 
select="normalize-space(substring-before(substring-after($revauthor,'Author:'),'$'))"/>
  </xsl:when>
  <xsl:otherwise>
   <xsl:value-of select="$revauthor"/>    
  </xsl:otherwise>
 </xsl:choose>
 <xsl:text>)</xsl:text>
</xsl:if>

</xsl:template>

<xsl:template match="title" mode="htmlheader">
	<xsl:apply-templates/>
</xsl:template>

</xsl:stylesheet>
