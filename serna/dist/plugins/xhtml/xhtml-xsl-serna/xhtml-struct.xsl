<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">
  <xsl:output method="xml"
              version="1.0"
              encoding="utf-8"
              indent="no"/>
  

  <xsl:template match="/html|/HTML" priority="1" mode="root.mode">
    <fo:page-sequence master-reference="body">
      <!-- fo:title>
        <xsl:value-of select="/html/head/title"/>
      </fo:title -->
      <fo:flow flow-name="xsl-region-body">
        <fo:block xsl:use-attribute-sets="body">
          <xsl:call-template name="process-common-attributes"/>
          <xsl:apply-templates/>
        </fo:block>
      </fo:flow>
    </fo:page-sequence>
  </xsl:template>
  
  <xsl:template match="html|head|script|body|HTML|HEAD|SCRIPT|BODY">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>
  
</xsl:stylesheet>
