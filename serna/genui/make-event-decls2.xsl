<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<xsl:output method="text" encoding="utf-8"/>

<xsl:include href="param.xsl"/>

<xsl:template match="/">
/// THIS FILE IS GENERATED AUTOMATICALLY FROM serna-ui.xml. DO NOT EDIT.
   
  <xsl:apply-templates select="//uiActions/uiAction"/>
/// END OF GENERATED FILE
</xsl:template>

<xsl:template match="uiAction">
    <xsl:if test="not(contains(@options, $eopt)) and commandEvent">
        <xsl:text>class </xsl:text>
        <xsl:value-of select="commandEvent"/>;
    </xsl:if>
</xsl:template>

</xsl:stylesheet>

