<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:exsl="http://exslt.org/common"
                extension-element-prefixes="exsl"
                version='1.1'>

<xsl:output method="xml"
            encoding="ISO-8859-1"
            indent="yes"/>

<xsl:include href="../dbhtml-utils.xsl"/>
<xsl:param name="base-dir" select="."/>

<xsl:template name="output-file">
  <xsl:param name="href"/>
  <xsl:call-template name="basename">
    <xsl:with-param name="filename" select="$href"/>
  </xsl:call-template>
  <xsl:text>.html</xsl:text>
</xsl:template>

<xsl:template match="*">
  <xsl:param name="href"/>
  <xsl:param name="referred"/>
  <xsl:param name="subdir"/>
  <xsl:apply-templates>
    <xsl:with-param name="href" select="$href"/>
    <xsl:with-param name="referred" select="$referred"/>
    <xsl:with-param name="subdir" select="$subdir"/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template match="*" mode="keywords">
  <xsl:param name="href"/>
  <xsl:apply-templates mode="keywords">
    <xsl:with-param name="href" select="$href"/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template match="*" mode="docbook">
  <xsl:param name="href"/>
  <xsl:apply-templates mode="docbook">
    <xsl:with-param name="href" select="$href"/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template match="text()|@*"/>
<xsl:template match="text()|@*" mode="docbook"/>
<xsl:template match="text()|@*" mode="keywords"/>
<xsl:template match="text()|@*" mode="terms2"/>
<xsl:template match="text()|@*" mode="referred"/>

