<?xml version='1.0'?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
                extension-element-prefixes="dtm"
                version='1.0'>

<xsl:attribute-set name="table.data" ><!-- use for overall table default characteristics -->
  <xsl:attribute name="table-layout">fixed</xsl:attribute>
  <xsl:attribute name="width">100%</xsl:attribute>
  <!--xsl:attribute name="inline-progression-dimension">auto</xsl:attribute-->
  <xsl:attribute name="space-before">10pt</xsl:attribute>
  <xsl:attribute name="space-after">10pt</xsl:attribute>
  <xsl:attribute name="background-color">white</xsl:attribute>
  <!--xsl:attribute name="start-indent">inherit</xsl:attribute-->
</xsl:attribute-set>

<xsl:attribute-set name="table.data.caption" >
  <xsl:attribute name="start-indent">inherit</xsl:attribute>
  <xsl:attribute name="font-weight">bold</xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="table.data.tbody" >
  <xsl:attribute name="background-color">white</xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="table.data.th" >
  <xsl:attribute name="color">black</xsl:attribute>
  <!--xsl:attribute name="background-color">silver</xsl:attribute-->
  <xsl:attribute name="padding">2pt</xsl:attribute>
  <xsl:attribute name="text-align">center</xsl:attribute>
  <xsl:attribute name="font-weight">bold</xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="table.data.tf" >
  <xsl:attribute name="color">blue</xsl:attribute>
  <xsl:attribute name="padding">2pt</xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="table.data.td" >
  <!--xsl:attribute name="background-color">grey</xsl:attribute-->
  <xsl:attribute name="padding">2pt</xsl:attribute>
</xsl:attribute-set>


<!-- simpletable support -->
<dtm:doc dtm:elements="simpletable|properties" dtm:status="finished" dtm:idref="topic.simpletable"/>
<xsl:template match="*[contains(@class,' topic/simpletable ')]" dtm:id="topic.simpletable">
<fo:block padding-top="1em">
  <fo:table xsl:use-attribute-sets="table.data frameall">
    <xsl:call-template name="semtbl-colwidth"/>
    <fo:table-body>
      <xsl:call-template name="gen-dflt-data-hdr"/>
      <xsl:apply-templates select="*"/>
    </fo:table-body>
  </fo:table>
</fo:block>
</xsl:template>

<!-- if cells have default spectitle values, copy these up to a first new hdr row -->
<dtm:doc dtm:status="testing" dtm:idref="hdr.gen-default-data"/>
<xsl:template name="gen-dflt-data-hdr" dtm:id="hdr.gen-default-data">
  <xsl:if test="*/*[contains(@class,' topic/stentry ')]/@specentry">
    <fo:table-row>
       <xsl:for-each select="*[contains(@class,' topic/strow ')][1]/*">
          <fo:table-cell start-indent="2pt" background-color="silver"
            padding="2pt" text-align="center" font-weight="bold"
            xsl:use-attribute-sets="frameall">
            <xsl:attribute name="column-number">
                <xsl:number count="*"/>
            </xsl:attribute>
            <fo:block>
                <xsl:value-of select="@specentry"/>
            </fo:block>
          </fo:table-cell>
        </xsl:for-each>
    </fo:table-row>
  </xsl:if>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="table.colwidth.proportional"/>
<xsl:template name="semtbl-colwidth" dtm:id="table.colwidth.proportional">
  <!-- Find the total number of relative units for the table. If @relcolwidth="1* 2* 2*",
       the variable is set to 5. -->
  <xsl:variable name="totalwidth">
    <xsl:if test="@relcolwidth and not(@relcolwidth='')">
      <xsl:call-template name="find-total-table-width"/>
    </xsl:if>
  </xsl:variable>
  <!-- Find how much of the table each relative unit represents. If @relcolwidth is 1* 2* 2*,
       there are 5 units. So, each unit takes up 100/5, or 20% of the table. Default to 0,
       which the entries will ignore. -->
  <!-- bad relcolwidth data will still generate "NaN" errors, however -->
  <xsl:variable name="width-multiplier">
    <xsl:choose>
      <xsl:when test="@relcolwidth and not(@relcolwidth='')">
        <xsl:value-of select="100 div $totalwidth"/>
      </xsl:when>
      <xsl:otherwise>0</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <!-- RDA: specialize strow, so choicetable will work -->
  <xsl:for-each select="*[contains(@class,' topic/sthead ')]/*"><!-- use cells as counters; avoid recursion! -->
    <!-- Determine which column this entry is in. -->
    <xsl:variable name="thiscolnum"><xsl:value-of select="position()"/></xsl:variable>
