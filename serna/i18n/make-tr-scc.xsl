<?xml version="1.0" encoding="utf-8"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">


<!--
    Specif to the XSLT-processor string param:
        ``context-string'' with value ``scc'' (related of file extension)

    for ``xsltproc'' it seems as:
            $ xsltproc - - string-param context-string scc make-tr-scc.xslt <FILE>
-->

    <xsl:output method="text" encoding="utf-8"/>

    <xsl:param name="context-string" select="'sui'"/>

    <xsl:template match="/">
    /// THIS FILE IS GENERATED AUTOMATICALLY. DO NOT EDIT.
    void <xsl:value-of select="$context-string"/>::translate() {
        <xsl:apply-templates/>
    };
    /// END OF GENERATED FILE
    </xsl:template>

    <xsl:template match="custom-content">
        <xsl:call-template name="translate">
            <xsl:with-param name="name" select="name"/>
            <xsl:with-param name="text" select="inscription"/>
            <xsl:with-param name="accel" select="accel"/>
        </xsl:call-template>
    </xsl:template>

    
    <xsl:template name="translate">
        <xsl:param name="name" select="''"/>
        <xsl:param name="text" select="''"/>
        <xsl:param name="accel" select="''"/>

        <xsl:if test="string-length($accel) > 0">
            <xsl:text> tr(&quot;</xsl:text>
            <xsl:value-of select="$accel"/>
            <xsl:text>&quot;</xsl:text>
            <xsl:if test="string-length($name) >0">
                <xsl:text>, &quot;&lt;custom-content(</xsl:text>
                <xsl:value-of select="$name"/>
                <xsl:text>)&gt;&quot;</xsl:text>
            </xsl:if>
            <xsl:text>);
 </xsl:text>
        </xsl:if>
        <xsl:if test="string-length($text) > 0">
            <xsl:text> tr(&quot;</xsl:text>
            <xsl:value-of select="$text"/>
            <xsl:text>&quot;</xsl:text>
            <xsl:if test="string-length($name) >0">
                <xsl:text>, &quot;&lt;custom-content(</xsl:text>
                <xsl:value-of select="$name"/>
                <xsl:text>)&gt;&quot;</xsl:text>
            </xsl:if>
            <xsl:text>);
            </xsl:text>
        </xsl:if>

    </xsl:template>


</xsl:stylesheet>
