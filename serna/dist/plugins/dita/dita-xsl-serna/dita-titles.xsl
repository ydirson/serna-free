<?xml version='1.0'?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:se="http://www.syntext.com/XSL/Format-1.0"
                xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
                extension-element-prefixes="dtm"
                version='1.0'>
 

<!-- =================== start of element rules ====================== -->

<!-- NESTED TOPIC TITLES (sensitive to nesting depth, but are still processed for contained markup) -->

<!-- h1 -->
<dtm:doc dtm:elements="topic/title|glossterm" dtm:status="finished" dtm:idref="title.h1"/>
<xsl:template match="*[contains(@class,' topic/topic ') or contains(@class, ' map/map ')]/*[contains(@class,' topic/title ')]" priority="2" dtm:id="title.h1">
  <fo:block xsl:use-attribute-sets="topictitle1" id="{generate-id()}" padding-top="1.4pc">
    <fo:block border-top-color="black" border-top-width="3pt" line-height="100%"
              border-left-width="0pt" border-right-width="0pt">
      <xsl:call-template name="get-title"/>
    </fo:block>
  </fo:block>
</xsl:template>


<!-- h2 -->
<dtm:doc dtm:elements="topic/topic/title" dtm:status="finished" dtm:idref="title.h2"/>
<xsl:template match="*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/title ')]" priority="3" dtm:id="title.h2">
  <fo:block xsl:use-attribute-sets="topictitle2" id="{generate-id()}" padding-top="1pc">
    <fo:block border-top-color="black" border-top-width="1pt"
              border-left-width="0pt" border-right-width="0pt">
      <xsl:call-template name="get-title"/>
    </fo:block>
  </fo:block>
</xsl:template>

<!-- h3 -->
<dtm:doc dtm:elements="topic/topic/topic/title" dtm:status="finished" dtm:idref="title.h3"/>
<xsl:template match="*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/title ')]" priority="4" dtm:id="title.h3">
  <fo:block xsl:use-attribute-sets="topictitle3" id="{generate-id()}">
    <xsl:call-template name="get-title"/>
  </fo:block>
</xsl:template>

<!-- h4 -->
<dtm:doc dtm:elements="topic/topic/topic/topic/title" dtm:status="finished" dtm:idref="title.h4"/>
<xsl:template match="*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/title ')]" priority="5" dtm:id="title.h4">
  <fo:block xsl:use-attribute-sets="topictitle4" id="{generate-id()}">
    <xsl:if test="$trace='yes'"><fo:inline color="purple"></fo:inline></xsl:if>
    <xsl:call-template name="get-title"/>
  </fo:block>
</xsl:template>

<!-- h5 -->
<dtm:doc dtm:elements="topic/topic/topic/topic/topic/title" dtm:status="finished" dtm:idref="title.h5"/>
<xsl:template match="*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/title ')]" priority="6" dtm:id="title.h5">
  <fo:block xsl:use-attribute-sets="topictitle5" id="{generate-id()}">
    <xsl:call-template name="get-title"/><xsl:text>: </xsl:text>
  </fo:block>
</xsl:template>

<!-- h6 -->
<dtm:doc dtm:elements="topic/topic/topic/topic/topic/topic/title" dtm:status="finished" dtm:idref="title.h6"/>
<xsl:template match="*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/topic ')]/*[contains(@class,' topic/title ')]" priority="7" dtm:id="title.h6">
  <fo:block xsl:use-attribute-sets="topictitle6" id="{generate-id()}">
    <xsl:call-template name="get-title"/><xsl:text>: </xsl:text>
  </fo:block>
</xsl:template>





<!-- section/title handling -->
<dtm:doc dtm:elements="section/title" dtm:status="finished" dtm:idref="section.title"/>
<xsl:template match="*[contains(@class,' topic/section ')]/*[contains(@class,' topic/title ')]" dtm:id="section.title">
  <fo:block font-weight="bold">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>


