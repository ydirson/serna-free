<?xml version="1.0" encoding="utf-8"?>
<xslx:stylesheet xmlns:xslx="http://www.w3.org/1999/XSL/Transform" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format" version="1.0">

<!--  This stylesheet was automatically generated.
      Do not edit it by hand!  -->

   <xsl:template name="ul">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:list-block xsl:use-attribute-sets="ul">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:list-block>
   </xsl:template>


   <xsl:template name="ul.nested">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:list-block xsl:use-attribute-sets="ul-nested">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:list-block>
   </xsl:template>


   <xsl:template name="ol">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:list-block xsl:use-attribute-sets="ol">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:list-block>
   </xsl:template>


   <xsl:template name="ol.nested">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:list-block xsl:use-attribute-sets="ol-nested">
         <xslx:choose>
            <xslx:when test="$content">
               <xslx:copy-of select="$content"/>
            </xslx:when>
            <xslx:otherwise>
               <xslx:call-template name="process-common-attributes-and-children"/>
            </xslx:otherwise>
         </xslx:choose>
      </fo:list-block>
   </xsl:template>


   <xsl:template name="ul-li">
      <fo:list-item xsl:use-attribute-sets="ul-li">
         <xsl:call-template name="process-ul-li"/>
      </fo:list-item>
   </xsl:template>


   <xsl:template name="ol-li">
      <fo:list-item xsl:use-attribute-sets="ol-li">
         <xsl:call-template name="process-ol-li"/>
      </fo:list-item>
   </xsl:template>


   <xsl:template name="dl">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:block xsl:use-attribute-sets="dl">
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


   <xsl:template name="dt">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:block xsl:use-attribute-sets="dt">
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


   <xsl:template name="dd">
      <xsl:param xmlns:xb="http://www.syntext.com/XslBricks-1.0" name="content"/>
      <fo:block xsl:use-attribute-sets="dd">
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


   <xsl:template name="process-ol-li">
      <xsl:call-template name="process-common-attributes"/>
      <fo:list-item-label end-indent="label-end()" text-align="end" wrap-option="no-wrap">
         <fo:block>
            <xsl:variable name="depth" select="count(ancestor::ol)"/>
            <xsl:choose>
               <xsl:when test="$depth = 1">
                  <fo:inline xsl:use-attribute-sets="ol-label-1">
                     <xsl:number format="1."/>
                  </fo:inline>
               </xsl:when>
               <xsl:when test="$depth = 2">
                  <fo:inline xsl:use-attribute-sets="ol-label-2">
                     <xsl:number format="a."/>
                  </fo:inline>
               </xsl:when>
               <xsl:otherwise>
                  <fo:inline xsl:use-attribute-sets="ol-label-3">
                     <xsl:number format="i."/>
                  </fo:inline>
               </xsl:otherwise>
            </xsl:choose>
         </fo:block>
      </fo:list-item-label>
      <fo:list-item-body start-indent="body-start()">
         <fo:block>
            <xsl:apply-templates/>
         </fo:block>
      </fo:list-item-body>
   </xsl:template>

   <xsl:template name="process-ul-li">
      <xsl:call-template name="process-common-attributes"/>
      <fo:list-item-label end-indent="label-end()" text-align="end" wrap-option="no-wrap">
         <fo:block>
            <xsl:variable name="depth" select="count(ancestor::ul)"/>
            <xsl:choose>
               <xsl:when test="$depth = 1">
                  <fo:inline xsl:use-attribute-sets="ul-label-1">
                     <xsl:value-of select="$ul.label-1"/>
                  </fo:inline>
               </xsl:when>
               <xsl:when test="$depth = 2">
                  <fo:inline xsl:use-attribute-sets="ul-label-2">
                     <xsl:value-of select="$ul.label-2"/>
                  </fo:inline>
               </xsl:when>
               <xsl:otherwise>
                  <fo:inline xsl:use-attribute-sets="ul-label-3">
                     <xsl:value-of select="$ul.label-3"/>
                  </fo:inline>
               </xsl:otherwise>
            </xsl:choose>
         </fo:block>
      </fo:list-item-label>
      <fo:list-item-body start-indent="body-start()">
         <fo:block>
            <xsl:apply-templates/>
         </fo:block>
      </fo:list-item-body>
   </xsl:template>

</xslx:stylesheet>