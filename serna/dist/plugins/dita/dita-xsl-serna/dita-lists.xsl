<?xml version='1.0'?>

<!DOCTYPE xsl:transform [
<!-- entities for use in the generated output (must produce correctly in FO) -->
  <!ENTITY rbl           "&#160;">
  <!ENTITY bullet        "&#x2022;"><!--check these two for better assignments -->
]>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
                extension-element-prefixes="dtm"
                version='1.0'>

<!-- ============ Lists (ul, ol, sl, dl) ============ -->
<!-- all dingbat lists start the same way (sl has a null dingbat, in effect) -->

<dtm:doc dtm:elements="sl" dtm:status="finished" dtm:idref="topic.ul|sl"/>
<dtm:doc dtm:elements="ol|steps|substeps" dtm:status="finished" dtm:idref="topic.ol"/>
<dtm:doc dtm:elements="ul|choices|steps-unordered" dtm:status="finished" dtm:idref="topic.ol|ul|sl"/>

<xsl:template match="*[contains(@class,' topic/ul ')]|
                     *[contains(@class,' topic/sl ')]" dtm:id="topic.ul|sl">
  <fo:list-block xsl:use-attribute-sets="ul">
    <xsl:apply-templates/>
  </fo:list-block>
</xsl:template>

<xsl:template match="*[contains(@class,' topic/ol ')]" dtm:id="topic.ol">
  <fo:list-block xsl:use-attribute-sets="ul">
    <xsl:apply-templates>
      <xsl:with-param name="level">
        <xsl:call-template name="get-list-level"/>
      </xsl:with-param>
    </xsl:apply-templates>
  </fo:list-block>
</xsl:template>

<dtm:doc dtm:elements="ul/li|choice" 
         dtm:status="finished" dtm:idref="topic.ul.li li.name"/>

<xsl:template match="*[contains(@class,' topic/ul ')]/
                     *[contains(@class,' topic/li ')]" dtm:id="topic.ul.li">
  <fo:list-item padding-bottom="0.4em">
    <fo:list-item-label end-indent="label-end()" text-align="end">
      <fo:block>
        <fo:inline>&#x2022;</fo:inline>
      </fo:block>
    </fo:list-item-label>
    <fo:list-item-body start-indent="body-start()"> 
      <fo:block>
        <xsl:apply-templates /> 
      </fo:block>
    </fo:list-item-body> 
  </fo:list-item>
</xsl:template>

<dtm:doc dtm:elements="ol/li|step|substep"
         dtm:status="finished" dtm:idref="get-list-level"/>
<xsl:template name="get-list-level" dtm:id="get-list-level">
  <xsl:value-of select="count(
    ancestor-or-self::*[contains(@class, ' topic/ul ')]|
    ancestor-or-self::*[contains(@class, ' topic/dl ')]|
    ancestor-or-self::*[contains(@class, ' topic/sl ')]|
    ancestor-or-self::*[contains(@class, ' topic/ol ')])"/>
</xsl:template>

<dtm:doc dtm:elements="li"
         dtm:status="finished" dtm:idref="generate-listitem-label"/>
<xsl:template name="generate-listitem-label" 
              dtm:id="generate-listitem-label">
  <xsl:param name="list-level"/>
  <xsl:param name="conref-context"/>
  <xsl:choose>
    <xsl:when test="($list-level mod 2) = 1">
      <xsl:choose>
        <xsl:when test="$conref-context">
          <xsl:for-each select="$conref-context">
            <xsl:number format="1." />
          </xsl:for-each>
        </xsl:when>
        <xsl:otherwise>
          <xsl:number format="1." />
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:choose>
        <xsl:when test="$conref-context">
          <xsl:for-each select="$conref-context">
            <xsl:number format="a." /> 
          </xsl:for-each>
        </xsl:when> 
        <xsl:otherwise> 
          <xsl:number format="a." /> 
        </xsl:otherwise> 
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<dtm:doc dtm:elements="ol/li|step|substep" 
         dtm:status="finished" dtm:idref="topic.ol.li li.name"/>

<xsl:template match="*[contains(@class,' topic/ol ')]/
                     *[contains(@class,' topic/li ')]" dtm:id="topic.ol.li">
  <xsl:param name="level"/>
  <xsl:param name="conref-context"/>
  <xsl:variable name="list-level">
    <xsl:choose>
      <xsl:when test="$conref-context">
        <xsl:for-each select="$conref-context">
          <xsl:call-template name="get-list-level"/>
        </xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$level"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <fo:list-item padding-bottom="0.4em">
    <fo:list-item-label end-indent="label-end()" text-align="end">
      <fo:block><!-- linefeed-treatment="ignore"-->
         <xsl:call-template name="generate-listitem-label">
          <xsl:with-param name="list-level" select="$list-level"/>
          <xsl:with-param name="conref-context" select="$conref-context"/>
        </xsl:call-template> 
      </fo:block>
    </fo:list-item-label>
    <fo:list-item-body start-indent="body-start()"> 
      <fo:block>
         <xsl:apply-templates/>
      </fo:block> 
    </fo:list-item-body> 
  </fo:list-item>
