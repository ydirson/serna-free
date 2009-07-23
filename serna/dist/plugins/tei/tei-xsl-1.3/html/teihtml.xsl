<!-- 
TEI XSLT stylesheet family version 1.3
RCS: $Date: 2003/11/24 14:50:28 $, $Revision: 1.1 $, $Author: ilia $

XSL stylesheet to format TEI XML documents to HTML or XSL FO

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
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  version="1.0">
  

<xsl:import href="teihtml-param.xsl"/>
<xsl:include href="teihtml-bibl.xsl"/>
<xsl:include href="teihtml-chunk.xsl"/>
<xsl:include href="teihtml-corpus.xsl"/>
<xsl:include href="teihtml-drama.xsl"/>
<xsl:include href="teihtml-figures.xsl"/>
<xsl:include href="teihtml-frames.xsl"/>
<xsl:include href="teihtml-front.xsl"/>
<xsl:include href="teihtml-lists.xsl"/>
<xsl:include href="teihtml-main.xsl"/>
<xsl:include href="teihtml-math.xsl"/>
<xsl:include href="teihtml-misc.xsl"/>
<xsl:include href="teihtml-notes.xsl"/>
<xsl:include href="teihtml-pagetable.xsl"/>
<xsl:include href="teihtml-poetry.xsl"/>
<xsl:include href="teihtml-struct.xsl"/>
<xsl:include href="teihtml-tables.xsl"/>
<xsl:include href="teihtml-xref.xsl"/>

<xsl:include href="teicommon.xsl"/>


</xsl:stylesheet>
