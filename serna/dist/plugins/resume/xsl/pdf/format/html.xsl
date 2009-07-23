<?xml version="1.0" encoding="UTF-8"?>
<!--
html.xsl
Transform XML resume into HTML.

Copyright (c) 2000-2002 Sean Kelly
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

$Id: html.xsl,v 1.17 2002/11/10 20:48:58 brandondoyle Exp $
-->
<xsl:stylesheet xmlns:r="http://xmlresume.sourceforge.net/resume/0.0" 
 xmlns:xsl="http://www.w3.org/1999/XSL/Transform"  version="1.0" 
 exclude-result-prefixes="r">
  <xsl:output method="html" omit-xml-declaration="yes" indent="no" encoding="UTF-8" 
   doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN" 
   doctype-system="http://www.w3.org/TR/xhtml1/DTD/strict.dtd"/>
   
  <xsl:strip-space elements="*"/>
  <xsl:include href="../params.xsl"/>
  <xsl:include href="../lib/common.xsl"/>
  <xsl:include href="../lib/string.xsl"/>
  <xsl:template name="Heading">
    <xsl:param name="Text">HEADING NOT DEFINED</xsl:param>
    <h2 class="heading">
      <span class="headingText">
        <xsl:copy-of select="$Text"/>
      </span>
    </h2>
  </xsl:template>
  <xsl:template match="/">
    <html>
      <head>
        <title>
          <xsl:apply-templates select="r:resume/r:header/r:name"/>
          <xsl:text> - </xsl:text>
          <xsl:value-of select="$resume.word"/>
        </title>
        <xsl:apply-templates select="r:resume/r:keywords" mode="header"/>
      </head>
      <body>
        <xsl:apply-templates select="r:resume"/>
      </body>
    </html>
  </xsl:template>
  <xsl:template match="r:resume">
    <div class="resume">
      <xsl:apply-templates/>
    </div>
  </xsl:template>

<!-- Suppress the keywords in the main body of the document -->
  <xsl:template match="r:keywords"/>

<!-- But put them into the HTML header. -->
  <xsl:template match="r:keywords" mode="header">
    <meta name="keywords">
      <xsl:attribute name="content">
        <xsl:apply-templates select="r:keyword"/>
      </xsl:attribute>
    </meta>
  </xsl:template>
  <xsl:template match="r:keyword">
    <xsl:apply-templates/>
    <xsl:if test="position() != last()">
      <xsl:text>, </xsl:text>
    </xsl:if>
  </xsl:template>

<!-- Output your name and the word "Resume". -->
  <xsl:template match="r:header" mode="standard">
    <div class="header">
      <h1 class="nameHeading">
        <xsl:apply-templates select="r:name"/>
      </h1>
      <xsl:apply-templates select="r:address"/>
      <xsl:apply-templates select="r:contact"/>
    </div>
  </xsl:template>

<!-- Alternate formatting for the page header. -->
<!-- Display the name and contact information in a single centered block. -->
<!-- Since the 'align' attribute is deprecated, we rely on a CSS -->
<!-- style to center the header block. -->
  <xsl:template match="r:header" mode="centered">
    <div class="header" style="text-align: center">
      <h1 class="nameHeading">
        <xsl:apply-templates select="r:name"/>
      </h1>
      <xsl:apply-templates select="r:address"/>
      <xsl:apply-templates select="r:contact"/>
    </div>
  </xsl:template>

<!-- Contact information -->
  <xsl:template match="r:contact">
    <p>
      <xsl:apply-templates/>
    </p>
  </xsl:template>
  <xsl:template match="r:contact/r:phone">
    <xsl:call-template name="PhoneLocation">	
      <xsl:with-param name="Location" select="@location"/>
    </xsl:call-template>    
    <xsl:text>: </xsl:text>
    <xsl:apply-templates/>
    <br/>
  </xsl:template>
  <xsl:template match="r:contact/r:fax">
    <xsl:call-template name="FaxLocation">
      <xsl:with-param name="Location" select="@location"/>
    </xsl:call-template>
    <xsl:text>: </xsl:text>
    <xsl:apply-templates/>
    <br/>
  </xsl:template>
  <xsl:template match="r:contact/r:pager">
    <xsl:value-of select="$pager.word"/>
    <xsl:text>: </xsl:text>
    <xsl:apply-templates/>
    <br/>
  </xsl:template>
  <xsl:template match="r:contact/r:email">
    <xsl:value-of select="$email.word"/>
    <xsl:text>: </xsl:text>
    <a href="mailto:{.}">
      <xsl:apply-templates/>
    </a>
    <br/>
  </xsl:template>
  <xsl:template match="r:contact/r:url">
    <xsl:value-of select="$url.word"/>
    <xsl:text>: </xsl:text>
    <a href="{.}">
      <xsl:apply-templates/>
    </a>
    <br/>
  </xsl:template>
  <xsl:template match="r:contact/r:instantMessage">
    <xsl:call-template name="IMServiceName">
      <xsl:with-param name="Service" select="@service"/>
    </xsl:call-template>
    <xsl:text>: </xsl:text>
    <xsl:apply-templates/>
    <br/>
  </xsl:template>
