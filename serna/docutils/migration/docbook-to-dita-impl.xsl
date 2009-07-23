<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xi="http://www.w3.org/2001/XInclude"
                xmlns:exsl="http://exslt.org/common"
                version='1.1'
                exclude-result-prefixes="xi"
                extension-element-prefixes="exsl">

<xsl:import href="docbook-to-dita-param.xsl"/>

<xsl:output method="xml"
            encoding="UTF-8"/>

<xsl:include href="../xsl/dbhtml-utils.xsl"/>
<xsl:include href="gen-topicrefs.xsl"/>
<xsl:include href="gen-element.xsl"/>

<xsl:template match="set/text()"/>
<xsl:template match="set/title"/>

<xsl:template match="/set">
<map title="{@title}">
  <xsl:message>Generating DITA content</xsl:message>
  <xsl:apply-templates/>
  <xsl:message>Generating map...</xsl:message>
  <xsl:apply-templates mode="topicrefgen"/>
</map>
</xsl:template>

<xsl:template match="xinclude">
  <xsl:if test="not(@role='docbook')">
    <xsl:variable name="output-file">
      <xsl:call-template name="output-file">
        <xsl:with-param name="href" select="@href"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:message>Writing to <xsl:value-of select="$output-file"/></xsl:message>
    <exsl:document href="{$output-file}" method="xml" encoding="utf-8"
                   doctype-public="-//OASIS//DTD DITA Topic//EN"
                   doctype-system="topic.dtd"
                   indent="yes">
      <xsl:apply-templates select="document(@href)"/>
    </exsl:document>
  </xsl:if>
</xsl:template>

<xsl:template name="output-file">
  <xsl:param name="href" select="''"/>
  <xsl:call-template name="basename">
    <xsl:with-param name="filename" select="$href"/>
  </xsl:call-template>
  <xsl:text>.dita</xsl:text>
</xsl:template>

<xsl:template name="gen-topic-id">
  <xsl:param name="prefix"/>
  <xsl:param name="generate-if-no-id"/>
  <xsl:param name="node" select="."/>
  <xsl:for-each select="$node">
    <xsl:choose>
      <xsl:when test="@id">
         <xsl:value-of select="concat($prefix, @id)"/>
      </xsl:when>
      <xsl:when test="title and not($generate-if-no-id)">
         <xsl:value-of select="concat($prefix,
           translate(normalize-space(
           translate(string(title),'(),?&quot;;','      ')),' ','-'))"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="concat($prefix, generate-id())"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
</xsl:template>

<xsl:template name="process-non-topic-source">
  <xsl:variable name="result-elem-spec">
    <xsl:call-template name="lookup.key" select="$element-map">
      <xsl:with-param name="key" select="local-name(.)"/>
      <xsl:with-param name="table" select="$element-map"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:if test="not(string($result-elem-spec))">
    <xsl:message terminate="yes">
      <xsl:text>Element '</xsl:text>
      <xsl:value-of select="local-name(.)"/>
      <xsl:text>' is unknown</xsl:text>
    </xsl:message>
  </xsl:if>
  <xsl:call-template name="gen-element">
    <xsl:with-param name="result-elem-spec" select="$result-elem-spec"/>
  </xsl:call-template>
</xsl:template>

<xsl:template match="*">
  <xsl:choose>
    <xsl:when test="local-name() = 'refentry'">
      <topic id="{@id}">
        <xsl:element name="title">
          <xsl:apply-templates select="refnamediv/refname[1]/node()"/>
        </xsl:element>
        <xsl:if test="refnamediv/refname[2]">
          <xsl:element name="titlealts">
            <xsl:element name="searchtitle">
              <xsl:for-each select="refnamediv/refname/node()">
                <xsl:value-of select="concat(., ' ')"/>
              </xsl:for-each>
            </xsl:element>
          </xsl:element>
        </xsl:if>
        <xsl:apply-templates select="refnamediv/*[local-name()!='refname']"/>
        <xsl:apply-templates select="refsection"/>
      </topic>
    </xsl:when>
    <xsl:when
      test="contains($topic-elements, concat('|', concat(local-name(.), '|')))">
      <xsl:variable name="topic-id">
        <xsl:call-template name="gen-topic-id"/>
      </xsl:variable>
      <xsl:element name="topic">
        <xsl:if test="$topic-id">
          <xsl:attribute name="id">
            <xsl:value-of select="$topic-id"/>
          </xsl:attribute>
        </xsl:if>
        <xsl:variable name="non-body-elements"
          select="*[contains($topic-non-body-elements, concat('|',
                             concat(local-name(), '|')))]"/>
        <xsl:apply-templates select="$non-body-elements"/>
        <xsl:variable name="body-elements"
          select="*[not(contains($topic-non-body-elements, concat('|',
                                 concat(local-name(), '|')))) and
                    not(contains($topic-elements, concat('|',
                                 concat(local-name(), '|')))) and
                    not(contains($topic-section-elements, concat('|',
                                 concat(local-name(), '|'))))]"/>
        <xsl:variable name="section-elements"
          select="*[contains($topic-section-elements, concat('|',
                  concat(local-name(), '|')))]"/>
        <xsl:if test="$body-elements or $section-elements">
          <body>
            <xsl:apply-templates select="$section-elements"/>
            <xsl:if test="$body-elements">
              <section>
                <xsl:apply-templates select="$body-elements"/>
              </section>
            </xsl:if>
            <xsl:apply-templates select="$body-elements" mode="example-mode"/>
            <xsl:apply-templates
              select="$non-body-elements" mode="example-mode"/>
            <xsl:apply-templates select="$section-elements" mode="example-mode"/>
          </body>
        </xsl:if>
        <xsl:apply-templates
          select="*[contains($topic-elements, concat('|',
                  concat(local-name(), '|')))]"/>
      </xsl:element>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="process-non-topic-source"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="example" mode="example-mode">
  <xsl:variable name="example-id">
    <xsl:call-template name="gen-topic-id"/>
  </xsl:variable>
  <example id="{$example-id}">
    <xsl:apply-templates/>
  </example>
