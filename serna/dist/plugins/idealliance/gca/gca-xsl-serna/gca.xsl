<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:import href="../../../../xml/stylesheets/xslbricks/fo/fonts.xsl"/>
  <xsl:import href="../../../../xml/stylesheets/xslbricks/fo/common.xsl"/>
  <xsl:import href="../../../../xml/stylesheets/xslbricks/fo/layoutsetup.xsl"/>
  <xsl:import href="../../../../xml/stylesheets/xslbricks/fo/default-elements.xsl"/>
  <xsl:import href="../../../../xml/stylesheets/xslbricks/fo/page-sizes.xsl"/>
  <xsl:import href="../../../../xml/stylesheets/xslbricks/fo/xhtml-tables.xsl"/>

  <xsl:include href="gca-param.xsl"/>
  <xsl:include href="gca-body.xsl"/>
  <xsl:include href="gca-front.xsl"/>
  <xsl:include href="gca-inlines.xsl"/>
  <xsl:include href="gca-rear.xsl"/>

  <xsl:output method="xml" indent="no"/>

  <xsl:strip-space elements="*"/>
  
  <xsl:preserve-space elements="code code.block"/>  

  <xsl:template match="gcapaper" priority="1" mode="root.mode">
    <!-- Body page sequence -->
    <fo:page-sequence master-reference="body">
      <fo:flow flow-name="xsl-region-body"
               font-size="{$body.font.size}"
               font-family="{$body.font.family}">
        <fo:block border-left-width="0pt" border-right-width="0pt">
          <xsl:apply-templates/>
          <xsl:if test="$footnotes">
            <xsl:call-template name="footnotes"/>
          </xsl:if>
          <xsl:variable name="glossary">
            <xsl:apply-templates select="body[not(self::processing-instruction('se:choice'))]"
                                 mode="glossary"/>
          </xsl:variable>
          <fo:block border-left-width="0pt" border-right-width="0pt">
            <xsl:if test="$glossary">
              <fo:block font-size="24pt"
                        xsl:use-attribute-sets="borderbefore headercontrol"
                        padding-top="20pt">
                <xsl:text>Glossary</xsl:text>
              </fo:block>
              <fo:block padding-top="4pt">
                <fo:list-block provisional-label-separation="0.25in"
                               provisional-distance-between-starts="2in">
                  <xsl:copy-of select="$glossary"/>
                </fo:list-block>
              </fo:block>
            </xsl:if>                    
            <xsl:if test="$authors">
              <xsl:call-template name="authorinfo"/>
            </xsl:if>
          </fo:block>
        </fo:block>
      </fo:flow>
    </fo:page-sequence>

    <!-- Glossary & Biography page sequence -->
    <!--fo:page-sequence master-reference="body">
      <fo:flow flow-name="xsl-region-body"
               font-size="{$bodySize}"
               font-family="{$serifFont}">
        <xsl:variable name="glossary">
          <xsl:apply-templates select="body[not(self::processing-instruction('se:choice'))]"
                               mode="glossary" se:sections="omit"/>
        </xsl:variable>

        <fo:block border-left-width="0pt" border-right-width="0pt">
          <xsl:if test="$glossary">
            <fo:block font-size="24pt"
                      xsl:use-attribute-sets="borderbefore headercontrol"
                      padding-top="20pt">
              <xsl:text>Glossary</xsl:text>
            </fo:block>
            <xsl:copy-of select="$glossary"/>
          </xsl:if>                    
          <xsl:if test="$authors">
            <xsl:call-template name="authorinfo"/>
          </xsl:if>
        </fo:block>
      </fo:flow>
    </fo:page-sequence-->

  </xsl:template>

</xsl:stylesheet>