<!-- Address, in various formats -->
  <xsl:template match="r:address" mode="free-form">
    <p class="address">
      <xsl:apply-templates/>
    </p>
  </xsl:template>
  <xsl:template match="r:address" mode="standard">
    <p class="address">
<!-- templates defined in address.xsl for setting standard fields -->
      <xsl:variable name="AdminDivision">
        <xsl:call-template name="AdminDivision"/>
      </xsl:variable>
      <xsl:variable name="CityDivision">
        <xsl:call-template name="CityDivision"/>
      </xsl:variable>
      <xsl:variable name="PostCode">
        <xsl:call-template name="PostCode"/>
      </xsl:variable>
      <xsl:for-each select="r:street">
        <xsl:apply-templates select="."/>
        <br/>
      </xsl:for-each>
      <xsl:if test="r:street2">
        <xsl:apply-templates select="r:street2"/>
        <br/>
      </xsl:if>
      <xsl:if test="string-length($CityDivision) &gt; 0">
        <xsl:value-of select="$CityDivision"/>
        <br/>
      </xsl:if>
      <xsl:apply-templates select="r:city"/>
      <xsl:if test="string-length($AdminDivision) &gt; 0">
        <xsl:text>, </xsl:text>
        <xsl:value-of select="$AdminDivision"/>
      </xsl:if>
      <xsl:if test="string-length($PostCode) &gt; 0">
        <xsl:text> </xsl:text>
        <xsl:value-of select="$PostCode"/>
      </xsl:if>
      <xsl:if test="r:country">
        <br/>
        <xsl:apply-templates select="r:country"/>
      </xsl:if>
    </p>
  </xsl:template>
  <xsl:template match="r:address" mode="european">
    <p class="address">
<!-- templates defined in address.xsl for setting standard fields -->
      <xsl:variable name="AdminDivision">
        <xsl:call-template name="AdminDivision"/>
      </xsl:variable>
      <xsl:variable name="CityDivision">
        <xsl:call-template name="CityDivision"/>
      </xsl:variable>
      <xsl:variable name="PostCode">
        <xsl:call-template name="PostCode"/>
      </xsl:variable>
      <xsl:for-each select="r:street">
        <xsl:apply-templates select="."/>
        <br/>
      </xsl:for-each>
      <xsl:if test="r:street2">
        <xsl:apply-templates select="r:street2"/>
        <br/>
      </xsl:if>
      <xsl:if test="string-length($CityDivision) &gt; 0">
        <xsl:value-of select="$CityDivision"/>
        <br/>
      </xsl:if>
      <xsl:if test="string-length($PostCode) &gt; 0">
        <xsl:value-of select="$PostCode"/>
        <xsl:text> </xsl:text>
      </xsl:if>
      <xsl:apply-templates select="r:city"/>
      <xsl:if test="string-length($AdminDivision) &gt; 0">
        <br/>
        <xsl:value-of select="$AdminDivision"/>
      </xsl:if>
      <xsl:if test="r:country">
        <br/>
        <xsl:apply-templates select="r:country"/>
      </xsl:if>
    </p>
  </xsl:template>
  <xsl:template match="r:address" mode="italian">
    <p class="address">
      <xsl:for-each select="r:street">
        <xsl:apply-templates select="."/>
        <br/>
      </xsl:for-each>
      <xsl:if test="r:street2">
        <xsl:apply-templates select="r:street2"/>
        <br/>
      </xsl:if>
      <xsl:if test="r:postalCode">
        <xsl:apply-templates select="r:postalCode"/>
        <xsl:text> </xsl:text>
      </xsl:if>
      <xsl:apply-templates select="r:city"/>
      <xsl:if test="r:province">
        <xsl:text> (</xsl:text>
        <xsl:apply-templates select="r:province"/>
        <xsl:text>)</xsl:text>
      </xsl:if>
      <xsl:if test="r:country">
        <br/>
        <xsl:apply-templates select="r:country"/>
      </xsl:if>
    </p>
  </xsl:template>
