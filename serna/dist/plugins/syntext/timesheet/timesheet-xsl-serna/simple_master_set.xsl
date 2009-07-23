<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                version='1.0'>
  <xsl:template match="/">
    <fo:root>

      <!-- TODO: Move the width, etc to named attribute sets A4, etc. -->
      <fo:layout-master-set>

        <fo:simple-page-master 
          master-name="default" 
          page-width="210mm"
          page-height="297mm"
          margin="0mm">
          <fo:region-body     
            region-name="xsl-region-body"
            margin-top="1.5cm" 
            margin-left="1.5cm"
            margin-right="1.5cm" 
            margin-bottom="1.5cm" 
            overflow="scroll"/>
        </fo:simple-page-master>

        <fo:page-sequence-master master-name="default-sequence-master">
          <fo:single-page-master-reference master-reference="default"/>
        </fo:page-sequence-master>

      </fo:layout-master-set>

      <fo:page-sequence master-reference="default-sequence-master">
        <fo:flow flow-name="xsl-region-body"
                 font-family="{$body.font.family}">
          <xsl:apply-templates/>
        </fo:flow>
      </fo:page-sequence>

    </fo:root>
  </xsl:template>
</xsl:stylesheet>
