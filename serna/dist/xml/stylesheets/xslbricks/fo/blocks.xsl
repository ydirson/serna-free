<?xml version="1.0" encoding="utf-8"?>
<xslx:stylesheet xmlns:xslx="http://www.w3.org/1999/XSL/Transform" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format" version="1.0">

<!--  This stylesheet was automatically generated.
      Do not edit it by hand!  -->

   <xsl:template name="para">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <fo:block xsl:use-attribute-sets="p">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:block>
   </xsl:template>


   <xsl:template name="pre">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <fo:block xsl:use-attribute-sets="pre">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:block>
   </xsl:template>


   <xsl:template name="div">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="background-color"/>
      <fo:block>
         <xslx:if test="$background-color">
            <xslx:attribute name="background-color">
               <xslx:value-of select="$background-color"/>
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
      </fo:block>
   </xsl:template>


   <xsl:template name="para.decoration">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="text-align"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="background-color"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="color"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-style"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-weight"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-family"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-size"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="text-decoration"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="padding"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-width">0</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-color">black</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-style">solid</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="start-indent"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="end-indent"/>
      <fo:block xsl:use-attribute-sets="p">
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
         <xslx:if test="$start-indent">
            <xslx:attribute name="start-indent">
               <xslx:value-of select="$start-indent"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$end-indent">
            <xslx:attribute name="end-indent">
               <xslx:value-of select="$end-indent"/>
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
      </fo:block>
   </xsl:template>


   <xsl:template name="pre.decoration">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="text-align"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="background-color"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="color"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-style"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-weight"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-family"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-size"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="text-decoration"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="padding"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-width">0</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-color">black</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-style">solid</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="start-indent"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="end-indent"/>
      <fo:block xsl:use-attribute-sets="pre">
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
         <xslx:if test="$start-indent">
            <xslx:attribute name="start-indent">
               <xslx:value-of select="$start-indent"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$end-indent">
            <xslx:attribute name="end-indent">
               <xslx:value-of select="$end-indent"/>
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
      </fo:block>
   </xsl:template>


   <xsl:template name="div.decoration">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="text-align"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="background-color"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="color"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-style"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-weight"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-family"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="font-size"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="text-decoration"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="padding"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-width">0</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-color">black</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="border-style">solid</xsl:param>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="start-indent"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="end-indent"/>
      <fo:block>
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
         <xslx:if test="$start-indent">
            <xslx:attribute name="start-indent">
               <xslx:value-of select="$start-indent"/>
            </xslx:attribute>
         </xslx:if>
         <xslx:if test="$end-indent">
            <xslx:attribute name="end-indent">
               <xslx:value-of select="$end-indent"/>
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
      </fo:block>
   </xsl:template>


   <xsl:template name="h1">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <fo:block xsl:use-attribute-sets="h1">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:block>
   </xsl:template>


   <xsl:template name="h2">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <fo:block xsl:use-attribute-sets="h2">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:block>
   </xsl:template>


   <xsl:template name="h3">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <fo:block xsl:use-attribute-sets="h3">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:block>
   </xsl:template>


   <xsl:template name="h4">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <fo:block xsl:use-attribute-sets="h4">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:block>
   </xsl:template>


   <xsl:template name="h5">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <fo:block xsl:use-attribute-sets="h5">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:block>
   </xsl:template>


   <xsl:template name="h6">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <fo:block xsl:use-attribute-sets="h6">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:block>
   </xsl:template>


   <xsl:template name="image.block">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="url"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="height"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="width"/>
      <fo:block>
         <fo:external-graphic xsl:use-attribute-sets="img-link">
            <xslx:if test="$height">
               <xslx:attribute name="content-height">
                  <xslx:value-of select="$height"/>
               </xslx:attribute>
            </xslx:if>
            <xslx:if test="$width">
               <xslx:attribute name="content-width">
                  <xslx:value-of select="$width"/>
               </xslx:attribute>
            </xslx:if>
            <xsl:if test="$url">
               <xsl:attribute name="src">
                  <xsl:text>url(</xsl:text>
                  <xsl:value-of select="$url"/>
                  <xsl:text>)</xsl:text>
               </xsl:attribute>
            </xsl:if>
         </fo:external-graphic>
      </fo:block>
   </xsl:template>


   <xsl:template name="blockquote">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <fo:block xsl:use-attribute-sets="blockquote">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:block>
   </xsl:template>


   <xsl:template name="center">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <fo:block text-align="center">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:block>
   </xsl:template>


   <xsl:template name="br">
      <fo:block xml:space="preserve" white-space-collapse="false">
      
         <xsl:call-template name="process-common-attributes"/>
      
         <xsl:text>

         </xsl:text>
    
      </fo:block>
   </xsl:template>


</xslx:stylesheet>