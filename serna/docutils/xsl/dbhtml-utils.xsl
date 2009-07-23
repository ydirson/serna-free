<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

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

<xsl:template name="stripname">
  <xsl:param name="pathname"/>
  <xsl:param name="sep" select="'.'"/>
  <xsl:variable name="end" select="substring-after($pathname, $sep)"/>
  <xsl:choose>
    <xsl:when test="contains($end, $sep)">
      <xsl:value-of
        select="concat(substring-before($pathname, $end), $sep)"/>
        <xsl:call-template name="basename">
          <xsl:with-param name="pathname" select="$end"/>
        </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of
        select="substring($pathname, 1,
                          string-length($pathname)-string-length($end)-1)"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="basename">
  <xsl:param name="filename"/>
  <xsl:call-template name="stripname">
    <xsl:with-param name="pathname" select="$filename"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="dirname">
  <xsl:param name="pathname"/>
  <xsl:call-template name="stripname">
    <xsl:with-param name="pathname" select="$pathname"/>
    <xsl:with-param name="sep" select="'/'"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="filename">
  <xsl:param name="pathname"/>
  <xsl:variable name="dirname">
    <xsl:call-template name="dirname">
      <xsl:with-param name="pathname" select="$pathname"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="$dirname=$pathname">
      <xsl:value-of select="$pathname"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="substring($pathname, string-length($dirname)+2)"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="dbhtml-filename">
  <xsl:param name="pis" select="./processing-instruction('dbhtml')"/>
  <xsl:param name="ext" select="'.html'"/>

  <xsl:call-template name="basename">
    <xsl:with-param name="filename">
      <xsl:call-template name="dbhtml-attribute">
        <xsl:with-param name="pis" select="$pis"/>
        <xsl:with-param name="attribute">filename</xsl:with-param>
      </xsl:call-template>
    </xsl:with-param>
  </xsl:call-template>
  <xsl:value-of select="$ext"/>
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

<xsl:template name="dbhtml-href">
  <xsl:param name="pis" select="processing-instruction('dbhtml')"/>
  <xsl:param name="ext" select="'.html'"/>
  <xsl:call-template name="dbhtml-dir">
    <xsl:with-param name="pis" select="$pis"/>
  </xsl:call-template>
  <xsl:call-template name="dbhtml-filename">
    <xsl:with-param name="pis" select="$pis"/>
    <xsl:with-param name="ext" select="$ext"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="has.key">
  <xsl:param name="key" select="''"/>
  <xsl:param name="list" select="''"/>

  <xsl:if test="$list">
    <xsl:choose>
      <xsl:when test="$list=$key">1</xsl:when>
      <xsl:when test="substring-before($list, '|')=$key">1</xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="has.key">
          <xsl:with-param name="key" select="$key"/>
          <xsl:with-param name="list" select="substring-after($list, '|')"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>
</xsl:template>

<xsl:template name="lookup.key">
  <xsl:param name="key" select="''"/>
  <xsl:param name="table" select="''"/>
  <xsl:param name="default" select="''"/>

  <xsl:choose>
    <xsl:when test="contains($table, ' ')">
      <xsl:variable name="has-key">
        <xsl:call-template name="has.key">
          <xsl:with-param name="key" select="$key"/>
          <xsl:with-param name="list"
            select="normalize-space(substring-before($table, ' '))"/>
        </xsl:call-template>
      </xsl:variable>
      <xsl:variable name="rest"
        select="normalize-space(substring-after($table, ' '))"/>
      <xsl:choose>
        <xsl:when test="string($has-key)">
          <xsl:choose>
            <xsl:when test="contains($rest, ' ')">
              <xsl:value-of select="substring-before($rest, ' ')"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="$rest"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="lookup.key">
            <xsl:with-param name="key" select="$key"/>
            <xsl:with-param name="table" select="substring-after($rest, ' ')"/>
            <xsl:with-param name="default" select="$default"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$default"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>