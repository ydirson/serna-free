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
<!-- paragraphs -->
 <xsl:template match="p">
  <fo:block font-size="{$bodySize}">
     <xsl:if test="preceding-sibling::p">
	<xsl:attribute name="text-indent">
              <xsl:value-of select="$parIndent"/>
        </xsl:attribute>
	<xsl:attribute name="space-before.optimum">
              <xsl:value-of select="$parSkip"/>
        </xsl:attribute>
	<xsl:attribute name="space-before.maximum">
              <xsl:value-of select="$parSkipmax"/>
        </xsl:attribute>
     </xsl:if>
 <xsl:if test="@xml:lang">
   <xsl:attribute name="country">
     <xsl:value-of select="substring-before(@xml:lang,'-')"/>
   </xsl:attribute>
   <xsl:attribute name="language">
     <xsl:value-of select="substring-after(@xml:lang,'-')"/>
   </xsl:attribute>
 </xsl:if>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>
</xsl:stylesheet>
