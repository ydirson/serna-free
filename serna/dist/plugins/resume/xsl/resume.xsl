<xsl:stylesheet version="1.0"
                xmlns:r="http://xmlresume.sourceforge.net/resume/0.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">


<xsl:include href="http://www.syntext.com/xslbricks-1.0/fo/fo.xsl"/>
<xsl:include href="params.xsl"/>
<xsl:include href="common.xsl"/>
<xsl:include href="contact.xsl"/>
<xsl:include href="string.xsl"/>
<xsl:include href="address.xsl"/>
<xsl:include href="pub.xsl"/>
<xsl:include href="i18n.xsl"/>

<xsl:strip-space elements="*"/>

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
        <fo:region-body overflow="hidden"
          margin-bottom="{$margin.bottom}"/>
      </fo:simple-page-master>
    </fo:layout-master-set>
    <fo:page-sequence master-reference="resume-page">
      <fo:flow flow-name="xsl-region-body">
        <fo:block start-indent="{$body.indent}"
          font-family="{$body.font.family}"
          font-size="{$body.font.size}">
          <xsl:apply-templates select="resume"/>
        </fo:block>
      </fo:flow>
    </fo:page-sequence>
  </fo:root>
</xsl:template>

<!--Header-->
<xsl:template match="header" mode="standard">
  <fo:block padding-bottom="{$para.break.space}">
    <fo:block
        font-style="{$header.name.font.style}"
        font-weight="{$header.name.font.weight}"
        font-size="{$header.name.font.size}">
      <xsl:apply-templates select="name"/>
    </fo:block>
    <xsl:apply-templates select="address"/>
    <xsl:apply-templates select="birth"/>
    <xsl:if test="contact">
      <fo:block padding-top="0.4em">
        <xsl:apply-templates select="contact"/>
      </fo:block>
    </xsl:if>
  </fo:block>
</xsl:template>

<xsl:template match="header" mode="centered">
  <fo:block
      padding-bottom="{$para.break.space}"
      start-indent="{$header.margin-left}"
      end-indent="{$header.margin-right}">
    <fo:block
        font-style="{$header.name.font.style}"
        font-weight="{$header.name.font.weight}"
        font-size="{$header.name.font.size}">
      <xsl:apply-templates select="name"/>
    </fo:block>
    <xsl:apply-templates select="address"/>
    <xsl:apply-templates select="birth"/>
    <fo:block padding-top="0.4em">
      <xsl:apply-templates select="contact"/>
    </fo:block>
  </fo:block>
</xsl:template>

<xsl:template name="heading">
  <fo:block start-indent="{$heading.indent}"
            font-size="{$heading.font.size}"
            font-family="{$heading.font.family}"
            font-weight="{$heading.font.weight}"
            padding-top="{$para.break.space}"
            padding-bottom="{$para.break.space}"
            border-bottom-width="{$heading.border.bottom.width}">
    <xsl:call-template name="gentext"/>
  </fo:block>
</xsl:template>

<!--Contact-->
<xsl:template match="contact/phone">
 <fo:block>
   <xsl:call-template name="hcontact">
     <xsl:with-param name="label">
       <xsl:call-template name="PhoneLocation">
         <xsl:with-param name="Location" select="@location"/>
       </xsl:call-template>
     </xsl:with-param>
     <xsl:with-param name="field">
       <xsl:apply-templates/>
     </xsl:with-param>
   </xsl:call-template>
   <xsl:call-template name="inline"/>
 </fo:block>
</xsl:template>

<xsl:template match="contact/fax">
  <fo:block>
    <xsl:call-template name="hcontact">
      <xsl:with-param name="label">
        <xsl:call-template name="FaxLocation">
          <xsl:with-param name="Location" select="@location"/>
        </xsl:call-template>
      </xsl:with-param>
      <xsl:with-param name="field">
        <xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:call-template name="inline"/>
  </fo:block>
</xsl:template>

<xsl:template match="contact/pager|contact/email|contact/url">
  <fo:block>
    <xsl:call-template name="hcontact">
      <xsl:with-param name="label">
        <xsl:call-template name="gentext"/>
      </xsl:with-param>
      <xsl:with-param name="field">
        <xsl:apply-templates/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:call-template name="inline"/>
  </fo:block>
</xsl:template>

