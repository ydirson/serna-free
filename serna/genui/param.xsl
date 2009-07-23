<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<!-- Platform may contain list for platforms only for which given tag
     is processed. Current platforms are: win32, mac, linux -->
<xsl:param name="platform"/>

<!-- Build options: debug, release -->

<xsl:param name="build_mode"/>

</xsl:stylesheet>
