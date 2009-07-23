<?xml version="1.0" encoding="utf-8"?>
<xslx:stylesheet xmlns:xslx="http://www.w3.org/1999/XSL/Transform" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format" version="1.0">

<!--  This stylesheet was automatically generated.
      Do not edit it by hand!  -->


   <xsl:template name="inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xslx:choose>
         <xslx:when test="$content">
            <xslx:copy-of select="$content"/>
         </xslx:when>
         <xslx:otherwise>
            <xslx:apply-templates/>
         </xslx:otherwise>
      </xslx:choose>
   </xsl:template>


   <xsl:template name="italic.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <i>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </i>
   </xsl:template>


   <xsl:template name="bold.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <b>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </b>
   </xsl:template>


   <xsl:template name="bold-italic.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <b>
         <i>
            <xslx:choose>
               <xslx:when test="$content">
                  <xslx:copy-of select="$content"/>
               </xslx:when>
               <xslx:otherwise>
                  <xslx:apply-templates/>
               </xslx:otherwise>
            </xslx:choose>
         </i>
      </b>
   </xsl:template>


   <xsl:template name="blue-underline.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <font style="color: blue; text-decoration: underline;">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </font>
   </xsl:template>


   <xsl:template name="monospace.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <code>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </code>
   </xsl:template>


   <xsl:template name="small.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <small>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </small>
   </xsl:template>


   <xsl:template name="big.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <big>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </big>
   </xsl:template>


   <xsl:template name="strike.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <s>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </s>
   </xsl:template>


   <xsl:template name="underline.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <u>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </u>
   </xsl:template>


   <xsl:template name="super.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <sup>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </sup>
   </xsl:template>


   <xsl:template name="sub.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <sub>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </sub>
   </xsl:template>


   <xsl:template name="quote.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <q>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </q>
   </xsl:template>


   <xsl:template name="image.inline">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="url"/>
      <img src="{$url}"/>
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
      <font>
         <xsl:attribute name="style">
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
            <xslx:if test="$vertical">
               <xslx:text>vertical-align: </xslx:text>
               <xslx:value-of select="$vertical"/>
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
            <xslx:if test="$padding">
               <xslx:text>padding: </xslx:text>
               <xslx:value-of select="$padding"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
         </xsl:attribute>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </font>
   </xsl:template>

</xslx:stylesheet>