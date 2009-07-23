<!-- 
TEI XSLT stylesheet family version 1.3
RCS: $Date: 2003/11/24 14:49:48 $, $Revision: 1.1 $, $Author: ilia $

XSL FO stylesheet to format TEI XML documents 

 Copyright 1999-2002 Sebastian Rahtz/Oxford University  <sebastian.rahtz@oucs.ox.ac.uk>

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and any associated documentation files (the
 ``Software''), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
 
-->

<xsl:stylesheet
  xmlns:fotex="http://www.tug.org/fotex"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns:fo="http://www.w3.org/1999/XSL/Format"
  >
<!-- special purpose
 domain-specific elements, whose interpretation
 is open to all sorts of questions -->

<!-- emphasis -->
<xsl:template match="emph">
    <fo:inline font-style="italic">
      <xsl:apply-templates/>
    </fo:inline>
</xsl:template>


<xsl:template match="add">
  <xsl:choose>
   <xsl:when test="@place='sup'">
    <fo:inline vertical-align="super">
      <xsl:apply-templates/>
    </fo:inline>
   </xsl:when>
   <xsl:when test="@place='sub'">
    <fo:inline vertical-align="sub">
      <xsl:apply-templates/>
    </fo:inline>
   </xsl:when>
   <xsl:otherwise>
    <xsl:apply-templates/>
   </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="code">
    <fo:inline font-family="{$typewriterFont}">
        <xsl:apply-templates/>
    </fo:inline>
</xsl:template>

<xsl:template match="sic">
 <xsl:apply-templates/><xsl:text> (sic)</xsl:text>
</xsl:template>

<xsl:template match="corr">
  <xsl:text>[</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>]</xsl:text>
  <xsl:choose>
   <xsl:when test="@sic">
    <fo:footnote>
     <fo:footnote-citation>
      <fo:inline font-size="8pt" vertical-align="super">
         <xsl:number format="a" level="any" count="corr"/>
      </fo:inline>
     </fo:footnote-citation>
    <fo:list-block 
	provisional-distance-between-starts="12pt" 
	provisional-label-separation="6pt">
    <fo:list-item>
     <fo:list-item-label>
       <fo:block>
         <fo:inline font-size="{$footnoteSize}" vertical-align="super">
          <xsl:number format="a" level="any" count="corr"/>
         </fo:inline>
       </fo:block>
       </fo:list-item-label><fo:list-item-body>
       <fo:block font-size="{$footnoteSize}">
               <xsl:value-of select="@sic"/>
       </fo:block>
       </fo:list-item-body>
    </fo:list-item>
       </fo:list-block>
    </fo:footnote> 
   </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template match="del">
    <fo:inline text-decoration="line-through">
       <xsl:apply-templates/>
    </fo:inline>
</xsl:template>


<xsl:template match="eg">
    <fo:block font-family="{$typewriterFont}" 
	white-space-collapse="false" 
	wrap-option="no-wrap" 
	text-indent="0em"
        hyphenate="false"
	start-indent="{$exampleMargin}"
	text-align="start"
	font-size="{$exampleSize}"
	space-before.optimum="4pt"
	space-after.optimum="4pt"
	>
       <xsl:if test="not($flowMarginLeft='')">
        <xsl:attribute name="padding-start">
         <xsl:value-of select="$exampleMargin"/>
        </xsl:attribute>
       </xsl:if>
       <xsl:if test="parent::exemplum">
         <xsl:text>
</xsl:text>
       </xsl:if>
      <xsl:value-of select="translate(.,' ','&#160;')"/>
    </fo:block>
</xsl:template>

<xsl:template match="seg">
    <fo:block font-family="{$typewriterFont}" 
	background-color="yellow"
	white-space-collapse="false" 
	wrap-option="no-wrap" 
	text-indent="0em"
	start-indent="{$exampleMargin}"
	text-align="start"
	font-size="{$exampleSize}"
	padding-before="8pt" 
	padding-after="8pt" 
	space-before.optimum="4pt"
	space-after.optimum="4pt"
	>
      <xsl:apply-templates/>
    </fo:block>
