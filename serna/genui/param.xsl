<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<!-- Platform may contain list for platforms only for which given tag
     is processed. Current platforms are: win32, mac, linux -->
<xsl:param name="platform"/>

<!-- Build options: debug, release -->

<xsl:param name="build_mode"/>
<xsl:param name="edition"/>

<!-- inversed edition type, for exclusion -->
<xsl:variable name="eopt">
    <xsl:choose>
        <xsl:when test="$edition = 'free'">enterprise</xsl:when>
        <xsl:when test="$edition = 'enterprise'">free</xsl:when>
        <xsl:otherwise/>
    </xsl:choose>
</xsl:variable>

</xsl:stylesheet>
