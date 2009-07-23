<?xml version="1.0" encoding="UTF-8"?>
<!--
13x-140.xsl
Convert XML Resume from version 1.3.1, 1.3.2, or 1.3.3 to version 1.4.0.

Copyright (c) 2002 Sean Kelly, Bruce Christensen
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

$Id: 13x-140.xsl,v 1.1 2002/05/26 03:45:51 bruckie Exp $
-->
<xsl:stylesheet version="1.0"
  xmlns:r="http://xmlresume.sourceforge.net/resume/0.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="xml" omit-xml-declaration="no" indent="yes"
    encoding="UTF-8"
    doctype-public="-//Sean Kelly//DTD Resume 1.4.0//EN"
    doctype-system="http://xmlresume.sourceforge.net/dtd/resume.dtd"
    />
  <xsl:preserve-space elements="*"/>

  <!-- <skillareas> is deprecated. <skillarea> may now appear directly in
  <resume>. -->
  <xsl:template match="r:skillareas">
    <xsl:apply-templates/>
  </xsl:template>

  <!-- <skills> is deprecated. <skill> may now appear directly in <skillset> -->
  <xsl:template match="r:skills">
    <xsl:apply-templates/>
  </xsl:template>

  <!-- <pubDate> is deprecated. It should be replaced with <date>. -->
  <xsl:template match="r:pubDate">
    <r:date><xsl:apply-templates/></r:date>
  </xsl:template>

  <!-- <street2> is deprecated. It should be replaced with <street>. -->
  <xsl:template match="r:street2">
    <r:street><xsl:apply-templates/></r:street>
  </xsl:template>

  <!-- <company> is deprecated. Display its child nodes. -->
  <xsl:template match="r:break">
    <xsl:apply-templates/>
  </xsl:template>

  <!-- <break> is deprecated. It should be removed. -->
  <xsl:template match="r:break">
    <!-- Suppress -->
  </xsl:template>

  <!-- <docpath> and related tags are deprecated. They should be removed. -->
  <xsl:template match="r:docpath | r:head | r:node | r:tail | r:label | r:uri">
    <!-- Suppress -->
  </xsl:template>

  <!-- Everything else stays the same -->
  <xsl:template match="node()|@*">
    <xsl:copy>
      <xsl:apply-templates select="node()|@*"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
