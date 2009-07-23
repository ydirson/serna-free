<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

<xsl:output method="xml"
            encoding="ISO-8859-1"
            indent="yes"/>

<xsl:include href="dbhtml-utils.xsl"/>

<xsl:key name="id" match="*" use="@id"/>

<xsl:template match="/">
    <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text()|@*"/>

<xsl:template match="/set">
    <xsl:variable name="startpage">
      <xsl:call-template name="dbhtml-href">
        <!--xsl:with-param name="pis" select="processing-instruction('dbhtml')"/-->
      </xsl:call-template>
      <!--xsl:call-template name="dbhtml-filename">
        <xsl:with-param name="pis" select="processing-instruction('dbhtml')"/>
      </xsl:call-template-->
    </xsl:variable>
    
<assistantconfig version="3.2.0">
    
    <profile>
        <property name="name">Serna_Help</property>
        <property name="title">Syntext Serna Help</property>
        <property name="applicationicon">serna.png</property>
        <property name="startpage"><xsl:value-of select="$startpage"/></property>
        <property name="aboutmenutext">About Help</property>
        <property name="abouturl">abouthelp/index.html</property>
<!--        <property name="assistantdocs">../../../doc/html</property> -->
    </profile>
    
    <DCF>
      <xsl:attribute name="ref">
        <xsl:value-of select="$startpage"/>
      </xsl:attribute>
      <xsl:attribute name="title">
        <xsl:value-of select="title"/>
      </xsl:attribute>
      <xsl:apply-templates/>
    </DCF>
  </assistantconfig>
</xsl:template>

<xsl:template match="book">
  <xsl:variable name="href">
    <xsl:call-template name="dbhtml-href"/>
  </xsl:variable>

  <xsl:call-template name="make-section">
    <xsl:with-param name="href" select="$href"/>
    <xsl:with-param name="title" select="title|bookinfo/title"/>
  </xsl:call-template>

</xsl:template>

<xsl:template match="article|chapter|reference|part">
  <xsl:variable name="href">
    <xsl:call-template name="dbhtml-href"/>
  </xsl:variable>
  <xsl:call-template name="make-section">
    <xsl:with-param name="href" select="$href"/>
    <xsl:with-param name="title" select="title|bookinfo/title"/>
  </xsl:call-template>
</xsl:template>

<xsl:template match="section|figure[@id]|refentry[@id]">
  <xsl:param name="href"/>
  <xsl:call-template name="make-section">
    <xsl:with-param name="href" select="$href"/>
    <xsl:with-param name="title" select="title|descendant::refname"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="make-section">
  <xsl:param name="href"/>
  <xsl:param name="title"/>
  <xsl:variable name="anchor">
    <xsl:value-of select="$href"/>
    <xsl:if test="@id">
      <xsl:text>#</xsl:text>
      <xsl:value-of select="@id"/>
    </xsl:if>
  </xsl:variable>
  <section>
    <xsl:attribute name="ref">
      <xsl:value-of select="$anchor"/>
    </xsl:attribute>
    <xsl:attribute name="title">
      <xsl:value-of select="$title"/>
    </xsl:attribute>
    <keyword>
      <xsl:attribute name="ref">
        <xsl:value-of select="$anchor"/>
      </xsl:attribute>
      <xsl:value-of select="$title"/>
    </keyword>

    <xsl:apply-templates mode="terms">
      <xsl:with-param name="anchor" select="$anchor"/>
    </xsl:apply-templates>

    <xsl:apply-templates>
      <xsl:with-param name="href" select="$href"/>
    </xsl:apply-templates>
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
  <xsl:apply-templates select="*[not(self::firstterm or self::glossterm)]" mode="terms">
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

<xsl:template match="section|chapter|article|reference|part|book|text()" mode="terms">
</xsl:template>

<!-- Special part for generated documentation (doxygen + boostbook) -->
<xsl:template match="book/section[contains(title, 'API Reference')]">
  <xsl:param name="href"/>
  <xsl:for-each select="section/refentry">
    <xsl:variable name="ref">
      <xsl:value-of select="concat(substring-before($href, 'index.html'), concat(@id, '.html'))"/>
    </xsl:variable>
    <section>
      <xsl:attribute name="ref">
        <xsl:value-of select="$ref"/>
      </xsl:attribute>
      <xsl:attribute name="title">
        <xsl:value-of select="refnamediv/refname"/>
      </xsl:attribute>
      <keyword>
        <xsl:attribute name="ref">
          <xsl:value-of select="$ref"/>
        </xsl:attribute>
        <xsl:value-of select="refnamediv/refname"/>
      </keyword>
    </section>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