</xsl:template>

<xsl:template match="foreign">
    <fo:inline font-style="italic">
      <xsl:apply-templates/>
    </fo:inline>
</xsl:template>


<xsl:template match="gap">
    <fo:inline border-style="solid">
     <xsl:text>[</xsl:text>
        <xsl:value-of select="@reason"/>
     <xsl:text>]</xsl:text>
    </fo:inline>
</xsl:template>

<xsl:template match="gi">
    <fo:inline         hyphenate="false"
color="{$giColor}" font-family="{$typewriterFont}">
      <xsl:text>&lt;</xsl:text>
        <xsl:apply-templates/>
      <xsl:text>&gt;</xsl:text>
    </fo:inline>
</xsl:template>


<xsl:template match="gloss">
    <fo:inline font-style="italic">
       <xsl:apply-templates/>
    </fo:inline>
</xsl:template>

<xsl:template match="hi">
<fo:inline>
 <xsl:call-template name="rend">
   <xsl:with-param name="defaultvalue" select="string('bold')"/>
   <xsl:with-param name="defaultstyle" select="string('font-weight')"/>
   <xsl:with-param name="rend" select="@rend"/>
 </xsl:call-template>
      <xsl:apply-templates/>
</fo:inline>
</xsl:template>

<xsl:template match="ident">
    <fo:inline color="{$identColor}" font-family="{$sansFont}">
        <xsl:apply-templates/>
    </fo:inline>
</xsl:template>

<xsl:template match="kw">
    <fo:inline font-style="italic">
      <xsl:apply-templates/>
    </fo:inline>
</xsl:template>

<xsl:template match="mentioned">
 <fo:inline>
 <xsl:call-template name="rend">
   <xsl:with-param name="defaultvalue" select="string('italic')"/>
   <xsl:with-param name="defaultstyle" select="string('font-style')"/>
 </xsl:call-template>
      <xsl:apply-templates/>
 </fo:inline>
</xsl:template>

<xsl:template match="q">
 <xsl:choose>

 <xsl:when test="@rend='display'">
   <fo:block 	
	text-align="start"
	text-indent="0pt"
	end-indent="{$exampleMargin}"
	start-indent="{$exampleMargin}"
	font-size="{$exampleSize}"
	space-before.optimum="{$exampleBefore}"
	space-after.optimum="{$exampleAfter}"
	>
   <xsl:apply-templates/>
 </fo:block>
 </xsl:when>

 <xsl:when test="@rend='eg'">
 <fo:block 	
	text-align="start"
	font-size="{$exampleSize}"
	space-before.optimum="4pt"
	text-indent="0pt"
	space-after.optimum="4pt"
	start-indent="{$exampleMargin}"
        font-family="{$typewriterFont}">
   <xsl:apply-templates/>
 </fo:block>
 </xsl:when>

 <xsl:when test="@rend = 'qwic'">
<fo:block
        space-before="{$spaceAroundTable}"
        space-after="{$spaceAroundTable}">
 <fo:inline-container>
   <fo:table 	
	font-size="{$exampleSize}"
        font-family="{$typewriterFont}"
	start-indent="{$exampleMargin}">
  <fo:table-column column-number="1" fotex:column-align="r" column-width="" />
  <fo:table-column column-number="2" fotex:column-align="l" column-width="" />
   <fo:table-body>
   <xsl:for-each select="q">
    <xsl:for-each select="term">
     <fo:table-row> 
     <fo:table-cell>
	<fo:block>
        <xsl:apply-templates select="preceding-sibling::node()"/>
	</fo:block>
      </fo:table-cell>
     <fo:table-cell> 
	<fo:block>
        <xsl:apply-templates/>
     <xsl:apply-templates select="following-sibling::node()"/> 
	</fo:block>
     </fo:table-cell>
     </fo:table-row>
   </xsl:for-each>
   </xsl:for-each>
   </fo:table-body>
   </fo:table>
 </fo:inline-container>
</fo:block>
 </xsl:when>

 <xsl:when test="starts-with(@rend,'kwic')">