<xsl:template match="contact/instantMessage">
  <fo:block>
    <xsl:call-template name="hcontact">
      <xsl:with-param name="label">
        <xsl:call-template name="IMServiceName">
          <xsl:with-param name="Service" select="@service"/>
        </xsl:call-template>
      </xsl:with-param>
      <xsl:with-param name="field">
        <xsl:apply-templates/>
      </xsl:with-param>
     </xsl:call-template>
     <xsl:call-template name="inline"/>
  </fo:block>
</xsl:template>

<xsl:template name="hcontact">
  <xsl:param name="label"/>
  <xsl:param name="field"/>
  <fo:inline font-style="{$header.item.font.style}">
    <xsl:value-of select="$label"/>: 
  </fo:inline>
</xsl:template>

<!--Adress-->
<xsl:template match="address">
  <fo:block padding-top="{$half.space}">
    <xsl:variable name="format">
      <xsl:choose>
        <xsl:when test="@format">
          <xsl:value-of select="@format"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$address.format"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="node()[1][not(self::*)]">
          <xsl:call-template name="div"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:choose>
          <xsl:when test="$format = 'standard'">
            <xsl:call-template name="address-standart"/>
          </xsl:when>
          <xsl:when test="$format = 'european'">
            <xsl:call-template name="address-european"/>
          </xsl:when>
          <xsl:when test="$format = 'italian'">
            <xsl:call-template name="address-italian"/>
          </xsl:when>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </fo:block>
</xsl:template>

<xsl:template name="address-standart">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template name="address-european">
  <xsl:apply-templates select="street"/>
  <xsl:apply-templates select="street2"/>
  <fo:block>
    <xsl:if test="suburb">
      <xsl:apply-templates select="suburb"/>
      <xsl:text> </xsl:text>
    </xsl:if>
    <xsl:if test="ward">
      <xsl:apply-templates select="ward"/>
      <xsl:text> </xsl:text>
    </xsl:if>   
    <xsl:if test="zip">
      <xsl:apply-templates select="zip"/>
      <xsl:text> </xsl:text>
    </xsl:if>
    <xsl:if test="postalCode">
      <xsl:apply-templates select="postalCode"/>
      <xsl:text> </xsl:text>
    </xsl:if>
  </fo:block>
  <fo:block>
    <xsl:apply-templates select="city"/>
    <xsl:if test="county">
      <xsl:apply-templates select="county"/> 
      <xsl:text>, </xsl:text>     
    </xsl:if>
    <xsl:if test="prefecture">
      <xsl:apply-templates select="prefecture"/> 
      <xsl:text>, </xsl:text>     
    </xsl:if>
    <xsl:if test="state">
      <xsl:apply-templates select="state"/>
      <xsl:text>, </xsl:text>
    </xsl:if>
    <xsl:if test="province">
      <xsl:apply-templates select="province"/>
      <xsl:text>, </xsl:text>
    </xsl:if>
    <xsl:apply-templates select="country"/>
  </fo:block>
</xsl:template>

<xsl:template name="address-italian">
  <xsl:if test="street">
    <xsl:apply-templates select="street"/>
  </xsl:if>
  <xsl:if test="street2">
    <xsl:apply-templates select="street2"/>
  </xsl:if>
  <fo:block>
    <xsl:if test="postalCode">
      <xsl:apply-templates select="postalCode"/>
      <xsl:text> </xsl:text>
    </xsl:if>
    <xsl:apply-templates select="city"/>
    <xsl:text>, </xsl:text>
    <xsl:if test="province">
      <xsl:text> (</xsl:text><xsl:apply-templates select="province"/>
      <xsl:text>), </xsl:text>
    </xsl:if>
  </fo:block>
  <xsl:apply-templates select="country"/>
</xsl:template>

<xsl:template match="city|county|postalCode|
                     state|province|zip|country|
                     prefecture|location/city|
                     location/county|location/country|
                     location/province|location/state">
  <xsl:call-template name="inline"/>
  <xsl:if test="following-sibling::*">
    <xsl:text>, </xsl:text>
  </xsl:if>
  <xsl:if test="position()=last()"> 
    <xsl:text>.</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="suburb|ward">
  <xsl:call-template name="inline"/>
  <xsl:if test="following-sibling::*">
    <xsl:text> </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="address/street|address/street2">
  <xsl:call-template name="div"/>
</xsl:template>

