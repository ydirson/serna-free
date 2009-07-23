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
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns:fo="http://www.w3.org/1999/XSL/Format"
  >

<!-- out of line objects -->
<xsl:template match="note" mode="endnote">
 <fo:block id="{generate-id()}">
   <xsl:call-template name="calculateEndNoteNumber"/>
  <xsl:text>. </xsl:text><xsl:apply-templates/>
 </fo:block>
</xsl:template>

<xsl:template match="note">
  <xsl:choose>
   <xsl:when test="@place='foot'">
<!--
    <fo:simple-link>
      <xsl:attribute name="internal-destination">
      <xsl:value-of select="generate-id()"/>
      </xsl:attribute>
-->
    <xsl:variable name="FootID">
      <xsl:choose>
        <xsl:when test="@n">
         <xsl:value-of select="@n"/>
        </xsl:when>
        <xsl:otherwise>
         <xsl:call-template name="calculateFootnoteNumber"/>
        </xsl:otherwise>
       </xsl:choose>
    </xsl:variable>
    <fo:footnote>
      <fo:inline font-size="{$footnotenumSize}" vertical-align="super">
         <xsl:value-of select="$FootID"/>
      </fo:inline>
     <fo:footnote-body>
       <fo:block end-indent="0pt"
               start-indent="0pt"
             text-indent="{$parIndent}" font-size="{$footnoteSize}">
         <fo:inline font-size="{$footnotenumSize}"
               vertical-align="super">
          <xsl:value-of select="$FootID"/>
        </fo:inline><xsl:text> </xsl:text>
       <xsl:apply-templates/>
       </fo:block>
     </fo:footnote-body>
    </fo:footnote> 
   </xsl:when>
   <xsl:when test="@place='end'">
    <fo:simple-link>
      <xsl:attribute name="internal-destination">
      <xsl:value-of select="generate-id()"/>
      </xsl:attribute>
      <fo:inline font-size="{$footnotenumSize}" vertical-align="super">
      <xsl:choose>
        <xsl:when test="@n">
         <xsl:value-of select="@n"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="calculateEndNoteNumber"/>
        </xsl:otherwise>
       </xsl:choose>
    </fo:inline>
    </fo:simple-link>
   </xsl:when>
  <xsl:when test="@place='display'">
   <fo:block 	
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
  <xsl:when test="@place='divtop'">
   <fo:block 	
	text-indent="0pt"
	end-indent="{$exampleMargin}"
	start-indent="{$exampleMargin}"
        font-style="italic"
	font-size="{$exampleSize}"
	space-before.optimum="{$exampleBefore}"
	space-after.optimum="{$exampleAfter}"
	>
   <xsl:apply-templates/>
 </fo:block>
 </xsl:when>
   <xsl:when test="@place='margin'">
   </xsl:when>
   <xsl:when test="@place='right'">
   </xsl:when>
   <xsl:when test="@place='left'">
   </xsl:when>
   <xsl:otherwise>
   <xsl:text> (</xsl:text>
      <xsl:apply-templates/>
   <xsl:text>) </xsl:text>
   </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<xsl:template name="calculateEndNoteNumber">
  <xsl:number  level="any" format="i" count="note[@place='end']"/>
</xsl:template>

<xsl:template name="calculateFootnoteNumber">
    <xsl:number  from="text" level="any" count="note[@place='foot']"/>
</xsl:template>

</xsl:stylesheet>