<!--xsl:comment>Relwidth should be (<xsl:call-template name="get-proportional-width">
            <xsl:with-param name="entry-num"><xsl:value-of select="$thiscolnum"/></xsl:with-param>
            <xsl:with-param name="all-widths"><xsl:value-of select="../../@relcolwidth"/></xsl:with-param>
          </xsl:call-template>)</xsl:comment-->    
    <fo:table-column>
      <xsl:if test="../../@relcolwidth and not(../../@relcolwidth='')">
        <!-- RDA: use relative width from called template, instead of old value with inches -->
        <!--<xsl:attribute name="column-width">
          <xsl:variable name="thispct">
            <xsl:call-template name="get-current-entry-percentage">
              <xsl:with-param name="multiplier"><xsl:value-of select="$width-multiplier"/></xsl:with-param>
              <xsl:with-param name="entry-num"><xsl:value-of select="$thiscolnum"/></xsl:with-param>
            </xsl:call-template>
          </xsl:variable> 
          <xsl:value-of select="number(198 * $thispct div 100 div 36)"/>in<xsl:text/>
        </xsl:attribute>-->
        <xsl:attribute name="column-width">proportional-column-width(<xsl:call-template name="get-proportional-width">
            <xsl:with-param name="entry-num"><xsl:value-of select="$thiscolnum"/></xsl:with-param>
            <xsl:with-param name="all-widths"><xsl:value-of select="../../@relcolwidth"/></xsl:with-param>
          </xsl:call-template>)</xsl:attribute>
      </xsl:if>
    </fo:table-column>
  </xsl:for-each>
</xsl:template>

<!-- RDA: used to get relative column width for simpletables. If in the first entry, take
          the first width. Otherwise, chop off a width, and proceed to the next value, until
          getting to the proper one. -->
<dtm:doc dtm:status="testing" dtm:idref="propwidth"/>
<xsl:template name="get-proportional-width" dtm:id="propwidth">
  <xsl:param name="entry-num">0</xsl:param>
  <xsl:param name="all-widths"><xsl:value-of select="../@relcolwidth"/></xsl:param>
  <xsl:choose>
    <xsl:when test="$entry-num &lt;= 0"/>
    <xsl:when test="$entry-num=1">
      <xsl:value-of select="substring-before($all-widths,'*')"/>
    </xsl:when> 
    <xsl:when test="not(contains($all-widths,' '))"/>
    <xsl:otherwise>
      <xsl:call-template name="get-proportional-width">
        <xsl:with-param name="entry-num"><xsl:value-of select="$entry-num - 1"/></xsl:with-param>
        <xsl:with-param name="all-widths"><xsl:value-of select="substring-after($all-widths,' ')"/></xsl:with-param>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- Use @relcolwidth to find the total width of the table. That is, if the attribute is set
     to 1* 2* 2* 1*, then the table is 6 units wide. -->
<dtm:doc dtm:status="testing" dtm:idref="table.totalwidth"/>
<xsl:template name="find-total-table-width" dtm:id="table.totalwidth">
  <!-- Start with relcolwidth, and each recursive call will remove the first value -->
  <xsl:param name="relcolwidth"><xsl:value-of select="@relcolwidth"/></xsl:param>
  <!-- Determine the first value, which is the value before the first asterisk -->
  <xsl:variable name="firstval">
    <xsl:if test="contains($relcolwidth,'*')">
      <xsl:value-of select="substring-before($relcolwidth,'*')"/>
    </xsl:if>
  </xsl:variable>
  <!-- Begin processing if we were able to find a first value -->
  <xsl:if test="string-length($firstval)>0">
    <!-- Chop off the first value, and set morevals to the remainder -->
    <xsl:variable name="morevals"><xsl:value-of select="substring-after($relcolwidth,' ')"/></xsl:variable>
    <xsl:choose>
      <!-- If there are additional values, call this template on the remainder.
           Add the result of that call to the first value. -->
      <xsl:when test="string-length($morevals)>0">
        <xsl:variable name="nextval">   <!-- The total of the remaining values -->
          <xsl:call-template name="find-total-table-width">
            <xsl:with-param name="relcolwidth"><xsl:value-of select="$morevals"/></xsl:with-param>
          </xsl:call-template>
        </xsl:variable>
        <xsl:value-of select="number($firstval)+number($nextval)"/>
      </xsl:when>
      <!-- If there are no more values, return the first (and only) value -->
      <xsl:otherwise><xsl:value-of select="$firstval"/></xsl:otherwise>
    </xsl:choose>
  </xsl:if>
