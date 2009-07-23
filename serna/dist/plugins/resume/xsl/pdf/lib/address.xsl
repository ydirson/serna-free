<?xml version="1.0" encoding="UTF-8"?>

<!--
address.xsl
Defines some common templates for address processing that are 
shared by all the stylesheets. 

Copyright (c) 2000-2001 Sean Kelly, Bruce Christensen
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

$Id: address.xsl,v 1.1 2002/05/26 03:45:51 bruckie Exp $
-->

<xsl:stylesheet version="1.0"
  xmlns:r="http://xmlresume.sourceforge.net/resume/0.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <!-- It would be nice if XSLT allowed variable reference in mode --> 
  <!-- selection; then instead of the ugly template below, we'd have -->
  <!-- '<xsl:apply-templates select="r:address" mode="$address.format"/>'-->
  <!-- But it doesn't, so we need this. -->
  <xsl:template match="r:address">

    <!-- Determine formatting method from format attr of this element if -->
    <!-- it exists, otherwise use $address.format parameter. -->
    <xsl:variable name="format">
      <xsl:choose>
        <xsl:when test="@format">
          <xsl:value-of select="@format"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$address.format"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>

    <xsl:choose>
      <!-- If the first child is a text node, assume that we -->
      <!-- have a preformatted address and bypass the address -->
      <!-- formatting below. -->
      <xsl:when test="node()[1][not(self::*)]">
        <xsl:apply-templates select="." mode="free-form"/>
      </xsl:when>
      <xsl:otherwise>

        <xsl:choose>
          <xsl:when test="$format = 'standard'">
            <xsl:apply-templates select="." mode="standard"/>
          </xsl:when>
          <xsl:when test="$format = 'european'">
            <xsl:apply-templates select="." mode="european"/>
          </xsl:when>
          <xsl:when test="$format = 'italian'">
            <xsl:apply-templates select="." mode="italian"/>
          </xsl:when>
        </xsl:choose>

      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <!-- template to determine the AdminDivision for use in addresses. -->
  <!-- AdminDivision is a pseudo-field for the principal administrative -->
  <!-- division of the country: state in the US, province in Canada, -->
  <!-- county in Ireland, for example. -->
  <xsl:template name="AdminDivision">

    <xsl:choose>
      <xsl:when test="r:state">
        <xsl:value-of select="normalize-space(r:state)"/>
      </xsl:when>
      <xsl:when test="r:province">
        <xsl:value-of select="normalize-space(r:province)"/>
      </xsl:when>
      <xsl:when test="r:county">
        <xsl:value-of select="normalize-space(r:county)"/>
      </xsl:when>
      <!-- Otherwise, leave blank -->
    </xsl:choose>
  </xsl:template>

  <!-- template to determine the CityDivision for use in addresses. -->
  <!-- "Suburb" is used in New Zealand; "Ward" in Brazil and Japan. -->
  <xsl:template name="CityDivision">
    <xsl:choose>
      <xsl:when test="r:suburb">
        <xsl:value-of select="normalize-space(r:suburb)"/>
      </xsl:when>
      <xsl:when test="r:ward">
        <xsl:value-of select="normalize-space(r:ward)"/>
      </xsl:when>
      <!-- Otherwise, leave blank -->
    </xsl:choose>
  </xsl:template>

  <!-- template to determine the code to be used in addresses. -->
  <xsl:template name="PostCode">
    <xsl:choose>
      <xsl:when test="r:zip">
        <xsl:value-of select="normalize-space(r:zip)"/>
      </xsl:when>
      <xsl:when test="r:postalCode">
        <xsl:value-of select="normalize-space(r:postalCode)"/>
      </xsl:when>
      <!-- Otherwise, leave blank -->
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>
