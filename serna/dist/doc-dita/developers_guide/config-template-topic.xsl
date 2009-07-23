<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:xs="http://www.w3.org/2001/XMLSchema"
                version='1.0'
                exclude-result-prefixes='xs'>

  <xsl:output method="xml" 
              omit-xml-declaration="no"
              doctype-public="-//OASIS//DTD DITA Topic//EN"
              doctype-system="topic.dtd"/>
  <!-- xsl:strip-space elements="*"/ -->

  <xsl:template match="/">
    <topic id="config-template-properties">
      <title>Configuration Template Properties</title>
      <body>
        <section>
          <p>The configuration template can contain the following elements:</p>
            <ul id="serna-config-parameters">
            <xsl:copy-of select="."/>
            </ul>
        </section>
      </body>
    </topic>
  </xsl:template>
  <xsl:template match="*"/>
</xsl:stylesheet>
