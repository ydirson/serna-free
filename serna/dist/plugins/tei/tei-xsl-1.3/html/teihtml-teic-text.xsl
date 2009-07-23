<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  version="1.0">
  
<xsl:import href="teihtml-teic.xsl"/>
<xsl:param name="splitLevel">-1</xsl:param>
<xsl:param name="autoToc">true</xsl:param>
<xsl:param name="leftLinks"></xsl:param>
<xsl:param name="makePageTable"></xsl:param>
<xsl:param name="cssFile"></xsl:param>
<xsl:param name="bgcolor">#000000</xsl:param>
<xsl:param name="textcolor">#FFFF00</xsl:param>

<xsl:param name="showFigures"></xsl:param>
<xsl:template name="metaHook"/>
<xsl:template name="bodyHook">
  <xsl:attribute name="bgcolor"><xsl:value-of 
       select="$bgcolor"/></xsl:attribute>
  <xsl:attribute name="text"><xsl:value-of select="$textcolor"/></xsl:attribute>
  <xsl:attribute name="link">#00FFFF</xsl:attribute>
  <xsl:attribute name="alink">#FFFF00</xsl:attribute>
  <xsl:attribute name="vlink">#00CCFF</xsl:attribute>
</xsl:template>

<xsl:template name="stdheader">
  <xsl:param name="title" select="'(no title)'"/>
<b>Institution: <xsl:value-of select="$institution"/></b><br/>
    <xsl:if test="not($department = '')">
<b>Department: <xsl:value-of select="$department"/></b>
</xsl:if>
<br/>
<br/>
<b><xsl:value-of select="$title"/></b>
<br/>
</xsl:template>

<xsl:template match="front/div|front/div0|front/div1|front/div2|front/div3|front/div4|front/front/div5|front/div6"> 
  <xsl:variable name="depth">
     <xsl:apply-templates select="." mode="depth"/>
  </xsl:variable>
   <font face="Verdana,Arial" size="5">
     <xsl:call-template name="doDivBody"/>
   </font>
</xsl:template>

<xsl:template match="back/div|back/div0|back/div1|back/div2|back/div3|back/div4|back/div5|back/div6"> 
  <xsl:variable name="depth">
     <xsl:apply-templates select="." mode="depth"/>
  </xsl:variable>
   <font face="Verdana,Arial" size="5">
     <xsl:call-template name="doDivBody"/>
   </font>
</xsl:template>

<xsl:template match="div|div0|div1|div2|div3|div4|div5|div6"> 
  <xsl:variable name="depth">
   <xsl:apply-templates select="." mode="depth"/>
 </xsl:variable>
   <font face="Verdana,Arial" size="5">
     <xsl:call-template name="doDivBody"/>
 </font>
</xsl:template>

<xsl:template name="doDivBody">
<xsl:param name="Type"/>
<xsl:variable name="ident">
   <xsl:apply-templates select="." mode="ident"/>
</xsl:variable>
<br/><br/>
<p><b><a name="{$ident}"></a>
<xsl:call-template name="header"/></b></p>
<xsl:apply-templates/>
</xsl:template>

<xsl:template name="simpleBody">
  <xsl:comment> front matter </xsl:comment>
 <xsl:apply-templates select="text/front"/>

 <xsl:if test="$autoToc='true' and (descendant::div or descendant::div0 or descendant::div1) and not(descendant::divGen)">
   <p>   <b><xsl:value-of select="$tocWords"/></b></p>
   <xsl:call-template name="maintoc"/>
 </xsl:if>

 <xsl:call-template name="doBody"/>

 <xsl:apply-templates select="text/back"/>

</xsl:template>


</xsl:stylesheet>


