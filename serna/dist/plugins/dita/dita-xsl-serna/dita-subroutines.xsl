<?xml version='1.0'?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:se="http://www.syntext.com/XSL/Format-1.0"
                xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
                extension-element-prefixes="dtm"                
                version='1.0'>
  
<!-- Most of these routines are specific to Print output. For authoring, expose
     these attributes to direct authoring whereever possible. -->


<!-- =================== start of processors  ====================== -->


<!--  SET UP KEYS FOR XREF MANAGEMENT  -->

<!-- set up keys based on xref's "type" attribute: %info-types;|hd|fig|table|li|fn -->
<!-- uses "title"
<xsl:key name="topic" match="topic" use="@id"/>
<xsl:key name="fig"   match="fig"   use="@id"/>
<xsl:key name="table" match="table" use="@id"/> 
-->

<!-- uses "?"
<xsl:key name="li"    match="li"    use="@id"/>
-->

<!-- uses "callout?"
<xsl:key name="fn"    match="fn"    use="@id"/>
-->

<!--  COMMON ATTRIBUTE PROCESSORS  -->

<!-- If the element has an ID, copy it through as an anchor. -->
<dtm:doc dtm:status="testing" dtm:idref="attribute.id"/>
<xsl:template match="@id" dtm:id="attribute.id">
<!-- Filler for A-name anchors (empty links)-->
<xsl:variable name="afill"></xsl:variable>
  <fo:inline>
    <xsl:attribute name="id">
      <xsl:if test="ancestor::*[contains(@class,' topic/body ')]">
       <xsl:value-of select="ancestor::*[contains(@class,' topic/body ')]/parent::*/@id"/><xsl:text>__</xsl:text>
      </xsl:if>
      <xsl:value-of select="."/>
    </xsl:attribute>
  <xsl:value-of select="$afill"/></fo:inline>
</xsl:template>


<!-- Create the class attribute for an element when the DITA element has an outputclass attribute. -->
<dtm:doc dtm:status="testing" dtm:idref="setclass"/>
<xsl:template name="setclass" dtm:id="setclass"/> <!-- turned off for now; not needed for authoring -->

<dtm:doc dtm:status="testing" dtm:idref="xsetclass"/>
<xsl:template name="xsetclass" dtm:id="xsetclass">
  <xsl:if test="@outputclass">
     <xsl:attribute name="class"><xsl:value-of select="@outputclass"/></xsl:attribute>
  </xsl:if>
</xsl:template>


<!-- If the element has a compact=yes attribute, assert it in contextually correct form. -->
<!-- (assumes that no compaction is default) -->
<dtm:doc dtm:status="testing" dtm:idref="attribute.compact"/>
<xsl:template match="@compact" dtm:id="attribute.compact">
  <xsl:if test="@compact = 'yes'">
   <!--xsl:attribute name="compact">compact</xsl:attribute-->
   <!-- NOOP for FO for now; must use padding attributes in block context -->
  </xsl:if>
</xsl:template>

<!-- setscale and setframe work are based on text properties. For display-atts
  used for other content, we'll need to develop content-specific attribute processors -->

<!-- Process the scale attribute for text contexts -->
<dtm:doc dtm:status="testing" dtm:idref="scale.set"/>
<xsl:template name="setscale" dtm:id="scale.set">
  <xsl:if test="@scale">
    <!-- Serna does not yet take percentages. need to divide by 10 and use "pt" -->
    <xsl:attribute name="font-size"><xsl:value-of select="@scale div 10"/>pt</xsl:attribute>
  </xsl:if>
</xsl:template>

<!-- Process the frame attribute -->
<!-- frame styles (setframe) must be called within a block that defines the content being framed -->
<dtm:doc dtm:status="testing" dtm:idref="frame.set"/>
<xsl:template name="setframe" dtm:id="frame.set">
    <!-- top | topbot -->
    <xsl:if test="contains(@frame,'top')">
      <xsl:attribute name="border-top-color">black</xsl:attribute>
      <xsl:attribute name="border-top-width">thin</xsl:attribute>
    </xsl:if>
    <!-- bot | topbot -->
    <xsl:if test="contains(@frame,'bot')">
      <xsl:attribute name="border-bottom-color">black</xsl:attribute>
      <xsl:attribute name="border-bottom-width">thin</xsl:attribute>
    </xsl:if>
    <!-- sides -->
    <xsl:if test="contains(@frame,'sides')">
      <xsl:attribute name="border-left-color">black</xsl:attribute>
      <xsl:attribute name="border-left-width">thin</xsl:attribute>
      <xsl:attribute name="border-right-color">black</xsl:attribute>
      <xsl:attribute name="border-right-width">thin</xsl:attribute>
    </xsl:if>
  <xsl:if test="contains(@frame,'all')">
    <xsl:attribute name="border-style">solid</xsl:attribute>
    <xsl:attribute name="border-color">black</xsl:attribute>
    <xsl:attribute name="border-width">thin</xsl:attribute>
  </xsl:if> 
</xsl:template>

<!-- =================== end of override stubs ====================== -->



</xsl:stylesheet>
