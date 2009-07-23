<?xml version="1.0" encoding="UTF-8" ?>
<!--  
 | Composite DITA topics to FO
 |
 | (C) Copyright IBM Corporation 2001, 2002, 2003. All Rights Reserved.
 | This file is part of the DITA package on IBM's developerWorks site.
 | See license.txt for disclaimers.
 +
 | updates:
 *-->

<xsl:stylesheet version="1.0"
        xmlns:fo="http://www.w3.org/1999/XSL/Format"
        xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
        xmlns:xse="http://syntext.com/Extensions/XSLT-1.0"
        xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
        xse:optimization="dita"
        extension-element-prefixes="dtm xse">


<!-- OTHER STYLESHEET INCLUDES/IMPORTS -->
<xsl:include href="topic2fo.xsl"/>
<xsl:include href="domains2fo.xsl"/>

<!-- ====================== template rules for merged content ==================== -->
<dtm:doc dtm:status="finished" dtm:idref="dita"/>
<xsl:template match="dita" dtm:id="dita">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

</xsl:stylesheet>