</xsl:template>

<xsl:template match="@*|text()" mode="example-mode"/>

<xsl:template match="*" mode="example-mode">
    <xsl:apply-templates mode="example-mode"/>
</xsl:template>

<xsl:template match="example">
  <xsl:variable name="topic-id">
    <xsl:call-template name="gen-topic-id">
      <xsl:with-param name="node"
        select="(ancestor::*[contains($topic-elements, concat('|',
                             concat(local-name(), '|')))])[last()]"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="example-id">
    <xsl:value-of select="concat('#', $topic-id)"/>
    <xsl:text>/</xsl:text>
    <xsl:call-template name="gen-topic-id"/>
  </xsl:variable>
  <xsl:text>&#10; See example </xsl:text>
  <xref href="{$example-id}" type="section">
    <xsl:apply-templates select="title/node()"/>
  </xref>
  <xsl:text>.</xsl:text>
</xsl:template>

<xsl:template match="abstract">
  <section>
    <xsl:if test="not(title)">
      <title>Abstract</title>
    </xsl:if>
    <xsl:apply-templates/>
  </section>
</xsl:template>

<xsl:template match="emphasis">
  <xsl:variable name="element-by-role">
    <xsl:call-template name="lookup.key">
      <xsl:with-param name="key" select="@role"/>
      <xsl:with-param name="table" select="'bold b italic i underline u'"/>
      <xsl:with-param name="default" select="'b'"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:element name="{$element-by-role}">
    <xsl:apply-templates/>
  </xsl:element>
</xsl:template>

<xsl:template match="colspec|entry|note|option|
                     row|screen|title|tbody|thead|varname">
  <xsl:element name="{local-name(.)}">
    <xsl:apply-templates/>
  </xsl:element>
</xsl:template>

<xsl:template match="formalpara">
    <xsl:choose>
      <xsl:when test="title/graphic|title/inlinegraphic">
        <xsl:apply-templates select="title/node()"/>
      </xsl:when>
      <xsl:otherwise>
        <b><xsl:apply-templates select="title/node()"/></b>
      </xsl:otherwise>
    </xsl:choose>
  <p><xsl:apply-templates select="para/node()"/></p>
</xsl:template>

<xsl:template match="guibutton|guilabel|guimenu|guimenuitem|guisubmenu">
  <xsl:choose>
    <xsl:when test="parent::menuchoice">
      <uicontrol>
        <xsl:apply-templates select="node()"/>
      </uicontrol>
    </xsl:when>
    <xsl:otherwise>
      <menucascade>
        <uicontrol>
          <xsl:apply-templates select="node()"/>
        </uicontrol>
      </menucascade>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="itemizedlist">
  <xsl:variable name="p" select="para"/>
  <xsl:if test="$p">
    <xsl:apply-templates select="$p"/>
  </xsl:if>
  <ul>
    <xsl:apply-templates select="*[local-name()!='para']"/>
  </ul>
</xsl:template>

<xsl:template match="filename">
  <xsl:choose>
    <xsl:when test="local-name(..)='keyword'">
      <b><xsl:apply-templates select="node()"/></b>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates select="node()"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="bookinfo">
  <shortdesc>
    <xsl:apply-templates select="title/node()"/>
    <xsl:apply-templates select="*[local-name()!='title']"/>
  </shortdesc>
</xsl:template>

<xsl:template match="replaceable">
  <xsl:apply-templates select="node()"/>
</xsl:template>

<xsl:template match="xref">
  <xsl:variable name="href">
      <xsl:if test="not(contains($topic-elements, concat('|',
                        concat(local-name(id(@linkend)), '|'))))">
        <xsl:variable name="parent-topic-id">
          <xsl:call-template name="gen-topic-id">
            <xsl:with-param name="node"
              select="(id(@linkend)/ancestor::*[contains($topic-elements,
                                                         concat('|',
                                                         concat(local-name(),
                                                           '|')))])[last()]"/>
          </xsl:call-template>
        </xsl:variable>
        <!-- Assume that if it's not possible to get parent topic id
             then linkend points to topic element -->
        <xsl:if test="string-length($parent-topic-id)">
          <!--xsl:message>parent topic id: '<xsl:value-of select="$parent-topic-id"/>'</xsl:message-->
          <xsl:value-of select="concat($parent-topic-id, '/')"/>
        </xsl:if>
    </xsl:if>
    <xsl:value-of select="@linkend"/>
  </xsl:variable>
  <!--xsl:message>href: <xsl:value-of select="$href"/></xsl:message-->
  <xref href="#{$href}" scope="local">
    <xsl:apply-templates select="node()"/>
  </xref>
</xsl:template>

</xsl:stylesheet>
