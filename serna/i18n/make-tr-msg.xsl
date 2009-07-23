<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<xsl:strip-space elements="*" />

<xsl:output method="text" encoding="utf-8"/>

<xsl:template match="/messages">
/// THIS FILE IS GENERATED AUTOMATICALLY. DO NOT EDIT.
class QObject {  
    <xsl:apply-templates select="message/text"/>
};
/// END OF GENERATED FILE
</xsl:template>

<xsl:template match="text">
    <xsl:text> tr(&quot;</xsl:text>
    <xsl:value-of select="normalize-space(text())"/>
    <xsl:text>&quot;); 
</xsl:text>
</xsl:template>

<xsl:template match="*"/>

</xsl:stylesheet>

