<?xml version='1.0'?>

<!DOCTYPE xsl:transform [
<!-- entities for use in the generated output (must produce correctly in FO) -->
  <!ENTITY rbl           "&#160;">
  <!ENTITY quotedblleft  "&#x201C;">
  <!ENTITY quotedblright "&#x201D;">
  <!ENTITY bullet        "&#x2022;"><!--check these two for better assignments -->
]>

<!-- Changes:
 18 Nov 2003 DRD added pre/post break updates for image/@placement
                 added alternate shortdesc echo in output location (can author in
                 either location, but easier to insert element in original loc.
 01 Dec 2003 DRD removed background color for navtitle
 02 Dec 2003 DRD Made the default view of required-cleanup to be visible
-->


<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:se="http://www.syntext.com/XSL/Format-1.0"
                xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
                extension-element-prefixes="dtm"
                version='1.0'>
 
<!-- =================== start of element rules ====================== -->

<!--==== pre-body subset ====-->
<!-- note that topic titles are handled in the calling stylesheet -->

<dtm:doc dtm:elements="titlealts" dtm:status="finished" dtm:idref="topic.titlealts"/>
<xsl:template match="*[contains(@class,' topic/titlealts ')]" dtm:id="topic.titlealts">
  <fo:block background-color="#f0f0d0">
    <xsl:attribute name="border-style">solid</xsl:attribute>
    <xsl:attribute name="border-color">black</xsl:attribute>
    <xsl:attribute name="border-width">thin</xsl:attribute>
    <xsl:attribute name="start-indent"><xsl:value-of select="$basic-start-indent"/></xsl:attribute>
    <xsl:attribute name="start-indent"><xsl:value-of select="$basic-start-indent"/></xsl:attribute>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="navtitle" dtm:status="finished" dtm:idref="topic.navtitle"/>
<xsl:template match="*[contains(@class,' topic/navtitle ')]" dtm:id="topic.navtitle">
  <fo:block>
    <fo:inline font-weight="bold">Navigation title: </fo:inline>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="searchtitle" dtm:status="finished" dtm:idref="topic.searchtitle"/>
<xsl:template match="*[contains(@class,' topic/searchtitle ')]" dtm:id="topic.searchtitle">
  <fo:block>
    <fo:inline font-weight="bold">Search title: </fo:inline>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<!-- shortdesc is called outside of body thus needs to set up its own indent. also relatedlinks -->
<dtm:doc dtm:elements="xshortdesc" dtm:status="testing" dtm:idref="topic.xshortdesc"/>
<xsl:template match="*[contains(@class,' topic/xshortdesc ')]" mode="xoutofline" dtm:id="topic.xshortdesc">
  <fo:block xsl:use-attribute-sets="p" start-indent="{$basic-start-indent}">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="shortdesc" dtm:status="finished" dtm:idref="topic.shortdesc"/>
<xsl:template match="*[contains(@class,' topic/shortdesc ')]" dtm:id="topic.shortdesc">
  <fo:block start-indent="{$basic-start-indent}" padding-top="0.5em"
            background-color="#F0F0F0" padding-bottom="0.5em">
  <xsl:call-template name="extra-info"/>
</fo:block>
</xsl:template>

<dtm:doc dtm:elements="abstract|glossdef" dtm:status="finished" dtm:idref="topic.abstract"/>
<xsl:template match="*[contains(@class,' topic/abstract ')]" dtm:id="topic.abstract">
  <fo:block xsl:use-attribute-sets="p" start-indent="{$basic-start-indent}">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<!--==== body content subset ===-->

<!-- this is the first body called, which sets the indent and overall organization -->
<dtm:doc dtm:elements="body|refbody|taskbody|conbody" dtm:status="finished" dtm:idref="topic.body.topbody topic.body"/>
<xsl:template match="*[contains(@class,' topic/body ')]" name="topbody" dtm:id="topic.body.topbody">
  <fo:block start-indent="{$basic-start-indent}">
    <!-- here, you can generate a toc based on what's a child of body -->
    <!--xsl:call-template  name="gen-sect-ptoc"/-->
    <!--xsl:apply-templates select="preceding-sibling::*[contains(@class,' topic/shortdesc ')]"/-->
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<!-- this is the fallthrough body for nested topics -->
<xsl:template match="*[contains(@class,' topic/body ')]" dtm:id="topic.body">
  <fo:block start-indent="{$basic-start-indent}">
    <!--xsl:apply-templates select="preceding-sibling::*[contains(@class,' topic/shortdesc ')]" mode="outofline"/-->
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="section|refsyn|result|prereq|postreq|context" dtm:status="finished" dtm:idref="topic.section"/>
<xsl:template match="*[contains(@class,' topic/section ')]" dtm:id="topic.section">
  <fo:block margin-top="15pt" line-height="12pt" id="{generate-id()}">
    <xsl:if test="@spectitle and not(title)">
      <fo:block font-weight="bold"
                margin-botton="12pt"
                font-size="14pt" 
                line-height="16pt"
                color="blue">
        <xsl:value-of select="@spectitle"/>
      </fo:block>
    </xsl:if>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="example" dtm:status="finished" dtm:idref="topic.example"/>
<xsl:template match="*[contains(@class,' topic/example ')]" dtm:id="topic.example">
  <fo:block margin-top="15pt" line-height="12pt" id="{generate-id()}">
    <!-- preferentially pull the head here; process in place for now -->
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="p" dtm:status="finished" dtm:idref="topic.p"/>
<xsl:template match="*[contains(@class,' topic/p ')]" dtm:id="topic.p">
  <!-- set id -->
  <xsl:choose>
    <xsl:when test="descendant::*[contains(@class,' topic/pre ')]">
      <xsl:call-template name="divlikepara"/>
    </xsl:when>
    <xsl:when test="descendant::*[contains(@class,' topic/ul ')]">
      <xsl:call-template name="divlikepara"/>
    </xsl:when>
    <xsl:when test="descendant::*[contains(@class,' topic/ol ')]">
      <xsl:call-template name="divlikepara"/>
    </xsl:when>
    <xsl:when test="descendant::*[contains(@class,' topic/lq ')]">
      <xsl:call-template name="divlikepara"/>
    </xsl:when>
    <xsl:when test="descendant::*[contains(@class,' topic/dl ')]">
      <xsl:call-template name="divlikepara"/>
    </xsl:when>
    <xsl:when test="descendant::*[contains(@class,' topic/note ')]">
      <xsl:call-template name="divlikepara"/>
    </xsl:when>
    <xsl:when test="descendant::*[contains(@class,' topic/lines ')]">
      <xsl:call-template name="divlikepara"/>
    </xsl:when>
    <xsl:when test="descendant::*[contains(@class,' topic/fig ')]">
      <xsl:call-template name="divlikepara"/>
    </xsl:when>
    <xsl:when test="descendant::*[contains(@class,' topic/table ')]">
      <xsl:call-template name="divlikepara"/>
    </xsl:when>
    <xsl:when test="descendant::*[contains(@class,' topic/simpletable ')]">
      <xsl:call-template name="divlikepara"/>
    </xsl:when>
    <xsl:otherwise>
      <fo:block xsl:use-attribute-sets="p">
        <xsl:if test="string(@id)"><xsl:attribute name="id"><xsl:value-of select="@id"/></xsl:attribute></xsl:if>
        <xsl:call-template name="extra-info"/>
      </fo:block>
    </xsl:otherwise>
  </xsl:choose>    
</xsl:template>

<dtm:doc dtm:elements="ul|ol|dl|table|simpletable" dtm:status="finished" dtm:idref="divlikepara"/>
<xsl:template name="divlikepara" dtm:id="divlikepara">
  <fo:block xsl:use-attribute-sets="divlike.p">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="note" dtm:status="finished" dtm:idref="topic.note divlikepara"/>
<xsl:template match="*[contains(@class,' topic/note ')]" dtm:id="topic.note">
  <xsl:variable name="type-map">
    <type val="">Note</type>
    <type val="note">Note</type>
    <type val="tip">Tip</type>
    <type val="fastpath">Fastpath</type>
    <type val="restriction">Restriction</type>
    <type val="important">Important</type>
    <type val="remember">Remember</type>
    <type val="attention">Attention</type>
    <type val="caution">Caution</type>
    <type val="danger">Danger</type>
    <type val="other">
      <xsl:choose>
        <xsl:when test="@othertype">
          <xsl:value-of select="@othertype"/>
        </xsl:when>
        <xsl:otherwise>
          [<xsl:value-of select="@type"/>]
        </xsl:otherwise>
      </xsl:choose>
    </type>
  </xsl:variable>
  <xsl:variable name="note-type" select="@type"/>
  <xsl:variable name="note-inscription" 
                select="$type-map/type[@val=$note-type]"/>
  <fo:block padding-bottom="0.4em">
    <fo:inline border-left-width="0pt" border-right-width="0pt" 
               font-weight="bold">
      <xsl:choose>
        <xsl:when test="@type='other'">
          <xsl:value-of select="$note-inscription"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="getString">
            <xsl:with-param name="stringName" 
              select="$note-inscription"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:text>: </xsl:text>
    </fo:inline>
    <xsl:text>  </xsl:text>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="desc" dtm:status="finished" dtm:idref="topic.desc"/>
<xsl:template match="*[contains(@class,' topic/desc ')]" dtm:id="topic.desc">
  <fo:inline border-left-width="0pt" border-right-width="0pt">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="lq" dtm:status="finished" dtm:idref="topic.lq divlikepara"/>
<xsl:template match="*[contains(@class,' topic/lq ')]" dtm:id="topic.lq">
  <fo:block xsl:use-attribute-sets="lq">
    <!-- setclass -->
    <!-- set id -->
    <xsl:apply-templates/>
    <xsl:choose>
      <xsl:when test="@href">
        <fo:block text-align="right"><xsl:value-of select="@href"/>,
<xsl:value-of select="@reftitle"/></fo:block>
      </xsl:when>
      <xsl:when test="@reftitle">
        <fo:block text-align="right"><xsl:value-of select="@reftitle"/></fo:block>
      </xsl:when>
      <xsl:otherwise><!--nop--></xsl:otherwise>
    </xsl:choose>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="q" dtm:status="finished" dtm:idref="topic.q"/>
<xsl:template match="*[contains(@class,' topic/q ')]" dtm:id="topic.q">
  <fo:inline border-left-width="0pt" border-right-width="0pt">
    &quotedblleft;<xsl:apply-templates />&quotedblright;
  </fo:inline>
</xsl:template>




<!-- figure setup -->
<dtm:doc dtm:elements="fig|imagemap" dtm:status="finished" dtm:idref="topic.fig divlikepara"/>
<xsl:template match="*[contains(@class,' topic/fig ')]" dtm:id="topic.fig">
  <fo:block xsl:use-attribute-sets="fig">
    <!-- setclass -->
    <!-- set id -->
    <xsl:call-template name="setframe"/>
 <xsl:if test="@expanse = 'page'">
  <xsl:attribute name="start-indent">-<xsl:value-of select="$basic-start-indent"/></xsl:attribute>
 </xsl:if>
    <!-- this is where the main fig rendering happens -->
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="figgroup|area" dtm:status="finished" dtm:idref="topic.figgroup"/>
<xsl:template match="*[contains(@class,' topic/figgroup ')]" dtm:id="topic.figgroup">
  <fo:inline border-left-width="0pt" border-right-width="0pt">
    <!-- setclass -->
    <!-- set id -->
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<!-- record end respecting data -->

<dtm:doc dtm:elements="pre" dtm:status="finished" dtm:idref="topic.pre divlikepara"/>
<xsl:template match="*[contains(@class,' topic/pre ')]" dtm:id="topic.pre">
  <xsl:call-template name="gen-att-label"/>
  <fo:block xsl:use-attribute-sets="pre">
    <!-- setclass -->
    <!-- set id -->
    <xsl:call-template name="setscale"/>
    <xsl:call-template name="setframe"/>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="lines" dtm:status="finished" dtm:idref="topic.lines divlikepara"/>
<xsl:template match="*[contains(@class,' topic/lines ')]" dtm:id="topic.lines">
  <xsl:call-template name="gen-att-label"/>
  <fo:block xsl:use-attribute-sets="lines">
     <!-- setclass -->
     <!-- set id -->
     <xsl:call-template name="setscale"/>
    <xsl:call-template name="setframe"/>
     <xsl:apply-templates/>
  </fo:block>
</xsl:template>



<!-- phrase elements -->
<dtm:doc dtm:elements="term" dtm:status="finished" dtm:idref="topic.term"/>
<xsl:template match="*[contains(@class,' topic/term ')]" dtm:id="topic.term">
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-style="italic">
    <!-- setclass -->
    <!-- set id -->
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="ph|coords|cmd|summary|completed|day|
                       year|month|started|revisionid|addressdetails|
                       administrativearea|country|locality|thoroughfare|
                       booktitlealt|organizationnamedetails|organizationname|
                       localityname|postalcode" dtm:status="finished" dtm:idref="topic.ph"/>
<xsl:template match="*[contains(@class,' topic/ph ')]" dtm:id="topic.ph">
  <fo:inline border-left-width="0pt" border-right-width="0pt" color="purple">
    <!-- setclass -->

    <!-- set id -->
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>


<dtm:doc dtm:elements="tm" dtm:status="finished" dtm:idref="topic.tm"/>
<xsl:template match="*[contains(@class,' topic/tm ')]" dtm:id="topic.tm">
  <fo:inline border-left-width="0pt" border-right-width="0pt">
    <!-- setclass -->
    <!-- set id -->
    <xsl:apply-templates/>
    <fo:inline baseline-shift="super" font-size="75%">
    <xsl:choose>
      <xsl:when test="@tmtype='tm'">(TM)</xsl:when>
      <xsl:when test="@tmtype='reg'">(R)</xsl:when>
      <xsl:when test="@tmtype='service'">(SM)</xsl:when>
      <xsl:otherwise>
        <xsl:text>Error in tm type.</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
    </fo:inline>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="boolean" dtm:status="finished" dtm:idref="topic.boolean"/>
<xsl:template match="*[contains(@class,' topic/boolean ')]" dtm:id="topic.boolean">
  <fo:inline border-left-width="0pt" border-right-width="0pt" color="green">
    <!-- setclass -->
    <!-- set id -->
    <xsl:value-of select="name()"/><xsl:text>: </xsl:text><xsl:value-of select="@state"/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="state" dtm:status="finished" dtm:idref="topic.state"/>
<xsl:template match="*[contains(@class,' topic/state ')]" dtm:id="topic.state">
  <fo:inline border-left-width="0pt" border-right-width="0pt" color="red">
    <!-- setclass -->
    <!-- set id -->
    <xsl:value-of select="name()"/><xsl:text>: </xsl:text><xsl:value-of select="@name"/><xsl:text>=</xsl:text><xsl:value-of select="@value"/>
  </fo:inline>
</xsl:template>


<!-- image and object data -->
<dtm:doc dtm:elements="image" dtm:status="finished" dtm:idref="topic.image topic-image graphic.insert"/>
<xsl:template match="*[contains(@class,' topic/image ')]" dtm:id="topic.image">
  <xsl:choose>
    <xsl:when test="@placement='inline'">
      <fo:inline>
        <xsl:call-template name="unused-insert-graphic"/> <!-- fix this! -->
        <xsl:apply-templates/>
      </fo:inline>
    </xsl:when>
    <xsl:otherwise>
      <fo:block>
        <!-- build any pre break indicated by style -->
        <xsl:if test="not(parent::fig[contains(@frame,'top')])">
          <xsl:text>&rbl;</xsl:text>
        </xsl:if>
        <fo:block>
          <xsl:if test="@align">
            <xsl:attribute name="text-align">
              <xsl:value-of select="@align"/>
            </xsl:attribute>
          </xsl:if>
          <!--xsl:call-template name="topic-image"/-->
          <fo:block>          
             <xsl:call-template name="unused-insert-graphic"/>
             <xsl:apply-templates/>
          </fo:block>
        </fo:block>
        <!-- build any post break indicated by style -->
        <xsl:if test="not(parent::fig[contains(@frame,'bot')])">
          <xsl:text>&rbl;</xsl:text>
        </xsl:if>
        <!-- build optional echo of the image name for review -->
        <xsl:if test="$ARTLBL='yes'">
          <fo:block font-weight="bold">
            <xsl:text>[</xsl:text>
            <xsl:value-of select="$IP"/>
            <xsl:text> </xsl:text>
            <xsl:value-of select="@href"/>
            <xsl:text>]</xsl:text>
          </fo:block>
        </xsl:if>
      </fo:block>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<dtm:doc dtm:elements="alt" dtm:status="finished" dtm:idref="topic.alt"/>
<xsl:template match="*[contains(@class,' topic/alt ')]" dtm:id="topic.alt">
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-family="Courier">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<xsl:template name="unused-insert-graphic" dtm:id="graphic.insert">
  <fo:external-graphic>
   
    <!-- Resolve picture filename -->
    <xsl:variable name="thisfn">
      <xsl:choose>
        <xsl:when test="contains(@href,'.xgif')">
          <xsl:value-of select="concat($img-path, substring-before(@href,'.gif'), $dflt-ext)"/>
        </xsl:when>
        <xsl:when test="contains(@href,'.xjpg')">
          <xsl:value-of select="concat($img-path, substring-before(@href,'.jpg'), $dflt-ext)"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="@href"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
     
    <!-- Scale resolving -->
    <xsl:variable name="scale">
      <xsl:choose>
        <xsl:when test="@width or @height">1.0</xsl:when>
        <xsl:when test="@scale">
          <xsl:value-of select="@scale div 100.0"/>
        </xsl:when>
        <xsl:otherwise>1.0</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <!-- Set attributes to external graphic -->
    <xsl:attribute name="src">url(<xsl:value-of select="$thisfn"/>)</xsl:attribute>
    <xsl:attribute name="content-width">
      <xsl:choose>
        <xsl:when test="@width">
          <xsl:value-of select="@width"/>
        </xsl:when>
        <xsl:when test="number($scale) != 1.0">
          <xsl:value-of select="$scale * 100"/>
          <xsl:text>%</xsl:text>
        </xsl:when>
        <xsl:otherwise>auto</xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
    <xsl:attribute name="content-height">
      <xsl:choose>
        <xsl:when test="@height">
          <xsl:value-of select="@height"/>
        </xsl:when>
        <xsl:when test="number($scale) != 1.0">
          <xsl:value-of select="$scale * 100"/>
          <xsl:text>%</xsl:text>
        </xsl:when>
        <xsl:otherwise>auto</xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
    <xsl:if test="@align">
      <xsl:attribute name="text-align">
        <xsl:value-of select="@align"/>
      </xsl:attribute>
    </xsl:if>
  </fo:external-graphic>
</xsl:template>

<!-- this is unused for now, but is browser specific; must be converted! -->
<xsl:template name="topic-image" dtm:id="topic-image">
  <!-- now invoke the actual content and its alt text -->
  <xsl:element name="img">
    <!-- setclass -->
    <xsl:attribute name="src">
     <xsl:value-of select="$IP"/>
       <xsl:choose>
         <xsl:when test="@objname">
           <!--xsl:call-template name="get-objdescinfo"/-->
         </xsl:when>
         <xsl:when test="@href">
           <xsl:value-of select="@href"/>
         </xsl:when>
         <xsl:otherwise>
           <!-- no action -->
         </xsl:otherwise>
       </xsl:choose>
     </xsl:attribute>
    <xsl:if test="@height"><xsl:attribute name="height"><xsl:value-of select="@height"/></xsl:attribute></xsl:if>
    <xsl:if test="@width"><xsl:attribute name="width"><xsl:value-of select="@width"/></xsl:attribute></xsl:if>
    <xsl:choose>
      <xsl:when test="@alt">
        <xsl:attribute name="alt"><xsl:value-of select="@alt"/></xsl:attribute>
      </xsl:when>
      <xsl:otherwise>
        <xsl:attribute name="alt"><xsl:value-of select="*[contains(@class,' topic/textalt')]"/></xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:element>
</xsl:template>

<dtm:doc dtm:elements="object" dtm:status="finished" dtm:idref="topic.object"/>
<xsl:template match="*[contains(@class,' topic/object ')]" dtm:id="topic.object">
<fo:block>
  <xsl:apply-templates/>
  <!-- copy through for browsers; unused for FO -->
</fo:block>
</xsl:template>

<dtm:doc dtm:elements="param" dtm:status="finished" dtm:idref="topic.param"/>
<xsl:template match="*[contains(@class,' topic/param ')]" dtm:id="topic.param">
<fo:block>
  <xsl:value-of select="@name"/>
  <xsl:text>: </xsl:text>
  <xsl:value-of select="@value"/>
  <!-- copy through for browsers; unused for FO -->
</fo:block>
</xsl:template>




<!-- content usually rendered out of sequence -->

<!-- by adding the [$DRAFT='yes'] predicate to this rule, you can cause
     the content to disappear in the ordinary case. When parameterization
     becomes possible, consider adding yes/no views so that content is not
     "lost" by being invisible!!!! -->

<!--DRD: this template needs to be parametrically controlled by user for visibility -->
<dtm:doc dtm:elements="draft-comment" dtm:status="finished" dtm:idref="topic.draft.comment"/>
<xsl:template match="*[contains(@class,' topic/draft-comment ')]" dtm:id="topic.draft.comment">
  <xsl:if test="$DRAFT='yes'">
    <fo:block background-color="#FF99FF" color="#CC3333">
    <xsl:attribute name="border-style">solid</xsl:attribute>
    <xsl:attribute name="border-color">black</xsl:attribute>
    <xsl:attribute name="border-width">thin</xsl:attribute>
      <fo:block font-weight="bold">
Disposition: <xsl:value-of select="@disposition"/> / 
Status: <xsl:value-of select="@status"/>
      </fo:block> 
      <xsl:apply-templates/>
    </fo:block>
  </xsl:if>
</xsl:template>


<!--DRD: this template needs to be parametrically controlled by user for visibility -->
<dtm:doc dtm:elements="required-cleanup" dtm:status="finished" dtm:idref="topic.required.cleanup"/>
<xsl:template match="*[contains(@class,' topic/required-cleanup ')]" dtm:id="topic.required.cleanup">
  <xsl:variable name="DRAFT">yes</xsl:variable>
  <xsl:if test="$DRAFT='yes'">
    <fo:inline background="yellow" color="#CC3333">  <!-- indents won't apply here; not a block context -->
     <xsl:attribute name="border-style">solid</xsl:attribute>
    <xsl:attribute name="border-color">black</xsl:attribute>
    <xsl:attribute name="border-width">thin</xsl:attribute>
     <!-- set id -->
      <fo:inline font-weight="bold">Required Cleanup <xsl:if test="string(@remap)">(<xsl:value-of select="@remap"/>) </xsl:if><xsl:text>: </xsl:text></fo:inline> 
      <xsl:apply-templates />
    </fo:inline>
  </xsl:if>
</xsl:template>

<dtm:doc dtm:elements="fn" dtm:status="finished" dtm:idref="topic.fn"/>
<xsl:template match="*[contains(@class,' topic/fn ')]" dtm:id="topic.fn">
    <fo:block font-size="8pt" color="purple">
      <xsl:if test="@id">
        <fo:inline font-style="italic">
          <xsl:text>[Footnote: </xsl:text>
          <xsl:value-of select="@id"/>
          <xsl:text>]</xsl:text>
        </fo:inline>
      </xsl:if>
      <xsl:if test="@callout">
        <fo:inline baseline-shift="super" font-size="75%">
           <xsl:value-of select="@callout"/>
           <!--xsl:number level="multiple" count="//fn"  format="1 "/-->
        </fo:inline>
      </xsl:if>
      <xsl:apply-templates/>
    </fo:block>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="footnotex"/>
<xsl:template match="footnotex" dtm:id="footnotex">
<fo:footnote>
  <fo:inline baseline-shift="super" font-size="7pt" font-family="Helvetica">
    <xsl:number count="//fn"    format="1" />
  </fo:inline>
  <fo:footnote-body>
    <!--fo:block space-after="0pt">
      <fo:leader leader-pattern="rule" leader-length="1in" rule-thickness=".5pt" color="black"/>
      <fo:leader leader-pattern="rule" leader-length="30%" rule-thickness="1pt" rule-style="solid"/> 
    </fo:block-->
    <fo:block xsl:use-attribute-sets="footnote">
      <fo:inline baseline-shift="super" font-size="75%">
         <xsl:number level="multiple" count="//fn"  format="1 "/>
      </fo:inline>
      <xsl:apply-templates />
    </fo:block>
  </fo:footnote-body>
</fo:footnote>
</xsl:template>



<!-- other special data -->

<!-- this rule is prolog-specific; can move it into dita-prolog.xsl if desired -->
<dtm:doc dtm:elements="keywords/keyword" dtm:status="finished" dtm:idref="topic.keyword.secondlevel"/>
<xsl:template match="*[contains(@class,' topic/keywords ')]/*[contains(@class,' topic/keyword ')]" priority="2" dtm:id="topic.keyword.secondlevel">
  <fo:inline>
    <xsl:text> [</xsl:text><xsl:apply-templates /><xsl:text>] </xsl:text>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="keyword|shape" dtm:status="finished" dtm:idref="topic.keyword"/>
<xsl:template match="*[contains(@class,' topic/keyword ')]" dtm:id="topic.keyword">
  <fo:inline background-color="#dddddd" border-left-width="0pt" 
             border-right-width="0pt">
    <!-- setclass -->
    <!-- set id -->
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="cite" dtm:status="finished" dtm:idref="topic.cite"/>
<xsl:template match="*[contains(@class,' topic/cite ')]" dtm:id="topic.cite">
  <fo:inline font-style="italic">
    <!-- setclass -->
    <!-- set id -->
    <xsl:apply-templates />
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="indextermref" dtm:status="finished" dtm:idref="topic.indextermref"/>
<xsl:template match="*[contains(@class,' topic/indextermref ')]" dtm:id="topic.indextermref">
  <xsl:if test="$INDEXSHOW='yes'">
      <fo:inline font-style="italic">
        <xsl:text>[-> </xsl:text>
        <xsl:value-of select="@keyref"/>
        <xsl:text>]</xsl:text>
      </fo:inline>
  </xsl:if>
</xsl:template>

<dtm:doc dtm:elements="indexterm" dtm:status="finished" dtm:idref="topic.indexterm"/>
<xsl:template match="*[contains(@class,' topic/indexterm ')]" dtm:id="topic.indexterm">
  <xsl:if test="$INDEXSHOW='yes'">
  <xsl:choose>
    <xsl:when test="node()[1]">
      <fo:inline margin="1pt">
        <xsl:text>[</xsl:text>
        <xsl:apply-templates/>
        <xsl:text>]</xsl:text>
      </fo:inline>
    </xsl:when>
    <xsl:otherwise>
        <fo:inline/>
    </xsl:otherwise>
  </xsl:choose>
  </xsl:if>
</xsl:template>


<!-- =================== end of element rules ====================== -->

<!-- mathml hack -->
<dtm:doc dtm:elements="math" dtm:status="testing" dtm:idref="topic.math"/>
<xsl:template match="math|*[contains(@class, ' topic/math ')]" dtm:id="topic.math">
    <se:inline-object font-family="Times"/>
</xsl:template>

<!-- ================== new DITA -1.3.1 elements =================== -->
<dtm:doc dtm:elements="data|data-about|foreign|unknown|
                       bookpartno|edition|isbn|volume|
                       booknumber|maintainer|bookid|bookrights|
                       bookchangehistory|reviewed|edited|
                       approved|bookevent|bookrights|
                       bookowner|organization|person|
                       bookrestriction|printlocation|
                       published|publishtype|organizationinfo|
                       contactnumbers|contactnumber|emailaddresses|
                       emailaddress|urls|url|bookeventtype|copyrfirst|
                       copyrlast|personname|personinfo|namedetails|
                       firstname|middlename|lastname|otherinfo|
                       honorific|generationidentifier|tested" dtm:status="finished" dtm:idref="topic.inlines"/>
<xsl:template match="*[contains(@class, ' topic/data ')]|
                     *[contains(@class, ' topic/foreign ')]|
                     *[contains(@class, ' topic/data-about ')]|
                     *[contains(@class, ' topic/unknown ')]" dtm:id="topic.inlines">
  <xsl:if test="$SHOWPROLOG='yes'">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:if>
</xsl:template> 

<!-- ======================= new DITA -1.3.1 indexing elements ============== --> 
<dtm:doc dtm:elements="index-base" dtm:status="finished" dtm:idref="topic.index-base"/>
<xsl:template match="*[contains(@class, ' topic/index-base ')]" dtm:id="topic.index-base">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="index-see" dtm:status="finished" dtm:idref="indexing-d.index-see"/>
<xsl:template match="*[contains(@class, ' indexing-d/index-see ')]" dtm:id="indexing-d.index-see">
  <xsl:call-template name="indexing">
    <xsl:with-param name="indexname">
      <xsl:text>See</xsl:text>
    </xsl:with-param>
  </xsl:call-template>
</xsl:template>

<dtm:doc dtm:elements="index-see-also" dtm:status="finished" dtm:idref="indexing-d.index-see-also"/>
<xsl:template match="*[contains(@class, ' indexing-d/index-see-also ')]" dtm:id="indexing-d.index-see-also">
  <xsl:call-template name="indexing">
    <xsl:with-param name="indexname">
      <xsl:text>See also</xsl:text>
    </xsl:with-param>
  </xsl:call-template>
</xsl:template>

<dtm:doc dtm:elements="index-sort-as" dtm:status="finished" dtm:idref="indexing-d.index-sort-as"/>
<xsl:template match="*[contains(@class, ' indexing-d/index-sort-as ')]" dtm:id="indexing-d.index-sort-as">
  <xsl:call-template name="indexing">
    <xsl:with-param name="indexname">
      <xsl:text>Sort as </xsl:text>
    </xsl:with-param>
  </xsl:call-template>
</xsl:template>

<xsl:template name="indexing">
  <xsl:param name="indexname" select =""/>
  <fo:block>
    <xsl:if test="preceding-sibling::*">
      <xsl:text> </xsl:text>
    </xsl:if>
    <fo:inline font-style="italic" font-weight="bold">
      <xsl:value-of select="$indexname"/>
      <xsl:text>: </xsl:text>
    </fo:inline>
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </fo:block>
  <xsl:choose>
    <xsl:when test="following-sibling::*">
      <xsl:text>, </xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>. </xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
