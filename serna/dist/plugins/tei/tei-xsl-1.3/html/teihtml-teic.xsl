<!-- 
TEI XSLT stylesheet family version 1.3
RCS: $Id: teihtml-teic.xsl,v 1.1 2003/11/24 14:50:28 ilia Exp $

XSL stylesheet to format TEI Lite XML documents to HTML or XSL FO

 Copyright 1999-2002 Sebastian Rahtz/Oxford University  <sebastian.rahtz@oucs.ox.ac.uk>

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and any associated documentation files (the
 ``Software''), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
--> 
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  version="1.0"
  xmlns:html="http://www.w3.org/1999/xhtml">

<xsl:import href="teihtml.xsl"/>

<xsl:output 
  method="html"  
  doctype-public="-//W3C//DTD HTML 4.0 Transitional//EN" 
  doctype-system="http://www.w3.org/TR/html4/loose.dtd"
  indent="no"/>

<!-- parameterization -->

<xsl:template name="metaHook">
  <xsl:param name="title"/>
 <meta name="DC.Title" content="{$title}"/>
 <meta name="DC.Language" content="(SCHEME=iso639) en"/> 
 <meta name="DC.Creator" content="TEI,Humanities Computing Unit, 13 Banbury Road, Oxford OX2 6NN, United Kingdom"/>
 <meta name="DC.Creator.Address" content="tei@oucs.ox.ac.uk"/>
</xsl:template>

<xsl:template name="bodyHook">
  <xsl:attribute name="background">/Pictures/background.gif</xsl:attribute>
</xsl:template>

<!-- *************** params ************************** -->
<xsl:param name="subTocDepth">-1</xsl:param>
<xsl:param name="cssFile">/Stylesheets/tei.css</xsl:param>
<xsl:param name="autoToc"></xsl:param>
<xsl:param name="leftLinks">true</xsl:param>
<xsl:param name="linksWidth">20%</xsl:param>
<xsl:param name="alignNavigationPanel"></xsl:param>
<xsl:param name="numberHeadings"></xsl:param>
<xsl:param name="topNavigationPanel"></xsl:param>
<xsl:param name="bottomNavigationPanel">true</xsl:param>
<xsl:param name="institution">Text Encoding Initiative</xsl:param>
<xsl:param name="homeURL">http://www.tei-c.org/</xsl:param>
<xsl:param name="homeWords">TEI Home</xsl:param>
<xsl:param name="parentURL"></xsl:param>
<xsl:param name="parentWords"></xsl:param>
<xsl:param name="feedbackURL">http://www.tei-c.org/Consortium/TEI-contact.html</xsl:param>
<xsl:param name="feedbackWords">Contact</xsl:param>
<xsl:param name="searchURL">http://wwwsearch.ox.ac.uk/cgi-bin/oxunit?tei</xsl:param>
<xsl:param name="searchWords">Search this site</xsl:param>
<xsl:template name="logoPicture">
<img src="/Pictures/jaco001d.gif" alt="" width="180" />
</xsl:template>
<!-- xsl:param name="useIDs"></xsl:param -->
<xsl:template name="copyrightStatement">Copyright TEI Consortium 2002</xsl:template>
<xsl:param name="dateWord"></xsl:param>
<xsl:param name="authorWord"></xsl:param>



<xsl:template match="html:*">
     <xsl:element name="{local-name()}">
       <xsl:copy-of select="@*"/>
       <xsl:apply-templates/>
     </xsl:element>
</xsl:template>


<xsl:template match="formerrors">
    <xsl:apply-templates select="..//error"/>
</xsl:template>

<xsl:template match="error">
  <br/>
   <span class="form_error"><xsl:value-of select="."/></span>
</xsl:template>

<xsl:template match="textfield">
    <input 
        type="text"
        name="{@name|name}" 
        value="{@value|value}" 
        size="{@width|width}" 
        maxlength="{@maxlength|maxlength}" />
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="password">
    <input 
        type="password"
        name="{@name|name}" 
        value="{@value|value}" 
        size="{@width|width}" 
        maxlength="{@maxlength|maxlength}" />
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="checkbox">
    <input
        type="checkbox"
        name="{@name|name}"
        value="{@value|value}" />
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="submit_button">
    <input
        type="submit"
        name="{@name|name}"
        value="{@value|value}" />
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="hidden">
    <input
        type="hidden"
        name="{@name|name}"
        value="{@value|value}" />
</xsl:template>

<xsl:template match="options/option">
  <option value="{@value|value}">
    <xsl:if test="selected[. = 'selected'] | @selected[. = 'selected']">
      <xsl:attribute name="selected">selected</xsl:attribute>
    </xsl:if>
    <xsl:value-of select="@text|text"/>
  </option>
</xsl:template>

<xsl:template match="single_select">
    <select name="{@name|name}">
        <xsl:apply-templates select="options/option"/>
    </select>
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="textarea">
    <textarea name="{@name|name}" cols="{@cols|cols}" rows="{@rows|rows}">
    <xsl:if test="@wrap|wrap"><xsl:attribute name="wrap">physical</xsl:attribute></xsl:if>
    <xsl:value-of select="@value|value"/>
    </textarea> <br />
    <xsl:apply-templates select="error"/>
</xsl:template>

<xsl:template match="form">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>