<!-- Preserve line breaks within a free format address -->
  <xsl:template match="r:address//text()">
    <xsl:call-template name="String-Replace">
      <xsl:with-param name="Text" select="."/>
      <xsl:with-param name="Search-For">
        <xsl:text>
</xsl:text>
      </xsl:with-param>
      <xsl:with-param name="Replace-With">
        <br/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>
<!-- Objective, with level 2 heading. -->
  <xsl:template match="r:objective">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text">
        <xsl:value-of select="$objective.word"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:apply-templates/>
  </xsl:template>
<!-- Past jobs, with level 2 heading. -->
  <xsl:template match="r:history">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text">
        <xsl:value-of select="$history.word"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:apply-templates select="r:job"/>
  </xsl:template>

<!-- Format each job -->
  <xsl:template match="r:job">
    <p class="job">
      <xsl:apply-templates select="r:jobtitle"/>
      <br/>
      <xsl:apply-templates select="r:employer"/>
      <xsl:apply-templates select="r:location"/>
      <br/>
      <xsl:apply-templates select="r:date|r:period"/>
    </p>
    <xsl:apply-templates select="r:description">
      <xsl:with-param name="css.class">jobDescription</xsl:with-param>
    </xsl:apply-templates>
    <xsl:if test="r:projects/r:project">
      <div class="projects">
        <p>
          <xsl:value-of select="$projects.word"/>
        </p>
        <xsl:apply-templates select="r:projects"/>
      </div>
    </xsl:if>
    <xsl:if test="r:achievements/r:achievement">
      <div class="achievements">
        <p>
          <xsl:value-of select="$achievements.word"/>
        </p>
        <xsl:apply-templates select="r:achievements"/>
      </div>
    </xsl:if>
  </xsl:template>

  <xsl:template match="r:location">
    <span class="location">
      <xsl:value-of select="$location.start"/>
      <xsl:apply-templates/>
      <xsl:value-of select="$location.end"/>
    </span>
  </xsl:template>
  <xsl:template match="r:institution">
    <span class="institution">
      <xsl:apply-templates/>
    </span>
  </xsl:template>

<!-- Format the projects section as a bullet list -->
  <xsl:template match="r:projects">
    <ul>
      <xsl:apply-templates select="r:project"/>
    </ul>
  </xsl:template>

<!-- Format a project as a bullet -->
  <xsl:template match="r:project">
      <li class="project">
        <xsl:if test="@title">
          <span class="projectTitle">
  	    <xsl:value-of select="@title"/>
  	    <xsl:value-of select="$title.separator"/>
          </span>
	</xsl:if>
        <xsl:apply-templates/>	
      </li>
  </xsl:template>


  <xsl:template match="r:jobtitle">
    <span class="jobTitle">
      <xsl:apply-templates/>
    </span>
  </xsl:template>

  <xsl:template match="r:employer">
    <span class="employer">
      <xsl:apply-templates/>
    </span>
  </xsl:template>

  <xsl:template match="r:organization">
    <span class="organization">
      <xsl:apply-templates/>
    </span>
  </xsl:template>

<!-- Format the period element -->
  <xsl:template match="r:period">
    <xsl:apply-templates select="r:from"/>-<xsl:apply-templates select="r:to"/>
   </xsl:template>

<!-- Format a date -->
   <xsl:template match="r:date" name="FormatDate">
     <xsl:if test="r:dayOfMonth">
       <xsl:apply-templates select="r:dayOfMonth"/>
       <xsl:text> </xsl:text>
     </xsl:if>
    <xsl:if test="r:month">
      <xsl:apply-templates select="r:month"/>
      <xsl:text> </xsl:text>
    </xsl:if>
    <xsl:apply-templates select="r:year"/>
  </xsl:template>
  <xsl:template match="r:present">
    <xsl:value-of select="$present.word"/>
  </xsl:template>

