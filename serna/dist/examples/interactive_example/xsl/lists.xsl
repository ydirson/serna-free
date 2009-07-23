<?xml version='1.0'?>

<!-- This stylesheet is a part of the Serna Interactive Example. -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:se="http://www.syntext.com/XSL/Format-1.0"
                version='1.0'>

 <!-- Ordered and Unordered Lists -->

 <xsl:template match="ol|dl">
  <fo:block start-indent="{count(ancestor-or-self::ol) +
                           count(ancestor-or-self::dl)}em">
    <fo:list-block provisional-label-separation="0.2em"
                   provisional-distance-between-starts="1.5em"
                   padding-before="1em"
                   padding-after="1em">
        <xsl:apply-templates select="*"/>
    </fo:list-block>
  </fo:block>
 </xsl:template>

 <xsl:template match="ol/li">
  <fo:list-item>
    <fo:list-item-label end-indent="label-end()">
      <fo:block>
        <xsl:number format="1." level="single" count="li"/>
      </fo:block>
    </fo:list-item-label>
    <fo:list-item-body start-indent="body-start()">
        <fo:block border-top-width="0pt">
            <xsl:apply-templates/>
        </fo:block>
    </fo:list-item-body>
  </fo:list-item>
 </xsl:template>
 
 <xsl:template match="dl/li">
  <fo:list-item>
    <fo:list-item-label end-indent="label-end()">
        <fo:block>-</fo:block>
    </fo:list-item-label>
    <fo:list-item-body start-indent="body-start()">
        <fo:block border-top-width="0pt">
            <xsl:apply-templates/>
        </fo:block>
    </fo:list-item-body>
  </fo:list-item>
 </xsl:template>
 
 <!-- This special para is needed to create blocks with zero top-width -->
 <xsl:template match="li/para">
    <fo:block border-top-width="0em">
        <xsl:apply-templates/>
    </fo:block>
 </xsl:template> 

</xsl:stylesheet>
