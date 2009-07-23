<?xml version='1.0'?>

<!-- This stylesheet is a part of the Serna Interactive Example. -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:se="http://www.syntext.com/XSL/Format-1.0"
                version='1.0'>
 
 <!-- In this file, we define templates for most commonly used elements. -->
 
 <xsl:template match="sect">
    <xsl:variable name="level" select="count(ancestor-or-self::sect)"/>
    <fo:block>
        <xsl:apply-templates select="*">
            <xsl:with-param name="level" select="$level"/>
        </xsl:apply-templates>
    </fo:block>
 </xsl:template>

 <xsl:template match="sect/title">
    <xsl:param name="level" select="'0'"/>
    <fo:block 
        font-size="{24 - 4 * $level}pt" 
        border-top-width="10pt"
        color="{$title.color}">
        <!-- Create bottom-line for top-level titles -->
        <xsl:if test="2 > $level">
          <xsl:attribute name="font-weight">bold</xsl:attribute>
          <xsl:attribute name="border-bottom-width">2pt</xsl:attribute>
          <xsl:attribute name="border-bottom-color">#5090c0</xsl:attribute>
        </xsl:if>
        <xsl:if test="$number.sections">
            <xsl:number count="sect" format="1. " level="multiple"/>
        </xsl:if>
        <xsl:apply-templates/>
    </fo:block>
 </xsl:template>
 
 <xsl:template match="para">
   <fo:block xsl:use-attribute-sets="block.properties">
     <xsl:apply-templates/>
   </fo:block>
 </xsl:template> 
 
 <xsl:template match="em">
    <fo:inline 
       border-left-width="0pt"
       border-right-width="0pt"
       color="#F49200">
        <xsl:if test="@weight">
          <xsl:attribute name="font-weight">
            <xsl:value-of select="@weight"/>
          </xsl:attribute>
        </xsl:if>
        <xsl:if test="@style">
          <xsl:attribute name="font-style">
            <xsl:value-of select="@style"/>
          </xsl:attribute>
        </xsl:if>
        <xsl:apply-templates/>
    </fo:inline>
 </xsl:template>

 <xsl:template match="pre">
    <fo:block background-color="#e0e0e0"
              white-space-collapse="false"
              linefeed-treatment="preserve"
              white-space-treatment="preserve"
              xsl:use-attribute-sets="block.properties">
        <xsl:apply-templates/>
    </fo:block>
 </xsl:template>

 <xsl:template name="serna.fold.template">
        <xsl:apply-templates select="." mode="serna.fold" 
                             xse:apply-serna-fold-template="false"/>
 </xsl:template>

 <xsl:template match="sect" mode="serna.fold">
   <fo:block se:fold="" color="gray" font-weight="bold"
             font-size="{24 - 4 * (count(ancestor-or-self::sect))}pt">
     <fo:inline>
        <se:fold show-element-name="false"/>
        <xsl:text> </xsl:text>
        <xsl:number count="sect" format="1. " level="multiple"/>
        <xsl:apply-templates select="title/text()"/>
        <xsl:text> (Folded)</xsl:text>
     </fo:inline>
   </fo:block>
 </xsl:template>

 <xsl:template match="xref">
    <fo:inline text-decoration="underline">
        <xsl:text>[Section </xsl:text>
        <xsl:variable name="sect-title">
            <xsl:apply-templates select="id(@idref)/title"/>
        </xsl:variable>
        <xsl:value-of select="$sect-title"/>
        <xsl:text>]</xsl:text>
    </fo:inline>
 </xsl:template>

 <xsl:template match="*" mode="serna.fold">
   <fo:block se:fold="" color="gray">
        <xsl:text>(Folded: </xsl:text>
        <xsl:value-of select="local-name(.)"/>
        <xsl:text>)</xsl:text>
   </fo:block>
 </xsl:template>
 
</xsl:stylesheet>
