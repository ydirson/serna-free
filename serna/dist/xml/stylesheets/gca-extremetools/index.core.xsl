<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:variable name="homepagefile" select="'index.html'"/>

<xsl:variable name="logofile" select="'ex03.jpg'"/>

<xsl:variable name="titleindexfile" select="'titles.html'"/>

<xsl:variable name="authorindexfile" select="'authors.html'"/>

<xsl:variable name="keywordindexfile" select="'keywords.html'"/>



<!-- templates to be used by the different indexing stylesheets, for
     a consistent look'n'feel -->

<!-- ADD A CONSISTENT HEADER                                       -->
<xsl:template name="header">
  <xsl:param name="thistitle" select="''"/>
  <a href="{$homepagefile}" value="Proceedings Home">
    <img src="icons/{$logofile}" align="right" border="0" height="60px"/>
  </a>
  <h3 class="header">
      <xsl:text/><i>Extreme Markup Languages</i>: Aggregated Proceedings<br/>
      <i><xsl:value-of select="$thistitle"/></i>
  </h3>
</xsl:template>

<!-- ADD THE NAVIGATION LINKS                                      -->
<xsl:template name="navbar">
  <xsl:param name="control" select="''"/>
  <!-- the 'control' parameter may be used to prevent a page from
       containing a link to itself -->
  <table class="navbar" border="0" width="100%" cellspacing="2" cellpadding="2">
    <tr>
      <xsl:if test="not($control='author')">
        <td width="33%" align="center">
          <a href="{$authorindexfile}" value="Author Index">Author Index</a>
        </td>
      </xsl:if>
      <xsl:if test="not($control='title')">
        <td width="33%" align="center">
          <a href="{$titleindexfile}" value="Title Index">Title Index</a>
        </td>
      </xsl:if>
      <xsl:if test="not($control='keyword')">
        <td width="33%" align="center">
          <a href="{$keywordindexfile}" value="Keyword Index">Keyword Index</a>
        </td>
      </xsl:if>
      <!-- td width="34%" align="center">
        <a href="{$homepagefile}" value="Proceedings Home">Proceedings Home</a>
      </td -->
    </tr>
  </table>
</xsl:template>

<!-- ADD A KEY TO EXPLAIN THE ICONS                                -->

<xsl:template name="legend">
  <table border="0" width="100%"><tr>
  <td><h5><img src="icons/htmlicon.med.jpg"/> HTML</h5></td>
  <h5><img src="icons/tppdficon.med.jpg"/> Typeset PDF</h5>
  <td><h5><img src="icons/fopdficon.med.jpg"/> PDF from XSLFO</h5></td>
  <td><h5><img src="icons/xmlicon.med.jpg"/> XML source</h5></td>
  <td><h5><img src="icons/siteicon.med.jpg"/> Author's package</h5></td>
  </tr></table>
</xsl:template>

<xsl:template name="formatslink">
  <!-- some boilerplate  -->
  <h5 class="formatslink">
    <a href="#formats">[Jump to a detailed explanation of the formats available]</a>
  </h5>
</xsl:template>