<fo:block
        space-before="{$spaceAroundTable}"
        space-after="{$spaceAroundTable}">
 <fo:inline-container>
   <fo:table 	
	font-size="{$exampleSize}"
	start-indent="{$exampleMargin}"
        font-family="{$typewriterFont}">
  <fo:table-column column-number="1" fotex:column-align="r" column-width="" />
  <fo:table-column column-number="2" fotex:column-align="l" column-width="" />
  <fo:table-body>
  <xsl:for-each select="term">
  <fo:table-row> 
     <fo:table-cell><fo:block><xsl:value-of select="preceding-sibling::node()[1]"/>
      </fo:block></fo:table-cell>
     <fo:table-cell><fo:block> <xsl:apply-templates/>
         <xsl:value-of select="following-sibling::node()[1]"/>
       </fo:block>
     </fo:table-cell>
   </fo:table-row>
   </xsl:for-each>
   </fo:table-body>
   </fo:table>
 </fo:inline-container>
</fo:block>
 </xsl:when>

 <xsl:when test="@rend='literal'">
 <fo:block 	
	white-space-collapse="false" 
	wrap-option="no-wrap" 
	font-size="{$exampleSize}"
	space-before.optimum="4pt"
	space-after.optimum="4pt"
	start-indent="{$exampleMargin}"
        font-family="{$typewriterFont}">
   <xsl:apply-templates/>
 </fo:block>
 </xsl:when>
 <xsl:otherwise>
  <xsl:text>&#x201C;</xsl:text>
   <xsl:apply-templates/>
  <xsl:text>&#x201D;</xsl:text>
 </xsl:otherwise>
 </xsl:choose>
</xsl:template>

<xsl:template match="epigraph/q">
 <fo:block 	
	space-before.optimum="4pt"
	space-after.optimum="4pt"
	start-indent="{$exampleMargin}">
   <xsl:apply-templates/>
 </fo:block>
</xsl:template>

<xsl:template match="reg">
    <fo:inline font-family="{$sansFont}">
     <xsl:apply-templates/>
    </fo:inline>
</xsl:template>

<xsl:template match="soCalled">
 <xsl:text>`</xsl:text><xsl:apply-templates/><xsl:text>'</xsl:text>
</xsl:template>


<xsl:template match="term">
    <fo:inline font-style="italic">
      <xsl:apply-templates/>
    </fo:inline>
</xsl:template>

<xsl:template match="title">
<xsl:choose>
<xsl:when test="@level='a'">
      <xsl:text>`</xsl:text><xsl:apply-templates/><xsl:text>'</xsl:text>
</xsl:when>
<xsl:when test="@level='m'">
    <fo:inline font-style="italic">
      <xsl:apply-templates/>
    </fo:inline>
</xsl:when>
<xsl:when test="@level='s'">
    <fo:inline font-style="italic">
      <xsl:apply-templates/>
    </fo:inline>
</xsl:when>
<xsl:otherwise>
    <fo:inline font-style="italic">
      <xsl:apply-templates/>
    </fo:inline>
</xsl:otherwise>
</xsl:choose>
</xsl:template>

<xsl:template match="unclear">
    <fo:inline text-decoration="blink">
       <xsl:apply-templates/>
    </fo:inline>
</xsl:template>

<xsl:template match="abbr">
 <xsl:apply-templates/>
</xsl:template>

<xsl:template match="date">
 <xsl:apply-templates/>
</xsl:template>

<xsl:template match="index">
 <xsl:apply-templates/>
</xsl:template>

<xsl:template match="interp">
 <xsl:apply-templates/>
</xsl:template>

<xsl:template match="interpGrp">
 <xsl:apply-templates/>
</xsl:template>

<xsl:template match="rs">
 <xsl:apply-templates/>
</xsl:template>

<xsl:template match="s">
 <xsl:apply-templates/>
</xsl:template>

<xsl:template match="name">
   <xsl:apply-templates/>
</xsl:template>

<xsl:template match="opener">
  <fo:block>
 <xsl:apply-templates/>
 </fo:block>
</xsl:template>

</xsl:stylesheet>
