<?xml version="1.0" encoding="UTF-8"?>
<!--

NOTE: Usage of this file is or should be 
deprecated.  We suggest using CSS instead.
- brandondoyle 10/29/02

html.xsl
Transform XML resume into HTML, styled similar to Microsoft Word's Contemporary
resume template.

Copyright (c) 2000-2002 Sean Kelly
All rights reserved.
[modifications to mimic standard contemporary resume by Rob Kelley]

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS'' AND
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


-->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" >
	<xsl:output method="html" omit-xml-declaration="yes" indent="yes" encoding="UTF-8" doctype-public="-//W3C//DTD HTML 4.0//EN"/>
	<xsl:strip-space elements="*"/>
	<xsl:include href="./common/params.xsl"/>
	<xsl:include href="./common/address.xsl"/>
	<xsl:include href="./common/pub.xsl"/>
	<xsl:template match="/">
		<html>
			<head>
				<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
				<title>
					<xsl:apply-templates select="resume/header/name/firstname"/>
					<xsl:text> </xsl:text>
					<xsl:apply-templates select="resume/header/name/surname"/>
					<xsl:text> - </xsl:text>
					<xsl:value-of select="$resume.word"/>
				</title>
					<!--override stored resume.css with resume_contemporary.css *RK* -->
					<link rel="stylesheet" type="text/css" href="./css/resume_contemporary.css"/>
					<!--<link rel="stylesheet" type="text/css">
					<xsl:attribute name="href"><xsl:value-of select="$css.href"/></xsl:attribute>
					</link>-->
				
				<xsl:apply-templates select="resume/keywords" mode="header"/>
			</head>
			<body class="resume">
				<table >
					<tr>
						<td>
							<xsl:apply-templates select="resume"/>
						</td>
					</tr>
				</table>
			</body>
		</html>
	</xsl:template>
	<!-- Suppress the keywords in the main body of the document -->
	<xsl:template match="keywords"/>
	<!-- But put them into the HTML header. -->
	<xsl:template match="keywords" mode="header">
		<meta name="keywords">
			<xsl:attribute name="content"><xsl:apply-templates select="keyword"/></xsl:attribute>
		</meta>
	</xsl:template>
	<xsl:template match="keyword">
		<xsl:value-of select="."/>
		<xsl:if test="position() != last()">
			<xsl:text>, </xsl:text>
		</xsl:if>
	</xsl:template>
	<!-- Output your name and the word "Resume". -->
	<xsl:template name="standard.header">
		<!--Apply "contemporary" styling to contact info *RK*-->
		<div>
			<table class="address">
				<tr>
					<td  class="address1cell" valign="top">
						<xsl:apply-templates select="address"/>
						<!--take out contact URL because it isn't displaying nicely. *RK*
							  <p class="Address1">
								<xsl:if test="contact/url">
								<xsl:value-of select="$url.word"/>:
								 <a>
										<xsl:attribute name="href">
											<xsl:value-of select="contact/url"/>
										</xsl:attribute>
									<xsl:value-of select="contact/url"/>
								</a>
								</xsl:if> 
							</p>	-->
					</td>
					<td class="address2cell">
						<xsl:if test="contact/email">
							<a>
								<xsl:attribute name="href"><xsl:text>mailto:</xsl:text><xsl:value-of select="contact/email"/></xsl:attribute>
								<xsl:value-of select="contact/email"/>
							</a>
							<br/>
						</xsl:if>
						<xsl:if test="contact/phone">
							<xsl:value-of select="contact/phone"/>
							<br/>
						</xsl:if>
					</td>
				</tr>
			</table>
		</div>
		<!--Apply "contemporary" styling to class name *RK*-->
			<div class="name" valign="top" >
				<xsl:apply-templates select="name/firstname"/>
					<xsl:text> </xsl:text>
					<xsl:apply-templates select="name/surname"/>
			</div>
	</xsl:template>
	<!-- Alternate formatting for the page header. -->
	<!-- Display the name and contact information in a single centered block. -->
	<!-- Since the 'align' attribute is deprecated, we rely on a CSS -->
	<!-- stylesheet to center the headerBlock. -->
	<xsl:template name="centered.header">
		<div class="headerBlock">
			<h1 class="nameHeading">
				<xsl:apply-templates select="name"/>
			</h1>
			<xsl:apply-templates select="address"/>
			<br/>
			<xsl:if test="contact/phone">
				<xsl:value-of select="$phone.word"/>: <xsl:value-of select="contact/phone"/>
				<br/>
			</xsl:if>
			<xsl:if test="contact/email">
				<xsl:value-of select="$email.word"/>: <a>
					<xsl:attribute name="href"><xsl:text>mailto:</xsl:text><xsl:value-of select="contact/email"/></xsl:attribute>
					<xsl:value-of select="contact/email"/>
				</a>
				<br/>
			</xsl:if>
			<xsl:if test="contact/url">
				<xsl:value-of select="$url.word"/>: <a>
					<xsl:attribute name="href"><xsl:value-of select="contact/url"/></xsl:attribute>
					<xsl:value-of select="contact/url"/>
				</a>
			</xsl:if>
		</div>
	</xsl:template>
	<xsl:template match="header">
		<xsl:choose>
			<xsl:when test="$header.format = 'centered'">
				<xsl:call-template name="centered.header"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:call-template name="standard.header"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<xsl:template match="address" mode="standard">
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
		<xsl:value-of select="street"/>
		<br/>
		<xsl:if test="street2">
			<xsl:value-of select="street2"/>
			<br/>
		</xsl:if>
		<xsl:if test="string-length($CityDivision) &gt; 0">
			<xsl:value-of select="$CityDivision"/>
			<br/>
		</xsl:if>
		<xsl:value-of select="city"/>
		<xsl:if test="string-length($AdminDivision) &gt; 0">
			<xsl:text>, </xsl:text>
			<xsl:value-of select="$AdminDivision"/>
		</xsl:if>
		<xsl:if test="string-length($PostCode) &gt; 0">
			<xsl:text/>
			<xsl:value-of select="$PostCode"/>
		</xsl:if>
		<xsl:if test="country">
			<br/>
			<xsl:value-of select="country"/>
		</xsl:if>
	</xsl:template>
	<xsl:template match="address" mode="european">
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
		<xsl:value-of select="street"/>
		<br/>
		<xsl:if test="street2">
			<xsl:value-of select="street2"/>
			<br/>
		</xsl:if>
		<xsl:if test="string-length($CityDivision) &gt; 0">
			<xsl:value-of select="$CityDivision"/>
			<br/>
		</xsl:if>
		<xsl:if test="string-length($PostCode) &gt; 0">
			<xsl:value-of select="$PostCode"/>
			<xsl:text/>
		</xsl:if>
		<xsl:value-of select="city"/>
		<xsl:if test="string-length($AdminDivision) &gt; 0">
			<br/>
			<xsl:value-of select="$AdminDivision"/>
		</xsl:if>
		<xsl:if test="country">
			<br/>
			<xsl:value-of select="country"/>
		</xsl:if>
	</xsl:template>
	<xsl:template match="address" mode="italian">
		<xsl:value-of select="street"/>
		<br/>
		<xsl:if test="street2">
			<xsl:value-of select="street2"/>
			<br/>
		</xsl:if>
		<xsl:if test="postalCode">
			<xsl:value-of select="postalCode"/>
			<xsl:text/>
		</xsl:if>
		<xsl:value-of select="city"/>
		<xsl:if test="province">
			<xsl:text> (</xsl:text>
			<xsl:value-of select="province"/>
			<xsl:text>)</xsl:text>
		</xsl:if>
		<xsl:if test="country">
			<br/>
			<xsl:value-of select="country"/>
		</xsl:if>
	</xsl:template>
	<!-- Preserve line breaks within a free format address -->
	<xsl:template match="address//text()">
		<xsl:call-template name="PreserveLinebreaks">
			<xsl:with-param name="Text" select="."/>
		</xsl:call-template>
	</xsl:template>
	<xsl:template name="PreserveLinebreaks">
		<xsl:param name="Text"/>
		<xsl:choose>
			<xsl:when test="contains($Text, '&#xA;')">
				<xsl:value-of select="substring-before($Text, '&#xA;')"/>
				<br/>
				<xsl:call-template name="PreserveLinebreaks">
					<xsl:with-param name="Text" select="substring-after($Text, '&#xA;')"/>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$Text"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<!--Give objective "contemporary" style *RK* -->
	<xsl:template match="objective">
		<table class="MsoNormalTable">
			<tr>
				<td class="SectionDescriptor">
					<p class="SectionTitle">
						<!--take out dynamic section title in order to show shorter (hardcoded) name.  *RK*
						<xsl:value-of select="$objective.word"/>
						-->
						<xsl:text>Objective</xsl:text>
					</p>
				</td>
				<td  class="SectionContent">
					<p class="Summary">
						<xsl:apply-templates/>
					</p>
				</td>
			</tr>
		</table>
	</xsl:template>
	<!-- Give History "contemporary" style *RK* -->
	<xsl:template match="history">
		<table class="MsoNormalTable">
			<tr>
				<td class="SectionDescriptor">
					<p class="SectionTitle">
						<!--take out dynamic section title in order to show shorter (hardcoded) name.  *RK*
						<xsl:value-of select="$history.word"/>
						-->
						<xsl:text>Experience</xsl:text>
					</p>
				</td>
				<td class="SectionContent">
					<p class="Summary">
						<xsl:apply-templates select="job"/>
					</p>
				</td>
			</tr>
		</table>
	</xsl:template>
	<!-- Academics, with "contemporary" style -->
	<xsl:template match="academics">
		<table>
			<tr>
				<td class="SectionDescriptor" >
					<p class="SectionTitle">
						<!--take out dynamic section title in order to show shorter (hardcoded) name.  *RK*
						<xsl:value-of select="$academics.word"/>
						-->
						<xsl:text>Education</xsl:text>
					</p>
				</td>
				<td class="SectionContent">
					<p class="Summary">
						<xsl:apply-templates select="degrees"/>
						<xsl:apply-templates select="note"/>
					</p>
				</td>
			</tr>
		</table>
	</xsl:template>
	<xsl:template match="degrees">
		<p>
			<xsl:apply-templates select="degree"/> 
		</p>
		<xsl:apply-templates select="note"/>
	</xsl:template>
	<xsl:template match="note">
		<div class="note">
			<xsl:apply-templates/>
		</div>
	</xsl:template>
	<xsl:template match="degree">
		<div class="date">
			<xsl:if test="date">
				<xsl:apply-templates select="date/month"/>
				<xsl:text> </xsl:text>
				<xsl:apply-templates select="date/year"/>
				<xsl:text> </xsl:text>
			</xsl:if>
		</div>	
		<div class="institution">
			<xsl:value-of select="institution"/>
		</div>
		<br/>
		<div class="skill" style="font-size:10.0pt">	
					§</div>
		<div class="achievement">
					<acronym class="level">
						<xsl:value-of select="level"/>
					</acronym>
					<xsl:text> </xsl:text>
					<xsl:value-of select="$in.word"/>
					<xsl:text> </xsl:text>
					<xsl:value-of select="major"/>
					<xsl:text> </xsl:text>
				<xsl:if test="subjects/subject">
					<xsl:apply-templates select="subjects"/>
				</xsl:if>
				</div>
			<br/>
		<div class="skill" style="font-size:10.0pt">
				§</div>
				<div class='achievement'>
			<xsl:if test="annotation">
			<xsl:apply-templates select="annotation"/>
			</xsl:if>
				</div>
	</xsl:template>
	<!-- Format the subjects -->
	<xsl:template match="subjects">
		<table>
			<xsl:for-each select="subject">
				<tr>
					<td />
					<td>
						<xsl:value-of select="title"/>
					</td>
					<td width="10"/>
					<td>
						<xsl:value-of select="result"/>
					</td>
				</tr>
			</xsl:for-each>
		</table>
	</xsl:template>
	<!-- Format each job -->
	<xsl:template match="job">
		<xsl:apply-templates select="period"/>
		<div class="employer">
			<xsl:value-of select="employer"/>
		</div>
		<xsl:if test="city">
		<div class="citystate">
			<xsl:apply-templates select="citystate"/>
		</div>
		
		</xsl:if>
		<div class="jobtitle"> 
			<xsl:value-of select="jobtitle"/>
		</div>

		<xsl:apply-templates select="description"/>
		<xsl:if test="projects/project">
			<xsl:value-of select="$projects.word"/>
			<xsl:apply-templates select="projects"/>
		</xsl:if>
		<xsl:if test="achievements/achievement">
			<xsl:apply-templates select="achievements"/>
		</xsl:if>
	</xsl:template>
		<!-- Format the projects section as a bullet list -->
	<xsl:template match="projects">
		<ul>
			<xsl:for-each select="project">
				<li class="achievement">
					<xsl:apply-templates/>
				</li>
			</xsl:for-each>
		</ul>
	</xsl:template>
	<xsl:template match="period">
		<div class="date">
			<xsl:apply-templates select="from"/> - <xsl:apply-templates select="to"/>
		</div>
	</xsl:template>
	<xsl:template match="date">
		<xsl:value-of select="month"/>
		<xsl:text> </xsl:text>
		<xsl:value-of select="year"/>
	</xsl:template>
	<xsl:template match="present">
		<xsl:value-of select="$present.word"/>
	</xsl:template>
	<!-- Format the achievements with "contemporary" styling *RK* -->
	<xsl:template match="achievements">
		<xsl:for-each select="achievement">
			<div class="skill">
				§</div>
				<div class="achievement">
						<xsl:apply-templates/>
				</div>
				<br/>
			</xsl:for-each>
			<p/>
	</xsl:template>
	<!-- Format the open-ended skills -->
	<xsl:template match="skillareas">
		<xsl:apply-templates select="skillarea"/>
	</xsl:template>
	<xsl:template match="skillarea">
		<h2 class="skillareaHeading">
			<xsl:value-of select="title"/>
		</h2>
		<xsl:apply-templates select="skillset"/>
	</xsl:template>
	<xsl:template match="skillset">
		<xsl:choose>
			<xsl:when test="$skills.format = 'comma'">
				<p>
					<xsl:apply-templates select="title" mode="comma"/>
					<xsl:apply-templates select="skills" mode="comma"/>
				</p>
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates select="title" mode="bullet"/>
				<xsl:apply-templates select="skills" mode="bullet"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<xsl:template match="skillset/title" mode="comma">
		<div class="skillsetTitle">
			<xsl:value-of select="."/>
			<xsl:text>: </xsl:text>
		</div>
	</xsl:template>
	<xsl:template match="skillset/title" mode="bullet">
		<h3 class="skillsetTitle">
			<xsl:value-of select="."/>
		</h3>
	</xsl:template>
	<!-- format as a comma-separated list -->
	<xsl:template match="skills" mode="comma">
		<div class="skills">
			<xsl:for-each select="skill[position() != last()]">
				<xsl:apply-templates/>
				<xsl:text>, </xsl:text>
			</xsl:for-each>
			<xsl:apply-templates select="skill[position() = last()]"/>
		</div>
	</xsl:template>
	<!-- format as a bullet list -->
	<xsl:template match="skills" mode="bullet">
		<ul class="skills">
			<xsl:for-each select="skill">
				<li class="skill">
					<xsl:apply-templates/>
				</li>
			</xsl:for-each>
		</ul>
	</xsl:template>
	<!-- Format publications -->
	<xsl:template match="pubs">
		<h2 class="pubsHeading">
			<xsl:value-of select="$publications.word"/>
		</h2>
		<ul class="pubs">
			<xsl:apply-templates select="pub"/>
		</ul>
	</xsl:template>
	<!-- Format a single publication -->
	<xsl:template match="pub">
		<li class="pub">
			<xsl:call-template name="formatPub"/>
		</li>
	</xsl:template>
	<!-- Format the misc info -->
	<xsl:template match="misc">
		<h2 class="miscHeading">
			<xsl:value-of select="$miscellany.word"/>
		</h2>
		<xsl:apply-templates/>
	</xsl:template>
	<!-- Format the legalese -->
	<xsl:template match="copyright">
		<address class="copyright">
			<p>
				<xsl:value-of select="$copyright.word"/>
				<xsl:text> </xsl:text>
				<xsl:value-of select="year"/>
				<xsl:text> </xsl:text>
				<xsl:value-of select="$by.word"/>
				<xsl:text> </xsl:text>
				<xsl:if test="name">
					<xsl:apply-templates select="name"/>
				</xsl:if>
				<xsl:if test="not(name)">
					<xsl:apply-templates select="/resume/header/name"/>
				</xsl:if>
				<xsl:text>. This document uses </xsl:text>
				<a>
				<xsl:attribute name="href">http://xmlresume.sourceforge.net</xsl:attribute>
				XMLResume</a>
				<xsl:value-of select="legalnotice"/>
			</p>
		</address>
	</xsl:template>
	<!-- Put a space between first and last name -->
	<xsl:template match="name">
		<xsl:value-of select="firstname"/>
		<xsl:text> </xsl:text>
		<xsl:if test="middlenames">
			<xsl:value-of select="middlenames"/>
			<xsl:text> </xsl:text>
		</xsl:if>
		<xsl:value-of select="surname"/>
		<xsl:if test="suffix">
			<xsl:text> </xsl:text>
			<xsl:value-of select="suffix"/>
		</xsl:if>
	</xsl:template>
	<!-- para -> p -->
	<xsl:template match="para">
		<p class="para">
			<xsl:apply-templates/>
		</p>
	</xsl:template>
	<!-- emphasis -> strong -->
	<xsl:template match="emphasis">
		<strong class="emphasis">
			<xsl:value-of select="."/>
		</strong>
	</xsl:template>
	<!-- url -> monospace along with href -->
	<xsl:template match="url">
			<a class="urlA">
			<xsl:attribute name="href"><xsl:value-of select="."/></xsl:attribute>
			<xsl:value-of select="."/>
		</a>
	</xsl:template>
	<!-- citation -> cite -->
	<xsl:template match="citation">
		<cite class="citation">
			<xsl:value-of select="."/>
		</cite>
	</xsl:template>
	<!-- Format the referees -->
	<xsl:template match="referees">
		<h2 class="refereesHeading">
			<xsl:value-of select="$referees.word"/>
		</h2>
		<xsl:apply-templates select="referee"/>
	</xsl:template>
	<xsl:template match="referee">
		<h3 class="refereeHeading">
			<xsl:apply-templates select="name"/>
		</h3>
		<p>
			<xsl:apply-templates select="address"/>
			<br/>
			<!-- Don't print the label if the field value is empty *SE* -->
			<xsl:if test="contact/phone">
				<xsl:value-of select="$phone.word"/>: <xsl:value-of select="contact/phone"/>
				<br/>
			</xsl:if>
			<xsl:if test="contact/email">
				<xsl:value-of select="$email.word"/>: <a>
					<xsl:attribute name="href"><xsl:text>mailto:</xsl:text><xsl:value-of select="contact/email"/></xsl:attribute>
					<xsl:value-of select="contact/email"/>
				</a>
				<br/>
			</xsl:if>
			<xsl:if test="contact/url">
				<xsl:value-of select="$url.word"/>: <a>
					<xsl:attribute name="href"><xsl:value-of select="contact/url"/></xsl:attribute>
					<xsl:value-of select="contact/url"/>
				</a>
			</xsl:if>
		</p>
	</xsl:template>
	<!-- Wrap a description up in a div -->
	<xsl:template match="description">
		<div class="description">
			<xsl:apply-templates/>
		</div>
	</xsl:template>
	<!-- Format the memberships with "contemporary" styling *RK* -->
	<xsl:template match="memberships">
		<table class="MsoNormalTable">
			<tr>
				<td class="SectionDescriptor" >
					<p class="SectionTitle" style="margin-top: 4.0pt">
						<xsl:apply-templates select="title"/>
					</p>
				</td>
				<td class="SectionContent">
					<p class="Summary" >
						<xsl:apply-templates select="membership"/>
					</p>
				</td>
			</tr>
		</table>
	</xsl:template>
	<!-- A single membership. -->
	<xsl:template match="membership">
		<xsl:if test="title">
			<div class="membershipTitle">
				<xsl:value-of select="title"/>
			</div>
			<br/>
		</xsl:if>
		<xsl:if test="organization">
			<div class="organization">
				<xsl:value-of select="organization"/>
			</div>
		</xsl:if>
		<xsl:if test="period">
			<xsl:apply-templates select="period"/>
			<br/>
		</xsl:if>
		<xsl:apply-templates select="description"/>
	</xsl:template>
</xsl:stylesheet>
