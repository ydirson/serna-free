<?xml version="1.0" encoding="UTF-8"?>

<!--
string.xsl
Library of string processing templates.

Copyright (c) 2000-2002 by Vlad Korolev, Sean Kelly, Bruce Christensen

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

$Id: string.xsl,v 1.1 2002/05/26 03:45:51 bruckie Exp $
-->

<xsl:stylesheet version="1.0"
    xmlns:r="http://xmlresume.sourceforge.net/resume/0.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <!-- Removes leading and trailing space -->
  <xsl:template name="Trim">
    <xsl:param name="Text"/>

    <xsl:variable name="LeadingSpace">
      <xsl:call-template name="LeadingSpace">
        <xsl:with-param name="Text" select="$Text"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="LeadingLen" select="string-length($LeadingSpace)"/>

    <xsl:variable name="TrailingSpace">
      <xsl:call-template name="TrailingSpace">
        <xsl:with-param name="Text" select="$Text"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:variable name="TrailingLen" select="string-length($TrailingSpace)"/>

    <xsl:value-of
      select="substring(
        $Text,
        1+$LeadingLen,
        string-length($Text) - $LeadingLen - $TrailingLen
        )"/>
  </xsl:template>

  <!-- Like normalize-space(), but leaves leading and trailing space intact -->
  <xsl:template name="NormalizeInternalSpace">
    <xsl:param name="Text"/>

    <xsl:call-template name="LeadingSpace">
      <xsl:with-param name="Text" select="$Text"/>
    </xsl:call-template>

    <xsl:value-of select="normalize-space($Text)"/>

    <xsl:call-template name="TrailingSpace">
      <xsl:with-param name="Text" select="$Text"/>
    </xsl:call-template>

  </xsl:template>

  <!-- Returns the leading whitespace of a string -->
  <xsl:template name="LeadingSpace">
    <xsl:param name="Text"/>

    <xsl:variable name="Whitespace">
      <xsl:text>&#x20;&#x9;&#xD;&#xA;</xsl:text>
    </xsl:variable>

    <xsl:if test="string-length($Text) &gt; 0">
      <xsl:variable name="First" select="substring($Text, 1, 1)"/>
      <xsl:variable name="Following" select="substring($Text, 2)"/>

      <xsl:if test="contains($Whitespace, $First)">
        <xsl:value-of select="$First"/>
        <xsl:call-template name="LeadingSpace">
          <xsl:with-param name="Text" select="$Following"/>
        </xsl:call-template>
      </xsl:if>
    </xsl:if>

  </xsl:template>

  <!-- Returns the trailing whitespace of a string -->
  <xsl:template name="TrailingSpace">
    <xsl:param name="Text"/>

    <xsl:variable name="Whitespace">
      <xsl:text>&#x20;&#x9;&#xD;&#xA;</xsl:text>
    </xsl:variable>

    <xsl:if test="string-length($Text) > 0">
      <xsl:variable name="Last"
        select="substring($Text, string-length($Text))"/>
      <xsl:variable name="Preceding"
        select="substring($Text, 1, string-length($Text) -1)"/>

      <xsl:if test="contains($Whitespace, $Last)">
        <xsl:call-template name="TrailingSpace">
          <xsl:with-param name="Text" select="$Preceding"/>
        </xsl:call-template>
        <xsl:value-of select="$Last"/>
      </xsl:if>
    </xsl:if>

  </xsl:template>

  <!--
  Replaces $Max-Replacements occurrences (default: as many as possible) of
  $Search-For in $Text with $Replace-With.

  Based on Sean's PreserveLineBreaks. *BC*
  -->
  <xsl:template name="String-Replace">
    <xsl:param name="Text"/>
    <xsl:param name="Search-For"/>
    <xsl:param name="Replace-With"/>
    <!-- less than 0 = as many replacements as possible -->
    <xsl:param name="Max-Replacements">-1</xsl:param>

    <xsl:choose>
      <xsl:when test="($Max-Replacements != 0) and contains($Text, $Search-For)">
        <xsl:value-of select="substring-before($Text, $Search-For)"/>
        <xsl:copy-of select="$Replace-With"/>
        <xsl:call-template name="String-Replace">
          <xsl:with-param
            name="Text"
            select="substring-after($Text, $Search-For)"/>
          <xsl:with-param
            name="Search-For"
            select="$Search-For"/>
          <xsl:with-param
            name="Replace-With"
            select="$Replace-With"/>
          <xsl:with-param
            name="Max-Replacements"
            select="$Max-Replacements - 1"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$Text"/>
      </xsl:otherwise>
    </xsl:choose>

  </xsl:template>

</xsl:stylesheet>
