<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="nitf-table">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="nitf-table-summary">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="nitf-table-metadata">
    <fo:block>
      <fo:block font-size="{$large.font.size}" font-weight="bold" 
                text-align="center" color="gray">NITF Table Metadata</fo:block>
      <xsl:if test="nitf-table-summary[not(self::processing-instruction('se:choice'))]">
        <fo:block font-size="{$body.font.size}" font-weight="bold" 
                  text-align="center" color="gray">
          <xsl:apply-templates select="nitf-table-summary"/>
        </fo:block>
      </xsl:if>
      <xsl:if test="(nitf-col|nitf-colgroup)[not(self::processing-instruction('se:choice'))]">
        <fo:table xsl:use-attribute-sets="xhtml-table">
          <xsl:apply-templates select="(nitf-col|nitf-colgroup)[not(self::processing-instruction('se:choice'))]"/>
          <fo:table-body xsl:use-attribute-sets="xhtml-tbody">
            <fo:table-row xsl:use-attribute-sets="xhtml-tr">
              <xsl:apply-templates select="(nitf-col|nitf-colgroup)
                                   [not(self::processing-instruction('se:choice'))]" mode="draft"/>
            </fo:table-row>
            <xsl:if test="(nitf-colgroup)[not(self::processing-instruction('se:choice'))]">
              <fo:table-row xsl:use-attribute-sets="xhtml-tr">
                <xsl:apply-templates select="(nitf-col|nitf-colgroup)
                                     [not(self::processing-instruction('se:choice'))]" mode="span"/>
              </fo:table-row>
            </xsl:if>
          </fo:table-body>
        </fo:table>  
      </xsl:if>
    </fo:block>
  </xsl:template>

  <xsl:template match="nitf-colgroup|nitf-col">
    <fo:table-column xsl:use-attribute-sets="xhtml-table-column">
      <xsl:call-template name="process-nitf-table-column"/>
    </fo:table-column>
  </xsl:template>
  
  <xsl:template match="nitf-colgroup[nitf-col]" name="colgrp">
    <xsl:param name="n" select="number(@occurrences)"/>    
    <xsl:for-each select="nitf-col">
      <fo:table-column xsl:use-attribute-sets="xhtml-table-column">
        <xsl:call-template name="process-nitf-table-column"/>
      </fo:table-column>
    </xsl:for-each>
    <xsl:if test="$n &gt; 1">
      <xsl:call-template name="colgrp">
        <xsl:with-param name="n" select="number($n)-1"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:template>

  <xsl:template name="process-nitf-table-column">
    <xsl:if test="@occurrences">
      <xsl:attribute name="number-columns-repeated">
        <xsl:value-of select="@occurrences"/>
      </xsl:attribute>
    </xsl:if>
  </xsl:template>

  <xsl:template match="nitf-col" mode="draft">
    <fo:table-cell xsl:use-attribute-sets="xhtml-td" font-family="{$body.font.family}"
                   color="green" background-color="#e6e6e6" 
                   border="1px" border-color="black" border-style="solid">
      <xsl:if test="not(parent::nitf-colgroup)">
        <xsl:attribute name="number-rows-spanned">
          <xsl:value-of select="2"/>
        </xsl:attribute>
      </xsl:if>
      <fo:block>
      <fo:inline>
        <xsl:choose>
          <xsl:when test="@value">
            <xsl:apply-templates select="@value"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>nitf-col</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </fo:inline>
      <xsl:if test="@occurrences &gt; 1">
        <fo:inline font-size="70%" baseline-shift="super">
          <xsl:value-of select="@occurrences"/>
        </fo:inline>
      </xsl:if>
      </fo:block>
    </fo:table-cell>
    <xsl:if test="@occurrences &gt; 1">
      <xsl:call-template name="process-nitf-draft-span">
        <xsl:with-param name="n" select="number(@occurrences)-1"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:template>

  <xsl:template match="nitf-col" mode="span"/>

  <xsl:template name="process-nitf-draft-span">  
    <xsl:param name="n"/>    
    <fo:table-cell 
      xsl:use-attribute-sets="xhtml-td"  
      font-family="{$body.font.family}"
      color="green" background-color="#e6e6e6" 
      border="1px" border-color="black" border-style="solid">
      <xsl:if test="not(parent::nitf-colgroup)">
        <xsl:attribute name="number-rows-spanned">
          <xsl:value-of select="2"/>
        </xsl:attribute>
      </xsl:if>
      <fo:block>
      <fo:inline>
        <xsl:choose>
          <xsl:when test="@value">
            <xsl:apply-templates select="@value"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:text>nitf-col</xsl:text>
          </xsl:otherwise>
        </xsl:choose>
      </fo:inline>
      <fo:inline font-size="70%" baseline-shift="super">
        <xsl:value-of select="$n"/>
      </fo:inline>
      </fo:block>
    </fo:table-cell>
    <xsl:if test="$n &gt; 1">
      <xsl:call-template name="process-draft-span">
        <xsl:with-param name="n" select="number($n)-1"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:template>

  <xsl:template match="nitf-colgroup" mode="draft" name="draft-colgrp">
    <xsl:param name="n" select="number(@occurrences)"/>    
    <fo:table-cell 
      xsl:use-attribute-sets="xhtml-td" 
      font-family="{$body.font.family}"
      color="green" background-color="#e6e6e6" 
      border="1px" border-color="black" border-style="solid">
      <xsl:if test="nitf-col">
        <xsl:attribute name="number-columns-spanned">
          <xsl:value-of select="count(nitf-col)"/>
        </xsl:attribute>
      </xsl:if>
      <fo:block>
      <xsl:choose>
        <xsl:when test="@value">
          <xsl:apply-templates select="@value"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>nitf-colgroup</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:if test="$n &gt; 1">
        <fo:inline font-size="70%" baseline-shift="super">
          <xsl:value-of select="$n"/>
        </fo:inline>
      </xsl:if>
      </fo:block>
    </fo:table-cell>
    <xsl:if test="$n &gt; 1">
      <xsl:call-template name="draft-colgrp">
        <xsl:with-param name="n" select="number($n)-1"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:template>

  <xsl:template match="nitf-colgroup" mode="span" name="span-colgrp">
    <xsl:param name="n" select="number(@occurrences)"/>    
    <xsl:apply-templates mode="draft"/>
    <xsl:if test="$n &gt; 1">
      <xsl:call-template name="span-colgrp">
        <xsl:with-param name="n" select="number($n)-1"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:template>

</xsl:stylesheet>
