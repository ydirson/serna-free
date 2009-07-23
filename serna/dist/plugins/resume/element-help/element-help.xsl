<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:r="http://xmlresume.sourceforge.net/resume/0.0"
  xmlns:h="http://www.syntext.com/Extensions/ElementHelp-1.0">
<xsl:output method="xml"/>
<xsl:strip-space elements="*"/>
<xsl:output indent="yes"/>
<xsl:template match="/">
   <h:serna-help adp-file="resume.adp">
    <h:element-help>
      <xsl:attribute name="match">
        <xsl:text>r:</xsl:text>
        <xsl:value-of select="//html/head/title"/>
      </xsl:attribute>
      <h:short-help>
        <xsl:value-of select="//div[@class='refnamediv']/p"/>
      </h:short-help>
     <h:qta-help adp-file="resume.adp">
      <xsl:attribute name="href">
        <xsl:value-of select="//div[@class='refentry']/a/@name"/>
        <xsl:text>.html</xsl:text>
      </xsl:attribute>
     </h:qta-help>
     <xsl:for-each select="//div[@class='variablelist']/dl">
       <h:attr-help>
         <xsl:attribute name="name">
           <xsl:value-of select="dt/span/tt"/>
         </xsl:attribute>
         <h:short-help>
           <xsl:value-of select="dd/p"/>
         </h:short-help>
       </h:attr-help>
     </xsl:for-each>
    </h:element-help>
  </h:serna-help>
</xsl:template>
</xsl:stylesheet>