<xsl:template match="projects">
  <fo:list-block padding-bottom="{$para.break.space}">
    <xsl:apply-templates select="project"/>
  </fo:list-block>
</xsl:template>

<xsl:template match="docpath|head|node|tail|label|uri"/>

<xsl:template match="project">
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

<xsl:template match="achievements">
  <fo:list-block padding-top="{$para.break.space}">
    <xsl:for-each select="achievement">
      <xsl:call-template name="bulletListItem"/>
    </xsl:for-each>
  </fo:list-block>
</xsl:template>

<xsl:template match="academics">
  <fo:block>
    <xsl:call-template name="heading"/>
    <xsl:apply-templates select="degrees"/>
    <xsl:if test="note">
      <fo:block font-weight="{$degrees-note.font.weight}"
                font-style="{$degrees-note.font.style}">
        <xsl:apply-templates select="note"/>
      </fo:block>
    </xsl:if>
  </fo:block>
</xsl:template>

<xsl:template match="degree">
  <fo:block padding-bottom="{$para.break.space}">
    <fo:block>
      <fo:inline>
        <fo:inline font-style="{$degree.font.style}"
                   font-weight="{$degree.font.weight}">
          <xsl:apply-templates select="level"/>
          <xsl:if test="major">
            <xsl:text> in </xsl:text>
            <xsl:apply-templates select="major"/>
          </xsl:if>
        </fo:inline> 
        <xsl:if test="minor">
          <xsl:text> (minor in </xsl:text>
          <xsl:apply-templates select="minor"/>
          <xsl:text>)</xsl:text>
        </xsl:if>
        <xsl:if test="date|period">
          <xsl:text>, </xsl:text>
          <xsl:apply-templates select="date|period"/>
        </xsl:if>
      </fo:inline>
      <xsl:if test="annotation">
        <xsl:apply-templates select="annotation"/>
      </xsl:if>
    </fo:block>
    <xsl:if test="institution">
      <fo:block>
        <xsl:apply-templates select="institution"/>
        <xsl:apply-templates select="location"/>
      </fo:block>
    </xsl:if>
    <xsl:apply-templates select="gpa"/>
    <xsl:if test="subjects/subject">
      <fo:block padding-bottom="{$half.space}">
        <xsl:apply-templates select="subjects"/>
      </fo:block>
    </xsl:if>
    <xsl:if test="projects/project">
      <fo:block>
        <fo:block font-style="{$job-subheading.font.style}"
                  font-weight="{$job-subheading.font.weight}">
          <xsl:text>Projects</xsl:text>
        </fo:block>
        <xsl:apply-templates select="projects"/>
      </fo:block>
    </xsl:if>
  </fo:block>
</xsl:template>

<xsl:template match="membership">
  <fo:block padding-bottom="{$half.space}">
    <xsl:if test="title">
      <fo:block font-weight="{$jobtitle.font.weight}"
                font-style="{$jobtitle.font.style}">
        <xsl:apply-templates select="title"/>
      </fo:block>
    </xsl:if>
    <xsl:apply-templates select="*[local-name(.) != 'title']"/>
  </fo:block>
</xsl:template>

<xsl:template match="interest">
  <fo:block>
    <fo:inline>
      <xsl:value-of select="$bullet.glyph"/>
      <xsl:text>  </xsl:text>
      <xsl:apply-templates select="title"/>
      <xsl:if test="$interest.description.format='single-line'
                    and description">
        <xsl:text>. </xsl:text>
      </xsl:if>
      <xsl:apply-templates select="description"/>
    </fo:inline>
  </fo:block>
</xsl:template>

<xsl:template match="clearances|awards|memberships|pubs|interests|skillarea">
  <fo:block> 
    <xsl:choose>
      <xsl:when test="title">
        <xsl:if test="title[not(self::processing-instruction('se:choice'))]">
          <fo:block start-indent="{$heading.indent}"
                    font-size="{$heading.font.size}"
                    font-family="{$heading.font.family}"
                    font-weight="{$heading.font.weight}"
                    padding-top="{$para.break.space}"
                    border-bottom-width="{$heading.border.bottom.width}">
            <xsl:apply-templates select="title"/>
          </fo:block>
        </xsl:if>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="heading"/>
      </xsl:otherwise>
    </xsl:choose>
    <fo:block padding-bottom="{$para.break.space}">
      <xsl:apply-templates select="*[local-name(.) != 'title']"/>
    </fo:block>
  </fo:block>
