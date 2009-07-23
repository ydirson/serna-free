<?xml version="1.0" encoding="UTF-8" ?>
<xsl:stylesheet version="1.0"
                xmlns:sf="http://www.syntext.com/Extensions/Functions" 
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xse="http://syntext.com/Extensions/XSLT-1.0">


  <xsl:import href="../../../docbook/docbook-xsl-serna/docbook.xsl"/>
  <xsl:import href="../../../../xml/stylesheets/xslbricks/fo/xhtml-tables.xsl"/>

  <xsl:attribute-set name="xhtml-inside-table">
    <xsl:attribute name="start-indent">0pt</xsl:attribute>
    <xsl:attribute name="end-indent">0pt</xsl:attribute>
    <xsl:attribute name="text-indent">0pt</xsl:attribute>
    <xsl:attribute name="last-line-end-indent">0pt</xsl:attribute>
    <xsl:attribute name="text-align">start</xsl:attribute>
    <xsl:attribute name="text-align-last">relative</xsl:attribute>
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-table-and-caption" >
    <xsl:attribute name="display-align">center</xsl:attribute>
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-table">
    <xsl:attribute name="border-collapse">separate</xsl:attribute>
    <xsl:attribute name="border-spacing">2px</xsl:attribute>
    <xsl:attribute name="border-width">1px</xsl:attribute>
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-table-caption" use-attribute-sets="xhtml-inside-table">
    <xsl:attribute name="text-align">center</xsl:attribute>
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-table-column">
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-thead" use-attribute-sets="xhtml-inside-table">
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-tfoot" use-attribute-sets="xhtml-inside-table">
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-tbody" use-attribute-sets="xhtml-inside-table">
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-tr">
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-th">
    <xsl:attribute name="font-weight">bolder</xsl:attribute>
    <xsl:attribute name="text-align">center</xsl:attribute>
    <xsl:attribute name="border-width">1px</xsl:attribute>
    <xsl:attribute name="padding">1px</xsl:attribute>
  </xsl:attribute-set>
  
  <xsl:attribute-set name="xhtml-td">
    <xsl:attribute name="border-width">1px</xsl:attribute>
    <xsl:attribute name="padding">1px</xsl:attribute>
  </xsl:attribute-set>



  <xsl:output method="xml"/>
    
  <xsl:strip-space elements="*"/>
  <xsl:preserve-space elements="pre"/>

</xsl:stylesheet>
