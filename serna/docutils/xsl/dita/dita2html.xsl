<?xml version="1.0" encoding="UTF-8" ?>
<!-- This file is part of the DITA Open Toolkit project hosted on
     Sourceforge.net. See the accompanying license.txt file for
     applicable licenses.-->
<!-- (c) Copyright IBM Corp. 2004, 2005 All Rights Reserved. -->

<!-- ereview.xsl
 | DITA topic to HTML for ereview & webreview

-->
<!DOCTYPE stylesheet [ 
   <!ENTITY % dita-dir-pub-decl PUBLIC "dita-dir-pub" "">
   %dita-dir-pub-decl;
]>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:saxon="http://icl.com/saxon"
                xmlns:xt="http://www.jclark.com/xt"
                extension-element-prefixes="saxon xt">

<!-- stylesheet imports -->
<!-- the main dita to xhtml converter -->
<xsl:import href="file://&dita-dir;/xsl/dita2xhtml.xsl"/>

<xsl:template name="make-alignment-attr">
  <xsl:param name="center" select="'middle'"/>
  <xsl:variable name="align" select="@align"/>
  <xsl:if test="$align">
    <xsl:attribute name="align">
      <xsl:choose>
        <xsl:when test="$align='center'">
          <xsl:value-of select="$center"/>
        </xsl:when>
        <xsl:when test="$align='left'">left</xsl:when>
        <xsl:when test="$align='right'">right</xsl:when>
        <xsl:when test="$align='current'">
          <xsl:choose>
            <xsl:when test="ancestor::*[@align]/@align">
              <xsl:value-of select="ancestor::*[@align]/@align"/>
            </xsl:when>
            <xsl:otherwise>middle</xsl:otherwise>
          </xsl:choose>
        </xsl:when>
        <xsl:otherwise><xsl:value-of select="$align"/></xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
  </xsl:if>
</xsl:template>

<xsl:template name="topic-image">
  <xsl:call-template name="make-alignment-attr">
    <xsl:with-param name="center" select="'center'"/>
  </xsl:call-template>
  <xsl:element name="img">
    <xsl:call-template name="commonattributes"/>
    <xsl:call-template name="setid"/>
    <xsl:apply-templates select="@href|@height|@width|@longdescref"/>
    <xsl:choose>
      <xsl:when test="*[contains(@class,' topic/alt ')]">
        <xsl:attribute name="alt"><xsl:apply-templates select="*[contains(@class,' topic/alt ')]" mode="text-only"/></xsl:attribute>
      </xsl:when>
      <xsl:when test="@alt">
        <xsl:attribute name="alt"><xsl:value-of select="@alt"/></xsl:attribute>
      </xsl:when>
    </xsl:choose>
    <xsl:call-template name="make-alignment-attr"/>
  </xsl:element>
</xsl:template>

<!-- Figure caption -->
<xsl:template name="place-fig-lbl">
<xsl:param name="stringName"/>
  <!-- Number of fig/title's including this one -->
  <xsl:variable name="fig-count-actual" select="count(preceding::*[contains(@class,' topic/fig ')]/*[contains(@class,' topic/title ')])+1"/>
  <xsl:variable name="ancestorlang">
    <xsl:call-template name="getLowerCaseLang"/>
  </xsl:variable>
  <xsl:choose>
    <!-- title -or- title & desc -->
    <xsl:when test="*[contains(@class,' topic/title ')]">
      <span class="figcap">
        <p class="title"><b>
         <xsl:choose>      <!-- Hungarian: "1. Figure " -->
          <xsl:when test="( (string-length($ancestorlang)=5 and contains($ancestorlang,'hu-hu')) or (string-length($ancestorlang)=2 and contains($ancestorlang,'hu')) )">
           <xsl:value-of select="$fig-count-actual"/><xsl:text>. </xsl:text>
           <xsl:call-template name="getString">
            <xsl:with-param name="stringName" select="'Figure'"/>
           </xsl:call-template><xsl:text> </xsl:text>
          </xsl:when>
          <xsl:otherwise>
           <xsl:call-template name="getString">
            <xsl:with-param name="stringName" select="'Figure'"/>
           </xsl:call-template><xsl:text> </xsl:text><xsl:value-of select="$fig-count-actual"/><xsl:text>. </xsl:text>
          </xsl:otherwise>
         </xsl:choose>
         <xsl:apply-templates select="./*[contains(@class,' topic/title ')]" mode="figtitle"/>
        </b></p>
       </span>
      <xsl:if test="desc">
       <xsl:text>. </xsl:text><span class="figdesc"><xsl:apply-templates select="./*[contains(@class,' topic/desc ')]" mode="figdesc"/></span>
      </xsl:if>
    </xsl:when>
    <!-- desc -->
    <xsl:when test="*[contains(@class, ' topic/desc ')]">
     <span class="figdesc"><xsl:apply-templates select="./*[contains(@class,' topic/desc ')]" mode="figdesc"/></span>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<!-- Unordered List -->
