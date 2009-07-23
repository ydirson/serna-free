<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:xse="http://www.syntext.com/Extensions/XSLT-1.0"
                version='1.0'>
  <xsl:param name="body.font.family" select="'Times'" xse:type="string" 
    xse:annotation="Specifies default font family"/>
  <xsl:param name="title.font.family" select="'Helvetica'" xse:type="string" 
    xse:annotation="Specifies font for titles"/>
  <xsl:param name="dingbat.font.family" select="'Times'"/>
  <xsl:param name="sans.font.family" select="'Helvetica'" xse:type="string" 
    xse:annotation="Specifies sans font family"/>
  <xsl:param name="serif.font.family" select="'Times'" xse:type="string" 
    xse:annotation="Specifies serif font family"/>
  <xsl:param name="monospace.font.family" select="'Courier'" xse:type="string" 
    xse:annotation="Specifies proportional font family"/>
  <xsl:param name="body.font.master"  select="12" xse:type="numeric" 
    xse:annotation="Specifies the default font size (pt)"/>
</xsl:stylesheet>
