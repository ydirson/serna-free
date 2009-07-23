<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                version='1.0'>

  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/fonts.xsl"/>
  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/common.xsl"/>
  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/layoutsetup.xsl"/>
  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/default-elements.xsl"/>
  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/page-sizes.xsl"/>
  <xsl:import href="../../../xml/stylesheets/xslbricks/fo/xhtml-tables.xsl"/>

  <xsl:include href="xhtml-param.xsl"/>
  <xsl:include href="xhtml-common.xsl"/>
  <xsl:include href="xhtml-struct.xsl"/>
  <xsl:include href="xhtml-blocks.xsl"/>
  <xsl:include href="xhtml-inlines.xsl"/>
  <xsl:include href="xhtml-lists.xsl"/>
  <xsl:include href="xhtml-images.xsl"/>

  <xsl:output method="xml"/>
    
  <xsl:strip-space elements="*"/>
  <xsl:preserve-space elements="pre"/>

</xsl:stylesheet>
