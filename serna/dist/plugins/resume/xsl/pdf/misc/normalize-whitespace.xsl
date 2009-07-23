<?xml version="1.0" encoding="UTF-8"?>

<!--
normalize-whitespace.xsl
Normalize whitespace in an XML resume. (First pass of two-pass processing.)

Copyright (c) 2000-2002 Bruce Christensen
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

$Id: normalize-whitespace.xsl,v 1.1 2002/06/24 21:50:14 bruckie Exp $
-->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:import href="../lib/string.xsl"/>

  <xsl:output method="xml" omit-xml-declaration="no" indent="no"
    encoding="UTF-8"/>
  <xsl:strip-space elements="*"/>

  <!-- Copy everything by default -->
  <xsl:template match="node()|@*">
<xsl:message>Element <xsl:value-of select="local-name()"/></xsl:message>
    <xsl:copy>
      <xsl:apply-templates select="node()|@*"/>
    </xsl:copy>
  </xsl:template>

  <!-- Do special processing for text nodes (normalize whitespace) -->
  <xsl:template match="//text()">

<xsl:message>Text: '<xsl:value-of select="."/>'</xsl:message>

    <!-- Output leading space (if necessary) -->
    <xsl:if test="preceding-sibling::node()[self::* or self::text()]">

      <xsl:variable name="LeadingWS">
        <xsl:call-template name="LeadingSpace">
          <xsl:with-param name="Text">
            <xsl:value-of select="."/>
          </xsl:with-param>
        </xsl:call-template>
      </xsl:variable>

      <xsl:if test="string-length($LeadingWS)">
        <xsl:text> </xsl:text>
<xsl:message>  Preceding sibling: '<xsl:value-of select="preceding-sibling::node()[self::* or self::text()][1]"/>'</xsl:message>
<xsl:message>  Output leading space</xsl:message>
      </xsl:if>
    </xsl:if>

    <!-- Output content -->
    <xsl:value-of select="normalize-space(.)"/>
<xsl:message>  Content: '<xsl:value-of select="normalize-space(.)"/>'</xsl:message>

    <!-- Output trailing space (if necessary) -->
    <xsl:if test="following-sibling::node()[self::* or self::text()][1]">

      <xsl:variable name="TrailingWS">
        <xsl:call-template name="TrailingSpace">
          <xsl:with-param name="Text">
            <xsl:value-of select="."/>
          </xsl:with-param>
        </xsl:call-template>
      </xsl:variable>

      <xsl:if test="string-length($TrailingWS)">
        <xsl:text> </xsl:text>
<xsl:message>  Following sibling: '<xsl:value-of select="following-sibling::node()[self::* or self::text()][1]"/>'</xsl:message>
<xsl:message>  Output trailing space</xsl:message>
      </xsl:if>
    </xsl:if>

<xsl:message>  Preceding siblings:</xsl:message>
<xsl:for-each select="preceding-sibling::node()[self::* or self::text()]">
  <xsl:message>    #<xsl:value-of select="position()"/>/<xsl:value-of select="last()"/>: '<xsl:value-of select="."/>'</xsl:message>
  <xsl:choose>
    <xsl:when test="self::text()">
      <xsl:message>      (Text)</xsl:message>
    </xsl:when>
    <xsl:when test="self::*">
      <xsl:message>      (Element)</xsl:message>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>      (Unknown node type)</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:for-each>

<xsl:message>  Following siblings:</xsl:message>
<xsl:for-each select="following-sibling::node()[self::* or self::text()]">
  <xsl:message>    #<xsl:value-of select="position()"/>/<xsl:value-of select="last()"/>: '<xsl:value-of select="."/>'</xsl:message>
  <xsl:choose>
    <xsl:when test="self::text()">
      <xsl:message>      (Text)</xsl:message>
    </xsl:when>
    <xsl:when test="self::*">
      <xsl:message>      (Element)</xsl:message>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>      (Unknown node type)</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:for-each>

  </xsl:template>
</xsl:stylesheet>