<!-- handle all levels thru browser processing -->
<xsl:template match="*[contains(@class,' topic/ul ')]" mode="ul-fmt">
 <xsl:call-template name="start-flagit"/>
 <xsl:call-template name="start-revflag"/>
 <xsl:call-template name="setaname"/>
 <ul>
   <xsl:call-template name="commonattributes"/>
   <xsl:apply-templates select="@compact"/>
   <xsl:call-template name="setid"/>
   <xsl:apply-templates/>
 </ul>
 <xsl:call-template name="end-revflag"/>
 <xsl:value-of select="$newline"/>
</xsl:template>

<!-- Simple List -->
<!-- handle all levels thru browser processing -->
<xsl:template match="*[contains(@class,' topic/sl ')]" name="topic.sl">
<xsl:call-template name="start-flagit"/>
  <xsl:choose> <!-- draft rev mode, add div w/ rev attr value -->
   <xsl:when test="@rev and not($FILTERFILE='') and ($DRAFT='yes')">
    <xsl:variable name="revtest"> <!-- Flag the revision? 1=yes; 0=no -->
     <xsl:call-template name="find-active-rev-flag">
      <xsl:with-param name="allrevs" select="@rev"/>
     </xsl:call-template>
    </xsl:variable>
    <xsl:choose>
     <xsl:when test="$revtest=1">
      <div class="{@rev}"><xsl:apply-templates select="."  mode="sl-fmt" /></div>
     </xsl:when>
     <xsl:otherwise>
      <xsl:apply-templates select="."  mode="sl-fmt" />
     </xsl:otherwise>
    </xsl:choose>
   </xsl:when>
   <xsl:otherwise>
    <xsl:apply-templates select="."  mode="sl-fmt" />
   </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- Ordered List - 1st level - Handle levels 1 to 9 thru OL-TYPE attribution -->
<!-- Updated to use a single template, use count and mod to set the list type -->
<xsl:template match="*[contains(@class,' topic/ol ')]" name="topic.ol">
<xsl:variable name="olcount" select="count(ancestor-or-self::*[contains(@class,' topic/ol ')])"/>
<xsl:call-template name="start-flagit"/>
<xsl:call-template name="start-revflag"/>
<xsl:call-template name="setaname"/>
<ol>
  <xsl:call-template name="commonattributes"/>
  <xsl:apply-templates select="@compact"/>
  <xsl:choose>
    <xsl:when test="$olcount mod 3 = 1"/>
    <xsl:when test="$olcount mod 3 = 2"><xsl:attribute name="type">a</xsl:attribute></xsl:when>
    <xsl:otherwise><xsl:attribute name="type">i</xsl:attribute></xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="setid"/>
  <xsl:apply-templates/>
</ol>
<xsl:call-template name="end-revflag"/>
<xsl:value-of select="$newline"/>
</xsl:template>

<xsl:output method="html"
            encoding="UTF-8"
            indent="no"
            omit-xml-declaration="yes"
            doctype-system="http://www.w3.org/TR/html4/loose.dtd"
            doctype-public="-//W3C//DTD HTML 4.01 Transitional//EN"
/>

</xsl:stylesheet>
