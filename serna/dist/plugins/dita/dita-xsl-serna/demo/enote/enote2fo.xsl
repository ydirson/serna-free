<?xml version="1.0" encoding="UTF-8"?>
<!--  
 | DITA domains support for the demo set; extend as needed
 |
 | (C) Copyright IBM Corporation 2001, 2004. All Rights Reserved.
 | This file is part of the DITA package on IBM's developerWorks site.
 | See license.txt for disclaimers and permissions.
 +
 | updates:
 *-->


<xsl:stylesheet version="1.0" 
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">


<xsl:output
    method="xml"
    encoding="utf-8"
    indent="no"
/>

<xsl:variable name="eNoteStringFile" select="document('enote_strings.xml')"/>


<xsl:template match="*[contains(@class,' enote/noteheader ')]"><!-- originally ul -->
<fo:block space-before="12pt">
  <xsl:attribute name="padding-bottom">12pt</xsl:attribute>
  <xsl:attribute name="border-bottom-color">blue</xsl:attribute>
  <xsl:attribute name="border-bottom-width">medium</xsl:attribute>
  <fo:table>
    <fo:table-column column-width="proportional-column-width(20)"/>
    <fo:table-column column-width="proportional-column-width(80)"/>
    <fo:table-body>
      <xsl:call-template name="gen-subject"/>
      <xsl:apply-templates/>
    </fo:table-body>
  </fo:table>
</fo:block>
</xsl:template>

<xsl:template name="gen-subject">
  <fo:table-row>
    <fo:table-cell start-indent="2pt" padding="2pt" text-align="right">
      <fo:inline font-weight="bold">
        <xsl:text>Subject: </xsl:text>
      </fo:inline>
    </fo:table-cell>
    <fo:table-cell start-indent="2pt" background-color="#fafafa" xsl:use-attribute-sets="frameall">
      <fo:inline font-weight="bold">
      <xsl:value-of select="//*[contains(@class,' enote/subject ')]"/>
      </fo:inline>
    </fo:table-cell>
  </fo:table-row>
</xsl:template>


<xsl:template match="*[contains(@class,' enote/From ')]|*[contains(@class,' enote/To ')]|*[contains(@class,' enote/Cc ')]|*[contains(@class,' enote/Bcc ')]">
  <fo:table-row>
    <fo:table-cell start-indent="2pt" padding="2pt" text-align="right">
      <fo:inline font-weight="bold">
        <xsl:value-of select="name()"/>
        <xsl:text>:</xsl:text>
      </fo:inline>
      <xsl:text> </xsl:text>
    </fo:table-cell>
    <fo:table-cell start-indent="2pt" background-color="#fafafa" xsl:use-attribute-sets="frameall">
          <xsl:for-each select="recipient">
      <fo:inline>
        <xsl:apply-templates/>
      </fo:inline>
            <xsl:choose>
              <xsl:when test="position() = last()"/>
              <xsl:otherwise>, </xsl:otherwise> 
            </xsl:choose>
          </xsl:for-each>
    </fo:table-cell>
  </fo:table-row>
</xsl:template>

<xsl:template match="*[contains(@class,' enote/Date ')]">
  <fo:table-row>
    <fo:table-cell start-indent="2pt" padding="2pt" text-align="right">
      <fo:inline font-weight="bold">
        <xsl:value-of select="name()"/>
        <xsl:text>:</xsl:text>
      </fo:inline>
      <xsl:text> </xsl:text>
    </fo:table-cell>
    <fo:table-cell start-indent="2pt" background-color="#fafafa" xsl:use-attribute-sets="frameall">
      <fo:inline>
        <xsl:value-of select="."/>
      </fo:inline>
    </fo:table-cell>
  </fo:table-row>
</xsl:template>

<xsl:template match="*[contains(@class,' enote/delivery ')]">
<!-- li: contains Priority, Importance, ReturnReceipt, and Encrypted
     each of which we'll treat as a table row in a nested table-->
          <xsl:apply-templates/>
</xsl:template>

<xsl:template match="*[contains(@class,' enote/Priority ')]">
  <fo:table-row>
    <fo:table-cell start-indent="2pt" padding="2pt" text-align="right">
      <fo:inline font-weight="bold">
        <xsl:value-of select="name()"/>
        <xsl:text>:</xsl:text>
      </fo:inline>
      <xsl:text> </xsl:text>
    </fo:table-cell>
    <fo:table-cell start-indent="2pt" background-color="#fafafa" xsl:use-attribute-sets="frameall">
      <fo:inline>
        <xsl:apply-templates/>
      </fo:inline>
    </fo:table-cell>
  </fo:table-row>
</xsl:template>

