<!-- 
     TEI XSL Stylesheet for Syntext Serna (c) 2003, Syntext Inc.

     The Stylesheet is based on Sebastian Rahtz/Oxford University 
     XSL TEI Stylesheet v1.3. See file SR-COPYING for 
     Sebastian Rahtz/Oxford University Copyright 
     information.
-->
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="p">
    <fo:block font-size="{$body.font.size}">
      <xsl:if test="preceding-sibling::p">
        <xsl:attribute name="text-indent">
          <xsl:value-of select="$parIndent"/>
        </xsl:attribute>
        <xsl:attribute name="padding-top">
          <xsl:value-of select="$parSkip"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="@xml:lang">
        <xsl:attribute name="country">
          <xsl:value-of select="substring-before(@xml:lang,'-')"/>
        </xsl:attribute>
        <xsl:attribute name="language">
          <xsl:value-of select="substring-after(@xml:lang,'-')"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>
</xsl:stylesheet>