<!-- Format the achievements section as a bullet list *SE* -->
  <xsl:template match="r:achievements">
    <ul>
      <xsl:for-each select="r:achievement">
        <li class="achievement">
          <xsl:apply-templates/>
        </li>
      </xsl:for-each>
    </ul>
  </xsl:template>

<!-- Degrees and stuff -->
  <xsl:template match="r:academics">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text">
        <xsl:value-of select="$academics.word"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:apply-templates select="r:degrees"/>
    <xsl:apply-templates select="r:note"/>
  </xsl:template>
  <xsl:template match="r:degrees">
    <ul class="degrees">
      <xsl:apply-templates select="r:degree"/>
    </ul>
    <xsl:apply-templates select="r:note"/>
  </xsl:template>
  <xsl:template match="r:note">
    <span class="note">
      <xsl:apply-templates/>
    </span>
  </xsl:template>

<!-- Format a degree -->
  <xsl:template match="r:degree">
    <li class="degree">
      <span class="degreeTitle">
        <xsl:apply-templates select="r:level"/>
        <xsl:if test="r:major">
          <xsl:text> </xsl:text>
          <xsl:value-of select="$in.word"/>
          <xsl:text> </xsl:text>
          <xsl:apply-templates select="r:major"/>
        </xsl:if>
      </span>
      <xsl:apply-templates select="r:minor"/>
      <xsl:if test="r:date|r:period">
        <xsl:text>, </xsl:text>
        <xsl:apply-templates select="r:date|r:period"/>
      </xsl:if>
      <xsl:if test="r:annotation">
        <xsl:text>. </xsl:text>
        <xsl:apply-templates select="r:annotation"/>
      </xsl:if>
      <xsl:if test="r:institution">
        <br/>
        <xsl:apply-templates select="r:institution"/>
        <xsl:apply-templates select="r:location"/>
      </xsl:if>
      <xsl:apply-templates select="r:gpa"/>
      <xsl:apply-templates select="r:subjects"/>
      <xsl:apply-templates select="r:projects"/>
    </li>
  </xsl:template>
<!-- Format a GPA -->
  <xsl:template match="r:gpa">
    <p>
      <span class="gpaPreamble">
        <xsl:choose>
          <xsl:when test="@type = 'major'">
            <xsl:value-of select="$major-gpa.word"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$overall-gpa.word"/>
          </xsl:otherwise>
        </xsl:choose>
      </span>
      <xsl:text>: </xsl:text>
      <xsl:apply-templates select="r:score"/>
      <xsl:if test="r:possible">
        <xsl:value-of select="$out-of.word"/>
        <xsl:apply-templates select="r:possible"/>
      </xsl:if>
      <xsl:if test="r:note">
        <xsl:text>. </xsl:text>
        <xsl:apply-templates select="r:note"/>
      </xsl:if>
    </p>
  </xsl:template>
<!-- Format the subjects as a table-->
  <xsl:template match="r:subjects" mode="table">
    <p class="subjectsHeading">
      <xsl:value-of select="$subjects.word"/>
    </p>
    <table class="subjects">
      <xsl:for-each select="r:subject">
        <tr>
          <td>
            <xsl:apply-templates select="r:title"/>
          </td>
          <xsl:if test="$subjects.result.display = 1">
            <td>
              <xsl:apply-templates select="r:result"/>
            </td>
          </xsl:if>
        </tr>
      </xsl:for-each>
    </table>
  </xsl:template>

<!-- Format the subjects as a list -->
  <xsl:template match="r:subjects" mode="comma">
    <p>
      <xsl:call-template name="Title">
	<xsl:with-param name="Title" select="$subjects.word"/>
        <xsl:with-param name="Separator" select="$title.separator"/>
      </xsl:call-template>
      <xsl:apply-templates select="r:subject" mode="comma"/>
      <xsl:value-of select="$subjects.suffix"/>
    </p>
  </xsl:template>

