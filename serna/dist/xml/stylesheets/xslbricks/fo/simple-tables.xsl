<?xml version="1.0" encoding="utf-8"?>
<xslx:stylesheet xmlns:xslx="http://www.w3.org/1999/XSL/Transform" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format" version="1.0">

<!--  This stylesheet was automatically generated.
      Do not edit it by hand!  -->

   <xsl:template name="simple-table">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="rows" select="row"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="text-align"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="background-color"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="color"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-style"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-weight"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-family"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-size"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="text-decoration"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="padding"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-width">1pt</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-color">black</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-style">solid</xsl:param>
      <fo:table>
         <xslx:if test="$text-align">
            <xslx:attribute name="text-align">
               <xslx:value-of select="$text-align"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$background-color">
            <xslx:attribute name="background-color">
               <xslx:value-of select="$background-color"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$color">
            <xslx:attribute name="color">
               <xslx:value-of select="$color"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$font-style">
            <xslx:attribute name="font-style">
               <xslx:value-of select="$font-style"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$font-weight">
            <xslx:attribute name="font-weight">
               <xslx:value-of select="$font-weight"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$font-family">
            <xslx:attribute name="font-family">
               <xslx:value-of select="$font-family"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$font-size">
            <xslx:attribute name="font-size">
               <xslx:value-of select="$font-size"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$text-decoration">
            <xslx:attribute name="text-decoration">
               <xslx:value-of select="$text-decoration"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$padding">
            <xslx:attribute name="padding">
               <xslx:value-of select="$padding"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$border-width">
            <xslx:attribute name="border-width">
               <xslx:value-of select="$border-width"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$border-color">
            <xslx:attribute name="border-color">
               <xslx:value-of select="$border-color"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$border-style">
            <xslx:attribute name="border-style">
               <xslx:value-of select="$border-style"/>
            </xslx:attribute>
         </xslx:if>
         <fo:table-body>
            <xsl:apply-templates select="$rows"/>
         </fo:table-body>
      </fo:table>
   </xsl:template>


   <xsl:template name="simple-row">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="cells" select="cell"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="text-align"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="background-color"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="color"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-style"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-weight"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-family"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-size"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="text-decoration"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-width">1pt</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-color">black</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-style">solid</xsl:param>
      <fo:table-row>
         <xslx:if test="$text-align">
            <xslx:attribute name="text-align">
               <xslx:value-of select="$text-align"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$background-color">
            <xslx:attribute name="background-color">
               <xslx:value-of select="$background-color"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$color">
            <xslx:attribute name="color">
               <xslx:value-of select="$color"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$font-style">
            <xslx:attribute name="font-style">
               <xslx:value-of select="$font-style"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$font-weight">
            <xslx:attribute name="font-weight">
               <xslx:value-of select="$font-weight"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$font-family">
            <xslx:attribute name="font-family">
               <xslx:value-of select="$font-family"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$font-size">
            <xslx:attribute name="font-size">
               <xslx:value-of select="$font-size"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$text-decoration">
            <xslx:attribute name="text-decoration">
               <xslx:value-of select="$text-decoration"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$border-width">
            <xslx:attribute name="border-width">
               <xslx:value-of select="$border-width"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$border-color">
            <xslx:attribute name="border-color">
               <xslx:value-of select="$border-color"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$border-style">
            <xslx:attribute name="border-style">
               <xslx:value-of select="$border-style"/>
            </xslx:attribute>
         </xslx:if>
         <xsl:apply-templates select="$cells"/>
      </fo:table-row>
   </xsl:template>


   <xsl:template name="simple-cell">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="span"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-width">1pt</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-color">black</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-style">solid</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="padding">2pt</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="text-align"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="background-color"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="color"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-style"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-weight"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-family"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-size"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="text-decoration"/>
      <fo:table-cell>
         <xslx:if test="$span">
            <xslx:attribute name="number-columns-spanned">
               <xslx:value-of select="$span"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$border-width">
            <xslx:attribute name="border-width">
               <xslx:value-of select="$border-width"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$border-color">
            <xslx:attribute name="border-color">
               <xslx:value-of select="$border-color"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$border-style">
            <xslx:attribute name="border-style">
               <xslx:value-of select="$border-style"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$padding">
            <xslx:attribute name="padding">
               <xslx:value-of select="$padding"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$text-align">
            <xslx:attribute name="text-align">
               <xslx:value-of select="$text-align"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$background-color">
            <xslx:attribute name="background-color">
               <xslx:value-of select="$background-color"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$color">
            <xslx:attribute name="color">
               <xslx:value-of select="$color"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$font-style">
            <xslx:attribute name="font-style">
               <xslx:value-of select="$font-style"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$font-weight">
            <xslx:attribute name="font-weight">
               <xslx:value-of select="$font-weight"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$font-family">
            <xslx:attribute name="font-family">
               <xslx:value-of select="$font-family"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$font-size">
            <xslx:attribute name="font-size">
               <xslx:value-of select="$font-size"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$text-decoration">
            <xslx:attribute name="text-decoration">
               <xslx:value-of select="$text-decoration"/>
            </xslx:attribute>
         </xslx:if>
         <fo:block>
            <xslx:choose>
               <xslx:when test="$content">
                  <xslx:copy-of select="$content"/>
               </xslx:when>
               <xslx:otherwise>
                  <xslx:call-template name="process-common-attributes-and-children"/>
               </xslx:otherwise>
            </xslx:choose>
         </fo:block>
      </fo:table-cell>
   </xsl:template>


</xslx:stylesheet>