<!-- 
     TEI XSL Stylesheet for Syntext Serna (c) 2003, Syntext Inc.

     The Stylesheet is based on Sebastian Rahtz/Oxford University 
     XSL TEI Stylesheet v1.3. See file SR-COPYING for 
     Sebastian Rahtz/Oxford University Copyright 
     information.
-->
<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="/TEI.2" priority="1" mode="root.mode">
    <fo:page-sequence text-align="{$alignment}"
                     hyphenate="{$hyphenate}"
                     language="{$language}"
                     master-reference="body">
      <fo:flow flow-name="xsl-region-body"
                font-family="{$body.font.family}"
                font-size="{$body.font.size}">
        <xsl:if test="not($flowMarginLeft='')">
          <xsl:attribute name="margin-left">
            <xsl:value-of select="$flowMarginLeft"/>
          </xsl:attribute>
        </xsl:if>
        <xsl:apply-templates select="."/>
      </fo:flow>
    </fo:page-sequence>
  </xsl:template>

  <xsl:template match="TEI.2">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="text">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="front | body | group | trailer">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="div">
    <fo:block>
      <xsl:choose>
        <xsl:when test="@type='bibliog' or @type='frontispiece' or @type='epistle' or @type='illustration'">
          <xsl:apply-templates/>
        </xsl:when>

        <xsl:when test="@type='abstract'">
          <fo:block keep-with-next.within-page="always"
                end-indent="{$exampleMargin}"
                start-indent="{$exampleMargin}">
            <xsl:attribute name="text-align">center</xsl:attribute>
            <xsl:call-template name="setupDiv2"/>
            <fo:inline font-style="italic">Abstract</fo:inline>
          </fo:block>
          <xsl:apply-templates/>
        </xsl:when>
    	

        <xsl:when test="@type='ack'">
          <fo:block keep-with-next.within-page="always">
            <xsl:attribute name="text-align">start</xsl:attribute>
            <xsl:call-template name="setupDiv3"/>
            <fo:inline font-style="italic">Acknowledgements</fo:inline>
          </fo:block>
          <xsl:apply-templates/>
        </xsl:when>

        <xsl:otherwise>
          <fo:block>
            <xsl:variable name="divlevel" select="count(ancestor::div)"/>
            <xsl:call-template name="NumberedHeading">
              <xsl:with-param name="level">
                <xsl:value-of select="$divlevel"/>
              </xsl:with-param>
            </xsl:call-template>
          </fo:block>
          <xsl:apply-templates select="*"/>
        </xsl:otherwise>
      </xsl:choose>
    </fo:block>
  </xsl:template>

  <xsl:template match="div0|div1|div2|div3|div4">
    <fo:block>
      <xsl:call-template name="NumberedHeading">
        <xsl:with-param name="level">
          <xsl:value-of select="substring-after(name(),'div')"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:apply-templates select="*"/>
    </fo:block>
  </xsl:template>

  <xsl:template name="NumberedHeading">
    <xsl:param name="level"/>
    <fo:block>
      <xsl:if test="@id">
        <xsl:variable name="divid">
          <xsl:call-template name="idLabel"/>
        </xsl:variable>
        <xsl:attribute name="id">
          <xsl:value-of select="$divid"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="text-align">start</xsl:attribute>

      <xsl:attribute name="font-family">
        <xsl:value-of select="$title.font.family"/>
      </xsl:attribute>

      <xsl:choose>
        <xsl:when test="$level=0">
          <xsl:call-template name="setupDiv0"/>
        </xsl:when>
        <xsl:when test="$level=1">
          <xsl:call-template name="setupDiv1"/>
        </xsl:when>
        <xsl:when test="$level=2">
          <xsl:call-template name="setupDiv2"/>
        </xsl:when>
        <xsl:when test="$level=3">
          <xsl:call-template name="setupDiv3"/>
        </xsl:when>
        <xsl:when test="$level=4">
          <xsl:call-template name="setupDiv4"/>
        </xsl:when>
      </xsl:choose>
      <xsl:call-template name="blockStartHook"/>

      <xsl:variable name="Number">
        <xsl:if test="$numberHeadings and $numberHeadingsDepth &gt; $level">
          <xsl:call-template name="calculateNumber">
            <xsl:with-param name="numbersuffix" select="$headingNumberSuffix"/>
          </xsl:call-template>
        </xsl:if>
      </xsl:variable>
      <xsl:value-of select="$Number"/>
      <xsl:text> </xsl:text>
      <xsl:apply-templates mode="section" select="head[not(self::processing-instruction('se:choice'))]"/>
    </fo:block>
  </xsl:template>

  <xsl:template mode="section" match="head">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="head">
    <xsl:variable name="parent" select="name(..)"/>
    <xsl:if test="not(starts-with($parent,'div'))">
      <fo:inline>
        <xsl:apply-templates/>
      </fo:inline>
    </xsl:if>
  </xsl:template>

  <xsl:template name="calculateNumber">
    <xsl:param name="numbersuffix"/>
    <xsl:choose>
      <xsl:when test="@n">
        <xsl:value-of select="@n"/>
        <xsl:value-of select="$numbersuffix"/>
      </xsl:when>
      <xsl:when test="ancestor::front">
        <xsl:if test="not($numberFrontHeadings='')">
          <xsl:choose>
            <xsl:when test="$numberFrontHeadings='1.'">
              <xsl:number format="1." level="multiple"  from="text"
               count="div|div1|div2|div3|div4"/>
            </xsl:when>
            <xsl:when test="$numberFrontHeadings='i.'">
              <xsl:number format="i." level="multiple"  from="text"
               count="div|div1|div2|div3|div4"/>
            </xsl:when>
            <xsl:when test="$numberFrontHeadings='I.'">
              <xsl:number format="I." level="multiple"  from="text"
               count="div|div1|div2|div3|div4"/>
            </xsl:when>
            <xsl:when test="$numberFrontHeadings='i.1'">
              <xsl:number format="i.1" level="multiple"  from="text"
               count="div|div1|div2|div3|div4"/>
            </xsl:when>
            <xsl:when test="$numberFrontHeadings='I.1'">
              <xsl:number format="I.1" level="multiple"  from="text"
               count="div|div1|div2|div3|div4"/>
            </xsl:when>
            <xsl:when test="$numberFrontHeadings='a.'">
              <xsl:number format="a." level="multiple"  from="text"
               count="div|div1|div2|div3|div4"/>
            </xsl:when>
            <xsl:when test="$numberFrontHeadings='a.1'">
              <xsl:number format="a.1" level="multiple"  from="text"
               count="div|div1|div2|div3|div4"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:number format="A.1" level="multiple"  from="text"
               count="div|div1|div2|div3|div4"/>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:value-of select="$numbersuffix"/>
        </xsl:if>
      </xsl:when>
      <xsl:when test="ancestor::back">
        <xsl:if test="not($numberBackHeadings='')">
          <xsl:value-of select="$appendixWords"/>
          <xsl:text> </xsl:text>
          <xsl:choose>
            <xsl:when test="$numberBackHeadings='1.'">
              <xsl:number format="1." level="multiple"  from="text"
               count="div|div1|div2|div3|div4"/>
            </xsl:when>
            <xsl:when test="$numberBackHeadings='i.'">
              <xsl:number format="i." level="multiple"  from="text"
               count="div|div1|div2|div3|div4"/>
            </xsl:when>
            <xsl:when test="$numberBackHeadings='I.'">
              <xsl:number format="I." level="multiple"  from="text"
               count="div|div1|div2|div3|div4"/>
            </xsl:when>
            <xsl:when test="$numberBackHeadings='i.1'">
              <xsl:number format="i.1" level="multiple"  from="text"
               count="div|div1|div2|div3|div4"/>
            </xsl:when>
            <xsl:when test="$numberBackHeadings='I.1'">
              <xsl:number format="I.1" level="multiple"  from="text"
               count="div|div1|div2|div3|div4"/>
            </xsl:when>
            <xsl:when test="$numberBackHeadings='a.'">
              <xsl:number format="a." level="multiple"  from="text"
               count="div|div1|div2|div3|div4"/>
            </xsl:when>
            <xsl:when test="$numberBackHeadings='a.1'">
              <xsl:number format="a.1" level="multiple"  from="text"
               count="div|div1|div2|div3|div4"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:number format="A.1" level="multiple"  from="text"
               count="div|div1|div2|div3|div4"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:if>
      </xsl:when>
      <xsl:otherwise>
        <xsl:number level="multiple"  from="text"
         count="div|div1|div2|div3|div4"/>
        <xsl:value-of select="$numbersuffix"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:stylesheet>
