<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
    xmlns:r="http://xmlresume.sourceforge.net/resume/0.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:output method="xml" omit-xml-declaration="no" indent="yes"
    encoding="UTF-8"/>
  <xsl:strip-space elements="*"/>

  <xsl:include href="../params.xsl"/>
  <xsl:include href="../lib/common.xsl"/>
  <xsl:include href="../lib/string.xsl"/>

  <xsl:param name="page.height">297mm</xsl:param> 
  <xsl:param name="page.width">210mm</xsl:param> 
  <xsl:param name="margin.top">20mm</xsl:param> 
  <xsl:param name="margin.left">20mm</xsl:param> 
  <xsl:param name="margin.right">20mm</xsl:param> 
  <xsl:param name="margin.bottom">20mm</xsl:param> 
  <xsl:param name="body.indent">20mm</xsl:param> 
  <xsl:param name="heading.indent">0mm</xsl:param> 
  <xsl:param name="header.margin-left">50mm</xsl:param> 
  <xsl:param name="header.margin-right" select="$header.margin-left"/> 

  <xsl:template match="/">
    <fo:root>
      <fo:layout-master-set>
        <fo:simple-page-master master-name="resume-page"
          margin-top="{$margin.top}"
          margin-left="{$margin.left}"
          margin-right="{$margin.right}"
          margin-bottom="0in"
          page-height="{$page.height}"
          page-width="{$page.width}">
          <!-- FIXME: should be error-if-overflow, but fop0.20.3 doesn't support it -->
          <fo:region-body overflow="hidden"
            margin-bottom="{$margin.bottom}"/>
          <!-- FIXME: should be error-if-overflow, but fop0.20.3 doesn't support it -->
          <fo:region-after overflow="hidden"
            extent="{$margin.bottom}"/>
        </fo:simple-page-master>
      </fo:layout-master-set>
      <fo:page-sequence master-reference="resume-page">
        <!-- Running footer with person's name and page number. -->
        <fo:static-content flow-name="xsl-region-after">
          <fo:block text-align="start" font-size="{$footer.font.size}"
            font-family="{$footer.font.family}" padding-top="{$para.break.space}">
            <xsl:apply-templates select="r:resume/r:header/r:name"/>
            <xsl:text> - </xsl:text>
            <xsl:value-of select="$resume.word"/>
            <xsl:text> - </xsl:text>
            <xsl:value-of select="$page.word"/>
            <xsl:text> </xsl:text>
            <fo:page-number/>
          </fo:block>
        </fo:static-content>

        <fo:flow flow-name="xsl-region-body">
          <!-- Main text is indented from start side. -->
          <fo:block start-indent="{$body.indent}"
            font-family="{$body.font.family}"
            font-size="{$body.font.size}">
            <xsl:apply-templates select="r:resume"/>
          </fo:block>
        </fo:flow>
      </fo:page-sequence>
    </fo:root>
  </xsl:template>

  <!-- Callable template to format a heading: -->
  <!--
      Call "heading" with parameter "text" being the text of the heading.
      GH: As heading.indent is less than body.indent, this is a hanging
          indent of the heading.
  -->
  <xsl:template name="heading">
    <xsl:param name="text">Heading Not Defined</xsl:param>
    <fo:block
      start-indent="{$heading.indent}"
      font-size="{$heading.font.size}"
      font-family="{$heading.font.family}"
      font-weight="{$heading.font.weight}"
      space-before="{$para.break.space}"
      space-after="{$para.break.space}"
      border-bottom-style="{$heading.border.bottom.style}"
      border-bottom-width="{$heading.border.bottom.width}"
      keep-with-next="always">
      <xsl:value-of select="$text"/>
    </fo:block>
  </xsl:template>
  
  <!-- Header information -->
  <xsl:template match="r:header" mode="standard">
    <fo:block space-after="{$para.break.space}">
      <fo:block
          font-style="{$header.name.font.style}"
          font-weight="{$header.name.font.weight}"
          font-size="{$header.name.font.size}">
        <xsl:apply-templates select="r:name"/>
      </fo:block>
      <xsl:apply-templates select="r:address"/>
      <fo:block space-before="{$half.space}">
        <xsl:apply-templates select="r:contact"/>
      </fo:block>
    </fo:block>
  </xsl:template>

  <xsl:template match="r:header" mode="centered">
    <fo:block
        space-after="{$para.break.space}"
        start-indent="{$header.margin-left}"
        end-indent="{$header.margin-right}">
      <fo:leader leader-length="100%" leader-pattern="{$header.line.pattern}"
        rule-thickness="{$header.line.thickness}"/>
      <fo:block
          font-style="{$header.name.font.style}"
          font-weight="{$header.name.font.weight}"
          font-size="{$header.name.font.size}">
        <xsl:apply-templates select="r:name"/>
      </fo:block>
      <xsl:apply-templates select="r:address"/>
      <fo:block space-before="{$half.space}">
        <xsl:apply-templates select="r:contact"/>
      </fo:block>
      <fo:leader leader-length="100%" leader-pattern="{$header.line.pattern}"
        rule-thickness="{$header.line.thickness}"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="r:address" mode="free-form">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="r:address" mode="standard">

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

    <fo:block>
      <xsl:for-each select="r:street">
        <fo:block><xsl:apply-templates/></fo:block>
      </xsl:for-each>
      <xsl:if test="r:street2">
        <fo:block><xsl:apply-templates select="r:street2"/></fo:block>
      </xsl:if>
      <xsl:if test="string-length($CityDivision) &gt; 0">
        <fo:block><xsl:value-of select="$CityDivision"/></fo:block>
      </xsl:if>
      <fo:block>
        <xsl:apply-templates select="r:city"/>
        <xsl:if test="string-length($AdminDivision) &gt; 0">
          <xsl:text>, </xsl:text><xsl:value-of select="$AdminDivision"/>
        </xsl:if>
        <xsl:if test="string-length($PostCode) &gt; 0">
          <xsl:text> </xsl:text><xsl:value-of select="$PostCode"/>
        </xsl:if>
      </fo:block>
        <xsl:if test="r:country">
          <fo:block>
            <xsl:apply-templates select="r:country"/>
          </fo:block>
        </xsl:if>
      </fo:block>
  </xsl:template>

  <xsl:template match="r:address" mode="european">

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

    <fo:block>
      <xsl:for-each select="r:street">
        <fo:block><xsl:apply-templates/></fo:block>
      </xsl:for-each>
      <xsl:if test="r:street2">
        <fo:block><xsl:apply-templates select="r:street2"/></fo:block>
      </xsl:if>
      <xsl:if test="string-length($CityDivision) &gt; 0">
        <fo:block><xsl:value-of select="$CityDivision"/></fo:block>
      </xsl:if>
      <fo:block>
        <xsl:if test="string-length($PostCode) &gt; 0">
          <xsl:value-of select="$PostCode"/><xsl:text> </xsl:text>
        </xsl:if>
	</fo:block><fo:block>
        <xsl:apply-templates select="r:city"/>
      </fo:block>
        <xsl:if test="string-length($AdminDivision) &gt; 0">
          <fo:block>
          <xsl:value-of select="$AdminDivision"/>
          </fo:block>
        </xsl:if>
        <xsl:if test="r:country">
          <fo:block>
          <xsl:apply-templates select="r:country"/>
          </fo:block>
        </xsl:if>
    </fo:block>
  </xsl:template>

  <xsl:template match="r:address" mode="italian">

    <fo:block>
      <xsl:for-each select="r:street">
        <fo:block><xsl:apply-templates/></fo:block>
      </xsl:for-each>
      <xsl:if test="r:street2">
        <fo:block><xsl:apply-templates select="r:street2"/></fo:block>
      </xsl:if>
      <fo:block>
        <xsl:if test="r:postalCode">
          <xsl:apply-templates select="r:postalCode"/><xsl:text> </xsl:text>
        </xsl:if>
        <xsl:apply-templates select="r:city"/>
        <xsl:if test="r:province">
          <xsl:text> (</xsl:text><xsl:apply-templates select="r:province"/>
          <xsl:text>)</xsl:text>
        </xsl:if>
      </fo:block>
        <xsl:if test="r:country">
          <fo:block>
            <xsl:apply-templates select="r:country"/>
          </fo:block>
        </xsl:if>
    </fo:block>
  </xsl:template>

  <!-- Preserve line breaks within a free format address -->
  <xsl:template match="r:address//text()">
    <xsl:call-template name="String-Replace">
      <xsl:with-param name="Text" select="."/>
      <xsl:with-param name="Search-For">
        <xsl:text>&#xA;</xsl:text>
      </xsl:with-param>
      <xsl:with-param name="Replace-With">
        <fo:block/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>



  <!-- Named template to format a single contact field *SE* -->
  <!-- Don't print the label if the field value is empty *SE* -->
  <xsl:template name="contact">
    <xsl:param name="label"/>
    <xsl:param name="field"/>
    <xsl:if test="string-length($field) > 0">
      <fo:block>
        <fo:inline font-style="{$header.item.font.style}"><xsl:value-of select="$label"/>:</fo:inline>
        <xsl:text> </xsl:text>
        <xsl:value-of select="$field"/>
      </fo:block>
    </xsl:if>
  </xsl:template>

  <!-- Format contact information. -->

  <xsl:template match="r:contact/r:phone">
    <xsl:call-template name="contact">
      <xsl:with-param name="label">
        <xsl:call-template name="PhoneLocation">
          <xsl:with-param name="Location" select="@location"/>
        </xsl:call-template>
      </xsl:with-param>
      <xsl:with-param name="field">
        <xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="r:contact/r:fax">
    <xsl:call-template name="contact">
      <xsl:with-param name="label">
        <xsl:call-template name="FaxLocation">
          <xsl:with-param name="Location" select="@location"/>
        </xsl:call-template>
      </xsl:with-param>
      <xsl:with-param name="field">
        <xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="r:contact/r:pager">
    <xsl:call-template name="contact">
      <xsl:with-param name="label">
        <xsl:value-of select="$pager.word"/>
      </xsl:with-param>
      <xsl:with-param name="field">
        <xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="r:contact/r:email">
    <xsl:call-template name="contact">
      <xsl:with-param name="label">
        <xsl:value-of select="$email.word"/>
      </xsl:with-param>
      <xsl:with-param name="field">
        <xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="r:contact/r:url">
    <xsl:call-template name="contact">
      <xsl:with-param name="label">
        <xsl:value-of select="$url.word"/>
      </xsl:with-param>
      <xsl:with-param name="field">
        <xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="r:contact/r:instantMessage">
    <xsl:call-template name="contact">
      <xsl:with-param name="label">
        <xsl:call-template name="IMServiceName">
          <xsl:with-param name="Service" select="@service"/>
        </xsl:call-template>
      </xsl:with-param>
      <xsl:with-param name="field">
        <xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- Format the objective with the heading "Professional Objective." -->
  <xsl:template match="r:objective">
    <xsl:call-template name="heading">
      <xsl:with-param name="text"><xsl:value-of select="$objective.word"/></xsl:with-param>
    </xsl:call-template>
    <xsl:apply-templates/>
  </xsl:template>

  <!-- Format the history with the heading "Employment History". -->
  <xsl:template match="r:history">
    <xsl:call-template name="heading">
      <xsl:with-param name="text"><xsl:value-of select="$history.word"/></xsl:with-param>
    </xsl:call-template>
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="r:location">
    <xsl:value-of select="$location.start"/>
    <xsl:apply-templates/>
    <xsl:value-of select="$location.end"/>
  </xsl:template>

  <!-- Format a single job. -->
  <xsl:template match="r:job">
    <fo:block>
      <fo:block space-after="{$half.space}" keep-with-next="always">
        <fo:block
            keep-with-next="always"
            font-style="{$jobtitle.font.style}"
            font-weight="{$jobtitle.font.weight}">
          <xsl:apply-templates select="r:jobtitle"/>
        </fo:block>
        <fo:block keep-with-next="always">
          <fo:inline
              font-style="{$employer.font.style}"
              font-weight="{$employer.font.weight}">
            <xsl:apply-templates select="r:employer"/>
          </fo:inline>
          <xsl:apply-templates select="r:location"/>
        </fo:block>
        <fo:block
            font-style="{$job-period.font.style}"
            font-weight="{$job-period.font.weight}">
          <xsl:apply-templates select="r:date|r:period"/>
        </fo:block>
      </fo:block>
      <xsl:if test="r:description">
        <fo:block
          provisional-distance-between-starts="0.5em">
          <xsl:apply-templates select="r:description"/>
        </fo:block>
      </xsl:if>
      <xsl:if test="r:projects/r:project">
        <fo:block>
          <fo:block
              keep-with-next="always"
              font-style="{$job-subheading.font.style}"
              font-weight="{$job-subheading.font.weight}">
            <xsl:value-of select="$projects.word"/>
          </fo:block>
          <xsl:apply-templates select="r:projects"/>
        </fo:block>
      </xsl:if>
      <xsl:if test="r:achievements/r:achievement">
        <fo:block>
          <fo:block
              keep-with-next="always"
              font-style="{$job-subheading.font.style}"
              font-weight="{$job-subheading.font.weight}">
            <xsl:value-of select="$achievements.word"/>
          </fo:block>
          <xsl:apply-templates select="r:achievements"/>
        </fo:block>
      </xsl:if>
    </fo:block>
  </xsl:template>

  <!-- Format the projects section -->
  <xsl:template match="r:projects">
    <fo:list-block space-after="{$para.break.space}"
      provisional-distance-between-starts="{$para.break.space}"
      provisional-label-separation="{$bullet.space}">
      <xsl:apply-templates select="r:project"/>
    </fo:list-block>
  </xsl:template>

  <!-- Format a single project as a bullet -->
  <xsl:template match="r:project">
    <xsl:call-template name="bulletListItem">
      <xsl:with-param name="text">
        <xsl:if test="@title">
          <xsl:value-of select="@title"/>
          <xsl:value-of select="$title.separator"/>
        </xsl:if>
	<xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- Format the achievements section as a bullet list *SE* -->
  <xsl:template match="r:achievements">
    <fo:list-block space-after="{$para.break.space}"
      provisional-distance-between-starts="{$para.break.space}"
      provisional-label-separation="{$bullet.space}">
      <xsl:for-each select="r:achievement">
        <xsl:call-template name="bulletListItem"/>
      </xsl:for-each>
    </fo:list-block>
  </xsl:template>

  <!-- Format academics -->
  <xsl:template match="r:academics">
    <xsl:call-template name="heading">
      <xsl:with-param name="text">
        <xsl:value-of select="$academics.word"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:apply-templates select="r:degrees"/>
    <fo:block
        font-weight="{$degrees-note.font.weight}"
        font-style="{$degrees-note.font.style}">
      <xsl:apply-templates select="r:note"/>
    </fo:block>
  </xsl:template>

  <!-- Format a single degree -->
  <xsl:template match="r:degree">
    <fo:block space-after="{$para.break.space}">
      <fo:block keep-with-next="always">
        <fo:inline
            font-style="{$degree.font.style}"
            font-weight="{$degree.font.weight}">
          <xsl:apply-templates select="r:level"/>
          <xsl:if test="r:major">
            <xsl:text> </xsl:text>
            <xsl:value-of select="$in.word"/>
            <xsl:text> </xsl:text>
            <xsl:apply-templates select="r:major"/>
          </xsl:if>
        </fo:inline>
        <xsl:apply-templates select="r:minor"/>
        <xsl:if test="r:date|r:period">
          <xsl:text>, </xsl:text>
          <xsl:apply-templates select="r:date|r:period"/>
        </xsl:if>
        <xsl:if test="r:annotation">
          <xsl:text>. </xsl:text>
          <xsl:apply-templates select="r:annotation"/>
        </xsl:if>
      </fo:block>
      <fo:block>
        <xsl:apply-templates select="r:institution"/>
        <xsl:apply-templates select="r:location"/>
      </fo:block>
      <xsl:apply-templates select="r:gpa"/>
      <xsl:if test="r:subjects/r:subject">
        <fo:block space-before="{$half.space}">
          <xsl:apply-templates select="r:subjects"/>
        </fo:block>
      </xsl:if>
      <xsl:if test="r:projects/r:project">
        <fo:block space-before="{$half.space}">
          <xsl:apply-templates select="r:projects"/>
        </fo:block>
      </xsl:if>
    </fo:block>

  </xsl:template>

  <!-- Format a GPA -->
  <xsl:template match="r:gpa">
    <fo:block space-before="{$half.space}">
      <fo:inline
          font-weight="{$gpa-preamble.font.weight}"
          font-style="{$gpa-preamble.font.style}">
        <xsl:choose>
          <xsl:when test="@type = 'major'">
            <xsl:value-of select="$major-gpa.word"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="$overall-gpa.word"/>
          </xsl:otherwise>
        </xsl:choose>
      </fo:inline>

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
    </fo:block>
  </xsl:template>

  <xsl:template match="r:subjects" mode="comma">
    <fo:inline font-style="{$job-subheading.font.style}">
      <xsl:value-of select="$subjects.word"/>
      <xsl:value-of select="$title.separator"/>
    </fo:inline>
    <xsl:apply-templates select="r:subject" mode="comma"/>
    <xsl:value-of select="$subjects.suffix"/>
  </xsl:template>

  <xsl:template match="r:subject" mode="comma">
    <xsl:apply-templates select="r:title"/>
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

  <!-- Format the subjects section as a list-block -->
  <xsl:template match="r:subjects" mode="table">
    <fo:inline font-style="{$job-subheading.font.style}">
      <xsl:value-of select="$subjects.word"/>
      <xsl:value-of select="$title.separator"/>
    </fo:inline>
    <fo:list-block
      start-indent="1.5in"
      provisional-distance-between-starts="150pt"
      provisional-label-separation="0.5em"
    >
      <xsl:for-each select="r:subject">
        <fo:list-item>
          <fo:list-item-label
              end-indent="label-end()"
          >
            <fo:block>
              <xsl:apply-templates select="r:title"/>
            </fo:block>
          </fo:list-item-label>
          <fo:list-item-body
              start-indent="body-start()"
          >
            <fo:block>
              <xsl:apply-templates select="r:result"/>
              <fo:leader leader-pattern="space" leader-length="2em"/>
            </fo:block>
          </fo:list-item-body>
        </fo:list-item>
      </xsl:for-each>
    </fo:list-block>
  </xsl:template>

  <xsl:template match="r:skillarea">
    <xsl:call-template name="heading">
      <xsl:with-param name="text"><xsl:apply-templates select="r:title"/></xsl:with-param>
    </xsl:call-template>
    <xsl:apply-templates select="r:skillset"/>
  </xsl:template>

  <!-- Format a skillset's title (if any) and then the skills underneath it. -->
  <xsl:template match="r:skillset">
    <xsl:choose>
      <xsl:when test="$skills.format = 'comma'">
        <fo:block space-after="{$half.space}">
          <fo:inline
           font-style="{$skillset-title.font.style}"
           font-weight="{$skillset-title.font.weight}">
            <xsl:apply-templates select="r:title">
	      <xsl:with-param name="Separator" select="$title.separator"/>
	    </xsl:apply-templates>
          </fo:inline>
          <xsl:apply-templates select="r:skill" mode="comma"/>
          <!-- The following line should be removed in a future version. -->
          <xsl:apply-templates select="r:skills" mode="comma"/>
        </fo:block>
      </xsl:when>
      <xsl:otherwise>
        <fo:block
         keep-with-next="always"
         font-style="{$skillset-title.font.style}"
         font-weight="{$skillset-title.font.weight}">
          <xsl:apply-templates select="r:title"/>
        </fo:block>
        <xsl:if test="r:skill">
          <fo:list-block space-after="{$para.break.space}"
            provisional-distance-between-starts="{$para.break.space}"
            provisional-label-separation="{$bullet.space}">
            <xsl:apply-templates select="r:skill" mode="bullet"/>
          </fo:list-block>
        </xsl:if>

        <!-- The following block should be removed in a future version. -->
        <xsl:if test="r:skills">
          <fo:list-block space-after="{$para.break.space}"
            provisional-distance-between-starts="{$para.break.space}"
            provisional-label-separation="{$bullet.space}">
            <xsl:apply-templates select="r:skills" mode="bullet"/>
          </fo:list-block>
        </xsl:if>

      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Format a single skill as part of a comma-separated list. -->
  <xsl:template match="r:skill" mode="comma">
    <xsl:apply-templates/>
    <xsl:apply-templates select="@level"/>
    <xsl:choose>
      <xsl:when test="following-sibling::r:skill">
        <xsl:text>, </xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$skills.suffix"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Format a single skill as a bullet item. -->
  <xsl:template match="r:skill" mode="bullet">
    <xsl:call-template name="bulletListItem">
      <xsl:with-param name="text">
        <xsl:apply-templates/>
        <xsl:apply-templates select="@level"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- Format a skill level -->
  <xsl:template match="r:skill/@level">
    <xsl:if test="$skills.level.display = 1">
      <xsl:value-of select="$skills.level.start"/>
      <xsl:value-of select="normalize-space(.)"/>
      <xsl:value-of select="$skills.level.end"/>
    </xsl:if>
  </xsl:template>


  <!-- Format a single bullet and its text -->
  <xsl:template name="bulletListItem">
    <xsl:param name="text"/>
    <fo:list-item>
      <fo:list-item-label start-indent="{$body.indent}"
        end-indent="label-end()">
        <fo:block><xsl:value-of select="$bullet.glyph"/></fo:block>
      </fo:list-item-label>
      <fo:list-item-body start-indent="body-start()">
        <fo:block>
          <xsl:choose>
            <xsl:when test="string-length($text) > 0">
              <xsl:copy-of select="$text"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:apply-templates/>
            </xsl:otherwise>
          </xsl:choose>
        </fo:block>
      </fo:list-item-body>
    </fo:list-item>
  </xsl:template>

  <!-- Format the publications section. -->
  <xsl:template match="r:pubs">
    <xsl:call-template name="heading">
      <xsl:with-param name="text"><xsl:value-of select="$publications.word"/></xsl:with-param>
    </xsl:call-template>
    <fo:list-block
        space-after="{$para.break.space}"
        provisional-distance-between-starts="{$para.break.space}"
        provisional-label-separation="{$bullet.space}">
      <xsl:apply-templates select="r:pub"/>
    </fo:list-block>
  </xsl:template>
  
  <!-- Format a single publication -->
  <xsl:template match="r:pub">
    <fo:list-item>
      <fo:list-item-label start-indent="{$body.indent}"
        end-indent="label-end()">
        <fo:block><xsl:value-of select="$bullet.glyph"/></fo:block>
      </fo:list-item-label>
      <fo:list-item-body start-indent="body-start()">
        <fo:block>
          <xsl:call-template name="FormatPub"/>
        </fo:block>
      </fo:list-item-body>
    </fo:list-item>
  </xsl:template>

  <!-- Title of book -->
  <xsl:template match="r:bookTitle" priority="1">
    <fo:inline font-style="{$citation.font.style}"><xsl:apply-templates/></fo:inline><xsl:value-of select="$pub.item.separator"/>
  </xsl:template>

  <!-- Format memberships. -->
  <xsl:template match="r:memberships">
    <xsl:call-template name="heading">
      <xsl:with-param name="text"><xsl:apply-templates select="r:title"/></xsl:with-param>
    </xsl:call-template>

    <xsl:apply-templates select="r:membership"/>
  </xsl:template>

  <!-- Format membership. -->
  <xsl:template match="r:membership">
    <fo:block space-after="{$half.space}" keep-with-next="always">
      <fo:block
       font-weight="{$jobtitle.font.weight}"
       font-style="{$jobtitle.font.style}">
        <xsl:apply-templates select="r:title"/>
      </fo:block>
      <xsl:if test="r:organization">
        <fo:block keep-with-next="always">
          <xsl:apply-templates select="r:organization"/>
          <xsl:apply-templates select="r:location"/>
        </fo:block>
      </xsl:if>
      <xsl:if test="r:date|r:period">
        <fo:block keep-with-next="always">
          <xsl:apply-templates select="r:date|r:period"/>
        </fo:block>
      </xsl:if>
    </fo:block>
    <xsl:apply-templates select="r:description"/>
  </xsl:template>

  <!-- Format interests. -->
  <xsl:template match="r:interests">
    <!-- Heading -->
    <xsl:call-template name="heading">
      <xsl:with-param name="text">
        <xsl:call-template name="Title">
          <xsl:with-param name="Title" select="$interests.word"/>
        </xsl:call-template>
      </xsl:with-param>
    </xsl:call-template>

    <!-- Interests -->
    <fo:list-block
        space-after="{$para.break.space}"
        provisional-distance-between-starts="{$para.break.space}"
        provisional-label-separation="{$bullet.space}">

      <xsl:apply-templates select="r:interest"/>

    </fo:list-block>
  </xsl:template>

  <!-- A single interest. -->
  <xsl:template match="r:interest">
    <xsl:call-template name="bulletListItem">
      <xsl:with-param name="text">

        <xsl:apply-templates select="r:title"/>

        <!-- Append period to title if followed by a single-line description -->
        <xsl:if test="$interest.description.format = 'single-line' and r:description">
          <xsl:text>. </xsl:text>
        </xsl:if>

        <xsl:apply-templates select="r:description"/>

      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- Format an interest description -->
  <xsl:template match="r:interest/r:description">
    <xsl:choose>  

      <xsl:when test="$interest.description.format = 'single-line'">
        <xsl:for-each select="r:para">
          <xsl:apply-templates/>
          <xsl:if test="following-sibling::*">
            <xsl:value-of select="$description.para.separator"/>
          </xsl:if>
        </xsl:for-each>
      </xsl:when>

      <xsl:otherwise> <!-- Block -->
        <fo:block
          space-after="{$para.break.space}"
          provisional-distance-between-starts="1em">
          <xsl:apply-templates/>
        </fo:block>
      </xsl:otherwise>

    </xsl:choose>
  </xsl:template>
  
  <!-- Format security clearance section. -->
  <xsl:template match="r:clearances">
    <!-- Heading -->
    <xsl:call-template name="heading">
      <xsl:with-param name="text">
        <xsl:call-template name="Title">
          <xsl:with-param name="Title" select="$security-clearances.word"/>
        </xsl:call-template>
      </xsl:with-param>
    </xsl:call-template>
 
    <!-- Clearances -->
    <fo:list-block
        space-after="{$para.break.space}"
        provisional-distance-between-starts="{$para.break.space}"
        provisional-label-separation="{$bullet.space}">

      <xsl:apply-templates select="r:clearance"/>

    </fo:list-block>
  </xsl:template>

  <!-- Format a single security clearance. -->
  <xsl:template match="r:clearance">
    <xsl:call-template name="bulletListItem">
      <xsl:with-param name="text">
        <fo:inline
            font-weight="{$clearance-level.font.weight}"
            font-style="{$clearance-level.font.style}">
          <xsl:apply-templates select="r:level"/>
        </fo:inline>
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
  </xsl:template>

  <!-- Format awards. -->
  <xsl:template match="r:awards">
    <!-- Heading -->
    <xsl:call-template name="heading">
      <xsl:with-param name="text">
        <xsl:call-template name="Title">
          <xsl:with-param name="Title" select="$awards.word"/>
        </xsl:call-template>
      </xsl:with-param>
    </xsl:call-template>
    <fo:list-block
        space-after="{$para.break.space}"
        provisional-distance-between-starts="{$para.break.space}"
        provisional-label-separation="{$bullet.space}">
      <xsl:apply-templates select="r:award"/>
    </fo:list-block>
  </xsl:template>

  <!-- Format a single award. -->
  <xsl:template match="r:award">
    <xsl:call-template name="bulletListItem">
      <xsl:with-param name="text">
        <fo:inline font-weight="{$emphasis.font.weight}">
          <xsl:apply-templates select="r:title"/>
        </fo:inline>
        <xsl:if test="r:organization"><xsl:text>, </xsl:text></xsl:if>
        <xsl:apply-templates select="r:organization"/>
        <xsl:if test="r:date|r:period"><xsl:text>, </xsl:text></xsl:if>
        <xsl:apply-templates select="r:date|r:period"/>
        <xsl:apply-templates select="r:description"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- Format miscellaneous information with "Miscellany" as the heading. -->
  <xsl:template match="r:misc">
    <xsl:call-template name="heading">
      <xsl:with-param name="text"><xsl:value-of select="$miscellany.word"/></xsl:with-param>
    </xsl:call-template>
    <xsl:apply-templates/>
  </xsl:template>

  <!-- Format the "last modified" date -->
  <xsl:template match="r:lastModified">
    <fo:block
        start-indent="{$heading.indent}"
        space-before="{$para.break.space}"
        space-after="{$para.break.space}"
        font-size="{$fineprint.font.size}">
      <xsl:value-of select="$last-modified.phrase"/>
      <xsl:text> </xsl:text>
      <xsl:apply-templates/>
      <xsl:text>.</xsl:text>
    </fo:block>
  </xsl:template>

  <!-- Format legalese. -->
  <xsl:template match="r:copyright">
    <fo:block
        start-indent="{$heading.indent}"
        font-size="{$fineprint.font.size}">
      <fo:block keep-with-next="always">
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
      </fo:block>
      <xsl:apply-templates select="r:legalnotice"/>
    </fo:block>
  </xsl:template>

  <!-- Format para's as block objects with 10pt space after them. -->
  <xsl:template match="r:para">
    <fo:block
        space-after="{$para.break.space}">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <!-- Format emphasized words in bold. -->
  <xsl:template match="r:emphasis">
    <fo:inline font-weight="{$emphasis.font.weight}">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <!-- Format citations to other works. -->
  <xsl:template match="r:citation">
    <fo:inline font-style="{$citation.font.style}">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <!-- Format a URL. -->
  <xsl:template match="r:url" name="FormatUrl">
    <fo:inline font-family="{$url.font.family}">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <!-- Format a period. -->
  <xsl:template match="r:period">
    <xsl:apply-templates select="r:from"/>&#x2013;<xsl:apply-templates select="r:to"/>
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

  <!-- In a date with just "present", format it as the word "present". -->
  <xsl:template match="r:present"><xsl:value-of select="$present.word"/></xsl:template>

  <!-- Suppress items not needed for print presentation -->
  <xsl:template match="r:keywords"/>

  <!-- Format the referees -->
  <xsl:template match="r:referees">
    <xsl:call-template name="heading">
      <xsl:with-param name="text"><xsl:value-of select="$referees.word"/></xsl:with-param>
    </xsl:call-template>
    <xsl:choose>
      <xsl:when test="$referees.display = 1">
        <xsl:choose>
	  <xsl:when test="$referees.layout = 'compact'">
            <fo:table table-layout="fixed" width="90%">
	      <fo:table-column width="40%"/>
	      <fo:table-column width="40%"/>
	      <fo:table-body>
                <xsl:apply-templates select="r:referee" mode="compact"/>
	      </fo:table-body>
            </fo:table>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:apply-templates select="r:referee" mode="standard"/>
          </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:otherwise>
        <fo:block space-after="{$para.break.space}">
          <xsl:value-of select="$referees.hidden.phrase"/>
        </fo:block>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Format a referee with the name, title, organiation in the 
       left column and the address in the right column -->
  <xsl:template match="r:referee" mode="compact">
    <fo:table-row>
      <fo:table-cell padding-bottom="{$half.space}">
        <fo:block
            font-style="{$referee-name.font.style}"
            font-weight="{$referee-name.font.weight}">
          <xsl:apply-templates select="r:name"/>
        </fo:block>
        <fo:block>
          <xsl:apply-templates select="r:title"/>
          <xsl:if test="r:title and r:organization">
            <xsl:text>, </xsl:text>
          </xsl:if>
          <xsl:apply-templates select="r:organization"/>
	</fo:block>
        <xsl:if test="r:contact">
          <fo:block>
            <xsl:apply-templates select="r:contact"/>
          </fo:block>
        </xsl:if>
      </fo:table-cell>
      <fo:table-cell padding-bottom=".5em">
        <xsl:if test="r:address">
          <fo:block>
            <xsl:apply-templates select="r:address"/>
          </fo:block>
        </xsl:if>
      </fo:table-cell>
    </fo:table-row>
  </xsl:template>

  <!-- Format a referee as a block element -->
  <xsl:template match="r:referee" mode="standard">
    <fo:block space-after="{$para.break.space}">
      <fo:block space-after="{$half.space}">
        <fo:block keep-with-next="always"
            font-style="{$referee-name.font.style}"
            font-weight="{$referee-name.font.weight}">
          <xsl:apply-templates select="r:name"/>
        </fo:block>
        <fo:block>
          <xsl:apply-templates select="r:title"/>
          <xsl:if test="r:title and r:organization">
            <xsl:text>, </xsl:text>
          </xsl:if>
          <xsl:apply-templates select="r:organization"/>
        </fo:block>
      </fo:block>
      <xsl:if test="r:address">
        <fo:block space-after="{$half.space}">
          <xsl:apply-templates select="r:address"/>
        </fo:block>
      </xsl:if>
      <xsl:if test="r:contact">
        <fo:block space-after="{$half.space}">
          <xsl:apply-templates select="r:contact"/>
        </fo:block>
      </xsl:if>
    </fo:block>
  </xsl:template>

</xsl:stylesheet>
