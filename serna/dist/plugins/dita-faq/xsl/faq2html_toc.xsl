<?xml version="1.0" encoding="UTF-8" ?>

<xsl:stylesheet version="1.0" 
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:xse="http://syntext.com/Extensions/XSLT-1.0">

<xsl:param name="SHOWTOC"
           xse:type="string" 
           xse:annotation="Hide TOC"
           select="'yes'" />

<!-- turn on return links from end of sections (applied to faqgroup) -->
<xsl:variable name="link-top-section">yes</xsl:variable> 

<!-- override the body template to place this toc in the expected location -->
<xsl:template match="*[contains(@class,' topic/body ')]">
  <div>
    <xsl:comment>topic body</xsl:comment>
    <xsl:call-template name="commonattributes"/>
    <xsl:apply-templates select="parent::*/titlealts" mode="outofline"/>
    <xsl:apply-templates select="parent::*/shortdesc" mode="outofline"/>
    <xsl:apply-templates select="parent::*/prolog" mode="outofline"/>
    <!-- here, you can generate a toc based on what's a child of body -->
    <xsl:if test="$SHOWTOC='yes'">
      <xsl:call-template name="gen-faqtoc"/>
    </xsl:if>
    <xsl:apply-templates/>
  </div>
</xsl:template>


<!-- Table of faqlist questions (by faqgroup) -->
<xsl:template name="gen-faqtoc">
  <hr/>
  <p>Questions in this FAQ list:</p>
  <xsl:apply-templates mode="toc"/>
  <hr/>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqgroup ')]"
	      mode="toc">
  <div>
    <b><xsl:value-of select="*[contains(@class,' topic/title ')]"/></b>
    <xsl:apply-templates select="*[contains(@class,' faq/faqlist ')]"
			 mode="toc"/>
  </div>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqlist ')]"
	      mode="toc">
  <ol>
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
  </ol>
</xsl:template>

<xsl:template match="*[contains(@class,' faq/faqitem ')]"
	      mode="toc">
  <li style="margin-left: 32pt;"><a>
      <xsl:attribute name="href">#<xsl:value-of select="generate-id()"/></xsl:attribute>
      <span style="font-style: italic;">
	<xsl:choose>
	  <xsl:when test="@conref">
	    <xsl:value-of select="id(@conref)/*[contains(@class,' faq/faqquest ')]"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:value-of select="*[contains(@class,' faq/faqquest ')]"/>
	  </xsl:otherwise>
	</xsl:choose>
      </span>
    </a>
  </li>
</xsl:template>


</xsl:stylesheet>
