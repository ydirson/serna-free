<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xi="http://www.w3.org/2001/XInclude"
                xmlns:exsl="http://exslt.org/common"
                version='1.1'
                exclude-result-prefixes="xi"
                extension-element-prefixes="exsl">

<xsl:import href="docbook-to-dita-param.xsl"/>

<xsl:template match="text()|@*" mode="topicrefgen"/>

<xsl:template match="xinclude" mode="topicrefgen">
  <xsl:choose>
    <xsl:when test="@role='docbook'">
      <xsl:variable name="html-file">
        <xsl:value-of
          select="concat(substring-before(@href, '.xml'), '.html')"/>
      </xsl:variable>

      <topicref href="{$html-file}" format="html"
                navtitle="{@navtitle}"
                type="topic"/>
      <!--xsl:message>topicref to href='<xsl:value-of select="@href"/>'</xsl:message-->
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="output-file">
        <xsl:call-template name="output-file">
          <xsl:with-param name="href" select="@href"/>
        </xsl:call-template>
      </xsl:variable>
      <xsl:apply-templates mode="topicrefgen" select="document(@href)">
        <xsl:with-param name="output-file" select="$output-file"/>
        <xsl:with-param name="top-level" select="true()"/>
      </xsl:apply-templates>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="*" mode="topicrefgen">
  <xsl:param name="output-file"/>
  <xsl:param name="top-level"/>
  <xsl:if test="not($output-file)">
    <xsl:message terminate="yes">Empty output file</xsl:message>
  </xsl:if>
  <xsl:choose>
    <xsl:when
      test="contains($topic-elements, concat('|', concat(local-name(.), '|')))">
      <xsl:variable name="topicref-uri">
        <xsl:value-of select="$output-file"/>
        <xsl:if test="not($top-level)">
          <xsl:call-template name="gen-topic-id">
            <xsl:with-param name="prefix" select="'#'"/>
          </xsl:call-template>
        </xsl:if>
      </xsl:variable>
      <!--xsl:message>
        <xsl:text>topicref to href='</xsl:text>
        <xsl:value-of select="$topicref-uri"/>
        <xsl:text>'</xsl:text>
      </xsl:message-->
      <topicref href="{$topicref-uri}" type="topic" collection-type="sequence"/>
      <xsl:apply-templates mode="topicrefgen">
        <xsl:with-param name="output-file" select="$output-file"/>
      </xsl:apply-templates>
      <!-- Doing hierarchical topicrefs slows down DITA OT HTML publishing
           by factor of ~40x -->
      <!--/topicref-->
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates mode="topicrefgen">
        <xsl:with-param name="output-file" select="$output-file"/>
      </xsl:apply-templates>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
