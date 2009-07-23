<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:import href="extreme.fo.xsl"/>

<xsl:param name="blockbgcolor" select="'wheat'"/>

<xsl:param name="mainbgcolor" select="'#FFFFFF'"/>

<xsl:param name="specialfacecolor" select="'darkred'"/>

<xsl:param name="rootdir" select="'../../..'"/>

<xsl:param name="extremelogo" select="concat($rootdir, '/icons/ex03.jpg')"/>

<xsl:param name="venue"
  select="'Montr&#xE9;al, Qu&#xE9;bec'"/>

<xsl:param name="dates"
  select="'August 4-8, 2003'"/>

<xsl:param name="conference">
  <xsl:text>Extreme Markup Languages 2003</xsl:text>
</xsl:param>

<xsl:param name="table.box.thickness" select="'thin'"/>



</xsl:stylesheet>