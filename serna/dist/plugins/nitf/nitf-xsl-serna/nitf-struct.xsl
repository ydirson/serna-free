<?xml version='1.0' encoding='utf-8'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format" version="1.0">
  <xsl:output method="xml" version="1.0" encoding="utf-8" indent="no"/>

  <xsl:template match="/nitf" priority="1" mode="root.mode">
    <fo:page-sequence master-reference="body">
      <fo:flow flow-name="xsl-region-body" 
        font-size="{$body.font.size}" 
        font-family="{$body.font.family}">
        <fo:block xsl:use-attribute-sets="body">
          <xsl:call-template name="process-common-attributes"/>
          <xsl:choose>
            <xsl:when test="not(*[not(self::processing-instruction('se:choice'))])">
              <xsl:apply-templates/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:if test="head[not(self::processing-instruction('se:choice'))] and not(head/node())">
                <fo:block color="gray" font-size="{$small.font.size}" font-style="italic">to edit "nitf/head" metadata - scroll to &quot;Document Metadata Page&quot;</fo:block>
              </xsl:if>
              <xsl:apply-templates select="*[not(self::head)]"/>
            </xsl:otherwise>
          </xsl:choose>
        </fo:block>
      </fo:flow>
    </fo:page-sequence>
    <fo:page-sequence master-reference="body">
      <fo:flow 
        flow-name="xsl-region-body" 
        font-size="{$body.font.size}" 
        font-family="{$body.font.family}">
        <fo:block border-left-width="0pt" border-right-width="0pt">
          <fo:block font-size="{$huge.font.size}" text-align="center">
            <xsl:text>Document Metadata Page</xsl:text>
          </fo:block>
          <xsl:apply-templates select="head"/>
        </fo:block>
      </fo:flow>
    </fo:page-sequence>
  </xsl:template>
  
  <xsl:template match="nitf|head|body|body.head|body.content|body.end|                        copyrite|iim">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>
  
</xsl:stylesheet>