</xsl:template>

<xsl:template match="clearance">
  <fo:block>
    <fo:inline>
      <xsl:value-of select="$bullet.glyph"/>
      <xsl:text>  </xsl:text>
    </fo:inline>
    <fo:inline font-weight="{$clearance-level.font.weight}"
               font-style="{$clearance-level.font.style}">
      <xsl:apply-templates select="level"/>
    </fo:inline>
    <xsl:apply-templates select="*[local-name(.) != 'level']"/>
  </fo:block>
</xsl:template>

<xsl:template match="clearance/organization|clearance/level|
                     award/organization|award/title">
  <xsl:call-template name="inline"/>
  <xsl:if test="following-sibling::*">
    <xsl:text>, </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="clearance/date|award/date">
  <xsl:call-template name="inline"/>
  <xsl:if test="following-sibling::*">
    <xsl:text>.</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="clearance/period|award/period">
  <fo:inline>
    <xsl:apply-templates select="from"/>&#x2013;
    <xsl:apply-templates select="to"/>
    <xsl:if test="following-sibling::*">
      <xsl:text>.</xsl:text>
    </xsl:if>
  </fo:inline>
</xsl:template>

<xsl:template match="award">
  <fo:block>
    <fo:inline>
      <xsl:value-of select="$bullet.glyph"/>
      <xsl:text>  </xsl:text>
    </fo:inline>
    <fo:inline font-weight="{$emphasis.font.weight}">
      <xsl:apply-templates select="title"/>
    </fo:inline>
    <xsl:apply-templates select="*[local-name(.) != 'title']"/>
  </fo:block>
</xsl:template>

<xsl:template match="keywords">
  <fo:block>  
    <xsl:call-template name="heading"/>
    <xsl:for-each select="keyword">
      <xsl:apply-templates select="."/>
      <xsl:if test="following-sibling::*">
        <xsl:text>, </xsl:text>
      </xsl:if>
      <xsl:if test="position()=last()"> 
        <xsl:text>.</xsl:text>
      </xsl:if>
    </xsl:for-each>
  </fo:block>
</xsl:template>

<xsl:template match="lastModified">
  <fo:block start-indent="{$heading.indent}"
            padding-top="{$para.break.space}"
            padding-bottom="{$para.break.space}"
            font-size="{$fineprint.font.size}">
    <fo:inline>
      <xsl:call-template name="gentext"/>
      <xsl:apply-templates/>
      <xsl:text>.</xsl:text>
    </fo:inline>
  </fo:block>
</xsl:template>

<xsl:template match="copyright">
  <fo:block start-indent="{$heading.indent}"
            font-size="{$fineprint.font.size}">
    <fo:inline>
      <xsl:call-template name="gentext"/>
      <xsl:apply-templates select="year"/>
      <xsl:text> by </xsl:text>
      <xsl:if test="name">
        <xsl:apply-templates select="name"/> 
      </xsl:if>
      <xsl:if test="not(name)">
        <xsl:apply-templates select="/resume/header/name"/> 
      </xsl:if>
      <xsl:text>.</xsl:text>
    </fo:inline>
    <xsl:apply-templates select="legalnotice"/>
  </fo:block>
</xsl:template>

<xsl:template match="name|FormatDate|skillareas|date">
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="name/title|firstname|middlenames|
                     surname|suffix|keyword">
  <xsl:call-template name="inline"/>
  <xsl:if test="following-sibling::*">
    <xsl:text> </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="para">
  <fo:block>
    <xsl:call-template name = "inline"/>
  </fo:block>
</xsl:template>

<xsl:template match="emphasis">
  <fo:inline font-weight="{$emphasis.font.weight}">
    <xsl:call-template name = "inline"/>
  </fo:inline>
</xsl:template>

<xsl:template match="citation">
  <fo:inline font-style="{$citation.font.style}">
    <xsl:call-template name = "inline"/>
  </fo:inline>
</xsl:template>

<xsl:template match="url">
  <fo:inline font-family="{$url.font.family}">
    <xsl:call-template name = "inline"/>
  </fo:inline>
</xsl:template>

<xsl:template match="present">
  <fo:inline>
    <xsl:call-template name="gentext"/>
  </fo:inline>
</xsl:template>

