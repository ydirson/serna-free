<?xml version='1.0'?>

<!-- This stylesheet is a part of the Serna Interactive Example. -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:se="http://www.syntext.com/XSL/Format-1.0"
                version='1.0'>

 <!-- This is a base element for special examples --> 
 <xsl:template match="example">
  <fo:block 
    border-top-width="5pt"
    border-bottom-width="5pt"
    background-color="#96cbf2"
    xsl:use-attribute-sets="block.properties">
    <xsl:apply-templates/>
  </fo:block>  
 </xsl:template>

 <!-- This element outputs item1 and item2 in reverse order -->
 <xsl:template match="order">
    <xsl:apply-templates select="item2"/>
    <xsl:apply-templates select="item1"/>
 </xsl:template>

 <xsl:template match="item1|item2">
  <fo:block 
    border-top-width="2pt" 
    border-bottom-width="2pt"
    xsl:use-attribute-sets="block.properties">
    <xsl:apply-templates/>
   </fo:block>
 </xsl:template>

 <!-- This template takes contents of <farenheit> tag as a number,
      and converts it using formula C = (F-32)*5/9 -->
 <xsl:template match="fahrenheit">
  <fo:block text-align="center">
    <fo:inline color="#FFF987" font-weight="bold"><xsl:copy-of select="text()"/></fo:inline> Farenheit = 
    <xsl:value-of select="5 * (number(.) - 32) div 9"/> Celsius.
  </fo:block>
 </xsl:template>

 <xsl:template match="baseline">
  <fo:inline border-top-color="black"   
             border-top-width="1px"   
             border-bottom-color="black"   
             border-bottom-width="1px"   
             border-left-color="black"   
             border-left-width="1px"   
             border-right-color="black"   
             border-right-width="1px"   
             background-color="#f0f0ff"
             color="navy" 
             font-weight="bold">
    <xsl:if test="not(contains(text(), '='))">
      <xsl:attribute name="baseline-shift">
        <xsl:value-of select="text()"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:apply-templates/>
  </fo:inline>
 </xsl:template>

 <xsl:template match="complextable">
   <xsl:variable name="file" select="'table_spans.fo'"/>
   <xsl:choose>
    <xsl:when test="$inline.refs">
       <fo:block font-size="8pt" background-color="white"
                 border-top-width="0pt" border-bottom-width="0pt">
         <xsl:copy-of select="document($file, /)/fo:table"/>
       </fo:block>
    </xsl:when>
    <xsl:otherwise>
        <fo:inline color="navy" font-weight="bold">
            <xsl:text>Reference To: </xsl:text>
            <xsl:value-of select="$file"/>
        </fo:inline>
    </xsl:otherwise>
   </xsl:choose>
 </xsl:template>
 
 <xsl:template match="math">
    <se:inline-object font-family="Times"/>
 </xsl:template>

</xsl:stylesheet>