<xsl:template match="*[contains(@class,' map/map ')]">

  <xsl:param name="startpage" select="'index.html'"/>
  <xsl:param name="referred"/>
  <xsl:param name="subdir"/>
  <xsl:param name="href"/>

  <xsl:choose>
    <xsl:when test="$referred">
      <xsl:attribute name="ref"><xsl:value-of select="$href"/></xsl:attribute>
      <xsl:attribute name="title"><xsl:value-of select="@title"/></xsl:attribute>
      <xsl:if test="@id">
        <xsl:attribute name="tag"><xsl:value-of select="@id"/></xsl:attribute>
      </xsl:if>
      <!--keyword ref="{$href}"><xsl:value-of select="title"/></keyword-->
        <xsl:apply-templates>
          <xsl:with-param name="subdir" select="$subdir"/>
          <xsl:with-param name="href" select="$href"/>
        </xsl:apply-templates>
    </xsl:when>
    <xsl:otherwise>
      <assistantconfig version="3.3.0">

          <profile>
              <property name="name">Serna_Help</property>
              <property name="title"><xsl:value-of select="@title"/></property>
              <property name="applicationicon">assistant.png</property>
              <property name="startpage">
                <xsl:value-of select="$startpage"/>
              </property>
              <property name="aboutmenutext">About Help</property>
              <property name="abouturl">abouthelp/index.html</property>
              <!--
              <property name="assistantdocs">../../../doc/html</property>
              -->
          </profile>

          <DCF ref="{$startpage}" title="{@title}">
            <xsl:apply-templates/>
          </DCF>
        </assistantconfig>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="*[contains(@class, ' topic/topic ')]">
  <xsl:param name="href"/>
  <xsl:param name="referred"/>
  <xsl:param name="subdir"/>

  <xsl:variable name="anchor">
    <xsl:value-of select="$href"/>
    <xsl:if test="@id"><xsl:value-of select="concat('#', @id)"/></xsl:if>
  </xsl:variable>

  <xsl:choose>
    <xsl:when test="$referred=1">
      <xsl:attribute name="ref"><xsl:value-of select="$anchor"/></xsl:attribute>
      <xsl:attribute name="title"><xsl:value-of select="title"/></xsl:attribute>
      <xsl:attribute name="tag"><xsl:value-of select="@id"/></xsl:attribute>
      <!--keyword ref="{$anchor}"><xsl:value-of select="title"/></keyword-->
      <xsl:apply-templates>
        <xsl:with-param name="href" select="$href"/>
        <xsl:with-param name="subdir" select="$subdir"/>
      </xsl:apply-templates>
    </xsl:when>
    <xsl:otherwise>
      <section ref="{$anchor}" title="{title}" tag="{@id}">
        <!--keyword ref="{$anchor}"><xsl:value-of select="title"/></keyword-->
        <xsl:apply-templates>
          <xsl:with-param name="href" select="$href"/>
          <xsl:with-param name="subdir" select="$subdir"/>
        </xsl:apply-templates>
      </section>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="process-docbook">
  <xsl:param name="basename"/>
  <xsl:param name="subdir"/>
  <xsl:param name="href"/>
  <xsl:variable name="src-path" select="concat($basename, '.xml')"/>
  <xsl:variable name="test-dst" select="document($src-path)"/>
  <xsl:choose>
    <xsl:when test="$test-dst">
      <xsl:apply-templates select="$test-dst" mode="docbook">
        <xsl:with-param name="href" select="$href"/>
        <xsl:with-param name="referred" select="1"/>
      </xsl:apply-templates>
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="src-filename">
        <xsl:call-template name="basename">
          <xsl:with-param name="filename" select="$href"/>
        </xsl:call-template>
        <xsl:text>.xml</xsl:text>
      </xsl:variable>
      <xsl:apply-templates select="document($src-filename, /map)"
                           mode="docbook">
        <xsl:with-param name="href" select="$href"/>
        <xsl:with-param name="referred" select="1"/>
      </xsl:apply-templates>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:apply-templates>
    <xsl:with-param name="href" select="$href"/>
    <xsl:with-param name="subdir" select="$subdir"/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template match="topicref[not(contains(@href, '#'))]">
  <xsl:param name="subdir"/>
  <xsl:param name="referred"/>
  <xsl:variable name="href">
    <xsl:if test="$subdir">
      <xsl:value-of select="concat($subdir, '/')"/>
    </xsl:if>
    <xsl:call-template name="output-file">
      <xsl:with-param name="href" select="@href"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="basename">
    <xsl:value-of select="concat(translate($base-dir, '\\', '/'), '/')"/>
    <xsl:call-template name="basename">
      <xsl:with-param name="filename" select="@href"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="@format='html'">
      <xsl:call-template name="process-docbook">
        <xsl:with-param name="basename" select="$basename"/>
        <xsl:with-param name="href" select="$href"/>
        <xsl:with-param name="subdir" select="$subdir"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="dita-subdir">
        <xsl:choose>
          <xsl:when test="@format='ditamap'">
            <xsl:call-template name="dirname">
              <xsl:with-param name="pathname" select="$href"/>
            </xsl:call-template>
          </xsl:when>
          <xsl:otherwise><xsl:value-of select="$subdir"/></xsl:otherwise>
        </xsl:choose>
      </xsl:variable>
      <section>
        <xsl:apply-templates select="document(@href)">
          <xsl:with-param name="href" select="$href"/>
          <xsl:with-param name="referred" select="1"/>
          <xsl:with-param name="subdir" select="$dita-subdir"/>
        </xsl:apply-templates>
        <xsl:apply-templates>
          <xsl:with-param name="href" select="$href"/>
          <xsl:with-param name="subdir" select="$dita-subdir"/>
        </xsl:apply-templates>
      </section>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="make-keyword">
  <xsl:param name="href"/>
  <xsl:param name="keyword"/>
  <xsl:variable name="ancestor-with-id" select="(ancestor::*[@id])[last()]"/>
  <xsl:variable name="ancestor-id" select="concat('#', $ancestor-with-id/@id)"/>
  <xsl:variable name="anchor" select="concat($href, $ancestor-id)"/>
  <keyword ref="{$anchor}"><xsl:value-of select="$keyword"/></keyword>
</xsl:template>

<xsl:template match="indexterm/keyword|indexterm/term">
  <xsl:param name="href"/>
  <xsl:call-template name="make-keyword">
    <xsl:with-param name="href" select="$href"/>
    <xsl:with-param name="keyword" select="."/>
  </xsl:call-template>
</xsl:template>

