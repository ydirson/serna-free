<?xml version="1.0" encoding="utf-8"?>
<xslx:stylesheet xmlns:xslx="http://www.w3.org/1999/XSL/Transform" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format" version="1.0">

<!--  This stylesheet was automatically generated.
      Do not edit it by hand!  -->

   <xsl:template name="inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:inline>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:inline>
   </xsl:template>


   <xsl:template name="italic.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:inline font-style="italic">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:inline>
   </xsl:template>


   <xsl:template name="bold.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:inline font-weight="bold">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:inline>
   </xsl:template>


   <xsl:template name="bold-italic.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:inline font-style="italic" font-weight="bold">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:inline>
   </xsl:template>


   <xsl:template name="blue-underline.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:inline color="blue" text-decoration="underline">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:inline>
   </xsl:template>


   <xsl:template name="monospace.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:inline xsl:use-attribute-sets="code">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:inline>
   </xsl:template>


   <xsl:template name="small.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:inline xsl:use-attribute-sets="small">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:inline>
   </xsl:template>


   <xsl:template name="big.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:inline xsl:use-attribute-sets="big">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:inline>
   </xsl:template>


   <xsl:template name="strike.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:inline xsl:use-attribute-sets="s">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:inline>
   </xsl:template>


   <xsl:template name="underline.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:inline xsl:use-attribute-sets="u">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:inline>
   </xsl:template>


   <xsl:template name="super.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:inline xsl:use-attribute-sets="sup">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:inline>
   </xsl:template>


   <xsl:template name="sub.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:inline xsl:use-attribute-sets="sub">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:inline>
   </xsl:template>


   <xsl:template name="quote.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:inline xsl:use-attribute-sets="q">
         <xsl:call-template name="process-common-attributes"/>
         <xsl:text>"</xsl:text>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
         <xsl:text>"</xsl:text>
      </fo:inline>
   </xsl:template>


   <xsl:template name="image.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="url"/>
      <fo:external-graphic xsl:use-attribute-sets="img-link">
         <xsl:if test="$url">
            <xsl:attribute name="src">
               <xsl:text>url(</xsl:text>
               <xsl:value-of select="$url"/>
               <xsl:text>)</xsl:text>
            </xsl:attribute>
         </xsl:if>
      </fo:external-graphic>
   </xsl:template>


   <xsl:template name="inline.decoration">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="background-color"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="color"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="vertical"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-style"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-weight"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-family"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-size"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="text-decoration"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-width">0</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-color">black</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-style">solid</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="padding"/>
      <fo:inline>
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
         <xslx:if test="$vertical">
            <xslx:attribute name="baseline-shift">
               <xslx:value-of select="$vertical"/>
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
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:inline>
   </xsl:template>


</xslx:stylesheet>