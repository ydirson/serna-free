<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<!--  -->

<xsl:template name="pi-attribute">
  <xsl:param name="pis" select="processing-instruction('')"/>
  <xsl:param name="attribute">filename</xsl:param>
  <xsl:param name="count">1</xsl:param>
  
  <xsl:choose>
    <xsl:when test="$count&gt;count($pis)">
      <!-- not found -->
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="pi">
        <xsl:value-of select="$pis[$count]"/>
      </xsl:variable>
      <xsl:choose>
        <xsl:when test="contains($pi,concat($attribute, '='))">
          <xsl:variable name="rest" select="substring-after($pi,concat($attribute,'='))"/>
          <xsl:variable name="quote" select="substring($rest,1,1)"/>
          <xsl:value-of select="substring-before(substring($rest,2),$quote)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="pi-attribute">
            <xsl:with-param name="pis" select="$pis"/>
            <xsl:with-param name="attribute" select="$attribute"/>
            <xsl:with-param name="count" select="$count + 1"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="dbhtml-attribute">
  <xsl:param name="pis" select="processing-instruction('dbhtml')"/>
  <xsl:param name="attribute">filename</xsl:param>

  <xsl:call-template name="pi-attribute">
    <xsl:with-param name="pis" select="$pis"/>
    <xsl:with-param name="attribute" select="$attribute"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="dbhtml-filename">
  <xsl:param name="pis" select="./processing-instruction('dbhtml')"/>
  <xsl:call-template name="dbhtml-attribute">
    <xsl:with-param name="pis" select="$pis"/>
    <xsl:with-param name="attribute">filename</xsl:with-param>
  </xsl:call-template>
  
  <!--xsl:variable name="fname">
    <xsl:call-template name="dbhtml-attribute">
      <xsl:with-param name="pis" select="$pis"/>
      <xsl:with-param name="attribute">filename</xsl:with-param>
    </xsl:call-template>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="$fname">
      <xsl:value-of select="$fname"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="dbhtml-filename">
        <xsl:with-param name="pis" select="../processing-instruction('dbhtml')">
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose-->
</xsl:template>

<xsl:template name="dbhtml-dir">
  <xsl:param name="context" select="."/>

  <!-- directories are now inherited from previous levels -->

  <xsl:variable name="ppath">
    <xsl:if test="$context/parent::*">
      <xsl:call-template name="dbhtml-dir">
        <xsl:with-param name="context" select="$context/parent::*"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:variable>

  <xsl:variable name="path">
    <xsl:call-template name="dbhtml-attribute">
      <xsl:with-param name="pis" select="$context/processing-instruction('dbhtml')"/>
      <xsl:with-param name="attribute">dir</xsl:with-param>
    </xsl:call-template>
  </xsl:variable>

  <xsl:choose>
    <xsl:when test="$path = ''">
      <xsl:if test="$ppath != ''">
        <xsl:value-of select="$ppath"/>
      </xsl:if>
    </xsl:when>
    <xsl:otherwise>
      <xsl:if test="$ppath != ''">
        <xsl:value-of select="$ppath"/>
        <xsl:if test="substring($ppath, string-length($ppath), 1) != '/'">
          <xsl:text>/</xsl:text>
        </xsl:if>
      </xsl:if>
      <xsl:value-of select="$path"/>
      <xsl:text>/</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>