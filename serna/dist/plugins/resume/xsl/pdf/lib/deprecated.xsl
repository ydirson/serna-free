<?xml version="1.0" encoding="UTF-8"?>

<!--
deprecated.xsl
Print warning messages about deprecated elements.

Copyright (c) 2000-2002 by Bruce Christensen

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

$Id: deprecated.xsl,v 1.1 2002/05/26 03:45:51 bruckie Exp $
-->

<!--
In general, each block is responsible for outputting a newline after itself.
-->

<xsl:stylesheet version="1.0"
         xmlns:r="http://xmlresume.sourceforge.net/resume/0.0"
         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <!-- Deprecated in 1.3.3 -->
  <xsl:template match="r:break">
    <xsl:message>
      WARNING: break deprecated
        The break element is deprecated as of version 1.3.3 of the XML Resume
        Library. It will be removed in a future version. It is recommended that
        you remove all break elements.
      You can use "make 13x-140" to fix this problem.
    </xsl:message>
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="r:company">
    <xsl:message>
      WARNING: company deprecated
        The company element is deprecated as of version 1.3.3 of the XML Resume
        Library. It will be removed in a future version. It is recommended that
        you remove all company elements.
      You can use "make 13x-140" to fix this problem.
    </xsl:message>
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="r:street2">
    <xsl:message>
      WARNING: street2 deprecated
        The street2 element is deprecated as of version 1.3.3 of the XML Resume
        Library. It will be removed in a future version. It is recommended that
        you convert all street2 elements to street elements.
      You can use "make 13x-140" to fix this problem.
    </xsl:message>
    <xsl:apply-templates/>
  </xsl:template>

  <!-- Deprecated in 1.4.0 -->
  <xsl:template match="r:skillareas">
    <xsl:message>
      WARNING: skillareas deprecated
        The skillareas element is deprecated as of version 1.4.0 of the XML
        Resume Library. It will be removed in a future version. It is
        recommended that you remove all skillareas elements. skillarea elements
        may now be direct children of resume.
      You can use "make 13x-140" to fix this problem.
    </xsl:message>
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template name="SkillsDeprecatedMessage">
    <xsl:message>
      WARNING: skills deprecated
        The skills element is deprecated as of version 1.4.0 of the XML Resume
        Library. It will be removed in a future version. It is recommended that
        you remove all skills elements. skill elements may now be direct
        children of skillset.
      You can use "make 13x-140" to fix this problem.
    </xsl:message>
  </xsl:template>
  <xsl:template match="r:skills">
    <xsl:call-template name="SkillsDeprecatedMessage"/>
    <xsl:apply-templates select="r:skill"/>
  </xsl:template>
  <xsl:template match="r:skills" mode="comma">
    <xsl:call-template name="SkillsDeprecatedMessage"/>
    <xsl:apply-templates select="r:skill" mode="comma"/>
  </xsl:template>
  <xsl:template match="r:skills" mode="bullet">
    <xsl:call-template name="SkillsDeprecatedMessage"/>
    <xsl:apply-templates select="r:skill" mode="bullet"/>
  </xsl:template>

  <xsl:template match="r:pubDate">
    <xsl:message>
      WARNING: pubDate deprecated
        The pubDate element is deprecated as of version 1.4.0 of the XML Resume
        Library. It will be removed in a future version. It is recommended that
        you replace pubDate elements with date elements.
      You can use "make 13x-140" to fix this problem.
    </xsl:message>
    <xsl:call-template name="FormatPubDate"/>
  </xsl:template>

  <xsl:template match="r:docpath | r:head | r:node | r:tail | r:label | r:uri">
    <xsl:message>
      WARNING: docpath, head, node, tail, label, and uri deprecated
        The docpath, head, node, tail, label, and uri elements are deprecated as
        of version 1.4.0 of the XML Resume Library. They will be removed in a
        future version. It is recommended that you remove all of these elements.
      You can use "make 13x-140" to fix this problem.
    </xsl:message>
    <xsl:apply-templates/>
  </xsl:template>

</xsl:stylesheet>
