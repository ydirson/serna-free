<?xml version='1.0'?>

<!DOCTYPE xsl:transform [
<!-- entities for use in the generated output (must produce correctly in FO) -->
  <!ENTITY bullet        "&#x2022;"><!--check these two for better assignments -->
]>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:se="http://www.syntext.com/XSL/Format-1.0"
                xmlns:xse="http://www.syntext.com/Extensions/XSLT-1.0"
                xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
                extension-element-prefixes="dtm"
                version='1.0'>

<!--==== related-links content subset ====-->
<dtm:doc dtm:elements="topic/related-links" dtm:status="finished" dtm:idref="related-links"/>
<xsl:template match="*[contains(@class,' topic/related-links ')]" dtm:id="related-links">
  <fo:block background-color="#f0f0d0">
    <xsl:attribute name="border-style">solid</xsl:attribute>
    <xsl:attribute name="border-color">black</xsl:attribute>
    <xsl:attribute name="border-width">thin</xsl:attribute>
    <xsl:attribute name="start-indent"><xsl:value-of select="$basic-start-indent"/></xsl:attribute>
    <xsl:apply-templates select="*[not(self::processing-instruction('docato-extra-info-title'))]"/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="linklist/title" dtm:status="finished" dtm:idref="linklist.titles"/>
<xsl:template match="*[contains(@class,' topic/linklist ')]/*[contains(@class,' topic/title ')]" dtm:id="linklist.titles">
  <fo:block font-weight="bold">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="linklist" dtm:status="finished" dtm:idref="linklist"/>
<xsl:template match="*[contains(@class,' topic/linklist ')]" dtm:id="linklist">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="linkpool" dtm:status="finished" dtm:idref="linkpool"/>
<xsl:template match="*[contains(@class,' topic/linkpool ')]" dtm:id="linkpool">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="linktext" dtm:status="finished" dtm:idref="linktext"/>
<xsl:template match="*[contains(@class,' topic/linktext ')]" dtm:id="linktext">
  <fo:inline>
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="linkinfo" dtm:status="finished" dtm:idref="linkinfo"/>
<xsl:template match="*[contains(@class,' topic/linkinfo ')]" dtm:id="linkinfo">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<!-- rule for when the link is still not pointing at anything -->
<dtm:doc dtm:elements="link" dtm:status="finished" dtm:idref="link.empty href2link"/>
<xsl:template match="*[contains(@class,' topic/link ')][@href = '']" dtm:id="link.empty">
  <fo:block color="red"
    start-indent="{$basic-start-indent} + {
                   count(ancestor-or-self::*[contains(@class,' topic/linklist ')]) +
                   count(ancestor-or-self::*[contains(@class,' topic/linkpool ')])}em">
    <xsl:text>&bullet; </xsl:text><xsl:apply-templates/>
  </fo:block>
</xsl:template>