<!-- example/title handing -->
<dtm:doc dtm:elements="example/title" dtm:status="finished" dtm:idref="example.title"/>
<xsl:template match="*[contains(@class,' topic/example ')]/*[contains(@class,' topic/title ')]" dtm:id="example.title">
  <fo:block font-weight="bold">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>


<!-- table/title handling -->
<dtm:doc dtm:elements="table/title" dtm:status="finished" dtm:idref="table.title table.title.place"/>
<xsl:template match="*[contains(@class,' topic/table ')]/*[contains(@class,' topic/title ')]" dtm:id="table.title">
  <xsl:variable name="tbl-pfx-txt">
    <xsl:call-template name="getString">
      <xsl:with-param name="stringName" select="'Table'"/>
    </xsl:call-template>
    <xsl:number level="single" count="title" from="/" />
  </xsl:variable>
  <fo:block font-weight="bold">
    <fo:inline color="red"><xsl:value-of select="$tbl-pfx-txt"/>. </fo:inline>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>


<!-- fig/title handling -->
<dtm:doc dtm:elements="fig/title" dtm:status="finished" dtm:idref="fig.title fig.title.place"/>
<xsl:template match="*[contains(@class,' topic/fig ')]/*[contains(@class,' topic/title ')]" dtm:id="fig.title">
  <xsl:variable name="fig-pfx-txt">
    <xsl:call-template name="getString">
      <xsl:with-param name="stringName" select="'Figure'"/>
    </xsl:call-template>
    <xsl:number level="single" count="title" from="/" />
  </xsl:variable>
  <fo:block font-weight="bold">
    <fo:inline color="red"><xsl:value-of select="$fig-pfx-txt"/>. </fo:inline>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>


<xsl:template name="place-tbl-lbl" dtm:id="table.title.place">
<xsl:variable name="tbl-count">                  <!-- Number of table/title's before this one -->
 <xsl:number count="*/table/title" level="multiple"/> <!-- was ANY-->
</xsl:variable>
<xsl:variable name="tbl-count-actual">           <!-- Number of table/title's including this one -->
 <xsl:choose>
   <xsl:when test="not($tbl-count&gt;0) and not($tbl-count=0) and not($tbl-count&lt;0)">1</xsl:when>
   <xsl:otherwise><xsl:value-of select="$tbl-count+1"/></xsl:otherwise>
 </xsl:choose>
</xsl:variable>
  <xsl:choose>
    <xsl:when test="*[contains(@class,' topic/title ')]">
      <fo:block><fo:inline font-weight="bold">
        <xsl:call-template name="getString">
         <xsl:with-param name="stringName" select="'Table'"/>
        </xsl:call-template><xsl:text> </xsl:text><xsl:value-of select="$tbl-count-actual"/>.<xsl:text> </xsl:text>
        <xsl:apply-templates select="*[contains(@class,' topic/title ')]" mode="exhibittitle"/>
      </fo:inline>
      <xsl:if test="*[contains(@class,' topic/desc ')]">
        <xsl:text>. </xsl:text><xsl:apply-templates select="*[contains(@class,' topic/desc ')]" mode="exhibitdesc"/>
      </xsl:if>
      </fo:block>
    </xsl:when>
    <xsl:when test="*[contains(@class,' topic/desc ')]">
      <fo:block>****<xsl:value-of select="*[contains(@class,' topic/desc ')]"/></fo:block>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template name="place-fig-lbl" dtm:id="fig.title.place">
<xsl:variable name="fig-count">                 <!-- Number of fig/title's before this one -->
 <xsl:number count="*/fig/title" level="multiple"/>
</xsl:variable>
<xsl:variable name="fig-count-actual">          <!-- Number of fig/title's including this one -->
 <xsl:choose>
   <xsl:when test="not($fig-count&gt;0) and not($fig-count=0) and not($fig-count&lt;0)">1</xsl:when>
   <xsl:otherwise><xsl:value-of select="$fig-count+1"/></xsl:otherwise>
 </xsl:choose>