<xsl:template match="title">
  <xsl:param name="href"/>
  <xsl:variable name="ancestor-class" select="../@class"/>
  <xsl:if test="contains($ancestor-class,' map/map ') or
                contains($ancestor-class,' topic/topic ') or
                contains($ancestor-class,' topic/fig ')">
    <xsl:call-template name="make-keyword">
      <xsl:with-param name="href" select="$href"/>
      <xsl:with-param name="keyword" select="."/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template match="*[not(self::topic)]" mode="keywords">
  <xsl:param name="href"/>
  <xsl:apply-templates select="keyword" mode="keywords">
    <xsl:with-param name="href" select="$href"/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template name="make-section" mode="docbook">
  <xsl:param name="href"/>
  <xsl:param name="title"/>
  <xsl:variable name="anchor">
    <xsl:value-of select="$href"/>
    <xsl:if test="@id"><xsl:value-of select="concat('#', @id)"/></xsl:if>
  </xsl:variable>
  <section ref="{$anchor}" title="{$title}" tag="{@id}">
    <keyword ref="{$anchor}"><xsl:value-of select="$title"/></keyword>

    <xsl:apply-templates mode="terms">
      <xsl:with-param name="anchor" select="$anchor"/>
    </xsl:apply-templates>

    <xsl:apply-templates mode="docbook">
      <xsl:with-param name="href" select="$href"/>
    </xsl:apply-templates>
  </section>
</xsl:template>

<xsl:template match="book|article|chapter|reference|part" mode="docbook">
  <xsl:param name="href"/>
  <xsl:param name="referred"/>
  <xsl:call-template name="make-section" mode="docbook">
    <xsl:with-param name="href" select="$href"/>
    <xsl:with-param name="title" select="title|bookinfo/title"/>
  </xsl:call-template>
</xsl:template>

<xsl:template match="section|figure[@id]|refentry[@id]" mode="docbook">
  <xsl:param name="href"/>
  <xsl:call-template name="make-section" mode="docbook">
    <xsl:with-param name="href" select="$href"/>
    <xsl:with-param name="title" select="title|descendant::refname"/>
  </xsl:call-template>
</xsl:template>

<!-- Special part for generated documentation (doxygen + boostbook) -->
<xsl:template match="section[contains(title, 'API Reference')]" mode="docbook">
  <xsl:param name="href"/>
  <section ref="{$href}" title="{title}" tag="{@id}">
    <xsl:variable name="baseref">
      <xsl:value-of select="substring-before($href, 'index.html')"/>
    </xsl:variable>
    <keyword ref="{$href}">
      <xsl:value-of select="title"/>
    </keyword>
    <xsl:for-each select="section/refentry[@id]">
      <xsl:variable name="ref">
        <xsl:value-of select="concat($baseref, concat(@id, '.html'))"/>
      </xsl:variable>
      <section ref="{$ref}" title="{refnamediv/refname}" tag="{@id}">
        <keyword ref="{$ref}">
          <xsl:value-of select="refnamediv/refname"/>
        </keyword>
        <xsl:apply-templates mode="terms">
          <xsl:with-param name="anchor" select="$href"/>
        </xsl:apply-templates>
      </section>
    </xsl:for-each>
  </section>
</xsl:template>

<xsl:template match="*[not(self::section or self::chapter or self::article or
                       self::reference or self::part or self::book)]"
              mode="terms">
  <xsl:param name="anchor"/>
  <xsl:apply-templates select="glossterm" mode="terms2">
    <xsl:with-param name="anchor" select="$anchor"/>
  </xsl:apply-templates>
  <xsl:apply-templates select="firstterm" mode="terms2">
    <xsl:with-param name="anchor" select="$anchor"/>
  </xsl:apply-templates>
  <xsl:apply-templates select="*[not(self::firstterm or self::glossterm)]"
                       mode="terms">
    <xsl:with-param name="anchor" select="$anchor"/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template match="firstterm|glossterm" mode="terms2">
  <xsl:param name="anchor"/>
  <keyword>
    <xsl:attribute name="ref">
      <xsl:value-of select="$anchor"/>
    </xsl:attribute>
    <xsl:value-of select="."/>
  </keyword>
</xsl:template>

<xsl:template
  match="section|chapter|article|reference|part|book|text()" mode="terms"/>

</xsl:stylesheet>