<!-- if there is an href, make it look like a link and remove prompt -->
<xsl:template match="*[contains(@class,' topic/link ')][not(@href = '')]" dtm:id="href2link">
  <fo:block color="blue" text-decoration="underline"
            start-indent="{$basic-start-indent} + {
                   count(ancestor-or-self::*[contains(@class,' topic/linklist ')]) +
                   count(ancestor-or-self::*[contains(@class,' topic/linkpool ')])}em">
    <xsl:text>&bullet; </xsl:text>
    <xsl:choose>
      <xsl:when test="string(@href) and (@format='dita' or @format='DITA' 
                    or not(@format))">
        <xsl:variable name="sourcefile">
          <xsl:choose>
            <xsl:when test="contains(@href, '#')">
                <xsl:value-of select="substring-before(@href,'#')"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="@href"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:variable>
        <xsl:variable name="topicid" select="substring-after(@href,'#')"/>
        <xsl:variable name="topicdoc" select="document($sourcefile,/)" xse:document-mode="validate"/>
        <xsl:variable name="linktext" select="*[contains(@class,' topic/linktext ')]"/>

        <xsl:if test="not(linktext) or not(linktext/text())">
          <xsl:choose>
            <xsl:when test="$topicdoc">
              <xsl:choose>
                <xsl:when test="$topicid and not(''=$sourcefile)">
                  <xsl:value-of select="id($topicid, $topicdoc)//*[contains(@class, 'topic/title')]"/>
                </xsl:when>
                <xsl:when test="$topicid and ''=$sourcefile">
                  <xsl:value-of select="id($topicid)//*[contains(@class, 'topic/title')]"/>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:value-of select="$topicdoc//*[contains(@class, 'topic/title')]"/>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:when>
            <xsl:when test="not(text())">
              <xsl:value-of select="preceding-sibling::node()[1][self::processing-instruction('docato-extra-info-title')]"/>
            </xsl:when>
          </xsl:choose>
        </xsl:if>
      </xsl:when>
      <xsl:otherwise>
        <xsl:if test="not(linktext) or not(linktext/text())">
          <xsl:value-of select="@href"/>
        </xsl:if>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<!-- ===============  end of related links markup  ================= -->


<!-- ===============  xref section (these occur in body) ================= -->

<!-- if there is an href and... -->
<dtm:doc dtm:elements="xref" dtm:status="finished" dtm:idref="topic.xref xref.title title.external href.process xref.reference"/>
<xsl:template match="*[contains(@class,' topic/xref ')]" dtm:id="topic.xref">
  <fo:inline color="blue" text-decoration="underline">
    <xsl:choose>
      <xsl:when test="'external'=@scope">
        <xsl:choose>
          <xsl:when test="normalize-space() or *">
            <xsl:apply-templates/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:apply-templates select="@href"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:when test="normalize-space() or *">
        <xsl:apply-templates/>
        <xsl:if test="string(@href) and (@format='dita' or @format='DITA'
                      or @format='ditamap' or @format='DITAMAP' or not(@format)
                      or string-length(@format)=0)">
          <xsl:call-template name="href">
            <xsl:with-param name="show-title" select="false()"/>
            <xsl:with-param name="href" select="@href"/>
          </xsl:call-template>
        </xsl:if>
      </xsl:when>
      <xsl:when test="not(@format='dita' or @format='DITA' or not(@format)
                          or @format='ditamap' or string-length(@format)=0 
                          or @format='DITAMAP')">
        <xsl:value-of select="@href"/>
      </xsl:when>
      <xsl:when test="string(@href)">
        <xsl:call-template name="href">
          <xsl:with-param name="href" select="@href"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise/>
    </xsl:choose>
    <xsl:variable name="nohref">
      <xsl:choose>
        <xsl:when test="not(@href)">
          <xsl:value-of select="'[no @href]'"/>
        </xsl:when>
        <xsl:when test="string-length(@href) = 0">
          <xsl:value-of select="'[empty @href]'"/>        
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="''"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:if test="'' != $nohref">
      <fo:inline color="red" text-decoration="normal">
        <xsl:value-of select="$nohref"/>
      </fo:inline>
    </xsl:if>
  </fo:inline>
</xsl:template>

<xsl:template match="*" mode="reference.title">
  <xsl:value-of select="title"/>     
</xsl:template>

<xsl:template match="*[contains(@class, 'glossentry/glossentry ')]" 
              mode="reference.title">
  <xsl:value-of select="glossterm"/>
</xsl:template>

