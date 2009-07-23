<?xml version="1.0" encoding="utf-8"?>
<xslx:stylesheet xmlns:xslx="http://www.w3.org/1999/XSL/Transform" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format" version="1.0">

<!--  This stylesheet was automatically generated.
      Do not edit it by hand!  -->


   <xsl:template name="ul">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <ul>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </ul>
   </xsl:template>


   <xsl:template name="ul.nested">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <ul>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </ul>
   </xsl:template>


   <xsl:template name="ol">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <ol>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </ol>
   </xsl:template>


   <xsl:template name="ol.nested">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <ol>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </ol>
   </xsl:template>


   <xsl:template name="ul-li">
      <li>
         <xsl:apply-templates/>
      </li>
   </xsl:template>


   <xsl:template name="ol-li">
      <li>
         <xsl:apply-templates/>
      </li>
   </xsl:template>


   <xsl:template name="dl">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <dl>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </dl>
   </xsl:template>


   <xsl:template name="dt">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <dt>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </dt>
   </xsl:template>


   <xsl:template name="dd">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <dd>
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:apply-templates/>
            </xslx:otherwise>
         </xslx:choose>
      </dd>
   </xsl:template>



</xslx:stylesheet>