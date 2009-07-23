<?xml version="1.0" encoding="utf-8"?>
<xslx:stylesheet xmlns:xslx="http://www.w3.org/1999/XSL/Transform" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format" version="1.0">

<!--  This stylesheet was automatically generated.
      Do not edit it by hand!  -->


   <xsl:template name="para">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <p class="{$class}">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </p>
   </xsl:template>


   <xsl:template name="pre">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <pre class="{$class}">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </pre>
   </xsl:template>


   <xsl:template name="div">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="background-color"/>
      <div class="{$class}">
         <xsl:attribute name="style">
            <xslx:if test="$background-color">
               <xslx:text>background-color: </xslx:text>
               <xslx:value-of select="$background-color"/>
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
      </div>
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
      <p class="{$class}">
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
            <xslx:if test="$start-indent">
               <xslx:text>start-indent: </xslx:text>
               <xslx:value-of select="$start-indent"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$end-indent">
               <xslx:text>end-indent: </xslx:text>
               <xslx:value-of select="$end-indent"/>
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
      </p>
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
      <pre class="{$class}">
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
            <xslx:if test="$start-indent">
               <xslx:text>start-indent: </xslx:text>
               <xslx:value-of select="$start-indent"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$end-indent">
               <xslx:text>end-indent: </xslx:text>
               <xslx:value-of select="$end-indent"/>
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
      </pre>
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
      <div class="{$class}">
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
            <xslx:if test="$start-indent">
               <xslx:text>start-indent: </xslx:text>
               <xslx:value-of select="$start-indent"/>
               <xslx:text>; </xslx:text>
            </xslx:if>
            <xslx:if test="$end-indent">
               <xslx:text>end-indent: </xslx:text>
               <xslx:value-of select="$end-indent"/>
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
      </div>
   </xsl:template>


   <xsl:template name="h1">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <h1 class="{$class}">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </h1>
   </xsl:template>


   <xsl:template name="h2">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <h2 class="{$class}">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </h2>
   </xsl:template>


   <xsl:template name="h3">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <h3 class="{$class}">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </h3>
   </xsl:template>


   <xsl:template name="h4">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <h4 class="{$class}">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </h4>
   </xsl:template>


   <xsl:template name="h5">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <h5 class="{$class}">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </h5>
   </xsl:template>


   <xsl:template name="h6">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <h6 class="{$class}">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </h6>
   </xsl:template>


   <xsl:template name="image.block">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="url"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="height"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="width"/>
      <div>
         <img src="{$url}">
            <xsl:if test="$height">
               <xsl:attribute name="height">
                  <xsl:value-of select="$height"/>
               </xsl:attribute>
            </xsl:if>
            <xsl:if test="$width">
               <xsl:attribute name="width">
                  <xsl:value-of select="$width"/>
               </xsl:attribute>
            </xsl:if>
         </img>
      </div>
   </xsl:template>


   <xsl:template name="blockquote">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <blockquote class="{$class}">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </blockquote>
   </xsl:template>


   <xsl:template name="center">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="class" select="local-name()"/>
      <center class="{$class}">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </center>
   </xsl:template>


   <xsl:template name="br">
      <br/>
   </xsl:template>

</xslx:stylesheet>