<xsl:template match="period">
  <fo:block>
    <xsl:apply-templates select="from"/>&#x2013;
    <xsl:apply-templates select="to"/>
  </fo:block>
</xsl:template>

<xsl:template match="interest/description">
  <fo:inline>
    <xsl:choose>  
      <xsl:when test="$interest.description.format = 'single-line'">
        <xsl:for-each select="para">
          <xsl:apply-templates/>
          <xsl:if test="following-sibling::*">
            <xsl:value-of select="$description.para.separator"/>
          </xsl:if>
        </xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
        <fo:block padding-bottom="{$para.break.space}">
          <xsl:apply-templates/>
        </fo:block>
      </xsl:otherwise>
    </xsl:choose>
  </fo:inline>
</xsl:template>

<xsl:template match="bookTitle" priority="1">
  <fo:inline font-style="{$citation.font.style}">
    <xsl:apply-templates/>
  </fo:inline>
  <xsl:value-of select="$pub.item.separator"/>
</xsl:template>
  
<xsl:template match="pub">
<fo:block>
  <fo:inline>
    <xsl:value-of select="$bullet.glyph"/>
    <xsl:text>  </xsl:text>
    <xsl:call-template name="FormatPub"/>
  </fo:inline>
</fo:block>
</xsl:template>

<!--Skills-->
<xsl:template match="skillset">
  <fo:block> 
    <xsl:choose>
      <xsl:when test="$skills.format = 'comma'">
        <fo:block padding-bottom="{$half.space}">
          <xsl:if test="title">
            <xsl:if test="title[not(self::processing-instruction('se:choice'))]">
              <fo:inline font-style="{$skillset-title.font.style}"
                         font-weight="{$skillset-title.font.weight}">
                <xsl:apply-templates select="title"/>
                <xsl:text>: </xsl:text>
	         </fo:inline>
            </xsl:if>
          </xsl:if>
          <xsl:apply-templates select="*[not(self::title)]" mode="comma"/>
        </fo:block>
      </xsl:when>
      <xsl:otherwise>
        <fo:block font-style="{$skillset-title.font.style}"
                  font-weight="{$skillset-title.font.weight}">
          <fo:inline>
            <xsl:apply-templates select="title"/>
            <xsl:text>: </xsl:text>
          </fo:inline>
        </fo:block>
        <fo:list-block padding-bottom="{$para.break.space}">
          <xsl:apply-templates select="*[local-name(.) != 'title']" mode="bullet"/>
        </fo:list-block>
      </xsl:otherwise>
    </xsl:choose>
  </fo:block> 
</xsl:template>

<xsl:template match="skills" mode="comma">
  <fo:inline>
    <xsl:apply-templates select="skill" mode="comma"/>
  </fo:inline>
</xsl:template>

<xsl:template match="skill" mode="comma">
  <fo:inline>
    <xsl:call-template name="inline"/>
    <xsl:apply-templates select="@level"/>
    <xsl:choose>
      <xsl:when test="following-sibling::skill">
        <xsl:text>, </xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$skills.suffix"/>
      </xsl:otherwise>
    </xsl:choose>
  </fo:inline>
</xsl:template>

<xsl:template match="skill" mode="bullet">
  <fo:inline>
    <xsl:call-template name="bulletListItem">
      <xsl:with-param name="text">
        <xsl:call-template name="inline"/>
        <xsl:apply-templates select="@level"/>
      </xsl:with-param>
    </xsl:call-template>
  </fo:inline>
</xsl:template>

<xsl:template match="skill/@level">
  <xsl:if test="$skills.level.display = 1">
    <xsl:value-of select="$skills.level.start"/>
    <xsl:value-of select="normalize-space(.)"/>
    <xsl:value-of select="$skills.level.end"/>
  </xsl:if>
</xsl:template>

<xsl:template match="objective|history|misc">
 <fo:block> 
    <xsl:call-template name="heading"/>
    <xsl:apply-templates/>
  </fo:block> 
</xsl:template>

<xsl:template match="location">
  <fo:inline>
    <xsl:value-of select="$location.start"/>
    <xsl:call-template name="inline"/>
    <xsl:value-of select="$location.end"/>
  </fo:inline>
</xsl:template>

<xsl:template match="jobtitle">
  <fo:block font-style="{$jobtitle.font.style}"
            font-weight="{$jobtitle.font.weight}">
    <xsl:call-template name="div"/>
  </fo:block>