<xsl:template match="*[contains(@class,' enote/Importance ')]">
  <fo:table-row>
    <fo:table-cell start-indent="2pt" padding="2pt" text-align="right">
      <fo:inline font-weight="bold">
        <xsl:value-of select="name()"/>
        <xsl:text>:</xsl:text>
      </fo:inline>
      <xsl:text> </xsl:text>
    </fo:table-cell>
    <fo:table-cell start-indent="2pt" background-color="#fafafa" xsl:use-attribute-sets="frameall">
      <fo:inline>
        <xsl:value-of select="@value"/>
      </fo:inline>
    </fo:table-cell>
  </fo:table-row>
</xsl:template>

<xsl:template match="*[contains(@class,' enote/ReturnReceipt ')]">
  <fo:table-row>
    <fo:table-cell start-indent="2pt" padding="2pt" text-align="right">
      <fo:inline font-weight="bold">
        <xsl:value-of select="name()"/>
        <xsl:text>:</xsl:text>
      </fo:inline>
      <xsl:text> </xsl:text>
    </fo:table-cell>
    <fo:table-cell start-indent="2pt" background-color="#fafafa" xsl:use-attribute-sets="frameall">
      <fo:inline>
        <xsl:value-of select="@state"/>
      </fo:inline>
    </fo:table-cell>
  </fo:table-row>
</xsl:template>

<xsl:template match="*[contains(@class,' enote/Encrypted ')]">
  <fo:table-row>
    <fo:table-cell start-indent="2pt" padding="2pt" text-align="right">
      <fo:inline font-weight="bold">
        <xsl:value-of select="name()"/>
        <xsl:text>:</xsl:text>
      </fo:inline>
      <xsl:text> </xsl:text>
    </fo:table-cell>
    <fo:table-cell start-indent="2pt" background-color="#fafafa" xsl:use-attribute-sets="frameall">
      <fo:inline>
        <xsl:apply-templates/>
      </fo:inline>
    </fo:table-cell>
  </fo:table-row>
</xsl:template>

<xsl:template match="*[contains(@class,' enote/references ')]">
<!-- li: contains InReplyTo and Reference (both xref),
     each of which we'll treat as a table row in a nested table-->
          <xsl:apply-templates/>
</xsl:template>

<xsl:template match="*[contains(@class,' enote/InReplyTo ')]">
  <fo:table-row>
    <fo:table-cell start-indent="2pt" padding="2pt" text-align="right">
      <fo:inline font-weight="bold">
        <xsl:value-of select="name()"/>
        <xsl:text>:</xsl:text>
      </fo:inline>
      <xsl:text> </xsl:text>
    </fo:table-cell>
    <fo:table-cell start-indent="2pt" background-color="#fafafa" xsl:use-attribute-sets="frameall">
      <fo:inline>
        <xsl:apply-templates/>
      </fo:inline>
    </fo:table-cell>
  </fo:table-row>
</xsl:template>

<xsl:template match="*[contains(@class,' enote/Reference ')]">
  <fo:table-row>
    <fo:table-cell start-indent="2pt" padding="2pt" text-align="right">
      <fo:inline font-weight="bold">
        <xsl:value-of select="name()"/>
        <xsl:text>:</xsl:text>
      </fo:inline>
      <xsl:text> </xsl:text>
    </fo:table-cell>
    <fo:table-cell start-indent="2pt" background-color="#fafafa" xsl:use-attribute-sets="frameall">
      <fo:inline>
        <xsl:apply-templates/>
      </fo:inline>
    </fo:table-cell>
  </fo:table-row>
</xsl:template>

<xsl:template match="*[contains(@class,' enote/InReplyTo ')]">
  <fo:table-row>
    <fo:table-cell start-indent="2pt" padding="2pt" text-align="right">
      <fo:inline font-weight="bold">
        <xsl:value-of select="name()"/>
        <xsl:text>:</xsl:text>
      </fo:inline>
      <xsl:text> </xsl:text>
    </fo:table-cell>
    <fo:table-cell start-indent="2pt" background-color="#fafafa" xsl:use-attribute-sets="frameall">
      <fo:inline>
        <xsl:value-of select="@href"/>
      </fo:inline>
    </fo:table-cell>
  </fo:table-row>
</xsl:template>

<xsl:template match="*[contains(@class,' enote/attachments ')]">
  <fo:table-row>
    <fo:table-cell start-indent="2pt" padding="2pt" text-align="right">
      <fo:inline font-weight="bold">
        <xsl:value-of select="name()"/>
        <xsl:text>:</xsl:text>
      </fo:inline>
      <xsl:text> </xsl:text>
    </fo:table-cell>
    <fo:table-cell start-indent="2pt" background-color="#fafafa" xsl:use-attribute-sets="frameall">
        <!-- this will be image or object; each has its own formatting support -->
        <xsl:apply-templates/>
    </fo:table-cell>
  </fo:table-row>
</xsl:template>


</xsl:stylesheet>