<!-- Format a subject -->
  <xsl:template match="r:subject" mode="comma">
    <xsl:value-of select="normalize-space(r:title)"/>
    <xsl:if test="$subjects.result.display = 1">
      <xsl:if test="r:result">
        <xsl:value-of select="$subjects.result.start"/>
        <xsl:value-of select="normalize-space(r:result)"/>
        <xsl:value-of select="$subjects.result.end"/>
      </xsl:if>
    </xsl:if>
    <xsl:if test="following-sibling::r:subject">
      <xsl:value-of select="$subjects.separator"/>
    </xsl:if>
  </xsl:template>

<!-- Format the open-ended skills -->
  <xsl:template match="r:skillarea">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text">
        <xsl:apply-templates select="r:title"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:apply-templates select="r:skillset"/>
  </xsl:template>

<!-- Format a skillset -->
  <xsl:template match="r:skillset">
    <xsl:choose>
      <xsl:when test="$skills.format = 'comma'">
        <p>
          <span class="skillSetTitle">
            <xsl:apply-templates select="r:title">
	      <xsl:with-param name="Separator" select="$title.separator"/>
	    </xsl:apply-templates>
	  </span>
          <xsl:if test="r:skill">
            <span class="skills">
              <xsl:apply-templates select="r:skill" mode="comma"/>
              <xsl:value-of select="$skills.suffix"/>
            </span>
          </xsl:if>
<!-- The following block should be removed in a future version. -->
          <xsl:if test="r:skills">
            <span class="skills">
              <xsl:apply-templates select="r:skills" mode="comma"/>
            </span>
          </xsl:if>
        </p>
      </xsl:when>
      <xsl:otherwise>
	<h3 class="skillsetTitle">
          <xsl:apply-templates select="r:title"/>
	</h3>
        <xsl:if test="r:skill">
          <ul class="skills">
            <xsl:apply-templates select="r:skill" mode="bullet"/>
          </ul>
        </xsl:if>
<!-- The following block should be removed in a future version. -->
        <xsl:if test="r:skills">
          <ul class="skills">
            <xsl:apply-templates select="r:skills" mode="bullet"/>
          </ul>
        </xsl:if>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

<!-- Format a skill as part of a comma-separated list -->
  <xsl:template match="r:skill" mode="comma">
    <xsl:apply-templates/>
    <xsl:apply-templates select="@level"/>
    <xsl:if test="following-sibling::r:skill">
      <xsl:value-of select="$skills.separator"/>
    </xsl:if>
  </xsl:template>

<!-- Format a skill as part of a bulleted list -->
  <xsl:template match="r:skill" mode="bullet">
    <li class="skill">
      <xsl:apply-templates/>
      <xsl:apply-templates select="@level"/>
    </li>
  </xsl:template>

<!-- Format a skill level -->
  <xsl:template match="r:skill/@level">
    <xsl:if test="$skills.level.display = 1">
      <xsl:value-of select="$skills.level.start"/>
      <xsl:value-of select="normalize-space(.)"/>
      <xsl:value-of select="$skills.level.end"/>
    </xsl:if>
  </xsl:template>

<!-- Format publications -->
  <xsl:template match="r:pubs">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text">
        <xsl:value-of select="$publications.word"/>
      </xsl:with-param>
    </xsl:call-template>
    <ul class="pubs">
      <xsl:apply-templates select="r:pub"/>
    </ul>
  </xsl:template>
<!-- Format a single publication -->
  <xsl:template match="r:pub">
    <li class="pub">
      <xsl:call-template name="FormatPub"/>
    </li>
  </xsl:template>
<!-- Memberships, with level 2 heading. -->
  <xsl:template match="r:memberships">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text">
        <xsl:apply-templates select="r:title"/>
      </xsl:with-param>
    </xsl:call-template>
    <ul>
      <xsl:apply-templates select="r:membership"/>
    </ul>
  </xsl:template>

<!-- A single membership. -->
  <xsl:template match="r:membership">
    <li>
      <xsl:if test="r:title">
	<div class="membershipTitle">
          <xsl:call-template name="Title">
	    <xsl:with-param name="Title" select="r:title"/>
	  </xsl:call-template>
	</div>
      </xsl:if>
      <xsl:if test="r:organization">
        <xsl:apply-templates select="r:organization"/>
        <xsl:apply-templates select="r:location"/>
        <br/>
      </xsl:if>
      <xsl:if test="r:date|r:period">
        <xsl:apply-templates select="r:date|r:period"/>
        <br/>
      </xsl:if>
      <xsl:apply-templates select="r:description">
        <xsl:with-param name="css.class">membershipDescription</xsl:with-param>
      </xsl:apply-templates>
    </li>
  </xsl:template>

