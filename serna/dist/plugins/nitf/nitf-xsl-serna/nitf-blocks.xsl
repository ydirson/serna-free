<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="hl1">
    <fo:block xsl:use-attribute-sets="h1">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:block>
  </xsl:template>
  
  <xsl:template match="hl2">
    <fo:block xsl:use-attribute-sets="h2">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="hl3">
    <fo:block xsl:use-attribute-sets="h3">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="p">
    <fo:block xsl:use-attribute-sets="p">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="byttl">
    <fo:block xsl:use-attribute-sets="i">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:block>
  </xsl:template>
  
  <xsl:template match="p[preceding-sibling::*[1][
                       self::hl1 or self::hl2 or self::block]]">
    <fo:block xsl:use-attribute-sets="p-initial">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:block>
  </xsl:template>
  
  <xsl:template match="p[not(preceding-sibling::*) and (
                       parent::block or parent::body.content or
                       parent::td)]">
    <xsl:choose>
      <xsl:when test="ancestor::note">
        <fo:inline>
          <xsl:call-template name="process-common-attributes-and-children"/>
        </fo:inline>
      </xsl:when>
      <xsl:otherwise>
        <fo:block xsl:use-attribute-sets="p-initial-first">
          <xsl:call-template name="process-common-attributes-and-children"/>
        </fo:block>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="byline">
    <fo:block xsl:use-attribute-sets="byline">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="hedline">
    <fo:block>
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:block>
  </xsl:template>
  
  <xsl:template match="bq">
    <fo:block xsl:use-attribute-sets="blockquote">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:block>
  </xsl:template>
  
  <xsl:template match="pre">
    <fo:block xsl:use-attribute-sets="pre">
      <xsl:call-template name="process-pre"/>
    </fo:block>
  </xsl:template>
  
  <xsl:template name="process-pre">
    <xsl:call-template name="process-common-attributes"/>
    <xsl:variable name="crlf"><xsl:text>&#xD;&#xA;</xsl:text></xsl:variable>
    <xsl:variable name="lf"><xsl:text>&#xA;</xsl:text></xsl:variable>
    <xsl:variable name="cr"><xsl:text>&#xD;</xsl:text></xsl:variable>
    <xsl:for-each select="node()">
      <xsl:choose>
        <xsl:when test="position() = 1 and self::text()">
          <xsl:choose>
            <xsl:when test="starts-with(., $lf)">
              <xsl:value-of select="substring(., 2)"/>
            </xsl:when>
            <xsl:when test="starts-with(., $crlf)">
              <xsl:value-of select="substring(., 3)"/>
            </xsl:when>
            <xsl:when test="starts-with(., $cr)">
              <xsl:value-of select="substring(., 2)"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:apply-templates select="."/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise>
          <xsl:apply-templates select="."/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
  </xsl:template>
  
  <xsl:template match="addressee">
    <fo:block xsl:use-attribute-sets="address">
      <xsl:call-template name="process-common-attributes-and-children"/>
    </fo:block>
  </xsl:template>
  
  <xsl:template match="hr">
    <fo:block xsl:use-attribute-sets="hr">
      <xsl:call-template name="process-common-attributes"/>
    </fo:block>
  </xsl:template>
  
  <xsl:template match="block">
    <fo:block start-indent="40pt">
      <xsl:call-template name="process-common-attributes"/>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>
  
  <xsl:template match="br">
    <fo:block xml:space="preserve"
              white-space-collapse="false">
      <xsl:call-template name="process-common-attributes"/>
      <xsl:text>&#xA;</xsl:text>
    </fo:block>
  </xsl:template>

  <xsl:template match="note">
    <fo:block start-indent="30pt" padding-top="6pt" padding-bottom="6pt" color="green">
      <fo:inline font-style="italic">Editor's Note:</fo:inline>
      <fo:inline><xsl:apply-templates/></fo:inline>
    </fo:block>
  </xsl:template>

  <xsl:template match="location">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="tagline|datasource|rp|rb|rbc|rt|rtc">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>
    
  <xsl:template match="bibliography">
    <fo:block>
      <fo:block font-size="concat(1.5 * $body.font.master,'pt')" font-weight="bold">Bibliography</fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="ruby">
    <fo:block start-indent="0pt" end-indent="0pt" text-align="center">
      <xsl:call-template name="process-common-attributes"/>
      <xsl:if test="rt|rtc[1]/rt">
        <fo:block font-size="50%"
                  padding-top="-1.1em"
                  padding-bottom="0.1em">
          <xsl:for-each select="rt|rtc[1]/rt">
            <xsl:call-template name="process-common-attributes"/>
            <xsl:apply-templates select="."/>
          </xsl:for-each>
        </fo:block>
      </xsl:if>
      <xsl:if test="rp|rb|rbc[1]/rb">
        <fo:block>
          <xsl:for-each select="rp|rb|rbc[1]/rb">
            <xsl:call-template name="process-common-attributes"/>
            <xsl:apply-templates select="."/>
          </xsl:for-each>
        </fo:block>
      </xsl:if>
      <xsl:if test="rtc[2]/rt">
        <fo:block font-size="50%"
                  padding-top="0.1em"
                  padding-bottom="-1.1em">
          <xsl:for-each select="rt|rtc[2]/rt">
            <xsl:call-template name="process-common-attributes"/>
            <xsl:apply-templates select="."/>
          </xsl:for-each>
        </fo:block>
      </xsl:if>
    </fo:block>
  </xsl:template>

</xsl:stylesheet>
