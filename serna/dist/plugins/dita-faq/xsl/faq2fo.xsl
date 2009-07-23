<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" 
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:fo="http://www.w3.org/1999/XSL/Format">


<xsl:variable name="FAQStringFile" select="document('i18n/faq_strings.xml')"/>

<xsl:template match="/">
  <fo:root
     font-family="{$body.font.family}"
     font-size="{$body.font.size}">
    <fo:layout-master-set>
      <fo:simple-page-master
         master-name="body"
         page-width="{$page.width}"
         page-height="{$page.height}"
         margin-top="{$page.margin.top}"
         margin-bottom="{$page.margin.bottom}"
         margin-left="{$page.margin.inner}"
         margin-right="{$page.margin.outer}">
        <fo:region-body margin-bottom="{$body.margin.bottom}"
			margin-top="{$body.margin.top}"
			region-name="xsl-region-body">
        </fo:region-body>
      </fo:simple-page-master>
    </fo:layout-master-set>
    <fo:page-sequence master-reference="body">
      <fo:flow flow-name="xsl-region-body">
        <xsl:apply-templates/>
      </fo:flow>
    </fo:page-sequence>
  </fo:root>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqgroup ')]">
  <fo:block padding-top="12pt"
	    start-indent="{$basic-start-indent}">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="*[contains(@class,' topic/title ')]">
  <fo:block xsl:use-attribute-sets="section.title">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqlist ')]">
  <fo:block start-indent="{$basic-start-indent}">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqitem ')]">
  <fo:block>
    <fo:list-block padding-top="12pt"
		   provisional-distance-between-starts="72pt"
		   provisional-label-separation="2pt">
      <xsl:apply-templates select="*[contains(@class,' faq/faqquest ')]"/>
      <xsl:apply-templates select="*[contains(@class,' faq/faqans ')]"/>
    </fo:list-block>
    <xsl:if test="*[contains(@class,' faq/faqprop ')]">
      <fo:block xsl:use-attribute-sets="footnote"
		text-align="end">
	<xsl:apply-templates select="*[contains(@class,' faq/faqprop ')]"/>
      </fo:block>
    </xsl:if>
  </fo:block>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqquest ')]">
  <fo:list-item padding-bottom="4pt">
    <xsl:call-template name="faqquest-label"/>
    <fo:list-item-body start-indent="body-start()"> 
      <fo:block font-style="italic">
	<xsl:apply-templates/>
      </fo:block> 
    </fo:list-item-body>
  </fo:list-item>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqans ')]">
  <fo:list-item>
    <xsl:call-template name="faqans-label"/>
    <fo:list-item-body start-indent="body-start()"> 
      <fo:block>
	<xsl:apply-templates/>
      </fo:block> 
    </fo:list-item-body> 
  </fo:list-item>
</xsl:template>

<xsl:template name="faqquest-label">
  <fo:list-item-label text-align="end"
		      end-indent="label-end()">
    <fo:block font-style="italic" font-weight="bold">
      <xsl:call-template name="getString">
	<xsl:with-param name="stringName" select="'Question'"/>
	<xsl:with-param name="stringFile" select="$FAQStringFile"/>
      </xsl:call-template>
      <xsl:text>:</xsl:text>
    </fo:block>
  </fo:list-item-label>
</xsl:template>

<xsl:template name="faqans-label">
  <fo:list-item-label text-align="end"
		      end-indent="label-end()">
    <fo:block font-style="italic" font-weight="bold">
      <xsl:call-template name="getString">
	<xsl:with-param name="stringName" select="'Answer'"/>
	<xsl:with-param name="stringFile" select="$FAQStringFile"/>
      </xsl:call-template>
      <xsl:text>:</xsl:text>
    </fo:block>
  </fo:list-item-label>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqprop ')]">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>


<!--
<xsl:template match="*[contains(@class,' faq/ownerEmail ')]">
  <fo:inline color="blue" text-decoration="underline">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/name ')]">
  <fo:inline border-left-width="0pt" border-right-width="0pt" color="purple">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>
-->


<!-- A specialized rule can customize the processing of a base element
     within its content.  This rule provides special behavior for keyword
     elements in the context of a faqlist. -->

<xsl:template match="*[contains(@class,' faq/faqlist ')] // 
    *[contains(@class,' topic/keyword ')]">
  <fo:inline font-style="italic">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

</xsl:stylesheet>