</xsl:variable>
  <xsl:choose>
    <xsl:when test="*[contains(@class,' topic/title ')]">
      <fo:block><fo:inline font-weight="bold">
        <xsl:call-template name="getString">
          <xsl:with-param name="stringName" select="'Figure'"/>
        </xsl:call-template><xsl:text> </xsl:text><xsl:value-of select="$fig-count-actual"/>.<xsl:text> </xsl:text>
        <xsl:apply-templates select="*[contains(@class,' topic/title ')]" mode="exhibittitle"/>
      </fo:inline>
      <xsl:if test="desc">
        <xsl:text>. </xsl:text><xsl:apply-templates select="*[contains(@class,' topic/desc ')]" mode="exhibitdesc"/>
      </xsl:if>
      </fo:block>
    </xsl:when>
    <xsl:when test="*[contains(@class, ' topic/desc ')]">
      <fo:block>****<xsl:value-of select="*[contains(@class,' topic/desc ')]"/></fo:block>
    </xsl:when>
  </xsl:choose>
</xsl:template>



<!-- ======== NAMED TEMPLATES for labels and titles related to topic structures ======== -->
<dtm:doc dtm:status="testing" dtm:idref="gettitle"/>
<xsl:template name="get-title" dtm:id="gettitle"><!-- get fully-processed title content by whatever mechanism -->
  <xsl:choose>
   <!-- add keycol here once implemented -->
   <xsl:when test="@spectitle">
     <xsl:value-of select="@spectitle"/>
   </xsl:when>
   <xsl:otherwise>
     <xsl:call-template name="extra-info"/>
   </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<dtm:doc dtm:status="testing" dtm:idref="section.heading.get"/>
<xsl:template name="get-sect-heading" dtm:id="section.heading.get">
     <xsl:choose>
      <!-- replace with keyref once implemented -->
      <xsl:when test="@spectitle">
        <xsl:value-of select="@spectitle"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="title"/>
      </xsl:otherwise>
     </xsl:choose>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="section.heading"/>
<xsl:template name="sect-heading" dtm:id="section.heading">
  <xsl:param name="deftitle" select="."/> <!-- get param by reference -->
  <xsl:variable name="heading">
     <xsl:choose>
      <xsl:when test="*[contains(@class,' topic/title ')]">
        <xsl:value-of select="*[contains(@class,' topic/title ')]"/>
      </xsl:when>
      <xsl:when test="@spectitle">
        <xsl:value-of select="@spectitle"/>
      </xsl:when>
      <xsl:otherwise/>
     </xsl:choose>
  </xsl:variable>

  <!-- based on graceful defaults, build an appropriate section-level heading -->
  <xsl:choose>
    <xsl:when test="not($heading='')">
      <xsl:if test="normalize-space($heading)=''">
        <!-- hack: a title with whitespace ALWAYS overrides as null -->
        <!--xsl:comment>no heading</xsl:comment-->
      </xsl:if>
      <!--xsl:call-template name="proc-ing"/--><xsl:value-of select="$heading"/>
    </xsl:when>
    <xsl:when test="$deftitle">
      <xsl:value-of select="$deftitle"/>
    </xsl:when>
    <xsl:otherwise><!-- no heading title, output section starting with a break --></xsl:otherwise>
  </xsl:choose>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="extrainfo"/>
<xsl:template name="extra-info" dtm:id="extrainfo">
  <xsl:choose>
    <xsl:when test="processing-instruction('serna-extra-info') and text()=''">
      <fo:block>
        <fo:inline background-color="#e0e0e0" font-style="italic"> 
           <xsl:apply-templates/>
           <xsl:text> </xsl:text>
        </fo:inline>
      </fo:block> 
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates select="*[not(self::processing-instruction('serna-extra-info'))]"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="serna.extrainfo"/>
<xsl:template match="processing-instruction('serna-extra-info')" dtm:id="serna.extrainfo">
  <xsl:value-of select="."/>
</xsl:template>

</xsl:stylesheet>
