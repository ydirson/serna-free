<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:import href="../../../stylesheets/xslbricks/fo/page-sizes.xsl"/>

  <xsl:param name="fop.publisher" select="0"/>

  <xsl:include href="cals-publish-table.xsl "/>
  <xsl:include href="cmm-param.xsl"/>
  <xsl:include href="cmm-blocks.xsl"/>
  <xsl:include href="cmm-compounds.xsl"/>
  <xsl:include href="cmm-lists.xsl"/>
  <xsl:include href="cmm-tables.xsl"/>
  <xsl:include href="cmm-graphic.xsl"/>

  <xsl:output method="xml" indent="no"/>

  <xsl:strip-space elements="*"/>
  
  <!--xsl:preserve-space elements="note"/-->  

  <xsl:template match="cmm" priority="1" mode="root.mode">
    <fo:page-sequence master-reference="body">
      <xsl:call-template name="static-content"/>
      <fo:flow flow-name="xsl-region-body"
               font-size="{$body.font.size}"
               font-family="{$serif.font.family}">
        <fo:block border-left-width="0pt" border-right-width="0pt">
          <xsl:apply-templates/>
        </fo:block>
      </fo:flow>
    </fo:page-sequence>
  </xsl:template>


  <xsl:template match="/">
    <fo:root xsl:use-attribute-sets="root">
      <xsl:call-template name="setup-pagemasters"/>
      <xsl:apply-templates mode="root.mode"/>
    </fo:root>
  </xsl:template>

  <xsl:template name="setup-pagemasters">
    <fo:layout-master-set>
      <fo:simple-page-master
        master-name="body"
        page-width="{$page.width}"
        page-height="{$page.height}"
        margin-top="0.3in"
        margin-bottom="0.5in"
        margin-left="{$page.margin.left}"
        margin-right="{$page.margin.right}">
          
        <fo:region-body margin-bottom="1in"
          margin-top="1.3in"
          region-name="xsl-region-body">
        </fo:region-body>
        <fo:region-before region-name="xsl-region-before" 
                          extent="12%"/>
        <fo:region-after  region-name="xsl-region-after" 
                          extent="10%"/>
      </fo:simple-page-master>
    </fo:layout-master-set>
  </xsl:template>



<xsl:template name="static-content">
  <fo:static-content flow-name="xsl-region-after">
    <fo:block  text-align="end" font-size="{$body.font.size}">
      <fo:block>
        <xsl:text>Page </xsl:text>
        <fo:page-number/>
      </fo:block>
      <fo:block>
         <fo:inline font-size="20pt">
            <xsl:choose>
              <xsl:when test="chapx/cmmx">
                <xsl:if test="chapx/cmmx/@chapnbr">
                  <xsl:value-of select="chapx/cmmx/@chapnbr"/>
                </xsl:if>
                <xsl:if test="chapx/cmmx/@sectnbr">
                  <xsl:text>-</xsl:text>
                  <xsl:value-of select="chapx/cmmx/@sectnbr"/>
                </xsl:if>
                <xsl:if test="chapx/cmmx/@subjnbr">
                  <xsl:text>-</xsl:text>
                  <xsl:value-of select="chapx/cmmx/@subjnbr"/>
                </xsl:if>
              </xsl:when>
              <xsl:otherwise>
              </xsl:otherwise>
            </xsl:choose>
        </fo:inline>

        <xsl:text>    Sep 17/2001</xsl:text>
      </fo:block>
    </fo:block>
  </fo:static-content>

  <fo:static-content flow-name="xsl-region-before">
    <fo:block start-indent="2em">
      <fo:inline baseline-shift="-3pt">
        <fo:external-graphic src="url(../xsl/sign.jpg)"/>
      </fo:inline>
      <fo:inline text-align="center" padding-left="2in">
        <fo:external-graphic src="url(../xsl/sign2.jpg)"/>
      </fo:inline>
      <fo:block font-family="{$sans.font.family}"
                font-size="{$body.font.size}"
                text-align="center">
        <fo:block padding-bottom="10pt">
          <xsl:text>COMPONENT MAINTENANCE MANUAL</xsl:text>
        </fo:block>
        <fo:block>
          <xsl:value-of select="chapx/cmmx/partinfo/cmpnom"/>
        </fo:block>
        <fo:block>
          <xsl:text>PART NUMBER </xsl:text>
          <xsl:value-of select="chapx/cmmx/partinfo/mfrpnr/pnr"/>
        </fo:block>
      </fo:block>
    </fo:block>
  </fo:static-content>
</xsl:template>


  <xsl:template match="cmmx/title"/>
  <xsl:template match="partinfo/cmpnom"/>
  <xsl:template match="mfrpnr/pnr"/>

</xsl:stylesheet>
