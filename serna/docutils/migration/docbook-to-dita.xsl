<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xi="http://www.w3.org/2001/XInclude"
                xmlns:exsl="http://exslt.org/common"
                version='1.1'
                exclude-result-prefixes="xi"
                extension-element-prefixes="exsl">

<xsl:import href="docbook-to-dita.xsl"/>

<xsl:output method="xml"
            encoding="UTF-8"
            doctype-public="-//OASIS//DTD DITA Map//EN"
            doctype-system="map.dtd"
            indent="yes"/>
            
</xsl:stylesheet>
