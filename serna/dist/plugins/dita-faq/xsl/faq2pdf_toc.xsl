<?xml version="1.0" encoding="UTF-8" ?>

<xsl:stylesheet version="1.0" 
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:fo="http://www.w3.org/1999/XSL/Format"
		xmlns:xse="http://syntext.com/Extensions/XSLT-1.0">

<xsl:param name="SHOWTOC"
           xse:type="string" 
           xse:annotation="Hide TOC"
           select="'yes'" />

<!-- override the body template to place this toc in the expected location -->
<xsl:template match="*[contains(@class,' faq/faqbody ')]">
  <fo:block>
    <!-- here, you can generate a toc based on what's a child of body -->
    <xsl:if test="$SHOWTOC='yes'">
      <xsl:call-template name="gen-faqtoc"/>
    </xsl:if>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<!-- Table of faqlist questions (by faqgroup) -->
<xsl:template name="gen-faqtoc">
  <fo:block start-indent="{$basic-start-indent}"
	    padding-top="12pt" padding-bottom="12pt" padding="6pt"
	    background-color="#f0f0f0">
    <xsl:text>Questions in this FAQ list:</xsl:text>
    <xsl:apply-templates mode="toc"/>
  </fo:block>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqgroup ')]"
	      mode="toc">
  <fo:block>
    <fo:block padding-top="4pt"
	      xsl:use-attribute-sets="section.title">
      <xsl:apply-templates select="*[contains(@class,' topic/title ')]"
			   mode="toc"/>
    </fo:block>
    <xsl:apply-templates select="*[contains(@class,' faq/faqlist ')]"
			 mode="toc"/>
  </fo:block>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqlist ')]"
	      mode="toc">
  <fo:block>
    <xsl:choose>
      <xsl:when test="@conref">
	<xsl:apply-templates select="id(@conref)/*[contains(@class,' faq/faqitem ')]"
			     mode="toc"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:apply-templates select="*[contains(@class,' faq/faqitem ')]"
			     mode="toc"/>
      </xsl:otherwise>
    </xsl:choose>
  </fo:block>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqitem ')]"
	      mode="toc">
  <fo:list-block provisional-distance-between-starts="72pt"
		 provisional-label-separation="2pt">
    <fo:list-item>
      <fo:list-item-label text-align="end"
			  end-indent="label-end()">
	<fo:block>
	  <xsl:number level="multiple" format="1. "/>
	</fo:block>
      </fo:list-item-label>
      <fo:list-item-body start-indent="body-start()"> 
	<fo:block font-style="italic">
	  <xsl:choose>
	    <xsl:when test="@conref">
	      <xsl:apply-templates select="id(@conref)/*[contains(@class,' faq/faqquest ')]"
				   mode="toc"/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:apply-templates select="*[contains(@class,' faq/faqquest ')]"
				   mode="toc"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</fo:block>
      </fo:list-item-body> 
    </fo:list-item>
  </fo:list-block>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqquest ')]"
	      mode="toc">
  <fo:block font-style="italic">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

</xsl:stylesheet>
