<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:xs="http://www.w3.org/2001/XMLSchema"
                version='1.0'
                exclude-result-prefixes='xs fo'>
  <!-- Run with command line: -->
  <!-- xsltproc -o config-template-doc.xml config-template-documentor.xsl ../../../../serna_2/dist/plugins/syntext/serna-config/serna-config.xsd -->
  <!-- IMPORTANT: remove <?xml version="1.0"?> from config-template-doc.xml -->

  <xsl:output method="xml" omit-xml-declaration="yes"/>
  <!-- xsl:strip-space elements="*"/ -->

  <xsl:template match="xs:schema">
    <xsl:apply-templates select="xs:element[1]"/>
  </xsl:template>

  <xsl:template match="xs:element[@name]">
    <!-- The documentation may come from above, or taken locally -->
    <xsl:param name="documentation" select="xs:annotation"/>
    <!-- The element name -->
    <xsl:variable name="name" select="@name"/>

    <!-- Do nothing, if there is no documentation -->
    <xsl:if test="$documentation">
    <listitem>
      <formalpara>
        <!-- Generate title for the element name -->
        <title><xsl:value-of select="$name"/></title>
        <!-- Take the content of the first node as the brief
             documentation line -->
        <para><xsl:copy-of select="$documentation/xs:documentation[1]/node()"/>
        </para>
      </formalpara>

      <!-- Take the second documentation node, as the extended
           documentation -->
      <xsl:if test="$documentation/xs:documentation[2]/node()">
        <xsl:copy-of select="$documentation/xs:documentation[2]/node()"/>
      </xsl:if>

      <!-- If there are child elements described, go get them. -->
      <xsl:if test="descendant::xs:element[@ref]|descendant::xs:element[@name]">
      <itemizedlist>
        <!-- Generate documentation for local elements -->
        <xsl:apply-templates select="descendant::xs:element[@name]"/>

        <!-- Generate documentation for global elements -->
        <xsl:for-each select="descendant::xs:element[@ref]">
          <xsl:variable name="child" select="@ref"/>
          <xsl:choose>
            <!-- If there is local documentation, then prefer it -->
            <xsl:when test="xs:annotation">
              <xsl:apply-templates select="/xs:schema/xs:element[@name=$child]">
                <xsl:with-param name="documentation" select="xs:annotation"/>
              </xsl:apply-templates>
            </xsl:when>
            <xsl:otherwise>
              <xsl:apply-templates select="/xs:schema/xs:element[@name=$child]"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:for-each>
      </itemizedlist>
      </xsl:if>
    </listitem>
    </xsl:if>
  </xsl:template>

</xsl:stylesheet>
