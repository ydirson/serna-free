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
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:template match="teiCorpus.2">
 <xsl:for-each select="TEI.2">
 <xsl:if test="$verbose">
   <xsl:message>Process <xsl:value-of select="teiHeader/fileDesc/titleStmt/title"/></xsl:message>
 </xsl:if>
   <xsl:apply-templates select="." mode="split"/>
 </xsl:for-each>
 <html><xsl:call-template name="addLangAtt"/> 
 <head>
 <title><xsl:apply-templates select="teiHeader/fileDesc/titleStmt/title/text()"/></title>
 <xsl:call-template name="includeCSS"/>
 </head>
 <body>
 <xsl:call-template name="bodyHook"/>
 <xsl:call-template name="bodyJavaScript"/>
 <xsl:call-template name="stdheader">
  <xsl:with-param name="title">
   <xsl:apply-templates select="teiHeader/fileDesc/titleStmt/title"/>
  </xsl:with-param>
 </xsl:call-template>

 <xsl:call-template name="corpusBody"/>

 <xsl:call-template name="stdfooter">
       <xsl:with-param name="date">
         <xsl:choose>
          <xsl:when test="teiHeader/revisionDesc//date[1]">
            <xsl:value-of select="teiHeader/revisionDesc//date[1]"/>
          </xsl:when>
          <xsl:otherwise>
    	   <xsl:text>(undated)</xsl:text>
          </xsl:otherwise>    
         </xsl:choose>
       </xsl:with-param>
       <xsl:with-param name="author"/>
   </xsl:call-template>
 </body>
 </html>
</xsl:template>


<xsl:template name="corpusBody">
<ul>
 <xsl:for-each select="TEI.2">
 <li>
    <a> <xsl:attribute name="href">
     <xsl:apply-templates mode="xrefheader" select="."/>
     </xsl:attribute>
     <xsl:call-template name="header">
     <xsl:with-param name="minimal"/>
     </xsl:call-template>
     </a>  
 </li>
 </xsl:for-each>
</ul>
</xsl:template>

<xsl:template match="catRef">
  <xsl:if test="preceding-sibling::catRef"><xsl:text> </xsl:text></xsl:if>
  <em><xsl:value-of select="@scheme"/></em>:
  <xsl:apply-templates select="key('IDS',@target)/catDesc"/>
</xsl:template>

</xsl:stylesheet>
