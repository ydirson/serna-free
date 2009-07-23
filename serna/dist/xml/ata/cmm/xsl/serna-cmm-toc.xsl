<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'
                xmlns="http://www.w3.org/TR/xhtml1/transitional"
                exclude-result-prefixes="#default">

  <xsl:output method="xml" indent="yes"/>
  <xsl:strip-space elements="*"/>

  <xsl:param name="autotoc.label.separator" select="'. '"/>
  <xsl:param name="title-limit" select="'50'"/>

  <xsl:template match="cmm|chapx|cmmx|section|subject|mfmatr|intro|
                       pgblk|tfmatr|chapter|vendlist|
                       vendata|mfr|mad|dplist|figure|
                       iplintro|cmm-ipl|topic|task|pretopic|vendlist|iplintro">
    <xsl:variable name="label">
      <xsl:apply-templates select="." mode="toc" se:sections="omit"/>
    </xsl:variable>

    <xsl:variable name="content">
      <xsl:value-of select="$label"/>
      <xsl:value-of select="$autotoc.label.separator"/>      
    </xsl:variable>

    <xsl:element name="{local-name()}">
      <xsl:if test="string-length($label)">
        <xsl:value-of select="$content"/>
      </xsl:if>
      <xsl:apply-templates se:sections="omit"/>
    </xsl:element>
  </xsl:template>

  <xsl:template mode="toc" match="cmm|chapx|cmmx|tfmatr|vendlist|pgblk|mfmatr|
                       vendata|mfr|mad|dplist|figure|cmm-ipl"/>

  <xsl:template mode="toc" match="topic|task|pretopic|vendlist|iplintro|intro">
    <xsl:if test="title">
      <xsl:value-of select="title"/>
    </xsl:if>
  </xsl:template>

  <xsl:template mode="toc" match="chapter">
    <xsl:if test="@chapnbr">
      <xsl:value-of select="@chapnbr"/>
    </xsl:if>
  </xsl:template>

  <xsl:template mode="toc" match="section">
    <xsl:if test="@sectnbr">
      <xsl:value-of select="@sectnbr"/>
    </xsl:if>
  </xsl:template>

  <xsl:template mode="toc" match="subject">
    <xsl:if test="@subjnbr">
      <xsl:value-of select="@subjnbr"/>
    </xsl:if>
  </xsl:template>

  <xsl:template mode="toc" match="cmmx">
    <xsl:if test="@chapnbr">
      <xsl:value-of select="@chapnbr"/>
    </xsl:if>
    <xsl:if test="@sectnbr">
      <xsl:text>-</xsl:text>
      <xsl:value-of select="@sectnbr"/>
    </xsl:if>
    <xsl:if test="@subjnbr">
      <xsl:text>-</xsl:text>
      <xsl:value-of select="@subjnbr"/>
    </xsl:if>
  </xsl:template>

  <xsl:template match="subtopic" mode="toc">
      <xsl:if test="title">
        <xsl:number level="multiple" format="1.1.1.1." count="subtopic"/>
        <xsl:value-of select="title"/>
        <!--xsl:apply-templates mode="toc"/-->
      </xsl:if>
  </xsl:template>

  <xsl:template match="text()">
  </xsl:template>

  <xsl:template match="*">
  </xsl:template>

  <xsl:template match="processing-instruction('se:choice')">
  </xsl:template>

</xsl:stylesheet>
