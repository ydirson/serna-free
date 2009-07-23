<?xml version="1.0" encoding="UTF-8"?>
<!--  faq_shell2fo.xsl
 | DITA domains support for the demo set; extend as needed
 |
 | (C) Copyright IBM Corporation 2001, 2002, 2003. All Rights Reserved.
 | This file is part of the DITA package on IBM's developerWorks site.
 | See license.txt for disclaimers.
 +
 | updates:
 *-->

<xsl:stylesheet version="1.0" 
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:import href="../../../xml/stylesheets/dita/topic2fo.xsl"/>
<xsl:import href="../../../xml/stylesheets/dita/domains2fo.xsl"/>
<xsl:import href="elementref2fo.xsl"/>

<xsl:output
    method="xml"
    encoding="utf-8"
    indent="no"
/>

<!-- =============== start of override tweaks ============== -->
<xsl:param name="Lang" select="'en-us'"/> <!-- try de-de or es-es -->


</xsl:stylesheet>
