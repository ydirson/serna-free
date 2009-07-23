<!-- 
TEI XSLT stylesheet family version 1.3
RCS: $Date: 2003/11/24 14:49:48 $, $Revision: 1.1 $, $Author: ilia $

XSL FO stylesheet to format TEI XML documents 

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
  xmlns:fotex="http://www.tug.org/fotex"
  xmlns:exsl="http://exslt.org/common"
  exclude-result-prefixes="exsl"
  extension-element-prefixes="exsl"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns:fo="http://www.w3.org/1999/XSL/Format"  >

<xsl:template match='table' mode="xref">
  <xsl:if test="$xrefShowTitle">
     <xsl:value-of select="$tableWord"/>
     <xsl:text> </xsl:text>
   </xsl:if>
   <xsl:call-template name="calculateTableNumber"/>
</xsl:template>

<xsl:template match="table">
<xsl:choose>
 <xsl:when test="@rend='eqnarray'">
   <fotex:eqnarray>
     <xsl:apply-templates select=".//formula"/>
   </fotex:eqnarray>
 </xsl:when>
 <xsl:when test=".//formula[@type='subeqn']">
   <fotex:eqnarray>
     <xsl:apply-templates select=".//formula"/>
   </fotex:eqnarray>
 </xsl:when>
 <xsl:when test="$inlineTables or @rend='inline'">
  <xsl:if test="head">
     <fo:block>
   <xsl:call-template name="tableCaptionstyle"/>       
     <xsl:call-template name="addID"/>
     <xsl:if test="$makeTableCaption='true'">
     <xsl:value-of select="$tableWord"/>
     <xsl:call-template name="calculateTableNumber"/>
     <xsl:text>. </xsl:text>       
     </xsl:if>
     <xsl:apply-templates select="head"/>       
     </fo:block>
  </xsl:if>
  <xsl:call-template name="blockTable"/>
 </xsl:when>
 <xsl:otherwise>
   <xsl:call-template name="floatTable"/>
 </xsl:otherwise>
</xsl:choose>
</xsl:template>

<xsl:template name="floatTable">
 <fo:table-and-caption>
      <xsl:if test="rend='landscape'">
        <xsl:attribute name="reference-direction">-90</xsl:attribute>
      </xsl:if>
      <xsl:call-template name="addID"/>
   <fo:table-caption>
  <fo:block text-align="{$tableCaptionAlign}"
        space-after="{$spaceBelowCaption}">
     <xsl:value-of select="$tableWord"/>
     <xsl:call-template name="calculateTableNumber"/>
     <xsl:text>. </xsl:text>
     <xsl:apply-templates select="head"/>
  </fo:block>
   </fo:table-caption>
   <xsl:call-template name="blockTable"/>
 </fo:table-and-caption>
</xsl:template>

<xsl:template name="blockTable">
 <fo:table  text-align="{$tableAlign}"
	     font-size="{$tableSize}">
   <xsl:call-template name="addID"/>
   <xsl:call-template name="deriveColSpecs"/>
   <fo:table-body text-indent="0pt">
     <xsl:for-each select="row">
       <xsl:text>
</xsl:text>
       <fo:table-row>
         <xsl:apply-templates select="cell"/>
       </fo:table-row>
      </xsl:for-each>
   </fo:table-body>
 </fo:table>
</xsl:template>

<xsl:template match="cell">
  <fo:table-cell>
  <xsl:if test="@cols &gt; 1">
    <xsl:attribute name="number-columns-spanned">
      <xsl:value-of select="@cols"/>
    </xsl:attribute>
  </xsl:if>
  <xsl:if test="@rows &gt; 1">
    <xsl:attribute name="number-rows-spanned">
      <xsl:value-of select="@rows"/>
    </xsl:attribute>
  </xsl:if>
  <xsl:call-template name="cellProperties"/>
 <fo:block>
  <xsl:choose>
   <xsl:when test="@role='label' or parent::row[@role='label']">
     <xsl:attribute name="font-weight">bold</xsl:attribute>
   </xsl:when>
  </xsl:choose>
     <xsl:apply-templates/>
 </fo:block>
</fo:table-cell>
</xsl:template>

<xsl:template name="cellProperties" >
  <xsl:if test="@role='hi' or @role='label' or parent::row/@role='label'">
    <xsl:attribute name="background-color">silver</xsl:attribute>
  </xsl:if>
  <xsl:choose>
  <xsl:when test="ancestor::table[1][@rend='frame']">
   <xsl:if test="not(parent::row/preceding-sibling::row)">
    <xsl:attribute name="border-before-style">solid</xsl:attribute>
   </xsl:if>
   <xsl:attribute name="border-after-style">solid</xsl:attribute>
   <xsl:if test="not(following-sibling::cell)">
     <xsl:attribute name="border-end-style">solid</xsl:attribute>
   </xsl:if>
   <xsl:attribute name="border-start-style">solid</xsl:attribute>
  </xsl:when>
  <xsl:otherwise>
  </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="not(ancestor::table/@rend='tight')">   
  <xsl:attribute name="padding">
     <xsl:value-of select="$tableCellPadding"/>
  </xsl:attribute>
  </xsl:if>
  <xsl:choose>
  <xsl:when test="@halign">
    <xsl:attribute name="text-align">
       <xsl:value-of select="@halign"/>
    </xsl:attribute>
  </xsl:when>
  <xsl:otherwise>
    <xsl:variable name="thiscol">
       <xsl:value-of select="position()"/>
    </xsl:variable>
    <xsl:variable name="tid"><xsl:value-of select="ancestor::table/@id"/></xsl:variable>
    <xsl:variable name="align">
    <xsl:value-of select="exsl:node-set($tableSpecs)/Info/TableSpec[@id=$tid]/fo:table-column[@column-number=$thiscol]/@fotex:column-align"/>
    </xsl:variable>
    <!--
    <xsl:message>    Cell: whats my position: <xsl:value-of select="$thiscol"/>, <xsl:value-of select="$align"/>, <xsl:value-of select="$tid"/>
</xsl:message>
-->
    <xsl:choose>
          <xsl:when test="$align='R'">
                <xsl:attribute name="text-align">right</xsl:attribute>
          </xsl:when>
          <xsl:when test="$align='L'">
                <xsl:attribute name="text-align">left</xsl:attribute>
          </xsl:when>
          <xsl:when test="$align='C'">
                <xsl:attribute name="text-align">center</xsl:attribute>
          </xsl:when>
          <xsl:when test="not($align='')">
	       <xsl:attribute name="text-align">
                   <xsl:value-of select="$align"/>
               </xsl:attribute>
	  </xsl:when>
          <xsl:otherwise>
                <xsl:attribute name="text-align">left</xsl:attribute>
          </xsl:otherwise>
    </xsl:choose>

  </xsl:otherwise>
  </xsl:choose>

</xsl:template>

<xsl:template name="calculateTableNumber">
     <xsl:number  from="text" level="any"/>
</xsl:template>

</xsl:stylesheet>
