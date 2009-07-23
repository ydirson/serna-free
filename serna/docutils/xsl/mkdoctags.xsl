<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.1'>

<xsl:output method="text" encoding="ISO-8859-1"/>
<xsl:template match="text()|@*"/>

<!-- ********************************************************************
     
     ********************************************************************

     This file is for generating C++ files with documentation ids
     
     ******************************************************************** -->

<!-- ==================================================================== -->

<xsl:template name="cppid">
  <xsl:param name="href" select="@ref"/>
  <xsl:variable name="anchor" select="translate(substring-after($href, '#'), '-.', '__')"/>
  <xsl:variable name="bookid" select="translate(substring-before($href, '/'), '-.', '__')"/>
  <xsl:variable name="cppname">
    <xsl:choose>
      <xsl:when test="$anchor">
        <xsl:value-of select="$anchor"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="translate(substring-before($href, '.html'), 
                                        '/\-.',
                                        '____')"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:text>extern DOC_EXPIMP const char </xsl:text>
  <xsl:value-of select="translate($cppname, 
                                  'abcdefghijklmnopqrstuvwxyz', 
                                  'ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>
  <xsl:text>[]</xsl:text>
</xsl:template>

<xsl:template name="gencpp">
  <xsl:param name="nodeset"/>
#include "doctags.hpp"

namespace doctags {

<xsl:for-each select="$nodeset">
  <xsl:call-template name="cppid">
    <xsl:with-param name="href" select="@ref"/>
  </xsl:call-template>
  
  <xsl:text> = &#xa;&#x9;"</xsl:text>
  <xsl:value-of select="@ref"/>
  <xsl:text>";&#xa;</xsl:text>
</xsl:for-each>
}
</xsl:template>

<xsl:template name="genhpp">
  <xsl:param name="nodeset"/>
#ifndef DOCTAGS_HPP_
#define DOCTAGS_HPP_

#include "docutils/doc_defs.h"
#include "common/StringDecl.h"

namespace doctags {

DOC_EXPIMP Common::String get_tag(const char*);

<xsl:for-each select="$nodeset">
  <xsl:call-template name="cppid">
    <xsl:with-param name="href" select="@ref"/>
  </xsl:call-template>
  <xsl:text>;&#xa;</xsl:text>
</xsl:for-each>
}

#endif
</xsl:template>

<xsl:template match="/assistantconfig">
<xsl:variable name="allrefs" select="//*[self::section or self::DCF][@ref][contains(@ref, '#') or starts-with(@ref, 'index.html')]"/>

<xsl:document href="doctags.hpp" method="text">
  <xsl:call-template name="genhpp">
    <xsl:with-param name="nodeset" select="$allrefs"/>
  </xsl:call-template>
</xsl:document>
<xsl:document href="doctags.cpp" method="text">
  <xsl:call-template name="gencpp">
    <xsl:with-param name="nodeset" select="$allrefs"/>
  </xsl:call-template>
</xsl:document>

</xsl:template>

</xsl:stylesheet>
