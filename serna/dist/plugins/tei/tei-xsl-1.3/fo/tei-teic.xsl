<!-- RCS $Date: 2003/11/24 14:49:48 $, $Revision: 1.1 $, $Author: ilia $

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
  xmlns:fo="http://www.w3.org/1999/XSL/Format">

<xsl:import href="tei.xsl"/>

<xsl:param name="inlineTables">true</xsl:param>
<xsl:param name="giColor">black</xsl:param>
<xsl:param name="identColor">black</xsl:param>

<xsl:template name="OUCSheader">
<fo:block>
<!--
<fo:external-graphic scale="0.25" src="newcrest"/>
           <fo:leader rule-thickness="0pt"/>
-->
<fo:inline-container>
 <fo:table>
   <fo:table-column column-number="1" column-align="R" column-width="100%"/>
   <fo:table-body border-top-style="solid" border-bottom-style="solid">
       <fo:table-row>
<fo:table-cell>
 <fo:block font-size="14pt" space-before="6pt">
<fo:inline>Oxford 
University Computing Services</fo:inline></fo:block></fo:table-cell>
       </fo:table-row>
       <fo:table-row>
<fo:table-cell><fo:block><xsl:apply-templates select=".//docTitle"/></fo:block></fo:table-cell>
       </fo:table-row>
       <fo:table-row>
<fo:table-cell><fo:block><xsl:apply-templates select=".//docAuthor"/></fo:block></fo:table-cell>
       </fo:table-row>
       <fo:table-row>
<fo:table-cell><fo:block><xsl:apply-templates select=".//docDate"/></fo:block></fo:table-cell>
       </fo:table-row>
   </fo:table-body>
 </fo:table>
</fo:inline-container>
</fo:block>
</xsl:template>


</xsl:stylesheet>


