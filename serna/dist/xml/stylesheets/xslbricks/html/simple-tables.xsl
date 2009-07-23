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
      <table>
         <xsl:attribute name="style">
            <xslx:if test="$text-align">
               <xslx:text>text-align: </xslx:text>
               <xslx:value-of select="$text-align"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$background-color">
               <xslx:text>background-color: </xslx:text>
               <xslx:value-of select="$background-color"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$color">
               <xslx:text>color: </xslx:text>
               <xslx:value-of select="$color"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$font-style">
               <xslx:text>font-style: </xslx:text>
               <xslx:value-of select="$font-style"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$font-weight">
               <xslx:text>font-weight: </xslx:text>
               <xslx:value-of select="$font-weight"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$font-family">
               <xslx:text>font-family: </xslx:text>
               <xslx:value-of select="$font-family"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$font-size">
               <xslx:text>font-size: </xslx:text>
               <xslx:value-of select="$font-size"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$text-decoration">
               <xslx:text>text-decoration: </xslx:text>
               <xslx:value-of select="$text-decoration"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$padding">
               <xslx:text>padding: </xslx:text>
               <xslx:value-of select="$padding"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$border-width">
               <xslx:text>border-width: </xslx:text>
               <xslx:value-of select="$border-width"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$border-color">
               <xslx:text>border-color: </xslx:text>
               <xslx:value-of select="$border-color"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$border-style">
               <xslx:text>border-style: </xslx:text>
               <xslx:value-of select="$border-style"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
         </xsl:attribute>
         <xsl:apply-templates select="$rows"/>
      </table>
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
      <tr>
         <xsl:attribute name="style">
            <xslx:if test="$text-align">
               <xslx:text>text-align: </xslx:text>
               <xslx:value-of select="$text-align"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$background-color">
               <xslx:text>background-color: </xslx:text>
               <xslx:value-of select="$background-color"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$color">
               <xslx:text>color: </xslx:text>
               <xslx:value-of select="$color"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$font-style">
               <xslx:text>font-style: </xslx:text>
               <xslx:value-of select="$font-style"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$font-weight">
               <xslx:text>font-weight: </xslx:text>
               <xslx:value-of select="$font-weight"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$font-family">
               <xslx:text>font-family: </xslx:text>
               <xslx:value-of select="$font-family"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$font-size">
               <xslx:text>font-size: </xslx:text>
               <xslx:value-of select="$font-size"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$text-decoration">
               <xslx:text>text-decoration: </xslx:text>
               <xslx:value-of select="$text-decoration"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$border-width">
               <xslx:text>border-width: </xslx:text>
               <xslx:value-of select="$border-width"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$border-color">
               <xslx:text>border-color: </xslx:text>
               <xslx:value-of select="$border-color"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$border-style">
               <xslx:text>border-style: </xslx:text>
               <xslx:value-of select="$border-style"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
         </xsl:attribute>
         <xsl:apply-templates select="$cells"/>
      </tr>
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
      <td>
         <xsl:attribute name="style">
            <xslx:if test="$text-align">
               <xslx:text>text-align: </xslx:text>
               <xslx:value-of select="$text-align"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$background-color">
               <xslx:text>background-color: </xslx:text>
               <xslx:value-of select="$background-color"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$color">
               <xslx:text>color: </xslx:text>
               <xslx:value-of select="$color"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$font-style">
               <xslx:text>font-style: </xslx:text>
               <xslx:value-of select="$font-style"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$font-weight">
               <xslx:text>font-weight: </xslx:text>
               <xslx:value-of select="$font-weight"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$font-family">
               <xslx:text>font-family: </xslx:text>
               <xslx:value-of select="$font-family"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$font-size">
               <xslx:text>font-size: </xslx:text>
               <xslx:value-of select="$font-size"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$text-decoration">
               <xslx:text>text-decoration: </xslx:text>
               <xslx:value-of select="$text-decoration"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$padding">
               <xslx:text>padding: </xslx:text>
               <xslx:value-of select="$padding"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$border-width">
               <xslx:text>border-width: </xslx:text>
               <xslx:value-of select="$border-width"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$border-color">
               <xslx:text>border-color: </xslx:text>
               <xslx:value-of select="$border-color"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$border-style">
               <xslx:text>border-style: </xslx:text>
               <xslx:value-of select="$border-style"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
         </xsl:attribute>
         <xsl:if test="$span">
            <xsl:attribute name="colspan">
               <xsl:value-of select="$span"/>
            </xsl:attribute>
         </xsl:if>
         <xsl:apply-templates/>
      </td>
   </xsl:template>

</xslx:stylesheet>