<xsl:template name="formats">
  <div>
  <table cellpadding="8" cellspacing="0" border="0" style="font-size:80%">
  <tr><td colspan="2">
  <h4><a name="formats">Key to icons for available formats</a></h4>
  <p><i>Note:</i> not all formats are available for all papers. In particular, author's packages are only available when the author provided one. Likewise, when a full paper was not submitted (probably because the presentation was accepted as late breaking), the paper that appears here may be limited to an abstract.</p>
  </td></tr>
  <tr><td>
  <h5><img src="icons/htmlicon.med.jpg"/><br/>for HTML</h5>
  </td>
  <td>
  <p>HTML renditions of the conference papers, generated via XSLT. This link opens a page showing an abstract and table of contents to the conference paper. If you wish to go straight to the main body of the paper, click on the paper's title in the listing.</p>
  </td></tr>
  <tr><td>
  <h5><img src="icons/tppdficon.med.jpg"/><br/>for Typeset PDF</h5>
  </td><td >
  <p>In 2001, PDF versions of conference papers were professionally typeset by <a href="http://impressions.com">Impressions</a>.</p>
  </td></tr>
  <tr><td>
  <h5><img src="icons/fopdficon.med.jpg"/><br/>for PDF from XSLFO</h5>
  </td><td>
  <p>PDF versions of the papers were also generated via XSL. 2002 papers were generated with <tt>XEP</tt>, an XSL formatter kindly provided by <b>RenderX, Inc.</b> (see <a href="http://www.renderx.com">http://www.renderx.com</a>). In 2003, we have used <i>XSLFormatter</i>, another excellent product from AntennaHouse, Inc. (see <a href="http://www.antennahouse.com">http://www.antennahouse.com</a> or <a href="http://www.antenna.co.jp">http://www.antenna.co.jp</a> for the Japanese-language site).</p>
  </td></tr>
  <tr><td>
  <h5><img src="icons/xmlicon.med.jpg"/><br/>for XML</h5>
  </td><td>
  <p>Link directly to XML source as created by the author. Note: no stylesheet is associated: you will get raw XML!</p>
  </td></tr>
  <tr><td>
  <h5><img src="icons/siteicon.med.jpg"/><br/>for author's package</h5>
  </td><td>
  <p>In many cases, the author has also provided us with one or more files, usually including a display version of the paper as presented (in Microsoft Powerpoint, Open Office, Adobe PDF, HTML or other format). In some cases, files used for technology demonstrations are also included. The packages are provided in <tt>.zip</tt> format; download, open and see what you get.</p>
  </td></tr></table>
  </div>
</xsl:template>

<xsl:template name="buttonbar">
  <xsl:param name="paperlabel" select="'p0001'"/>
  <xsl:param name="authorpackage" select="'no'"/>
  <xsl:param name="fullpaper" select="'yes'"/>
  <xsl:param name="typeset" select="'no'"/>
  <xsl:if test="$fullpaper = 'yes'">
    <a class="icon" href="html/{$paperlabel}.html" target="{$paperlabel}-toc">
      <img src="icons/htmlicon.jpg" alt="HTML version" border="0"/>
    </a>&#160;&#160;<xsl:text/>
    <xsl:if test="$typeset='yes'">
      <a class="icon" href="typeset-pdf/{$paperlabel}.pdf" target="{$paperlabel}">
        <img src="icons/tppdficon.jpg" alt="Typeset PDF version" border="0" />
      </a>&#160;&#160;<xsl:text/>
    </xsl:if>
    <a class="icon" href="xslfo-pdf/{$paperlabel}.pdf" target="{$paperlabel}">
      <img src="icons/fopdficon.jpg" alt="Auto-generated PDF version" border="0"/>
    </a>&#160;&#160;<xsl:text/>
    <a class="icon" href="xml/{$paperlabel}.xml" target="{$paperlabel}">
      <img src="icons/xmlicon.jpg" alt="XML source" border="0"/>
    </a>&#160;&#160;<xsl:text/>
  </xsl:if>
  <xsl:if test="$authorpackage='yes'">
    <a class="icon" href="author-pkg/{$paperlabel}.zip">
      <img src="icons/siteicon.jpg" alt="Author's package" border="0"/>
    </a>
  </xsl:if>
  <xsl:if test="starts-with($authorpackage, 'http://')">
    <a class="icon" href="{$authorpackage}" target="_new">
      <img src="icons/siteicon.jpg" alt="Author's package" border="0"/>
    </a>
  </xsl:if>
</xsl:template>

<!-- Some reg'lar old templates                          -->
<xsl:template match="title" mode="textonly">
   <b>
     <xsl:apply-templates mode="textonly"/>
     <xsl:for-each select="following-sibling::subt">
       <xsl:text/>: <xsl:apply-templates/>
     </xsl:for-each>
   </b>
   <xsl:if test="ancestor::*/@latebreaking='yes'">
     <i> [late breaking]</i>
  </xsl:if>
   <xsl:if test="ancestor::*/@poster='yes'">
     <i> [poster]</i>
  </xsl:if>
</xsl:template>

<xsl:template match="fname">
  <xsl:apply-templates/>
  <xsl:text> </xsl:text>
</xsl:template>

<xsl:template match="conference">
  <i>
    <xsl:apply-templates/>
  </i>
</xsl:template>

<!-- special mode for processing, e.g. titles, without getting
     footnotes -->
<xsl:template match="ftnote" mode="textonly"/>



</xsl:stylesheet>