</xsl:template>

<!-- Find the width of the current cell. Multiplier is how much each unit of width is multiplied to total 100.
     Entry-num is the current entry. Current-col is what column we are at when scanning @relcolwidth.
     Relcolvalues is the unscanned part of @relcolwidth. -->
<dtm:doc dtm:status="testing" dtm:idref="entry.percentage"/>
<xsl:template name="get-current-entry-percentage" dtm:id="entry.percentage">
  <xsl:param name="multiplier">1</xsl:param>  <!-- Each relative unit is worth this many percentage points -->
  <xsl:param name="entry-num"/>               <!-- The entry number of the cell we are evaluating now -->
  <xsl:param name="current-col">1</xsl:param> <!-- Position within the recursive call to evaluate @relcolwidth -->
  <!-- relcolvalues begins with @relcolwidth. Each call to the template removes the first value. -->
  <xsl:param name="relcolvalues"><xsl:value-of select="parent::*/parent::*/@relcolwidth"/></xsl:param>

  <xsl:choose>
    <!-- If the recursion has moved up to the proper cell, multiply $multiplier by the number of
         relative units for this column. -->
    <xsl:when test="$entry-num = $current-col">
      <xsl:variable name="relcol"><xsl:value-of select="substring-before($relcolvalues,'*')"/></xsl:variable>
      <xsl:value-of select="$relcol * $multiplier"/>
    </xsl:when>
    <!-- Otherwise, call this template again, removing the first value form @relcolwidth. Also add one
         to $current-col. -->
    <xsl:otherwise>
      <xsl:call-template name="get-current-entry-percentage">
        <xsl:with-param name="multiplier"><xsl:value-of select="$multiplier"/></xsl:with-param>
        <xsl:with-param name="entry-num"><xsl:value-of select="$entry-num"/></xsl:with-param>
        <xsl:with-param name="current-col"><xsl:value-of select="$current-col + 1"/></xsl:with-param>
        <xsl:with-param name="relcolvalues"><xsl:value-of select="substring-after($relcolvalues,' ')"/></xsl:with-param>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<dtm:doc dtm:elements="sthead|prophead" dtm:status="finished" dtm:idref="topic.sthead"/>
<xsl:template match="*[contains(@class,' topic/sthead ')]" dtm:id="topic.sthead">
  <fo:table-row>
    <xsl:apply-templates select="*"/>
  </fo:table-row>
</xsl:template>

<dtm:doc dtm:elements="strow|property" dtm:status="finished" dtm:idref="topic.strow"/>
<xsl:template match="*[contains(@class,' topic/strow ')]" dtm:id="topic.strow">
  <fo:table-row>
    <xsl:apply-templates select="*"/>
  </fo:table-row>
</xsl:template>

<dtm:doc dtm:elements="chrow" dtm:status="finished" dtm:idref="topic.chrow"/>
<xsl:template match="*[contains(@class,' topic/chrow ')]" dtm:id="topic.chrow">
  <fo:table-row>
    <xsl:apply-templates select="*"/>
  </fo:table-row>
</xsl:template>

<!-- in the following two rules, the xsl:number needs to give the
  sequence number of either the current semdh or sementry in the current
  semrow OR semhdr. -->
<dtm:doc dtm:elements="sthead/stentry|proptypehd|propvaluehd|propdeschd" dtm:status="finished" dtm:idref="topic.sthead.stentry"/>
<xsl:template match="*[contains(@class,' topic/sthead ')]/*[contains(@class,' topic/stentry ')]" priority="2" dtm:id="topic.sthead.stentry">
  <fo:table-cell start-indent="2pt" background-color="silver" padding="2pt" text-align=
