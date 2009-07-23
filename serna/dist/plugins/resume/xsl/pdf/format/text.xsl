<?xml version="1.0" encoding="UTF-8"?>

<!--
text.xsl
Transform XML resume into plain text.

Copyright (c) 2000-2002 by Vlad Korolev, Sean Kelly, and Bruce Christensen

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

$Id: text.xsl,v 1.14 2002/11/25 02:35:22 brandondoyle Exp $
-->

<!--
In general, each block is responsible for outputting a newline after itself.
-->

<xsl:stylesheet version="1.0"
    xmlns:r="http://xmlresume.sourceforge.net/resume/0.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="text" omit-xml-declaration="yes" indent="no"
    encoding="UTF-8"/>
  <xsl:output doctype-public="-//W3C//DTD HTML 4.0//EN"/>
  <xsl:strip-space elements="*"/>

  <xsl:include href="../params.xsl"/>
  <xsl:include href="../lib/common.xsl"/>
  <xsl:include href="../lib/string.xsl"/>
  <xsl:include href="../lib/textlayout.xsl"/>

  <xsl:template match="/">
    <xsl:apply-templates select="r:resume"/>
    <xsl:call-template name="NewLine"/>
  </xsl:template>

  <!-- Suppress the keywords in the main body of the document -->
  <xsl:template match="r:keywords"/>

  <xsl:template match="r:keyword">
    <!--
    <xsl:value-of select="."/>
    <xsl:if test="position() != last()">
      <xsl:text>, </xsl:text>
    </xsl:if>
    -->
  </xsl:template>

  <xsl:template match="r:header" mode="centered">
    <xsl:call-template name="Center">
      <xsl:with-param name="Text">
        <xsl:apply-templates select="r:name"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:call-template name="NewLine"/>

    <xsl:call-template name="CenterBlock">
      <xsl:with-param name="Text">
        <xsl:if test="r:address">
          <xsl:call-template name="NewLine"/>
          <xsl:apply-templates select="r:address"/>
        </xsl:if>
        <xsl:if test="r:contact">
          <xsl:call-template name="NewLine"/>
          <xsl:apply-templates select="r:contact"/>
        </xsl:if>
      </xsl:with-param>
    </xsl:call-template>

  </xsl:template>

  <xsl:template match="r:header" mode="standard">
    <!-- Your name, address, and stuff. -->
    <xsl:call-template name="Center">
      <xsl:with-param name="Text">
        <xsl:apply-templates select="r:name"/>
        <xsl:text> - </xsl:text>
        <xsl:value-of select="$resume.word"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:call-template name="NewLine"/>

    <xsl:call-template name="Heading">
      <xsl:with-param name="Text" select="$contact.word"/>
    </xsl:call-template>

    <xsl:apply-templates select="r:name"/><xsl:call-template name="NewLine"/>
    <xsl:apply-templates select="r:address"/> 

    <xsl:call-template name="NewLine"/>

    <xsl:apply-templates select="r:contact"/>
  </xsl:template>

  <!-- Objective, with level 2 heading. -->
  <xsl:template match="r:objective">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text" select="$objective.word"/>
    </xsl:call-template>

    <xsl:call-template name="Indent">
      <xsl:with-param name="Text">
        <xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- Contact information -->
  <xsl:template match="r:contact/r:phone">
    <xsl:call-template name="PhoneLocation">
      <xsl:with-param name="Location" select="@location"/>
    </xsl:call-template>
    <xsl:text>: </xsl:text>
    <xsl:apply-templates/>
    <xsl:call-template name="NewLine"/>
  </xsl:template>

  <xsl:template match="r:contact/r:fax">
    <xsl:call-template name="FaxLocation">
      <xsl:with-param name="Location" select="@location"/>
    </xsl:call-template>
    <xsl:text>: </xsl:text>
    <xsl:apply-templates/>
    <xsl:call-template name="NewLine"/>
  </xsl:template>

  <xsl:template match="r:contact/r:pager">
    <xsl:value-of select="$pager.word"/><xsl:text>: </xsl:text>
    <xsl:apply-templates/>
    <xsl:call-template name="NewLine"/>
  </xsl:template>

  <xsl:template match="r:contact/r:email">
    <xsl:value-of select="$email.word"/><xsl:text>: </xsl:text>
    <xsl:apply-templates/>
    <xsl:call-template name="NewLine"/>
  </xsl:template>

  <xsl:template match="r:contact/r:url">
    <xsl:value-of select="$url.word"/><xsl:text>: </xsl:text>
    <xsl:apply-templates/>
    <xsl:call-template name="NewLine"/>
  </xsl:template>

  <xsl:template match="r:contact/r:instantMessage">
    <xsl:call-template name="IMServiceName">
      <xsl:with-param name="Service" select="@service"/>
    </xsl:call-template>
    <xsl:text>: </xsl:text>
    <xsl:apply-templates/>
    <xsl:call-template name="NewLine"/>
  </xsl:template>

  <!-- Addresses, in various modes -->
  <xsl:template match="r:address" mode="free-form">
    <xsl:apply-templates/>
    <xsl:call-template name="NewLine"/>
  </xsl:template>

  <xsl:template match="r:address" mode="standard">
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
      <xsl:value-of select="normalize-space(.)"/>
      <xsl:call-template name="NewLine"/>
    </xsl:for-each>
    <xsl:if test="r:street2">
      <xsl:value-of select="normalize-space(r:street2)"/>
      <xsl:call-template name="NewLine"/>
    </xsl:if>
    <xsl:if test="string-length($CityDivision) &gt; 0">
      <xsl:value-of select="$CityDivision"/>
      <xsl:call-template name="NewLine"/>
    </xsl:if>
    <xsl:value-of select="normalize-space(r:city)"/>
    <xsl:if test="string-length($AdminDivision) &gt; 0">
      <xsl:text>, </xsl:text><xsl:value-of select="$AdminDivision"/>
    </xsl:if>
    <xsl:if test="string-length($PostCode) &gt; 0">
      <xsl:text> </xsl:text>
      <xsl:value-of select="$PostCode"/>
    </xsl:if> 
    <xsl:call-template name="NewLine"/>
    <xsl:if test="r:country">
      <xsl:value-of select="normalize-space(r:country)"/>
      <xsl:call-template name="NewLine"/>
    </xsl:if>
  </xsl:template>

  <xsl:template match="r:address" mode="european">

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
      <xsl:value-of select="normalize-space(.)"/>
      <xsl:call-template name="NewLine"/>
    </xsl:for-each>
    <xsl:if test="r:street2">
      <xsl:value-of select="normalize-space(r:street2)"/>
      <xsl:call-template name="NewLine"/>
    </xsl:if>
    <xsl:if test="string-length($CityDivision) &gt; 0">
      <xsl:value-of select="$CityDivision"/>
      <xsl:call-template name="NewLine"/>
    </xsl:if>
    <xsl:if test="string-length($PostCode) &gt; 0">
      <xsl:value-of select="$PostCode"/>
      <xsl:text> </xsl:text>
    </xsl:if>
    <xsl:value-of select="normalize-space(r:city)"/>
    <xsl:if test="string-length($AdminDivision) &gt; 0">
      <xsl:call-template name="NewLine"/>
      <xsl:value-of select="$AdminDivision"/>
    </xsl:if>
    <xsl:call-template name="NewLine"/>
    <xsl:if test="r:country">
      <xsl:value-of select="normalize-space(r:country)"/>
      <xsl:call-template name="NewLine"/>
    </xsl:if>
  </xsl:template>

  <xsl:template match="r:address" mode="italian">
    <xsl:for-each select="r:street">
      <xsl:value-of select="normalize-space(.)"/>
      <xsl:call-template name="NewLine"/>
    </xsl:for-each>
    <xsl:if test="r:street2">
      <xsl:value-of select="normalize-space(r:street2)"/>
      <xsl:call-template name="NewLine"/>
    </xsl:if>
    <xsl:if test="r:postalCode">
      <xsl:value-of select="normalize-space(r:postalCode)"/>
      <xsl:text> </xsl:text>
    </xsl:if>
    <xsl:value-of select="normalize-space(r:city)"/>
    <xsl:if test="r:province">
      <xsl:text> (</xsl:text>
      <xsl:apply-templates select="r:province"/>
      <xsl:text>)</xsl:text>
    </xsl:if>
    <xsl:call-template name="NewLine"/>
    <xsl:if test="r:country">
      <xsl:value-of select="normalize-space(r:country)"/>
      <xsl:call-template name="NewLine"/>
    </xsl:if>
  </xsl:template>

  <!-- Past jobs, with level 2 heading. -->
  <xsl:template match="r:history">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text" select="$history.word"/>
    </xsl:call-template>

    <xsl:call-template name="Indent">
      <xsl:with-param name="Text">
        <xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="r:location">
    <xsl:value-of select="$location.start"/>
    <xsl:apply-templates/>
    <xsl:value-of select="$location.end"/>
  </xsl:template>

  <!-- Format each job -->
  <xsl:template match="r:job">
    <xsl:apply-templates select="r:jobtitle"/>
    <xsl:call-template name="NewLine"/>
    <xsl:apply-templates select="r:employer"/>
    <xsl:apply-templates select="r:location"/>
    <xsl:call-template name="NewLine"/>
    <xsl:apply-templates select="r:date|r:period"/>

    <xsl:if test="r:description">
      <xsl:call-template name="NewLine"/>
      <xsl:apply-templates select="r:description"/>
    </xsl:if>

    <xsl:if test="r:projects">
      <xsl:call-template name="NewLine"/>
      <xsl:apply-templates select="r:projects"/>
    </xsl:if>

    <xsl:if test="r:achievements">
      <xsl:call-template name="NewLine"/>
      <xsl:apply-templates select="r:achievements"/>
    </xsl:if>

    <xsl:if test="following-sibling::*">
      <xsl:call-template name="NewLine"/>
      <xsl:call-template name="NewLine"/>
    </xsl:if>

  </xsl:template>

  <!-- Format a description as either a block or a single line -->
  <xsl:template match="r:description" name="r:description">
    <!-- Possible values: 'block', 'single-line' -->
    <xsl:param name="paragraph.format">block</xsl:param>
    <xsl:param name="Width" select="$text.width - $text.indent.width"/>

    <xsl:choose>
      <xsl:when test="$paragraph.format = 'single-line'">
        <xsl:call-template name="Wrap">
          <xsl:with-param name="Width" select="$Width"/>
          <xsl:with-param name="Text">
            <xsl:for-each select="r:para">
              <xsl:apply-templates/>
              <xsl:if test="following-sibling::*">
                <xsl:value-of select="$description.para.separator"/>
              </xsl:if>
            </xsl:for-each>
          </xsl:with-param>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise> <!-- block -->
        <xsl:apply-templates>
          <xsl:with-param name="Width" select="$Width"/>
        </xsl:apply-templates>
      </xsl:otherwise>
    </xsl:choose>

  </xsl:template>

  <!-- Format the projects section as a bullet list -->
  <xsl:template match="r:projects">
    <xsl:if test="r:project">
      <xsl:value-of select="$projects.word"/>
      <xsl:call-template name="NewLine"/>
      <xsl:apply-templates select="r:project"/>
    </xsl:if>
  </xsl:template>

  <!-- Format a single project -->
  <xsl:template match="r:project">
    <xsl:variable name="Text">
      <xsl:if test="@title">
        <xsl:value-of select="@title"/>
        <xsl:value-of select="$title.separator"/>
      </xsl:if>
      <xsl:apply-templates/>
    </xsl:variable>
    <xsl:call-template name="FormatBulletListItem">
      <xsl:with-param name="Text">
        <xsl:value-of select="normalize-space($Text)"/>
      </xsl:with-param>
      <xsl:with-param name="Width" select="$text.width - $text.indent.width"/>
    </xsl:call-template>
    <xsl:call-template name="NewLine"/>
  </xsl:template>

  <!-- Format the achievements section as a bullet list *SE* -->
  <xsl:template match="r:achievements">
    <xsl:if test="r:achievement">
      <xsl:if test="string-length($achievements.word) > 0">
        <xsl:value-of select="$achievements.word"/>
        <xsl:call-template name="NewLine"/>
      </xsl:if>
      <xsl:apply-templates select="r:achievement"/>
    </xsl:if>
  </xsl:template>

  <xsl:template match="r:achievement">
    <xsl:variable name="Text">
      <xsl:apply-templates/>
    </xsl:variable>
    <xsl:call-template name="FormatBulletListItem">
      <xsl:with-param name="Text">
        <xsl:value-of select="normalize-space($Text)"/>
      </xsl:with-param>
      <xsl:with-param name="Width" select="$text.width - $text.indent.width"/>
    </xsl:call-template>

    <xsl:call-template name="NewLine"/>
  </xsl:template>
                
  <xsl:template match="r:period">
    <xsl:apply-templates select="r:from"/>-<xsl:apply-templates select="r:to"/>
    <xsl:if test="parent::r:job">
      <xsl:call-template name="NewLine"/>
    </xsl:if>
  </xsl:template>

  <!-- Format a date. -->
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

  <xsl:template match="r:employer | r:jobtitle | r:year | r:month | r:annotation | r:level">
    <xsl:value-of select="normalize-space(.)"/>
  </xsl:template>

  <!-- Degrees and stuff -->
  <xsl:template match="r:academics">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text" select="$academics.word"/>
    </xsl:call-template>

    <xsl:call-template name="Indent">
      <xsl:with-param name="Text">
        <xsl:apply-templates select="r:degrees"/>
        <xsl:if test="r:note">
          <xsl:call-template name="NewLine"/>
          <xsl:apply-templates select="r:note"/>
        </xsl:if>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="r:degrees">
    <xsl:apply-templates select="r:degree"/>
  </xsl:template>

  <xsl:template match="r:note">
      <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="r:degree">
    <xsl:call-template name="Wrap">
      <xsl:with-param name="Text">
        <xsl:apply-templates select="r:level"/>
        <xsl:if test="r:major">
          <xsl:text> </xsl:text>
          <xsl:value-of select="$in.word"/>
          <xsl:text> </xsl:text>
          <xsl:apply-templates select="r:major"/>
        </xsl:if>
        <xsl:apply-templates select="r:minor"/>
        <xsl:if test="r:date|r:period">     
          <xsl:text>, </xsl:text>
          <xsl:apply-templates select="r:date|r:period"/>
        </xsl:if>
        <xsl:if test="r:annotation">
          <xsl:text>. </xsl:text>
          <xsl:apply-templates select="r:annotation"/>
        </xsl:if>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:call-template name="NewLine"/>

    <xsl:if test="r:institution">
      <xsl:call-template name="Wrap">
        <xsl:with-param name="Text">
          <xsl:apply-templates select="r:institution"/>
          <xsl:apply-templates select="r:location"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:call-template name="NewLine"/>
    </xsl:if>
    <xsl:if test="r:gpa">
      <xsl:call-template name="NewLine"/>
      <xsl:apply-templates select="r:gpa"/>
    </xsl:if>
    <xsl:if test="r:subjects/r:subject">
      <xsl:call-template name="NewLine"/>
      <xsl:apply-templates select="r:subjects"/>
    </xsl:if>
    <xsl:if test="r:projects/r:project">
      <xsl:call-template name="NewLine"/>
      <xsl:apply-templates select="r:projects"/>
    </xsl:if>
    <xsl:if test="following-sibling::*">
      <xsl:call-template name="NewLine"/>
    </xsl:if>
  </xsl:template>

  <!-- Format a GPA -->
  <xsl:template match="r:gpa">
    <xsl:call-template name="Wrap">
      <xsl:with-param name="Width" select="$text.width - $text.indent.width -
        floor($text.indent.width div 2)"/>
      <xsl:with-param name="Indent" select="floor($text.indent.width div 2)"/>

      <xsl:with-param name="Text">

        <xsl:choose>
          <xsl:when test="@type = 'major'">
            <xsl:value-of select="$major-gpa.word"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$overall-gpa.word"/>
          </xsl:otherwise>
        </xsl:choose>

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

      </xsl:with-param>
    </xsl:call-template>
    <xsl:call-template name="NewLine"/>
  </xsl:template>

  <!-- Format the subjects as a comma-separated list -->
  <xsl:template match="r:subjects" mode="comma">
    <xsl:call-template name="Wrap">
      <xsl:with-param name="Width" select="$text.width - $text.indent.width -
        floor($text.indent.width div 2)"/>
      <xsl:with-param name="Indent" select="floor($text.indent.width div 2)"/>
      <xsl:with-param name="Text">
        <xsl:value-of select="$subjects.word"/>
        <xsl:value-of select="$title.separator"/>
        <xsl:apply-templates select="r:subject" mode="comma"/>
        <xsl:value-of select="$subjects.suffix"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:call-template name="NewLine"/>
  </xsl:template>

  <!-- Format a subject as part of comma-separated list -->
  <xsl:template match="r:subject" mode="comma">
    <xsl:value-of select="normalize-space(r:title)"/>
    <xsl:if test="$subjects.result.display = 1">
      <xsl:if test="r:result">
        <xsl:value-of select="$subjects.result.start"/>
        <xsl:value-of select="normalize-space(r:result)"/>
        <xsl:value-of select="$subjects.result.end"/>
      </xsl:if>   
    </xsl:if>   
    <xsl:if test="following-sibling::*">
      <xsl:value-of select="$subjects.separator"/>
    </xsl:if>
  </xsl:template>

  <!-- Format the subjects as a 2-column table -->
  <xsl:template match="r:subjects" mode="table">
    <xsl:param name="MaxChars">
      <xsl:call-template name="MaxSubjectTitleLength"/>
    </xsl:param>

    <!-- This block has to be a <param> (instead of a <with-param> within the
    <call-template> below) to work around a bug in Xalan. -->
    <xsl:param name="Text">
      <xsl:value-of select="$subjects.word"/>
      <xsl:for-each select="r:subject">
        <xsl:apply-templates select="r:title"/>
        <!-- Pad over to the second column -->
        <xsl:call-template name="NSpace">
          <xsl:with-param
            name="n"
            select="2 + $MaxChars - string-length(normalize-space(r:title))"/>
        </xsl:call-template>

        <xsl:apply-templates select="r:result"/>
        <xsl:if test="following-sibling::*">
          <xsl:call-template name="NewLine"/>
        </xsl:if>
      </xsl:for-each>
    </xsl:param>

    <xsl:call-template name="Indent">
      <xsl:with-param name="Text" select="$Text"/>
    </xsl:call-template>

    <xsl:call-template name="NewLine"/>
  </xsl:template>

  <!-- When called with an r:subjects as the context node, returns an integer
  indicating number of characters in the longest subject title -->
  <xsl:template name="MaxSubjectTitleLength">
    <xsl:param name="SubjectIndex">1</xsl:param>

    <xsl:choose>
      <xsl:when test="$SubjectIndex &lt;= count(r:subject)">

        <xsl:variable name="CurLen" select="string-length(normalize-space(r:subject[$SubjectIndex]/r:title))"/>

        <xsl:variable name="MaxFollowingLen">
          <xsl:call-template name="MaxSubjectTitleLength">
            <xsl:with-param name="SubjectIndex" select="$SubjectIndex + 1"/>
          </xsl:call-template>
        </xsl:variable>

        <xsl:choose>
          <xsl:when test="$CurLen > $MaxFollowingLen">
            <xsl:value-of select="$CurLen"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$MaxFollowingLen"/>
          </xsl:otherwise>
        </xsl:choose>

      </xsl:when>
      <xsl:otherwise> <!-- $SubjectIndex > count(r:subject) -->
        <xsl:text>0</xsl:text>
      </xsl:otherwise>
    </xsl:choose>

  </xsl:template>

  <xsl:template match="r:skillarea">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text">
        <xsl:apply-templates select="r:title"/>
      </xsl:with-param>
    </xsl:call-template>

    <xsl:call-template name="Indent">
      <!-- We use ceiling so that the total indent of skills from the margin
      is exactly equal to $text.indent.width. If we used floor in both places
      and text.indent.width was odd (e.g. 3), then the total skill indent would
      be $text.indent.width - 1. -->
      <xsl:with-param name="Length" select="ceiling($text.indent.width div 2)"/>
      <xsl:with-param name="Text">
        <xsl:apply-templates select="r:skillset"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="r:skillset">
    <xsl:choose>
      <xsl:when test="$skills.format = 'comma'">
        <xsl:apply-templates select="." mode="comma"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="." mode="bullet"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- format a skillset as comma-separated lists.  Need to use -->
  <!-- Wrap so long lists wrap onto multiple lines.  -->
  <xsl:template match="r:skillset" mode="comma">
    <xsl:call-template name="Wrap">
      <xsl:with-param name="Width"
        select="$text.width - floor($text.indent.width div 2)"/>
      <xsl:with-param name="Indent" select="floor($text.indent.width div 2)"/>
      <xsl:with-param name="Text">
        <xsl:if test="r:title">
          <xsl:apply-templates select="r:title">
	    <xsl:with-param name="Separator" select="$title.separator"/>
	  </xsl:apply-templates>
        </xsl:if>

        <xsl:apply-templates select="r:skill" mode="comma"/>
        <!-- The following line should be removed in a future version. -->
        <xsl:apply-templates select="r:skills" mode="comma"/>

        <xsl:value-of select="$skills.suffix"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:call-template name="NewLine"/>

    <xsl:if test="following-sibling::*">
      <xsl:call-template name="NewLine"/>
    </xsl:if>
  </xsl:template>

  <xsl:template match="r:skillset" mode="bullet">
    <xsl:if test="r:title">
      <xsl:apply-templates select="r:title"/>
      <xsl:call-template name="NewLine"/>
      <xsl:call-template name="NewLine"/>
    </xsl:if>
    <xsl:call-template name="Indent">
      <xsl:with-param name="Text">
        <xsl:apply-templates select="r:skill" mode="bullet"/>
        <!-- The following line should be removed in a future version. -->
        <xsl:apply-templates select="r:skills" mode="bullet"/>
      </xsl:with-param>
      <xsl:with-param name="Length" select="2"/>
    </xsl:call-template>

    <xsl:if test="following-sibling::*">
      <xsl:call-template name="NewLine"/>
    </xsl:if>
  </xsl:template>

  <!-- Format individual skill as part of a comma-separated list -->
  <xsl:template match="r:skill" mode="comma">
    <xsl:variable name="Text">
      <xsl:apply-templates/>
    </xsl:variable>
    <xsl:value-of select="normalize-space($Text)"/>
    <xsl:apply-templates select="@level"/>
    <xsl:if test="following-sibling::r:skill">
      <xsl:text>, </xsl:text>
    </xsl:if>
  </xsl:template>

  <!-- Format individual skill as a bullet item -->
  <xsl:template match="r:skill" mode="bullet">
    <xsl:variable name="Text">
      <xsl:apply-templates/>
      <xsl:apply-templates select="@level"/>
    </xsl:variable>
    <xsl:call-template name="FormatBulletListItem">
      <xsl:with-param name="Text">
        <xsl:value-of select="normalize-space($Text)"/>
      </xsl:with-param>
      <xsl:with-param name="Width" select="$text.width - $text.indent.width"/>
    </xsl:call-template>
    <xsl:call-template name="NewLine"/>
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
      <xsl:with-param name="Text" select="$publications.word"/>
    </xsl:call-template>

    <xsl:apply-templates select="r:pub"/>
  </xsl:template>

  <xsl:template match="r:pub">
    <xsl:variable name="Text">   
      <xsl:call-template name="FormatPub"/>
    </xsl:variable>

    <xsl:call-template name="Wrap">
      <xsl:with-param name="Indent" select="$text.indent.width"/>
      <xsl:with-param name="Text">
        <xsl:value-of select="normalize-space($Text)"/>
      </xsl:with-param>
    </xsl:call-template>

    <xsl:call-template name="NewLine"/>

    <xsl:if test="following-sibling::*">
      <xsl:call-template name="NewLine"/>
    </xsl:if>
  </xsl:template>

  <!-- Format memberships. -->
  <xsl:template match="r:memberships">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text">
        <xsl:apply-templates select="r:title"/>
      </xsl:with-param>
    </xsl:call-template>

    <xsl:apply-templates select="r:membership"/>
  </xsl:template>

  <xsl:template match="r:membership">
    <xsl:call-template name="Indent">
      <xsl:with-param name="Text">

        <xsl:if test="r:title">
          <xsl:call-template name="Wrap">
            <xsl:with-param name="FirstIndent" select="0"/>
            <xsl:with-param name="Indent" select="$text.indent.width"/>
            <xsl:with-param name="Text">
              <xsl:apply-templates select="r:title"/>
            </xsl:with-param>
          </xsl:call-template>
          <xsl:call-template name="NewLine"/>
        </xsl:if>

        <xsl:if test="r:organization">
          <xsl:call-template name="Wrap">
            <xsl:with-param name="FirstIndent" select="0"/>
            <xsl:with-param name="Indent" select="$text.indent.width"/>
            <xsl:with-param name="Text">
              <xsl:apply-templates select="r:organization"/>
              <xsl:apply-templates select="r:location"/>
            </xsl:with-param>
          </xsl:call-template>
          <xsl:call-template name="NewLine"/>
        </xsl:if>

        <xsl:if test="r:date|r:period">
          <xsl:call-template name="Wrap">
            <xsl:with-param name="FirstIndent" select="0"/>
            <xsl:with-param name="Indent" select="$text.indent.width"/>
            <xsl:with-param name="Text">
              <xsl:apply-templates select="r:date|r:period"/>
            </xsl:with-param>
          </xsl:call-template>
          <xsl:call-template name="NewLine"/>
        </xsl:if>

        <xsl:if test="r:description">
          <xsl:call-template name="NewLine"/>
          <xsl:apply-templates select="r:description"/>
        </xsl:if>

      </xsl:with-param>
    </xsl:call-template>

    <xsl:if test="following-sibling::*">
      <xsl:call-template name="NewLine"/>
      <xsl:call-template name="NewLine"/>
    </xsl:if>
      
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

    <xsl:call-template name="Indent">
      <xsl:with-param name="Text">
        <xsl:apply-templates select="r:interest"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- A single interest. -->
  <xsl:template match="r:interest">
    <xsl:call-template name="FormatBulletListItem">
      <xsl:with-param name="Width" select="$text.width - $text.indent.width"/>
      <xsl:with-param name="Text">
        <xsl:apply-templates select="r:title"/>
        <xsl:if test="r:description">
          <xsl:choose>
            <xsl:when test="$interest.description.format = 'single-line'">
              <xsl:text>. </xsl:text>
              <xsl:apply-templates select="r:description"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:call-template name="NewLine"/>
              <xsl:apply-templates select="r:description"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:if>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:if test="$interest.description.format = 'single-line'">
      <xsl:call-template name="NewLine"/>
      <!-- Block-formatted descriptions already provide their own trailing
      newline, courtesy of the r:para template. -->
    </xsl:if>
  </xsl:template>

  <!-- Format an interest description -->
  <xsl:template match="r:interest/r:description">
    <xsl:call-template name="r:description">
      <xsl:with-param name="Width"
        select="$text.width - $text.indent.width"/>
      <xsl:with-param name="paragraph.format"
        select="$interest.description.format"/>
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
    <xsl:call-template name="Indent">
      <xsl:with-param name="Text">
        <xsl:apply-templates select="r:clearance"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- Format a single security clearance. -->
  <xsl:template match="r:clearance">
    <xsl:call-template name="FormatBulletListItem">
      <xsl:with-param name="Text">
        <xsl:apply-templates select="r:level"/>
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
      </xsl:with-param>
    </xsl:call-template>
    <xsl:call-template name="NewLine"/>
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
    <xsl:call-template name="Indent">
      <xsl:with-param name="Text">
        <xsl:apply-templates select="r:award"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  
  <!-- Format a single award. -->
  <xsl:template match="r:award">
    <xsl:call-template name="FormatBulletListItem">
      <xsl:with-param name="Text">
        <xsl:apply-templates select="r:title"/>
        <xsl:if test="r:organization"><xsl:text>, </xsl:text></xsl:if>
        <xsl:apply-templates select="r:organization"/>
        <xsl:if test="r:date|r:period"><xsl:text>, </xsl:text></xsl:if>
        <xsl:apply-templates select="r:date|r:period"/>
        <xsl:call-template name="NewLine"/>
        <xsl:apply-templates select="r:description">
          <xsl:with-param name="Width"
            select="$text.width - $text.indent.width - string-length($text.bullet.prefix)"/>
        </xsl:apply-templates>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- Format the misc info -->
  <xsl:template match="r:misc">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text" select="$miscellany.word"/>
    </xsl:call-template>

    <xsl:variable name="Text">
      <xsl:apply-templates/>
    </xsl:variable>
    <xsl:call-template name="Indent">
      <xsl:with-param name="Text">
        <xsl:value-of select="$Text"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- Format the "last modified" date -->
  <xsl:template match="r:lastModified">
    <xsl:call-template name="NewLine"/>
    <xsl:call-template name="NewLine"/>

    <xsl:call-template name="Wrap">
      <xsl:with-param name="Width" select="$text.width"/>
      <xsl:with-param name="Text">
        <xsl:value-of select="$last-modified.phrase"/>
        <xsl:text> </xsl:text>
        <xsl:apply-templates/>
        <xsl:text>.</xsl:text>
      </xsl:with-param>
    </xsl:call-template>

  </xsl:template>

  <!-- Format the legalese -->
  <xsl:template match="r:copyright">
    <xsl:call-template name="NewLine"/>
    <xsl:call-template name="NewLine"/>

    <xsl:call-template name="Wrap">
      <xsl:with-param name="Text">
        <xsl:value-of select="$copyright.word"/>
        <xsl:text> </xsl:text>
        <xsl:apply-templates select="r:year"/>
        <xsl:text> </xsl:text>
        <xsl:value-of select="$by.word"/>
        <xsl:text> </xsl:text>
        <xsl:apply-templates select="r:name"/>
        <xsl:if test="not(r:name)">
          <xsl:apply-templates select="/r:resume/r:header/r:name"/>
        </xsl:if>
        <xsl:text>. </xsl:text>
        <xsl:apply-templates select="r:legalnotice"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- para -> p -->
  <xsl:template match="r:para">
    <xsl:param name="Width" select="$text.width - $text.indent.width"/>

    <!-- Format Paragraph -->
    <xsl:variable name="Text">
      <xsl:apply-templates/>
    </xsl:variable>
    <xsl:call-template name="Wrap">
      <xsl:with-param name="Width" select="$Width"/>
      <xsl:with-param name="Text">
        <xsl:value-of select="normalize-space($Text)"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:call-template name="NewLine"/>
    <xsl:if test="following-sibling::*">
      <xsl:call-template name="NewLine"/>
    </xsl:if>
  </xsl:template>

  <!-- emphasis -> strong -->
  <xsl:template match="r:emphasis">
    <xsl:value-of select="$text.emphasis.start"/>
    <xsl:value-of select="."/> 
    <xsl:value-of select="$text.emphasis.end"/>
  </xsl:template>

  <!-- url -> monospace along with href -->
  <xsl:template match="r:url" name="FormatUrl">
        <xsl:value-of select="."/>
  </xsl:template>

  <!-- citation -> cite -->
  <xsl:template match="r:citation">
    <xsl:value-of select="."/>
  </xsl:template>

  <!-- Format the referees -->
  <xsl:template match="r:referees">
    <xsl:call-template name="Heading">
      <xsl:with-param name="Text" select="$referees.word"/>
    </xsl:call-template>

    <xsl:call-template name="Indent">
      <xsl:with-param name="Text">
        <xsl:choose>

          <xsl:when test="$referees.display = 1">
            <xsl:apply-templates select="r:referee"/>
          </xsl:when>

          <xsl:otherwise>
            <xsl:call-template name="Wrap">
              <xsl:with-param name="Text">
                <xsl:value-of select="$referees.hidden.phrase"/>
              </xsl:with-param>
            </xsl:call-template>
            <xsl:call-template name="NewLine"/>
          </xsl:otherwise>

        </xsl:choose>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="r:referee">
    <!-- Your name, address, and stuff. -->
    <xsl:apply-templates select="r:name"/><xsl:call-template name="NewLine"/>

    <xsl:if test="r:title or r:organization">
      <xsl:call-template name="Wrap">
        <xsl:with-param name="Text">
          <xsl:apply-templates select="r:title"/>
          <xsl:if test="r:title and r:organization">
            <xsl:text>, </xsl:text>
          </xsl:if>
          <xsl:apply-templates select="r:organization"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:call-template name="NewLine"/>
    </xsl:if>

    <xsl:apply-templates select="r:address"/>
    <xsl:apply-templates select="r:contact"/>

    <xsl:if test="following-sibling::*">
      <xsl:call-template name="NewLine"/>
    </xsl:if>
  </xsl:template>

</xsl:stylesheet>