</xsl:template>

<xsl:template match="employer">
  <fo:inline font-style="{$employer.font.style}"
             font-weight="{$employer.font.weight}">
    <xsl:call-template name="div"/>
  </fo:inline>
</xsl:template>

<xsl:template match="job/date">
  <fo:block font-style="{$job-period.font.style}"
            font-weight="{$job-period.font.weight}">
    <xsl:call-template name="div"/>
  </fo:block>
</xsl:template>

<xsl:template match="job/period">
  <fo:block font-style="{$job-period.font.style}"
            font-weight="{$job-period.font.weight}">
    <xsl:apply-templates select="from"/>&#x2013;
    <xsl:apply-templates select="to"/>
  </fo:block>
</xsl:template>

<xsl:template match="job/projects">
  <fo:block>
    <fo:block font-style="{$job-subheading.font.style}"
              font-weight="{$job-subheading.font.weight}">
      <xsl:call-template name="gentext"/>
    </fo:block>
    <fo:list-block padding-top="{$para.break.space}">
        <xsl:apply-templates/>
    </fo:list-block>
  </fo:block>
</xsl:template>

<xsl:template match="job/achievements">
  <fo:block>
    <fo:block font-style="{$job-subheading.font.style}"
              font-weight="{$job-subheading.font.weight}">
      <xsl:call-template name="gentext"/>
    </fo:block>
    <fo:list-block padding-top="{$para.break.space}">
      <xsl:for-each select="achievement">
        <xsl:call-template name="bulletListItem"/>
      </xsl:for-each>
    </fo:list-block>
  </fo:block>
</xsl:template>

<xsl:template match="job">
  <fo:block padding-bottom="{$half.space}">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="gpa">
  <fo:block padding-top="{$half.space}">
    <fo:inline font-weight="{$gpa-preamble.font.weight}"
               font-style="{$gpa-preamble.font.style}">
      <xsl:choose>
        <xsl:when test="@type = 'major'">
          <xsl:text>Major gpa</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>Overall gpa</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
    </fo:inline>
    <xsl:text>: </xsl:text>
    <xsl:apply-templates select="score"/>
    <xsl:if test="possible">
      <xsl:text>out of</xsl:text>
      <xsl:apply-templates select="possible"/>
    </xsl:if>
    <xsl:if test="note">
      <xsl:text>. </xsl:text>
      <xsl:apply-templates select="note"/>
    </xsl:if>
  </fo:block>
</xsl:template>

<xsl:template match="subjects" mode="comma">
  <fo:block>
    <fo:inline font-style="{$job-subheading.font.style}">
      <xsl:call-template name="gentext"/>
      <xsl:value-of select="$title.separator"/>
    </fo:inline>
    <xsl:apply-templates select="subject" mode="comma"/>
  </fo:block>
</xsl:template>

<xsl:template match="subject" mode="comma">
  <fo:block>
    <xsl:apply-templates select="title"/>
    <xsl:if test="$subjects.result.display = 1">
      <xsl:if test="result">
        <xsl:value-of select="$subjects.result.start"/>
        <xsl:value-of select="normalize-space(result)"/>
        <xsl:value-of select="$subjects.result.end"/>
      </xsl:if>   
    </xsl:if>   
    <xsl:if test="following-sibling::*">
      <xsl:value-of select="$subjects.separator"/>
    </xsl:if>
    <xsl:if test="position()=last()"> 
      <xsl:text>.</xsl:text>
    </xsl:if>
  </fo:block>
</xsl:template>

<xsl:template match="subjects" mode="table">
  <fo:block>
    <fo:inline font-style="{$job-subheading.font.style}">
      <xsl:call-template name="gentext"/>
      <xsl:value-of select="$title.separator"/>
    </fo:inline>
    <fo:list-block start-indent="1.5in">
      <xsl:for-each select="subject">
        <fo:list-item>
          <fo:list-item-label end-indent="label-end()">
            <fo:block>
              <xsl:apply-templates select="title"/>
            </fo:block>
          </fo:list-item-label>
          <fo:list-item-body start-indent="body-start()">
            <fo:block>
              <xsl:apply-templates select="result"/>
              <fo:leader leader-pattern="space" leader-length="2em"/>
            </fo:block>
          </fo:list-item-body>
        </fo:list-item>
      </xsl:for-each>
    </fo:list-block>
  </fo:block>
