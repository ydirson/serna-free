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

<xsl:template match="table[@rend='simple']">
  <table>
 <xsl:for-each select="@*">
  <xsl:if test="name(.)='summary'
or name(.) = 'width'
or name(.) = 'border'
or name(.) = 'frame'
or name(.) = 'rules'
or name(.) = 'cellspacing'
or name(.) = 'cellpadding'">
    <xsl:copy-of select="."/>
 </xsl:if>
 </xsl:for-each>
<xsl:apply-templates/></table>
</xsl:template>

<xsl:template match='table'>
 <div>
 <xsl:attribute name="align">
 <xsl:choose>
  <xsl:when test="@align">
      <xsl:value-of select="@align"/>
  </xsl:when>
  <xsl:otherwise>
      <xsl:value-of select="$tableAlign"/>
  </xsl:otherwise>
 </xsl:choose>
 </xsl:attribute>
 <table>
 <xsl:if test="@rend='frame' or @rend='rules'">
  <xsl:attribute name="rules">all</xsl:attribute>
  <xsl:attribute name="border">1</xsl:attribute>
 </xsl:if>
 <xsl:for-each select="@*">
  <xsl:if test="name(.)='summary'
or name(.) = 'width'
or name(.) = 'border'
or name(.) = 'frame'
or name(.) = 'rules'
or name(.) = 'cellspacing'
or name(.) = 'cellpadding'">
    <xsl:copy-of select="."/>
 </xsl:if>
 </xsl:for-each>
 <xsl:apply-templates/>
 </table>
 </div>
</xsl:template>

<xsl:template match='row'>
 <tr>
<xsl:if test="@rend and starts-with(@rend,'class:')">
 <xsl:attribute name="class">
    <xsl:value-of select="substring-after(@rend,'class:')"/>
 </xsl:attribute>
</xsl:if>
<xsl:if test="not(@role = 'data') and not(@role='')">
 <xsl:attribute name="class"><xsl:value-of select="@role"/></xsl:attribute>
</xsl:if>
 <xsl:apply-templates/>
 </tr>
</xsl:template>

<xsl:template match='cell'>
 <td valign="top">
   <xsl:if test="@id"><a name="{@id}"/></xsl:if>
<xsl:choose>
<xsl:when test="@rend and starts-with(@rend,'width:')">
 <xsl:attribute name="width">
    <xsl:value-of select="substring-after(@rend,'width:')"/>
 </xsl:attribute>
</xsl:when>
<xsl:when test="@rend and starts-with(@rend,'class:')">
 <xsl:attribute name="class">
    <xsl:value-of select="substring-after(@rend,'class:')"/>
 </xsl:attribute>
</xsl:when>
<xsl:when test="@rend">
 <xsl:attribute name="bgcolor"><xsl:value-of select="@rend"/></xsl:attribute>
</xsl:when>
</xsl:choose>
<xsl:if test="@cols">
 <xsl:attribute name="colspan"><xsl:value-of select="@cols"/></xsl:attribute>
</xsl:if>
<xsl:if test="@rows">
 <xsl:attribute name="rowspan"><xsl:value-of select="@rows"/></xsl:attribute>
</xsl:if>
<xsl:choose>
  <xsl:when test="@align">
   <xsl:attribute name="align"><xsl:value-of select="@align"/></xsl:attribute>
  </xsl:when>
  <xsl:when test="not($cellAlign='left')">
   <xsl:attribute name="align"><xsl:value-of select="$cellAlign"/></xsl:attribute>
  </xsl:when>
</xsl:choose>
<xsl:if test="not(@role = 'data') and not(@role='')">
 <xsl:attribute name="class"><xsl:value-of select="@role"/></xsl:attribute>
</xsl:if>
 <xsl:apply-templates/>
 </td>
</xsl:template>

</xsl:stylesheet>