"center" font-weight="bold" xsl:use-attribute-sets="frameall">
    <xsl:attribute name="column-number"><xsl:number count="*"/></xsl:attribute>
    <fo:block>
    <xsl:call-template name="get-title"/>
    </fo:block>
  </fo:table-cell>
</xsl:template>


<xsl:template name="stentry.colnum" dtm:id="stentry.colnum">
  <xsl:variable name="elem-name">
    <xsl:value-of select="local-name()"/>
  </xsl:variable>
  <xsl:variable name="localkeycol">
    <xsl:choose>
      <xsl:when test="ancestor::*[contains(@class,' topic/simpletable ')]/@keycol">
        <xsl:value-of select="ancestor::*[contains(@class,' topic/simpletable ')]/@keycol"/>
      </xsl:when>
      <xsl:otherwise>0</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="thisnum">
    <xsl:choose>
      <xsl:when test="not(ancestor::*[contains(@class,' topic/simpletable ')]/
                                    *[contains(@class,' topic/sthead ')]/
                                    *[concat($elem-name, 'hd')=local-name()])">
        <xsl:number count="*"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:for-each select="ancestor::*[contains(@class,' topic/simpletable ')]/
                                        *[contains(@class,' topic/sthead ')]/*">
          <xsl:if test="concat($elem-name, 'hd')=local-name()">
            <xsl:value-of select="position()"/>
          </xsl:if>
        </xsl:for-each>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:if test="$thisnum=$localkeycol">
    <xsl:attribute name="font-weight">bold</xsl:attribute>
  </xsl:if>
  <xsl:attribute name="column-number">
    <xsl:value-of select="$thisnum"/>
  </xsl:attribute>
  <xsl:attribute name="start-indent">2pt</xsl:attribute>
  <xsl:attribute name="padding">2pt</xsl:attribute>
  <xsl:attribute name="background-color">#fafafa</xsl:attribute>
</xsl:template>

<dtm:doc dtm:elements="stentry|proptype|propvalue|propdesc" dtm:status="finished" dtm:idref="stentry.colnum topic.stentry topic.sthead.stentry"/>
<xsl:template match="*[contains(@class,' topic/stentry ')]" dtm:id="topic.stentry">
  <fo:table-cell xsl:use-attribute-sets="frameall">
     <xsl:call-template name="stentry.colnum"/>
     <fo:block>
      <xsl:apply-templates/>
     </fo:block>
  </fo:table-cell>
</xsl:template>

<!-- CHOICETABLE -->
<dtm:doc dtm:elements="chrow" dtm:status="testing"/>

<dtm:doc dtm:elements="choicetable" dtm:status="finished" dtm:idref="task.choicetable choicetable.header.default"/>
<xsl:template match="*[contains(@class,' task/choicetable ')]"
priority="2" dtm:id="task.choicetable">
<fo:block space-before="12pt">
  <fo:table xsl:use-attribute-sets="table.data frameall">
    <xsl:call-template name="semtbl-colwidth"/>
    <fo:table-body>
      <!--xsl:call-template name="gen-dflt-data-hdr"/-->
      <xsl:call-template name="get-chhead"/>
      <xsl:apply-templates select="*"/>
    </fo:table-body>
  </fo:table>
</fo:block>
</xsl:template>

<!--If the choicetable has no header - output a default one-->
<xsl:template name="get-chhead" dtm:id="choicetable.header.default">
  <xsl:choose>
  <xsl:when test="not(./*[contains(@class,' task/chhead ')])">
  <fo:table-row>
    <fo:table-cell start-indent="2pt" background-color="silver"
                   padding="2pt" text-align= "center" 
                   font-weight="bold" xsl:use-attribute-sets="frameall">
    <fo:block>
    <xsl:call-template name="getString">
      <xsl:with-param name="stringName" select="'Option'"/>
    </xsl:call-template>
    </fo:block>
    </fo:table-cell>
    <fo:table-cell start-indent="2pt" background-color="silver"
                   padding="2pt" text-align="center" 
                   font-weight="bold" xsl:use-attribute-sets="frameall">
    <fo:block>
        <xsl:call-template name="getString">
            <xsl:with-param name="stringName" select="'Description'"/>
        </xsl:call-template>
    </fo:block>
    </fo:table-cell>
  </fo:table-row>
  </xsl:when>
  <xsl:otherwise>
  <fo:table-row>
    <fo:table-cell start-indent="2pt" background-color="silver"
padding="2pt" text-align=
  "center" font-weight="bold" xsl:use-attribute-sets="frameall">
<fo:block>
     <xsl:apply-templates select="*[contains(@class,' task/chhead ')]
        /*[contains(@class,' task/choptionhd ')]" mode="chtabhdr"/>
</fo:block>
    </fo:table-cell>

    <fo:table-cell start-indent="2pt" background-color="silver"
padding="2pt" text-align=
  "center" font-weight="bold" xsl:use-attribute-sets="frameall">
     <xsl:attribute name="id">
     <xsl:choose>
      <!-- if the description header has an ID, use that -->
      <xsl:when test="*[contains(@class,' task/chhead ')]/
                      *[contains(@class,' task/chdeschd ')]/@id">
       <xsl:value-of select="*[contains(@class,' task/chhead ')]/
            *[contains(@class,' task/chdeschd ')]/@id"/>
       <xsl:text>-desc</xsl:text>
      </xsl:when>
      <xsl:otherwise>  <!-- output a default descr header ID -->
       <xsl:value-of select="generate-id(.)"/><xsl:text>-desc</xsl:text>
      </xsl:otherwise>
     </xsl:choose>
     </xsl:attribute>
    <fo:block>
     <xsl:apply-templates select="*[contains(@class,' task/chhead ')]/
            *[contains(@class,' task/chdeschd ')]" mode="chtabhdr"/>
    </fo:block>
    </fo:table-cell>

  </fo:table-row>
  </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- headers are called above, hide the fall thru -->

<dtm:doc dtm:elements="chhead" dtm:status="finished" dtm:idref="task.chhead"/>
<xsl:template match="*[contains(@class, ' task/chhead ')]"  priority="2" dtm:id="task.chhead"/>

<dtm:doc dtm:elements="chhead/choptionhd" dtm:status="finished" dtm:idref="chhead.choptionhd.chtabhdr task.chhead.choptionhd"/>
<xsl:template match="*[contains(@class, ' task/chhead ')]/
        *[contains(@class,' task/choptionhd ')]"  priority="2" dtm:id="task.chhead.choptionhd"/>

<dtm:doc dtm:elements="chhead/chdeschd" dtm:status="finished" dtm:idref="chhead.chdeschd.chtabhdr task.chhead.chdeschd"/>
<xsl:template match="*[contains(@class, ' task/chhead ')]/
        *[contains(@class,' task/chdeschd ')]"  priority="2" dtm:id="task.chhead.chdeschd"/>

<!-- Option & Description headers -->
<xsl:template match="*[contains(@class, ' task/chhead ')]/
                     *[contains(@class,' task/choptionhd ')]"
              mode="chtabhdr" priority="2" dtm:id="chhead.choptionhd.chtabhdr">
 <fo:block>
   <xsl:apply-templates/>
 </fo:block>
</xsl:template>

<xsl:template match="*[contains(@class,' task/chhead ')]/
                     *[contains(@class,' task/chdeschd ')]"
              mode="chtabhdr" priority="2" dtm:id="chhead.chdeschd.chtabhdr">
 <fo:block>
   <xsl:apply-templates/>
 </fo:block>
</xsl:template>

<dtm:doc dtm:elements="choption" dtm:status="finished" dtm:idref="task.choption"/>
<xsl:template match="*[contains(@class,' task/choption ')]" priority="2" dtm:id="task.choption">
  <fo:table-cell start-indent="2pt" background-color="#fafafa"
                 padding="2pt" xsl:use-attribute-sets="frameall">
    <xsl:attribute name="column-number"><xsl:number count="*"/></xsl:attribute>
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </fo:table-cell>
</xsl:template>

<dtm:doc dtm:elements="chdesc" dtm:status="finished" dtm:idref="task.chdesc"/>
<xsl:template match="*[contains(@class,' task/chdesc ')]" priority="2" dtm:id="task.chdesc">
  <fo:table-cell start-indent="2pt" background-color="#fafafa"
                 padding="2pt" xsl:use-attribute-sets="frameall">
    <xsl:attribute name="column-number"><xsl:number count="*"/></xsl:attribute>
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </fo:table-cell>
</xsl:template>

<!-- end of simpletable section -->

</xsl:stylesheet>
