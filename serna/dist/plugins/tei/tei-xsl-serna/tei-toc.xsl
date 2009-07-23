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

  <xsl:template match="divGen">
    <fo:block>
      <xsl:choose>
        <xsl:when test="@type='toc'">
          <xsl:call-template name="mainTOC"/>
        </xsl:when>
      </xsl:choose>
    </fo:block>
  </xsl:template>

  <xsl:template name="mainTOC">
    <fo:block>
      <xsl:call-template name="setupDiv1"/>
      <xsl:text>Contents</xsl:text>
    </fo:block>
    <xsl:choose>
      <xsl:when test="ancestor::text/group">
        <xsl:for-each select="ancestor::text/group">
          <xsl:apply-templates select="text" mode="toc"/>
        </xsl:for-each>
      </xsl:when>
      <xsl:when test="ancestor::text/body/div1">
        <xsl:if test="$tocFront">
          <xsl:for-each select="ancestor::text/front/div1|ancestor::text/front//div2|ancestor::text/front//div3">
            <xsl:apply-templates mode="toc" select="(.)"/>
          </xsl:for-each>
        </xsl:if>
        <xsl:for-each select="ancestor::text/body/div1|ancestor::text/body//div2|ancestor::text/body//div3">
          <xsl:apply-templates mode="toc" select="(.)"/>
        </xsl:for-each>
        <xsl:if test="$tocBack">
          <xsl:for-each select="ancestor::text/back/div1|ancestor::text/back//div2|ancestor::text/back//div3">
            <xsl:apply-templates mode="toc" select="(.)"/>
          </xsl:for-each>
        </xsl:if>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="tocBits"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <xsl:template name="tocBits">
  

    <xsl:if test="$tocFront">
      <xsl:for-each select="ancestor::text/front//div">
        <xsl:apply-templates mode="toc" select="(.)"/>
      </xsl:for-each>
    </xsl:if>
    <xsl:for-each select="ancestor::text/body//div">
      <xsl:apply-templates mode="toc" select="(.)"/>
    </xsl:for-each>
    <xsl:if test="$tocBack">
      <xsl:for-each select="ancestor::text/back//div">
        <xsl:apply-templates mode="toc" select="(.)"/>
      </xsl:for-each>
    </xsl:if>
  </xsl:template>

  <xsl:template mode="toc" match="div">
    <xsl:variable name="divlevel" select="count(ancestor::div)"/>
    <xsl:call-template name="tocheading">
      <xsl:with-param name="level">
        <xsl:value-of select="$divlevel"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template mode="toc" match="div0|div1|div2|div3|div4">
    <xsl:call-template name="tocheading">
      <xsl:with-param name="level">
        <xsl:value-of select="substring-after(name(),'div')"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="tocheading">
    <xsl:param name="level"/>
    <xsl:variable name="tocindent">
      <xsl:choose>
        <xsl:when test="$level='0'">
          <xsl:value-of select="$div0Tocindent"/>
        </xsl:when>
        <xsl:when test="$level='1'">
          <xsl:value-of select="$div1Tocindent"/>
        </xsl:when>
        <xsl:when test="$level='2'">
          <xsl:value-of select="$div2Tocindent"/>
        </xsl:when>
        <xsl:when test="$level='3'">
          <xsl:value-of select="$div3Tocindent"/>
        </xsl:when>
        <xsl:when test="$level='4'">
          <xsl:value-of select="$div4Tocindent"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="$div1Tocindent"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <fo:block>
      <xsl:if test="$level='0'">
        <xsl:attribute name="font-weight">bold</xsl:attribute>
      </xsl:if>
      <xsl:attribute name="text-indent">
        <xsl:value-of select="$tocindent"/>
      </xsl:attribute>
      <xsl:variable name="Number">
        <xsl:if test="$numberHeadings and $numberHeadingsDepth &gt; $level">
          <xsl:call-template name="calculateNumber">
            <xsl:with-param name="numbersuffix" select="$tocNumberSuffix"/>
          </xsl:call-template>
        </xsl:if>
      </xsl:variable>
      <xsl:value-of select="$Number"/>
      <xsl:text>&#x2003;</xsl:text>
      <fo:inline>
        <xsl:apply-templates mode="section" select="head"/>
      </fo:inline>
    </fo:block>
  </xsl:template>

  <xsl:template match="text" mode="toc">
    <fo:block>
      <xsl:attribute name="font-weight">bold</xsl:attribute>
      <xsl:number/>
      <xsl:text>&#x2003;</xsl:text>
      <fo:inline>
        <xsl:choose>
          <xsl:when test="front/docTitle[@n]">
            <xsl:value-of select="front/docTitle/@n"/>
          </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="front/docTitle"/>
          </xsl:otherwise>
        </xsl:choose>
      </fo:inline>
    </fo:block>
    <fo:block  font-style="italic"  space-after="10pt" space-before="6pt">
      <xsl:apply-templates select="front//docAuthor" mode="heading"/>
    </fo:block>
  

  </xsl:template>

</xsl:stylesheet>