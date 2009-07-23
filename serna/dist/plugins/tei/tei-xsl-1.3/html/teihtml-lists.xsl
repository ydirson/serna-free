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
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">


<!-- lists -->


<xsl:template match="list">
<xsl:if test="head">
  <p><em><xsl:apply-templates select="head"/></em></p>
</xsl:if>
<xsl:choose>
 <xsl:when test="@type='catalogue'">
  <p><dl>
    <xsl:for-each select="item">
       <p/>
       <xsl:apply-templates select="."  mode="gloss"/>
    </xsl:for-each>
  </dl></p>
 </xsl:when>
  <xsl:when test="@type='gloss' and @rend='multicol'">
    <xsl:variable name="nitems">
      <xsl:value-of select="count(item)div 2"/>
    </xsl:variable>
    <p><table>
    <tr>
      <td valign="top">
      <dl>
         <xsl:apply-templates mode="gloss" select="item[position()&lt;=$nitems ]"/>
      </dl>
      </td>
      <td  valign="top">
      <dl>
         <xsl:apply-templates mode="gloss" select="item[position() &gt;$nitems]"/>
      </dl>
      </td>
     </tr>
    </table>
    </p>
  </xsl:when>

 <xsl:when test="@type='gloss'">
  <p><dl><xsl:apply-templates mode="gloss" select="item"/></dl></p>
 </xsl:when>
 <xsl:when test="@type='glosstable'">
  <table><xsl:apply-templates mode="glosstable" select="item"/></table>
 </xsl:when>
 <xsl:when test="@type='vallist'">
  <table><xsl:apply-templates mode="glosstable" select="item"/></table>
 </xsl:when>
 <xsl:when test="@type='inline'">
   <xsl:if test="not(item)">None</xsl:if>
  <xsl:apply-templates select="item" mode="inline"/>
 </xsl:when>
 <xsl:when test="@type='runin'">
  <p><xsl:apply-templates select="item" mode="runin"/></p>
 </xsl:when>
 <xsl:when test="@type='unordered'">
  <ul>
  <xsl:choose>
  <xsl:when test="@rend and starts-with(@rend,'class:')">
    <xsl:attribute name="class">
      <xsl:value-of select="substring-after(@rend,'class:')"/>
    </xsl:attribute>
  </xsl:when>
  <xsl:when test="@rend">
    <xsl:attribute name="class"><xsl:value-of select="@rend"/></xsl:attribute>
  </xsl:when>
  </xsl:choose>
  <xsl:apply-templates select="item"/></ul>
 </xsl:when>
 <xsl:when test="@type='bibl'">
  <xsl:apply-templates select="item" mode="bibl"/>
 </xsl:when>
 <xsl:when test="@type='ordered'">
  <ol>
    <xsl:choose>
        <xsl:when test="@rend and starts-with(@rend,'class:')">
    <xsl:attribute name="class">
      <xsl:value-of select="substring-after(@rend,'class:')"/>
    </xsl:attribute>
  </xsl:when>
  <xsl:when test="@rend">
    <xsl:attribute name="class"><xsl:value-of select="@rend"/></xsl:attribute>
  </xsl:when>
</xsl:choose>
  <xsl:apply-templates select="item"/></ol>
 </xsl:when>
 <xsl:otherwise>
  <ul>
    <xsl:choose>
        <xsl:when test="@rend and starts-with(@rend,'class:')">
    <xsl:attribute name="class">
      <xsl:value-of select="substring-after(@rend,'class:')"/>
    </xsl:attribute>
  </xsl:when>
  <xsl:when test="@rend">
    <xsl:attribute name="class"><xsl:value-of select="@rend"/></xsl:attribute>
  </xsl:when>
</xsl:choose>
  <xsl:apply-templates select="item"/></ul>
 </xsl:otherwise>
</xsl:choose>
</xsl:template>

<xsl:template mode="bibl" match="item">
 <p>
   <xsl:apply-templates/>
 </p>
</xsl:template>

<xsl:template mode="glosstable" match="item">
 <tr>
   <td valign="top"><strong>
     <xsl:apply-templates mode="print" select="preceding-sibling::*[1]"/></strong></td>
   <td><xsl:apply-templates/></td>
 </tr>
</xsl:template>

<xsl:template mode="gloss" match="item">
   <dt><strong>
     <xsl:apply-templates mode="print" select="preceding-sibling::label[1]"/>
   </strong>
   </dt>
   <dd><xsl:apply-templates/></dd>
</xsl:template>

<xsl:template match="item/label">
    <xsl:choose>
	<xsl:when test="@rend">
          <xsl:call-template name="rendering"/>
	</xsl:when>
        <xsl:otherwise>
          <strong><xsl:apply-templates/></strong>
        </xsl:otherwise>     
    </xsl:choose>
</xsl:template>

<xsl:template match="list/label"/>

<xsl:template match="item">
 <li><a>
 <xsl:attribute name="name">
     <xsl:choose>
     <xsl:when test="@id">
       <xsl:value-of select="@id"/>
     </xsl:when>
     <xsl:otherwise>
      <xsl:value-of select="generate-id()"/>
     </xsl:otherwise>
    </xsl:choose>
  </xsl:attribute></a>
<xsl:apply-templates/></li>
</xsl:template>

<xsl:template match="item" mode="runin">
  &#8226; <xsl:apply-templates/>&#160;
</xsl:template>

<xsl:template match="item" mode="inline">
  <xsl:if test="preceding-sibling::item">,  </xsl:if>
  <xsl:if test="not(following-sibling::item) and preceding-sibling::item"> and  </xsl:if>   
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="label" mode="print">
<xsl:if test="@id"><a name="{@id}"/></xsl:if>
<xsl:choose>
	<xsl:when test="@rend">
		<xsl:call-template name="rendering"/>
	</xsl:when>
	<xsl:otherwise>
		<xsl:apply-templates/>
	</xsl:otherwise>
</xsl:choose>
</xsl:template>

</xsl:stylesheet>
