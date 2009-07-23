<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xi="http://www.w3.org/2001/XInclude"
                xmlns:exsl="http://exslt.org/common"
                version='1.1'
                exclude-result-prefixes="xi"
                extension-element-prefixes="exsl">

<xsl:template name="result-elem-attributes">
  <xsl:param name="attr-spec" select="''"/>
  <xsl:if test="$attr-spec">
    <xsl:choose>
      <xsl:when test="contains($attr-spec, ',')">
        <xsl:call-template name="result-elem-attributes">
          <xsl:with-param name="attr-spec"
            select="substring-before($attr-spec, ',')"/>
        </xsl:call-template>
        <xsl:call-template name="result-elem-attributes">
          <xsl:with-param name="attr-spec"
            select="substring-after($attr-spec, ',')"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:variable name="attr-name"
          select="substring-before($attr-spec, '=')"/>
        <xsl:variable name="attr-value"
          select="substring-after($attr-spec, '=')"/>

        <xsl:variable name="computed-attr-value">
          <xsl:choose>
            <xsl:when test="starts-with($attr-value,'@')">
              <xsl:choose>
                <xsl:when test="$attr-value='@'">
                  <xsl:value-of
                    select="string(attribute::*[name()=$attr-name])"/>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:value-of
                    select="string(attribute::*[name()=
                      substring-after($attr-value, '@')])"/>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="$attr-value"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:variable>
        <xsl:if test="string($computed-attr-value)">
          <xsl:attribute name="{$attr-name}">
            <xsl:value-of select="$computed-attr-value"/>
          </xsl:attribute>
        </xsl:if>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>
</xsl:template>

<xsl:template name="gen-element">
  <xsl:param name="result-elem-spec" select="$result-elem-spec"/>
  <xsl:variable name="elem-spec">
    <xsl:choose>
      <xsl:when test="contains($result-elem-spec, '/')">
        <xsl:value-of select="substring-before($result-elem-spec, '/')"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$result-elem-spec"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="elem-name">
    <xsl:choose>
      <xsl:when test="contains($elem-spec, ',')">
        <xsl:value-of select="substring-before($elem-spec, ',')"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$elem-spec"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:element name="{$elem-name}">
    <xsl:if test="@id">
      <xsl:attribute name="id">
        <xsl:value-of select="@id"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:call-template name="result-elem-attributes">
      <xsl:with-param name="attr-spec"
        select="substring-after($elem-spec, ',')"/>
    </xsl:call-template>
    <xsl:choose>
      <xsl:when test="substring-after($result-elem-spec, '/')">
        <xsl:call-template name="gen-element">
          <xsl:with-param name="result-elem-spec"
            select="substring-after($result-elem-spec, '/')"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:element>
</xsl:template>

</xsl:stylesheet>