</xsl:template>
 
<dtm:doc dtm:elements="sl/sli" dtm:status="finished" dtm:idref="topic.sl.sli"/>
<xsl:template match="*[contains(@class,' topic/sl ')]/
                     *[contains(@class,' topic/sli ')]" dtm:id="topic.sl.sli">
  <fo:list-item padding-bottom="0.4em">
    <fo:list-item-label end-indent="label-end()" text-align="end">
      <fo:block linefeed-treatment="ignore">
        <fo:inline><xsl:text> </xsl:text></fo:inline>
      </fo:block>
    </fo:list-item-label>
    <fo:list-item-body start-indent="body-start()"> 
      <fo:block> 
         <xsl:apply-templates /> 
      </fo:block> 
    </fo:list-item-body> 
  </fo:list-item>
</xsl:template>

<dtm:doc dtm:elements="itemgroup|tutorialinfo|stepresult|stepxmp|info" 
         dtm:status="finished" dtm:idref="topic.itemgroup"/>
<xsl:template match="*[contains(@class, ' topic/itemgroup ')]" 
              dtm:id="topic.itemgroup">
  <fo:block xsl:use-attribute-sets="p">
    <!-- setclass -->
    <xsl:apply-templates select="@compact"/>
    <!-- set id -->
    <xsl:apply-templates />
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="dl" dtm:status="finished" dtm:idref="topic.dl"/>
<xsl:template match="*[contains(@class,' topic/dl ')]" dtm:id="topic.dl">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="dlhead" dtm:status="finished" dtm:idref="topic.dlhead"/>
<xsl:template match="*[contains(@class, ' topic/dlhead ')]"
              dtm:id="topic.dlhead">
  <fo:block font-weight="bold" text-decoration="underline">
    <xsl:apply-templates />
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="dthd" dtm:status="finished" dtm:idref="topic.dthd"/>
<xsl:template match="*[contains(@class,' topic/dthd ')]" dtm:id="topic.dthd">
  <fo:block xsl:use-attribute-sets="dt" font-weight="bold">
    <!-- setclass -->
    <!-- set id -->
    <xsl:call-template name="apply-for-phrases"/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="ddhd" dtm:status="finished" dtm:idref="topic.ddhd"/>
<xsl:template match="*[contains(@class,' topic/ddhd ')]" dtm:id="topic.ddhd">
  <fo:block xsl:use-attribute-sets="dd" font-weight="bold">
    <!-- setclass -->
    <!-- set id -->
    <xsl:call-template name="apply-for-phrases"/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="dlentry" 
         dtm:status="finished" dtm:idref="topic.dlentry"/>
<xsl:template match="*[contains(@class,' topic/dlentry ')]"
              dtm:id="topic.dlentry">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>


<dtm:doc dtm:elements="dt" dtm:status="finished" dtm:idref="topic.dt"/>
<xsl:template match="*[contains(@class,' topic/dt ')]" dtm:id="topic.dt">
  <fo:block xsl:use-attribute-sets="dt">
    <!-- setclass -->
    <!-- set id -->
    <xsl:choose>
      <xsl:when test="*"> <!-- tagged content - do not default to bold -->
        <xsl:apply-templates/>
      </xsl:when>
      <xsl:otherwise>
        <fo:inline font-weight="bold">
            <xsl:call-template name="apply-for-phrases"/>
        </fo:inline> <!-- text only - bold it -->
      </xsl:otherwise>
    </xsl:choose>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="dd" dtm:status="finished" dtm:idref="topic.dd"/>
<xsl:template match="*[contains(@class,' topic/dd ')]" dtm:id="topic.dd">
  <fo:block>
    <xsl:attribute name="start-indent">
        <xsl:call-template name="get-list-level"/>
        <xsl:text>* 16 +</xsl:text>
        <xsl:value-of select="$basic-start-indent"/>
    </xsl:attribute>
    <xsl:apply-templates />
  </fo:block>
</xsl:template>

<!-- case of dl within a table cell -->
<dtm:doc dtm:elements="entry//dd" 
         dtm:status="finished" dtm:idref="topic.entry.dd"/>
<xsl:template match="*[contains(@class,' topic/entry ')]//
                     *[contains(@class,' topic/dd ')]" dtm:id="topic.entry.dd">
  <fo:block xsl:use-attribute-sets="dd.cell">
    <xsl:apply-templates />
  </fo:block>
</xsl:template>

<!-- case of dl within a simpletable cell -->
<dtm:doc dtm:elements="stentry//dd" 
         dtm:status="finished" dtm:idref="topic.stentry.dd"/>
<xsl:template match="*[contains(@class,' topic/stentry ')]//
                *[contains(@class,' topic/dd ')]" dtm:id="topic.stentry.dd">
  <fo:block xsl:use-attribute-sets="dd.cell">
    <xsl:apply-templates />
  </fo:block>
</xsl:template>

</xsl:stylesheet>