<xsl:template name="step-number">
  <xsl:for-each select="ancestor::*[contains(@class,' topic/li ')][1]">
    <xsl:call-template name="step-number"/>     
  </xsl:for-each>
  <xsl:variable name="list-level" 
    select="count(ancestor-or-self::*[contains(@class,' topic/ul ')] | 
                  ancestor-or-self::*[contains(@class,' topic/dl ')] |
                  ancestor-or-self::*[contains(@class,' topic/sl ')] |
                  ancestor-or-self::*[contains(@class,' topic/ol ')] )" />
  <xsl:choose>
    <xsl:when test="($list-level mod 2) = 1">
      <!--          arabic         -->
      <xsl:number format="1." />
    </xsl:when>
    <xsl:otherwise>
      <!--          alphabetic     -->
      <xsl:number format="a." />
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="*[contains(@class,' topic/ol ')]/
                     *[contains(@class,' topic/li ')]"
              mode="reference.title">
  <xsl:text>#</xsl:text>
  <xsl:call-template name="step-number"/>
</xsl:template>

<xsl:template name="xref-title" dtm:id="xref.title">
  <xsl:param name="elem"/>

  <xsl:variable name="title.text">
    <xsl:for-each select="$elem">
      <xsl:apply-templates select="." mode="reference.title"/>
    </xsl:for-each>
  </xsl:variable>

  <xsl:choose>
    <xsl:when test="string-length($title.text) > 0">
      <xsl:value-of select="$title.text"/>     
    </xsl:when>
    <xsl:otherwise>
      <xsl:choose>
        <xsl:when test="$elem[contains(@class,' topic/topic ')]">
          <fo:inline color="red" text-decoration="normal">
            <xsl:text>[no title: </xsl:text>
            <xsl:value-of select="string($elem)"/>     
            <xsl:text>]</xsl:text>
          </fo:inline>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$elem/ancestor::*[contains(@class,
                                ' topic/topic ')]/title"/>
          <xsl:text>: </xsl:text>
          <fo:inline font-style="italic">
            <xsl:value-of select="substring($elem, 0, 50)"/>
            <xsl:text>... </xsl:text>
          </fo:inline>
        </xsl:otherwise>        
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="get.topic.id">
  <xsl:param name="root-elem" select="/"/>
  <xsl:param name="id"/>
    <xsl:choose>
      <xsl:when test="string-length($id) = 0">
        <xsl:value-of select="$root-elem//*[contains(@class,
                              ' topic/topic ')][@id][1]/@id|
                              $root-elem//*[contains(@class,
                              ' map/map ')][@id][1]/@id|
                              $root-elem//*[contains(@class,
                              ' map/topicref ')][@id][1]/@id"/>
      </xsl:when>
      <xsl:when test="contains($id,'/')">
        <xsl:value-of select="substring-before($id, '/')"/>    
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$id"/>
      </xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="xref-reference.local">
  <xsl:param name="root-elem" select="/"/>
  <xsl:param name="id"/>
  <xsl:param name="show-title" select="true()"/>
  <xsl:param name="type"/>
  <xsl:param name="conrefs-queue"/>
  <xsl:variable name="topic-id">
    <xsl:call-template name="get.topic.id">
      <xsl:with-param name="id" select="$id"/>
      <xsl:with-param name="root-elem" select="$root-elem"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="topic" select="id($topic-id)"/>
  <xsl:call-template name="xref-reference.do">
    <xsl:with-param name="id" select="$id"/>
    <xsl:with-param name="root-elem" select="$root-elem"/>
    <xsl:with-param name="show-title" select="$show-title"/>
    <xsl:with-param name="topic" select="$topic"/>
    <xsl:with-param name="topic-id" select="$topic-id"/>
    <xsl:with-param name="type" select="$type"/>
    <xsl:with-param name="conrefs-queue" select="$conrefs-queue"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref-reference.external">
  <xsl:param name="root-elem" select="/"/>
  <xsl:param name="type"/>
  <xsl:param name="id"/>
  <xsl:param name="conrefs-queue"/>
  <xsl:param name="show-title" select="true()"/>
  <xsl:variable name="topic-id">
    <xsl:call-template name="get.topic.id">
      <xsl:with-param name="id" select="$id"/>
      <xsl:with-param name="root-elem" select="$root-elem"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="topic" select="$root-elem//*[contains(@class,
                                                   ' topic/topic ')][@id = $topic-id][1]|
                                     $root-elem//*[contains(@class,
                                                   ' map/map ')][@id = $topic-id][1]|
                                     $root-elem//*[contains(@class,
                                                   ' map/topicref ')][@id = $topic-id][1]"/>
  <xsl:call-template name="xref-reference.do">
    <xsl:with-param name="id" select="$id"/>
    <xsl:with-param name="root-elem" select="$root-elem"/>
    <xsl:with-param name="show-title" select="$show-title"/>
    <xsl:with-param name="topic" select="$topic"/>
    <xsl:with-param name="topic-id" select="$topic-id"/>
    <xsl:with-param name="type" select="$type"/>
    <xsl:with-param name="conrefs-queue" select="$conrefs-queue"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="xref-reference.do" dtm:id="xref.reference">
  <xsl:param name="root-elem" select="/"/>
  <xsl:param name="id"/>
  <xsl:param name="type"/>
  <xsl:param name="show-title" select="true()"/>
  <xsl:param name="topic"/>
  <xsl:param name="topic-id"/>
  <xsl:param name="conrefs-queue"/>
  <xsl:variable name="element-id">
    <xsl:if test="contains($id,'/')">
      <xsl:value-of select="substring-after($id, '/')"/>
    </xsl:if>
  </xsl:variable>
  <xsl:variable name="elem" select="id($id, $topic)"/>
  <xsl:choose>
    <xsl:when test="'href' = $type">
      <xsl:choose>
        <xsl:when test="$topic">
          <xsl:choose>
            <xsl:when test="string-length($element-id) > 0">
              <xsl:choose>
                <xsl:when test="$elem">
                  <xsl:if test="$show-title">
                    <xsl:call-template name="xref-title">
                      <xsl:with-param name="elem" select="$elem"/>
                    </xsl:call-template>
                  </xsl:if>
                </xsl:when>
                <!-- if no element found -->
                <xsl:otherwise>
                  <fo:inline color="red" text-decoration="normal">
                    <xsl:text>[Reference to unknown ID: </xsl:text>
                    <fo:inline font-weight="bold">
                        <xsl:value-of select="$id"/>
                    </fo:inline>
                    <xsl:text>]</xsl:text>
                  </fo:inline>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:when>
            <!-- if no element-id defined -->
            <xsl:otherwise>
              <xsl:if test="$show-title">
                <xsl:call-template name="xref-title">
                  <xsl:with-param name="elem" select="$topic"/>
                </xsl:call-template>
              </xsl:if>
             </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:when test="$root-elem//*[contains(@class, ' map/map ')]">
          <xsl:variable name="title" select="$root-elem//*[contains(@class,
                                             ' map/map ')]/@title"/>
          <xsl:choose>
            <xsl:when test="$title">
              <xsl:value-of select="$title"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:variable name="map-title">
                <xsl:value-of select="$root-elem//*[contains(@class,
                                      ' map/map ')]/title"/>
              </xsl:variable>
              <xsl:choose>
                <xsl:when test="string-length($map-title) > 0">
                  <xsl:value-of select="$map-title"/>
                </xsl:when>
                <xsl:otherwise>
                  <fo:inline color="red" text-decoration="normal">
                    <xsl:text>(empty title)</xsl:text>
                  </fo:inline>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <!-- if no topic found -->
        <xsl:otherwise>
          <fo:inline color="red" text-decoration="normal">
            <xsl:text>[no such topic: </xsl:text>
            <xsl:value-of select="$topic-id"/>     
            <xsl:text>]</xsl:text>
          </fo:inline>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:if test="$DBG='yes'">
        <xsl:call-template name="debug-conref">
          <xsl:with-param name="topicid" select="$topic-id"/>
          <xsl:with-param name="elemid" select="$element-id"/>
        </xsl:call-template>
      </xsl:if>
      <xsl:choose>
        <xsl:when test="string-length($element-id)>0">
          <xsl:call-template name="make.conref">
            <xsl:with-param name="elemid" select="$element-id"/>
            <xsl:with-param name="content" select="$elem"/>
            <xsl:with-param name="conrefs-queue" select="$conrefs-queue"/>
          </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="make.conref">
            <xsl:with-param name="elemid" select="$topic-id"/>
            <xsl:with-param name="content" select="$topic"/>
            <xsl:with-param name="conrefs-queue" select="$conrefs-queue"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>  
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="make.conref">
  <xsl:param name="elemid"/>
  <xsl:param name="content"/>
  <xsl:param name="conrefs-queue"/>
  <xsl:variable name="ancestor-id" select="generate-id(ancestor-or-self::*[@id = $elemid])"/>
  <xsl:variable name="referred-id" select="generate-id($content)"/>
  <xsl:choose>
    <xsl:when test="$ancestor-id = $referred-id">
      <xsl:call-template name="process-cyclic-conref"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="apply.conref">
        <xsl:with-param name="content" select="$content"/>
        <xsl:with-param name="conrefs-queue" select="$conrefs-queue"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="external-title" dtm:id="title.external">
  <xsl:param name="href"/>
  <xsl:param name="id"/>
  <xsl:param name="type"/>
  <xsl:param name="conrefs-queue"/>
  <xsl:param name="show-title" select="true()"/>
  <xsl:variable name="doc" select="document($href,/)" xse:document-mode="validate ignore-errors"/>
  <xsl:choose>
    <xsl:when test="count($doc/*)">
      <xsl:call-template name="xref-reference.external">
        <xsl:with-param name="root-elem" select="$doc"/>
        <xsl:with-param name="id" select="$id"/>
        <xsl:with-param name="show-title" select="$show-title"/>
        <xsl:with-param name="type" select="$type"/>
        <xsl:with-param name="conrefs-queue" select="$conrefs-queue"/>
      </xsl:call-template>
    </xsl:when>
    <!-- File not found -->
    <xsl:otherwise>
      <xsl:variable name="error-message">
        <xsl:text>[no file: </xsl:text>
        <xsl:value-of select="$href"/>     
        <xsl:text>]</xsl:text>
      </xsl:variable>
      <xsl:call-template name="show-conref-error">
        <xsl:with-param name="message" select="$error-message"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="href" dtm:id="href.process">
  <xsl:param name="show-title" select="true()"/>
  <xsl:param name="href"/>
  <xsl:param name="type" select="'href'"/>
  <xsl:param name="conrefs-queue"/>
  <xsl:choose>
    <xsl:when test="starts-with($href,'#')">
      <xsl:call-template name="xref-reference.local">
        <xsl:with-param name="id" select="substring-after($href,'#')"/>
        <xsl:with-param name="show-title" select="$show-title"/>
        <xsl:with-param name="type" select="$type"/>
        <xsl:with-param name="conrefs-queue" select="$conrefs-queue"/>
      </xsl:call-template>
    </xsl:when>

    <xsl:when test="contains($href,'#')">
      <xsl:call-template name="external-title">
        <xsl:with-param name="href" select="substring-before($href,'#')"/>
        <xsl:with-param name="id" select="substring-after($href,'#')"/>
        <xsl:with-param name="show-title" select="$show-title"/>
        <xsl:with-param name="type" select="$type"/>
        <xsl:with-param name="conrefs-queue" select="$conrefs-queue"/>
      </xsl:call-template>
    </xsl:when>

    <xsl:otherwise>
      <xsl:call-template name="external-title">
        <xsl:with-param name="href" select="$href"/>
        <xsl:with-param name="show-title" select="$show-title"/>
        <xsl:with-param name="type" select="$type"/>
        <xsl:with-param name="conrefs-queue" select="$conrefs-queue"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- =============== end of related links and xrefs ====================== -->

</xsl:stylesheet>
