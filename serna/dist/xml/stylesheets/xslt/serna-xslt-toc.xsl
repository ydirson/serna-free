<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'
                xmlns="http://www.w3.org/TR/xhtml1/transitional"
                xmlns:xse="http://www.syntext.com/Extensions/XSLT-1.0"
                exclude-result-prefixes="#default">

  <xsl:output method="xml" indent="yes"/>

  <xsl:param name="toc-depth" select="'5'"/>
  <xsl:param name="title-limit" select="'50'"/>

  <xsl:template match="/*">
    <xsl:choose>
      <xsl:when test="node()">
        <xsl:call-template name="all"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:element name="{local-name()}">
          <xsl:value-of select="."/>
        </xsl:element>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="*" name="all">
    <xsl:variable name="depth" select="count(ancestor::*)"/>

    <xsl:variable name="res">
      <xsl:element name="{local-name()}">
        <xsl:choose>

          <!-- If there are text nodes, print the content of the
               first, not going deeper. -->
          <xsl:when test="node()[1]/self::text()">
            <xsl:variable name="text" select="text()[1]"/>
            <xsl:choose>
              <xsl:when test="string-length($text) &gt; $title-limit">
                <xsl:value-of 
                    select="concat(substring($text, 0, $title-limit), '...')"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="$text"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>

          <!-- If there were no text nodes try to go deeper -->
          <xsl:otherwise>
            <xsl:choose>

              <!-- If we about to work with leaf nodes, then process
                   only those having text as the first child. -->
              <xsl:when test="$depth = ($toc-depth - 2)">
                <xsl:apply-templates 
                    select="*[node()[1]/self::text()][1]" xse:sections="omit"/>
              </xsl:when>

              <!-- Process only first child that have text children, and all
                   those who have no text children -->
              <xsl:otherwise>
                <xsl:apply-templates 
                    select="*[node()[1]/self::text()][1]|*[not (text())]"
                    xse:sections="omit"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:element>
    </xsl:variable>

    <!-- If element depth is equivalent to toc-depth, do nothing -->
    <xsl:if test="($depth &lt; $toc-depth) and string($res)">
      <xsl:copy-of select="$res"/>
    </xsl:if>

  </xsl:template>

  <xsl:template match="text()">
  </xsl:template>

</xsl:stylesheet>
