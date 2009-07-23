<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:doc="http://nwalsh.com/xsl/documentation/1.0"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                exclude-result-prefixes="doc"
                version='1.0'>

  <!-- Counts the depth of the sections/refsections/sectN when
      "title" is a context node -->
  <xsl:template name="section.level">
    <xsl:param name="parent" select="parent::*"/>
    <xsl:variable name="title.parent" select="name($parent)"/>
    <xsl:choose>
      <xsl:when test="$title.parent='section'">
        <xsl:value-of select="count(ancestor::section)"/>
      </xsl:when>
      <xsl:when test="$title.parent='refsection'">
        <xsl:value-of select="count(ancestor::refsection)"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:variable 
          name="nmbr" 
          select="translate($title.parent, 'sectionrfmpl', '')"/>
        <xsl:choose>
          <xsl:when test="number($nmbr) = 'NaN'">1</xsl:when>
          <xsl:otherwise><xsl:value-of select="$nmbr"/></xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>