<!-- Format interests section. -->
  <xsl:template match="r:interests">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text">
        <xsl:call-template name="Title">
          <xsl:with-param name="Title" select="$interests.word"/>
        </xsl:call-template>
      </xsl:with-param>
    </xsl:call-template>
    <ul>
      <xsl:apply-templates select="r:interest"/>
    </ul>
  </xsl:template>

<!-- A single interest. -->
  <xsl:template match="r:interest">
    <li>
      <span class="interestTitle">
        <xsl:apply-templates select="r:title"/>
      </span>
<!-- Append period to title if followed by a single-line description -->
      <xsl:if test="$interest.description.format = 'single-line' and r:description">
        <xsl:text>. </xsl:text>
      </xsl:if>
      <xsl:apply-templates select="r:description"/>
    </li>
  </xsl:template>
<!-- Format an interest description -->
  <xsl:template match="r:interest/r:description">
    <xsl:call-template name="r:description">
      <xsl:with-param name="paragraph.format" select="$interest.description.format"/>
      <xsl:with-param name="css.class">interestDescription</xsl:with-param>
    </xsl:call-template>
  </xsl:template>
<!-- Format security clearance section. -->
  <xsl:template match="r:clearances">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text">
        <xsl:call-template name="Title">
          <xsl:with-param name="Title" select="$security-clearances.word"/>
        </xsl:call-template>
      </xsl:with-param>
    </xsl:call-template>
    <ul>
      <xsl:apply-templates select="r:clearance"/>
    </ul>
  </xsl:template>
<!-- Format a single security clearance. -->
  <xsl:template match="r:clearance">
    <li class="clearance">
      <span class="clearanceLevel">
        <xsl:apply-templates select="r:level"/>
      </span>
      <xsl:if test="r:organization">
        <xsl:text>, </xsl:text>
        <xsl:apply-templates select="r:organization"/>
      </xsl:if>
      <xsl:if test="r:date|r:period">
        <xsl:text>, </xsl:text>
        <xsl:apply-templates select="r:date|r:period"/>
      </xsl:if>
      <xsl:if test="r:note">
        <xsl:text>. </xsl:text>
        <xsl:apply-templates select="r:note"/>
      </xsl:if>
    </li>
  </xsl:template>
<!-- Format awards section. -->
  <xsl:template match="r:awards">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text">
        <xsl:call-template name="Title">
          <xsl:with-param name="Title" select="$awards.word"/>
        </xsl:call-template>
      </xsl:with-param>
    </xsl:call-template>
    <ul>
      <xsl:apply-templates select="r:award"/>
    </ul>
  </xsl:template>
<!-- A single award. -->
  <xsl:template match="r:award">
    <li class="award">
      <span class="awardTitle">
        <xsl:apply-templates select="r:title"/>
      </span>
      <xsl:if test="r:organization">
        <xsl:text>, </xsl:text>
      </xsl:if>
      <xsl:apply-templates select="r:organization"/>
      <xsl:if test="r:date|r:period">
        <xsl:text>, </xsl:text>
      </xsl:if>
      <xsl:apply-templates select="r:date|r:period"/>
      <xsl:apply-templates select="r:description"/>
    </li>
  </xsl:template>
<!-- Format the misc info -->
  <xsl:template match="r:misc">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text">
        <xsl:value-of select="$miscellany.word"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:apply-templates/>
  </xsl:template>
<!-- Format the "last modified" date -->
  <xsl:template match="r:lastModified">
    <p class="lastModified">
      <xsl:value-of select="$last-modified.phrase"/>
      <xsl:text> </xsl:text>
      <xsl:apply-templates/>
      <xsl:text>.</xsl:text>
    </p>
  </xsl:template>
<!-- Format the legalese -->
  <xsl:template match="r:copyright">
    <div class="copyright">
      <xsl:value-of select="$copyright.word"/>
      <xsl:text> </xsl:text>
      <xsl:apply-templates select="r:year"/>
      <xsl:text> </xsl:text>
      <xsl:value-of select="$by.word"/>
      <xsl:text> </xsl:text>
      <xsl:if test="r:name">
        <xsl:apply-templates select="r:name"/>
      </xsl:if>
      <xsl:if test="not(r:name)">
        <xsl:apply-templates select="/r:resume/r:header/r:name"/>
      </xsl:if>
      <xsl:text>. </xsl:text>
      <xsl:apply-templates select="r:legalnotice"/>
    </div>
  </xsl:template>
