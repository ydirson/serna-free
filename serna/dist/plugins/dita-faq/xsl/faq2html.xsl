<?xml version="1.0" encoding="UTF-8" ?>

<xsl:stylesheet version="1.0"
     xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:variable name="FAQStringFile" select="document('i18n/faq_strings.xml')"/>

<xsl:template match="*[contains(@class,' faq/faqgroup ')]">
  <div>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqlist ')]">
  <div>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqitem ')]">
  <div>
  <a>
     <xsl:attribute name="name"><xsl:value-of select="generate-id()"/></xsl:attribute>
     <xsl:text> </xsl:text>
  </a>
  <table style="display: block; margin-top: 12pt; margin-left: 32pt;">
    <xsl:apply-templates select="*[contains(@class,' faq/faqquest ')]"/>
    <xsl:apply-templates select="*[contains(@class,' faq/faqans ')]"/>
    <xsl:if test="*[contains(@class,' faq/faqprop ')]">
      <tr>
	<td></td>
	<td align="right" valign="top">
	  <span style="font-style: italic;">
	    <xsl:apply-templates select="*[contains(@class,' faq/faqprop ')]"/>
	  </span>
	</td>
      </tr>
    </xsl:if>
  </table>
  </div>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqquest ')]">
  <tr>
    <td align="right" valign="top">
      <xsl:call-template name="faqquest-label"/>
    </td>
    <td valign="top">
      <span style="font-style: italic;">
	<xsl:apply-templates/>
      </span>
    </td>
  </tr>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqans ')]">
  <tr>
    <td align="right" valign="top">
      <xsl:call-template name="faqans-label"/>
    </td>
    <td valign="top">
      <xsl:apply-templates/>
    </td>
  </tr>
</xsl:template>

<xsl:template name="faqquest-label">
  <span style="font-style: italic; font-weight: bold;">
    <xsl:call-template name="getString">
      <xsl:with-param name="stringName" select="'Question'"/>
      <xsl:with-param name="stringFile" select="$FAQStringFile"/>
    </xsl:call-template>
    <xsl:text>:</xsl:text>
  </span>
</xsl:template>

<xsl:template name="faqans-label">
  <span style="font-style: italic; font-weight: bold;">
    <xsl:call-template name="getString">
      <xsl:with-param name="stringName" select="'Answer'"/>
      <xsl:with-param name="stringFile" select="$FAQStringFile"/>
    </xsl:call-template>
    <xsl:text>:</xsl:text>
  </span>
</xsl:template>


<!-- A specialized rule can customize the processing of a base element
     within its content.  This rule provides special behavior for keyword
     elements in the context of a faqlist. -->

<xsl:template match="*[contains(@class,' faq/faqlist ')] // 
    *[contains(@class,' topic/keyword ')]">
  <span style="font-style: italic;">
    <xsl:apply-templates/>
  </span>
</xsl:template>

</xsl:stylesheet>