</xsl:template>

<xsl:template name="bulletListItem">
  <xsl:param name="text"/>
  <fo:list-item>
    <fo:list-item-label start-indent="{$body.indent}"
                        end-indent="label-end()">
      <fo:block>
        <xsl:value-of select="$bullet.glyph"/>
      </fo:block>
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

<xsl:template match="referees">
  <fo:block> 
    <xsl:call-template name="heading"/>
    <xsl:choose>
      <xsl:when test="$referees.display = 1">
        <xsl:choose>
	     <xsl:when test="$referees.layout = 'compact'">
            <fo:table table-layout="fixed" width="90%">
	          <fo:table-column width="40%"/>
	          <fo:table-column width="40%"/>
	          <fo:table-body>
                 <xsl:apply-templates select="referee" mode="compact"/>
	          </fo:table-body>
             </fo:table>
	     </xsl:when>
	     <xsl:otherwise>
	       <xsl:apply-templates select="referee" mode="standard"/>
          </xsl:otherwise>
	   </xsl:choose>
      </xsl:when>
      <xsl:otherwise>
        <fo:block padding-bottom="{$para.break.space}">
          <xsl:value-of select="$referees.hidden.phrase"/>
        </fo:block>
      </xsl:otherwise>
    </xsl:choose>
  </fo:block> 
</xsl:template>
  
<xsl:template match="birth">
  <fo:block padding-top="{$half.space}">
    <fo:inline>
      <xsl:call-template name="gentext"/>
      <xsl:apply-templates select="date"/>
    </fo:inline>
  </fo:block>
</xsl:template>
 
<xsl:template match="referee" mode="compact">
  <fo:table-row>
    <fo:table-cell padding-bottom="{$half.space}">
      <fo:block font-style="{$referee-name.font.style}"
                font-weight="{$referee-name.font.weight}">
        <xsl:apply-templates select="name"/>
      </fo:block>
      <xsl:if test="title or organization">
        <fo:block>
          <xsl:apply-templates select="title"/>
          <xsl:if test="title and organization">
            <xsl:text>, </xsl:text>
          </xsl:if>
          <xsl:apply-templates select="organization"/>
        </fo:block>
      </xsl:if>
      <xsl:if test="contact">
        <fo:block>
          <xsl:apply-templates select="contact"/>
        </fo:block>
      </xsl:if>
    </fo:table-cell>
    <xsl:if test="address">
      <fo:table-cell padding-bottom=".5em">
        <fo:block>
          <xsl:apply-templates select="address"/>
        </fo:block>
      </fo:table-cell>
    </xsl:if>
  </fo:table-row>
</xsl:template>

<xsl:template match="referee" mode="standard">
  <fo:block padding-bottom="{$para.break.space}">
    <fo:block padding-bottom="{$half.space}">
      <fo:block font-style="{$referee-name.font.style}"
                font-weight="{$referee-name.font.weight}">
        <xsl:apply-templates select="name"/>
      </fo:block>
      <fo:block>
        <xsl:apply-templates select="title"/>
        <xsl:if test="title and organization">
          <xsl:text>, </xsl:text>
        </xsl:if>
        <xsl:apply-templates select="organization"/>
      </fo:block>
    </fo:block>
    <xsl:if test="address">
      <fo:block padding-bottom="{$half.space}">
        <xsl:apply-templates select="address"/>
      </fo:block>
    </xsl:if>
    <xsl:if test="contact">
      <fo:block padding-bottom="{$half.space}">
        <xsl:apply-templates select="contact"/>
      </fo:block>
    </xsl:if>
  </fo:block>
</xsl:template>

<xsl:template match="year|
                     description|from|to|organization|
                     level|minor|major|institution|
                     annotation|link|score|
                     possible|title">
  <xsl:call-template name="inline"/>
</xsl:template>

<xsl:template match="dayOfMonth">
  <xsl:call-template name="inline"/>
  <xsl:text> </xsl:text>
</xsl:template>

<xsl:template match="month">
  <xsl:call-template name="inline"/>
  <xsl:text>, </xsl:text>
</xsl:template>

<xsl:template match="street2|degrees|resume|contact|
                     legalnotice|note|award/description">
  <xsl:call-template name="div"/>
</xsl:template>

<xsl:template match="break"/>

</xsl:stylesheet>
