<!-- 
     TEI XSL Stylesheet for Syntext Serna (c) 2003, Syntext Inc.

     The Stylesheet is based on Sebastian Rahtz/Oxford University 
     XSL TEI Stylesheet v1.3. See file SR-COPYING for 
     Sebastian Rahtz/Oxford University Copyright 
     information.
-->
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                version='1.0'>

  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/fonts.xsl"/>
  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/common.xsl"/>
  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/layoutsetup.xsl"/>
  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/default-elements.xsl"/>
  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/page-sizes.xsl"/>

  <xsl:include href="tei-param.xsl"/>
  <xsl:include href="tei-struct.xsl"/>
  <xsl:include href="tei-para.xsl"/>
  <xsl:include href="tei-lib.xsl"/>
  <xsl:include href="tei-poetry.xsl"/>
  <xsl:include href="tei-drama.xsl"/>
  <xsl:include href="tei-lists.xsl"/>
  <xsl:include href="tei-bib.xsl"/>
  <xsl:include href="tei-special.xsl"/>
  <xsl:include href="tei-front.xsl"/>
  <xsl:include href="tei-figure.xsl"/>
  <xsl:include href="tei-xref.xsl"/>
  <xsl:include href="tei-notes.xsl"/>
  <xsl:include href="tei-table.xsl"/>
  <xsl:include href="tei-toc.xsl"/>
  <xsl:include href="tei-markers.xsl"/>

  <xsl:output method="xml"/>
    
  <xsl:strip-space elements="*"/>
  <xsl:preserve-space elements="eg"/>

</xsl:stylesheet>
