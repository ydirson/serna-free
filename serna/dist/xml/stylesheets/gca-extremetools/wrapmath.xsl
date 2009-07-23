<?xml version="1.0" encoding="iso-8859-1"?>

<!-- Wraps all occurrences of characters in the provided $symbol.chars set
     in a <sym> element; otherwise an identity transform -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                version="1.0">

  <xsl:output method="xml" indent="no"/>

  <xsl:variable name="symbol.chars"
      select="concat ('&#x391;&#x392;&#x393;&#x394;&#x395;&#x396;&#x397;&#x398;&#x399;&#x39A;&#x39B;&#x39C;&#x39D;&#x39E;&#x39F;',
                      '&#x3A0;&#x3A1;&#x3A3;&#x3A4;&#x3A5;&#x3A6;&#x3A7;&#x3A8;&#x3A9;',
                      '&#x3B1;&#x3B2;&#x3B3;&#x3B4;&#x3B5;&#x3B6;&#x3B7;&#x3B8;&#x3B9;&#x3BA;&#x3BB;&#x3BC;&#x3BD;&#x3BE;&#x3BF;',
                      '&#x3C0;&#x3C1;&#x3C2;&#x3C3;&#x3C4;&#x3C5;&#x3C6;&#x3C7;&#x3C8;&#x3C9;&#x3D1;&#x3D2;&#x3D5;&#x3D6;',
                      '&#x2032;&#x2033;',
                      '&#x2111;&#x2118;&#x211C;',
                      '&#x2135;',
                      '&#x2190;&#x2191;&#x2192;&#x2193;&#x2194;',
                      '&#x21B5;',
                      '&#x21D0;&#x21D1;&#x21D2;&#x21D3;&#x21D4;',
                      '&#x2200;&#x2202;&#x2203;&#x2205;&#x2207;&#x2208;&#x2209;&#x220B;&#x220F;',
                      '&#x2211;&#x2212;&#x2217;&#x221A;&#x221D;&#x221E;',
                      '&#x2220;&#x2227;&#x2228;&#x2229;&#x222A;&#x222B;',
                      '&#x2234;&#x223C;',
                      '&#x2245;&#x2248;',
                      '&#x2260;&#x2261;&#x2264;&#x2265;',
                      '&#x2282;&#x2283;&#x2284;&#x2286;&#x2287;',
                      '&#x2295;&#x2297;',
                      '&#x22A5;',
                      '&#x22C5;',
                      '&#x2320;&#x2321;&#x2329;&#x232A;',
                      '&#x25CA;',
                      '&#x2660;&#x2663;&#x2665;&#x2666;')"/>


  <xsl:template match="/ | * | @* | processing-instruction()">
    <xsl:copy>
      <xsl:apply-templates select="*|@*|text()|comment()|processing-instruction()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="graphic">
    <xsl:copy>
      <xsl:attribute name="file">
        <xsl:call-template name="trimURI">
          <xsl:with-param name="URIstring" select="unparsed-entity-uri(@figname)"/>
        </xsl:call-template>
      </xsl:attribute>
      <xsl:apply-templates select="*|@*|text()|comment()|processing-instruction()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="graphic/@figname"/>

  <xsl:template match="text()">
    <xsl:choose>
      <xsl:when test="translate(. , $symbol.chars, '') = ."><xsl:copy/></xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="string-lookup">
          <xsl:with-param name="str" select="."/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="string-lookup">
    <xsl:param name="str" select="''"/>
    <xsl:if test="string-length($str) != 0">
      <xsl:variable name="c" select="substring($str, 1, 1)"/>
      <xsl:choose>
        <xsl:when test="contains($symbol.chars, $c)">
          <sym>
            <xsl:value-of select="$c"/>
          </sym>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$c"/>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:call-template name="string-lookup">
        <xsl:with-param name="str" select="substring($str, 2)"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:template>

  <xsl:template name="trimURI">
    <!-- trims a URI to just what follows the final '/' character
         (i.e. should be just a relative path to a file in the same subdirectory) -->
    <xsl:param name="URIstring" select="''"/>
    <xsl:choose>
      <xsl:when test="contains($URIstring, '/')">
        <xsl:call-template name="trimURI">
          <xsl:with-param name="URIstring" select="substring-after($URIstring, '/')"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$URIstring"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


</xsl:stylesheet>
