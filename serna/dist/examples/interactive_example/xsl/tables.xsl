<?xml version='1.0'?>

<!-- This stylesheet is a part of the Serna Interactive Example. -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:se="http://www.syntext.com/XSL/Format-1.0"
                version='1.0'>

 <!-- Simple Table -->
 <xsl:template match="table">
    <fo:table>
      <fo:table-body>
        <xsl:apply-templates select="row"/>
      </fo:table-body>
    </fo:table>
 </xsl:template>

 <xsl:template match="row">
    <fo:table-row>
        <xsl:apply-templates select="cell"/>
    </fo:table-row>
 </xsl:template>

 <xsl:template match="cell">
    <fo:table-cell>
        <xsl:attribute name="background-color">
            <xsl:choose>
              <xsl:when test="count(ancestor::row/preceding-sibling::row) = 0">
                <xsl:text>yellow</xsl:text>    
              </xsl:when>
              <xsl:otherwise>silver</xsl:otherwise>
            </xsl:choose>
        </xsl:attribute>
        <fo:block>
            <xsl:apply-templates/>
        </fo:block>
    </fo:table-cell>
 </xsl:template>

</xsl:stylesheet>
