<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="rear">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="acknowl">
    <fo:block id="acknowl">              
      <fo:block font-size="18pt" xsl:use-attribute-sets="borderbefore headercontrol">
        <xsl:text>Acknowledgements</xsl:text>
      </fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="acknowl/title">
    <fo:block font-size="16pt" xsl:use-attribute-sets="headercontrol">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="bibliog">
    <fo:block id="bibliog">
      <fo:block font-size="24pt"
              xsl:use-attribute-sets="borderbefore headercontrol"
              padding-top="20pt">
        <xsl:text>Bibliography</xsl:text>
      </fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="bibitem">
    <fo:block padding-top="6pt">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="bib">
    <fo:inline>
      <xsl:if test="node()">
        <xsl:text>[</xsl:text>
        <xsl:apply-templates/>
        <xsl:text>] </xsl:text>
      </xsl:if>
    </fo:inline>
  </xsl:template>

  <xsl:template match="pub">
    <fo:inline>
      <xsl:if test="node()">      
        <xsl:text> </xsl:text>
        <xsl:apply-templates/>
      </xsl:if>
    </fo:inline>
  </xsl:template>

  <xsl:template match="fname|suffix">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>
  
  <xsl:template match="surname">
    <fo:inline font-weight="bold" 
               padding-left="2pt" 
               padding-right="2pt">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="email">
    <fo:inline color="{$special.face.color}" font-size="10pt">
        <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="web">
    <fo:inline font-weight="bold" font-size="10pt" color="{$special.face.color}">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template name="authorinfo">
    <fo:block id="authors" padding-top="6pt">
      <fo:block font-size="18pt"
                xsl:use-attribute-sets="borderbefore headercontrol">              
        <xsl:text>Biography</xsl:text>      
      </fo:block>
      <xsl:apply-templates select="$authors" mode="authorinfo"/>
    </fo:block>  
  </xsl:template>

  <xsl:template match="author" mode="authorinfo">
    <fo:block font-size="10pt"
              font-family="{$sans.font.family}"
              line-height="11.5pt"
              padding-top="6pt"
              padding-bottom="6pt">
      <fo:block>
        <xsl:apply-templates select="fname"/>
        <xsl:text> </xsl:text>
        <xsl:apply-templates select="surname"/>
        <xsl:apply-templates select="suffix"/>
      </fo:block>
      <fo:block start-indent="0.3in">
        <xsl:apply-templates select="jobtitle|address|bio"/>
      </fo:block>
    </fo:block>
  </xsl:template>

  <xsl:template match="jobtitle">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="address">
    <fo:block>
      <xsl:apply-templates select="affil|subaffil"/>
      <xsl:apply-templates
        select="aline|city|stateorprovince|country|email|web"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="affil">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="subaffil">
    <fo:inline>      
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="aline|city|country|stateorprovince">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="address/email">
    <fo:inline font-size="10pt">
      <xsl:if test="node()">
        <xsl:text>&lt;</xsl:text>
        <xsl:apply-templates/>
        <xsl:text>&gt;</xsl:text>
      </xsl:if>
    </fo:inline>
  </xsl:template>

  <xsl:template match="address/web">
    <fo:block font-size="11pt">
      <fo:inline color="{$special.face.color}">
        <xsl:apply-templates/>
      </fo:inline>
    </fo:block>
  </xsl:template>

  <xsl:template match="bio">
    <fo:block font-family="{$serif.font.family}">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

</xsl:stylesheet>
