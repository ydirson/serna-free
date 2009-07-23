<?xml version="1.0" encoding="utf-8"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:csl="http://www.syntext.com/Extensions/CSL-1.0"
                version="1.0">


    <xsl:output method="text" encoding="utf-8"/>
    <xsl:strip-space elements="*"/>

    <xsl:template match="/">
    /// THIS FILE IS GENERATED AUTOMATICALLY. DO NOT EDIT.
    void csl::translate() {
        <xsl:apply-templates select="//csl:profile"/>
    };
    /// END OF GENERATED FILE
    </xsl:template>

    <xsl:template match="csl:profile">
        <xsl:apply-templates select="@*"/>
    </xsl:template>

    <xsl:template match="@*"/>
    
    <xsl:template match="@inscription|@tooltip">
        <xsl:text> tr(&quot;</xsl:text>
        <xsl:value-of select="."/>
        <xsl:text>&quot;, &quot;profile | </xsl:text>
        <xsl:value-of select="local-name(.)"/>
        <xsl:text>&quot;);
        </xsl:text>
    </xsl:template>


</xsl:stylesheet>