<!-- para -> p -->
  <xsl:template match="r:para">
    <p class="para">
      <xsl:apply-templates/>
    </p>
  </xsl:template>
<!-- emphasis -> strong -->
  <xsl:template match="r:emphasis">
    <strong class="emphasis">
      <xsl:apply-templates/>
    </strong>
  </xsl:template>
<!-- url -> monospace along with href -->
  <xsl:template match="r:url" name="FormatUrl">
    <a class="urlA">
      <xsl:attribute name="href">
        <xsl:apply-templates/>
      </xsl:attribute>
      <xsl:apply-templates/>
    </a>
  </xsl:template>
<!-- link -> make link from href attribute -->
  <xsl:template match="r:link">
    <a class="linkA">
      <xsl:attribute name="href">
        <xsl:apply-templates select="@href"/>
      </xsl:attribute>
      <xsl:apply-templates/>
    </a>
  </xsl:template>
<!-- citation -> cite -->
  <xsl:template match="r:citation">
    <cite class="citation">
      <xsl:apply-templates/>
    </cite>
  </xsl:template>

<!-- Format the referees -->
  <xsl:template match="r:referees">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text">
        <xsl:value-of select="$referees.word"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:choose>
      <xsl:when test="$referees.display = 1">
        <xsl:choose>
	  <xsl:when test="$referees.layout = 'compact'">
            <table class="referees">
              <xsl:apply-templates select="r:referee" mode="compact"/>
            </table>
          </xsl:when>
	  <xsl:otherwise>
	    <div class="referees">
	      <xsl:apply-templates select="r:referee" mode="standard"/>
	    </div>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:otherwise>
        <p>
          <xsl:value-of select="$referees.hidden.phrase"/>
        </p>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="r:referee" mode="standard">
    <div class="referee">
      <div class="refereeName">
        <xsl:apply-templates select="r:name"/>
        <xsl:if test="r:title or r:organization">
          <div>
            <xsl:apply-templates select="r:title"/>
            <xsl:if test="r:title and r:organization">
              <xsl:text>, </xsl:text>
            </xsl:if>
            <xsl:apply-templates select="r:organization"/>
          </div>
        </xsl:if>
      </div>
      <div class="refereeAddress"><xsl:apply-templates select="r:address"/></div>
      <div class="refereeContact"><xsl:apply-templates select="r:contact"/></div>
    </div>
  </xsl:template>

  <xsl:template match="r:referee" mode="compact">
    <tr>
      <td>
        <div class="refereeName">
          <xsl:apply-templates select="r:name"/>
          <xsl:if test="r:title or r:organization">
            <div>
              <xsl:apply-templates select="r:title"/>
              <xsl:if test="r:title and r:organization">
                <xsl:text>, </xsl:text>
              </xsl:if>
              <xsl:apply-templates select="r:organization"/>
            </div>
          </xsl:if>
        </div>
        <div class="refereeContact"><xsl:apply-templates select="r:contact"/></div>
      </td>
      <td class="refereeAddress"><xsl:apply-templates select="r:address"/></td>
    </tr>
  </xsl:template>

<!-- Format a description as either a block (div) or a single line (span) -->
  <xsl:template match="r:description" name="r:description">
<!-- Possible values: 'block', 'single-line' -->
    <xsl:param name="paragraph.format">block</xsl:param>
    <xsl:param name="css.class">description</xsl:param>
    <xsl:choose>
      <xsl:when test="$paragraph.format = 'single-line'">
        <span class="{$css.class}">
          <xsl:for-each select="r:para">
            <xsl:apply-templates/>
            <xsl:if test="following-sibling::*">
              <xsl:value-of select="$description.para.separator"/>
            </xsl:if>
          </xsl:for-each>
        </span>
      </xsl:when>
      <xsl:otherwise>
<!-- block -->
        <div class="{$css.class}">
          <xsl:apply-templates/>
        </div>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
</xsl:stylesheet>
