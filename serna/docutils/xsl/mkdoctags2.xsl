<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:exsl="http://exslt.org/common"
                version='1.1'
                extension-element-prefixes="exsl">

<xsl:output method="text" encoding="ISO-8859-1"/>

<xsl:param name="base-dir" select="'.'"/>

<xsl:template match="text()|@*"/>

<!-- ********************************************************************

     ********************************************************************

     This file is for generating C++ files with documentation ids

     ******************************************************************** -->

<!-- ==================================================================== -->

<xsl:template name="cppid">
  <xsl:param name="href"/>
  <xsl:param name="tag"/>
  <xsl:variable name="anchor">
    <xsl:variable name="actual-tag">
      <xsl:choose>
        <xsl:when test="$tag">
          <xsl:value-of select="$tag"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="substring-after($href, '#')"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:value-of select="translate($actual-tag, '-.', '__')"/>
  </xsl:variable>
  <xsl:variable name="cppname">
    <xsl:choose>
      <xsl:when test="string-length($anchor) > 0">
        <xsl:value-of select="$anchor"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="translate(substring-before($href, '.html'),
                                        '/\-.',
                                        '____')"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:value-of select="translate($cppname,
                                  'abcdefghijklmnopqrstuvwxyz',
                                  'ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>
</xsl:template>

<xsl:template name="gencpp">
  <xsl:param name="nodeset"/>
#include "docutils/impl/DocTag.h"

namespace doctags {

const DocTag DOCTAGS[] = {
<xsl:for-each select="$nodeset">
  <xsl:sort select="@tag"/>
  <xsl:text>&#x9;{ "</xsl:text>
  <xsl:value-of select="@tag"/>
  <xsl:text>", "</xsl:text>

  <xsl:value-of select="@url"/>
  <xsl:text>" },&#xa;</xsl:text>
</xsl:for-each>
<xsl:text>&#x9;{ 0, 0 }</xsl:text>
};

// <xsl:value-of select="count($nodeset)"/>
const int DOCTAGS_SIZE = sizeof(DOCTAGS)/sizeof(DOCTAGS[0]) - 1;

}
</xsl:template>

<xsl:template name="genlst">
  <xsl:param name="nodeset"/>

<xsl:for-each select="$nodeset">
  <xsl:value-of select="@tag"/><xsl:text>&#x9;&#x9;&#x9;</xsl:text><xsl:value-of select="@url"/>
  <xsl:text>&#xa;</xsl:text>
</xsl:for-each>
</xsl:template>

<xsl:param name="doctags-lst" select="concat(concat($base-dir, '/'), 'doctags.lst')"/>
<xsl:param name="doctags-cpp" select="concat(concat($base-dir, '/'), 'doctags.cpp')"/>

<xsl:template match="/assistantconfig">
  <xsl:variable name="allrefs" select="//*[self::section or self::DCF][@ref][not(@notag)]"/>

  <xsl:variable name="refs-rtf">

  <xsl:for-each select="$allrefs">
    <xsl:element name="doctag">
      <xsl:attribute name="tag">
        <xsl:call-template name="cppid">
          <xsl:with-param name="href" select="@ref"/>
          <xsl:with-param name="tag" select="@tag"/>
        </xsl:call-template>
      </xsl:attribute>
      <xsl:attribute name="url">
        <xsl:value-of select="@ref"/>
      </xsl:attribute>
    </xsl:element>
  </xsl:for-each>

  </xsl:variable>

<exsl:document href="{$doctags-lst}" method="text">
  <xsl:call-template name="genlst">
    <xsl:with-param name="nodeset" select="exsl:node-set($refs-rtf)/*"/>
  </xsl:call-template>
</exsl:document>
<!--exsl:document href="{$doctags-cpp}" method="text">
  <xsl:call-template name="gencpp">
    <xsl:with-param name="nodeset" select="exsl:node-set($refs-rtf)/*"/>
  </xsl:call-template>
</exsl:document-->

</xsl:template>

</xsl:stylesheet>
