<?xml version="1.0" encoding="UTF-8"?>

<!--
params.xsl
Default XML resume transformation parameters.

Copyright (c) 2000-2001 Sean Kelly
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

$Id: params.xsl,v 1.17 2002/11/28 00:49:03 brandondoyle Exp $
-->

<xsl:stylesheet version="1.0"
  xmlns:r="http://xmlresume.sourceforge.net/resume/0.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:import href="country/us.xsl"/>

  <!-- USER-CHANGEABLE SETTINGS =========================================== -->

  <!-- Format for skills lists; available values are: -->
  <!-- 'bullet' for bulleted lists -->
  <!-- 'comma' for comma-separated lists -->
  <xsl:param name="skills.format">comma</xsl:param>

  <!-- Should skill <level> elements be displayed when formatting? -->
  <xsl:param name="skills.level.display">1</xsl:param>

  <!-- Should referees be displayed when formatting? -->
  <!-- '1' to display referees -->
  <!-- '0' to display referees.hidden.phrase instead. -->
  <xsl:param name="referees.display">1</xsl:param>

  <!-- Layout of the referees section: "compact" or "standard" -->
  <!-- Does not affect the layout of the text output, only html and pdf -->
  <xsl:param name="referees.layout">compact</xsl:param>

  <!-- Format for name/contact header: 'standard' or 'centered' *SE* -->
  <!-- Set here or override on command line -->
  <xsl:param name="header.format">standard</xsl:param>

  <!-- Format of interest descriptions. Available values are: -->
  <!-- 'single-line' for <para>s on same line as title, separated by dashes -->
  <!-- 'block' for typical block-style paragraphs -->
  <xsl:param name="interest.description.format">single-line</xsl:param>

  <!-- Format for address; available values are: -->
  <!-- 'standard' for US/Canadian/UK style addresses -->
  <!-- 'european' for European format (with postal code preceding city). -->
  <!-- 'italian' for Italian format (postal code city (province) ). -->
  <xsl:param name="address.format">standard</xsl:param>

  <!-- SEPARATORS AND DELIMITERS (all formats) ============================ -->

  <!-- Separator between a list title and its list -->
  <!-- (applies only to comma-separated lists) -->
  <xsl:param name="title.separator">: </xsl:param>

  <!-- Separator between individual skills -->
  <!-- (applies only when skills.format = comma) -->
  <xsl:param name="skills.separator">, </xsl:param>

  <!-- Output after the last skill in a skillset -->
  <!-- (applies only when skills.format = comma) -->
  <xsl:param name="skills.suffix">.</xsl:param>

  <!-- Text to use to indicate start and end of skill level in all formats -->
  <xsl:param name="skills.level.start"> (</xsl:param>
  <xsl:param name="skills.level.end">)</xsl:param>

  <!-- Word to use for "subjects", eg "Courses" or "Coursework" -->
  <xsl:param name="subjects.word">Subjects</xsl:param>

  <!-- Format for subjects lists; available values are: -->
  <!-- 'table' for a subjects table -->
  <!-- 'comma' for comma-separated lists -->
  <xsl:param name="subjects.format">comma</xsl:param>

  <!-- Separator between individual subjects
  (applies only when subjects.format = comma) -->
  <xsl:param name="subjects.separator">, </xsl:param>

  <!-- Output after the last subject in a list
  (applies only when subjects.format = comma) -->
  <xsl:param name="subjects.suffix">.</xsl:param>

  <!-- Should result elements be displayed when formatting? -->
  <xsl:param name="subjects.result.display">1</xsl:param>

  <!-- Text to use to indicate start and end of subject -->
  <!-- (applies only when subjects.format = comma -->
  <xsl:param name="subjects.result.start"> (</xsl:param>
  <xsl:param name="subjects.result.end">)</xsl:param>

  <!-- Text to use to indicate start and end of location in all formats -->
  <xsl:param name="location.start"> (</xsl:param>
  <xsl:param name="location.end">)</xsl:param>

  <!-- Separator between <para>s in a description. -->
  <!-- (applies only when interest.description.format = single-line) -->
  <!-- Note: &#x2014; == em-dash -->
  <xsl:param name="description.para.separator"> &#x2014; </xsl:param>

  <!-- Default separator between authors in publication details.  -->
  <xsl:param name="pub.author.separator">, </xsl:param>

  <!-- Default separator between items in publication details.  -->
  <xsl:param name="pub.item.separator">. </xsl:param>

  <!-- HTML-SPECIFIC ====================================================== -->

  <!-- Cascading stylesheet to use -->
  <!-- Make sure the path to the css file is correct! -->
  <xsl:param name="css.href">../../css/compact.css</xsl:param>

  <!-- Should style sheets be embedded within the html file? -->
  <!-- '1' to embed the css in the html (allows for easy emailing) -->
  <!-- '0' to link to the css in a seperate file -->
  <xsl:param name="css.embed">1</xsl:param>

  <!-- PLAIN TEXT-SPECIFIC ================================================ -->

  <!-- Bullet equivalent in plain text *SE* -->
  <xsl:param name="text.bullet.prefix">* </xsl:param>

  <!-- Text to use to indicate start and end of emphasis in plain text -->
  <xsl:param name="text.emphasis.start">*</xsl:param>
  <xsl:param name="text.emphasis.end">*</xsl:param>

  <!-- Max chars allowed on a line in plain text -->
  <xsl:param name="text.width">72</xsl:param>

  <!-- Number of characters to indent in plain text -->
  <xsl:param name="text.indent.width">4</xsl:param>

  <!-- XSL-FO-SPECIFIC ==================================================== -->

  <!-- Settings for lines around the header of the print resume -->
  <xsl:param name="header.line.pattern">rule</xsl:param>
  <xsl:param name="header.line.thickness">1pt</xsl:param>

  <!-- Space betwixt paragraphs -->
  <xsl:param name="para.break.space">0.750em</xsl:param>

  <!-- Half space; for anywhere line spacing is needed but should be less -->
  <!-- than a full paragraph break; between comma-separated skills lists, -->
  <!-- between job header and description/achievements. -->
  <xsl:param name="half.space">0.4em</xsl:param>

  <!-- Bullet Symbol -->
  <xsl:param name="bullet.glyph">&#x2022;</xsl:param>
  <!-- Space between bullet and its text in bulleted item -->
  <xsl:param name="bullet.space">1.0em</xsl:param>

  <xsl:param name="header.name.font.style">normal</xsl:param>
  <xsl:param name="header.name.font.weight">bold</xsl:param>
  <xsl:param name="header.name.font.size" select="$body.font.size"/>

  <xsl:param name="header.item.font.style">italic</xsl:param>

  <xsl:param name="body.font.size">10pt</xsl:param>
  <xsl:param name="body.font.family">serif</xsl:param>

  <xsl:param name="footer.font.size">8pt</xsl:param>
  <xsl:param name="footer.font.family">serif</xsl:param>

  <xsl:param name="heading.font.size">10pt</xsl:param>
  <xsl:param name="heading.font.family">sans-serif</xsl:param>
  <xsl:param name="heading.font.weight">bold</xsl:param>
  <xsl:param name="heading.border.bottom.style">none</xsl:param>
  <xsl:param name="heading.border.bottom.width">thin</xsl:param>

  <!-- Used for copyright notice and "last modified" date -->
  <xsl:param name="fineprint.font.size">8pt</xsl:param>

  <xsl:param name="emphasis.font.weight">bold</xsl:param>
  <xsl:param name="citation.font.style">italic</xsl:param>
  <xsl:param name="url.font.family">monospace</xsl:param>

  <xsl:param name="jobtitle.font.style">normal</xsl:param>
  <xsl:param name="jobtitle.font.weight">bold</xsl:param>

  <!-- Used on degree major and level -->
  <xsl:param name="degree.font.style">normal</xsl:param>
  <xsl:param name="degree.font.weight">bold</xsl:param>

  <xsl:param name="referee-name.font.style">italic</xsl:param>
  <xsl:param name="referee-name.font.weight">normal</xsl:param>

  <xsl:param name="employer.font.style">italic</xsl:param>
  <xsl:param name="employer.font.weight">normal</xsl:param>

  <xsl:param name="job-period.font.style">italic</xsl:param>
  <xsl:param name="job-period.font.weight">normal</xsl:param>

  <!-- Used for "Projects" and "Achievements" -->
  <xsl:param name="job-subheading.font.style">italic</xsl:param>
  <xsl:param name="job-subheading.font.weight">normal</xsl:param>

  <xsl:param name="skillset-title.font.style">italic</xsl:param>
  <xsl:param name="skillset-title.font.weight">normal</xsl:param>

  <xsl:param name="degrees-note.font.style">italic</xsl:param>
  <xsl:param name="degrees-note.font.weight">normal</xsl:param>

  <xsl:param name="clearance-level.font.style">italic</xsl:param>
  <xsl:param name="clearance-level.font.weight">normal</xsl:param>

  <!-- Used on "*Overall GPA*: 3.0" -->
  <xsl:param name="gpa-preamble.font.style">italic</xsl:param>
  <xsl:param name="gpa-preamble.font.weight">normal</xsl:param>

</xsl:stylesheet>
