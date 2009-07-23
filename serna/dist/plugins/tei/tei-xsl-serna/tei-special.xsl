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

  <xsl:template match="emph">
    <fo:inline font-style="italic">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>


  <xsl:template match="add">
    <xsl:choose>
      <xsl:when test="@place='sup'">
        <fo:inline baseline-shift="super">
          <xsl:apply-templates/>
        </fo:inline>
      </xsl:when>
      <xsl:when test="@place='sub'">
        <fo:inline baseline-shift="sub">
          <xsl:apply-templates/>
        </fo:inline>
      </xsl:when>
      <xsl:otherwise>
        <fo:inline>
          <xsl:apply-templates/>
        </fo:inline>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="code">
    <fo:inline font-family="{$monospace.font.family}">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="sic">
    <fo:inline>
      <xsl:apply-templates/>
      <xsl:text> (sic)</xsl:text>
    </fo:inline>
  </xsl:template>

  <xsl:template match="corr">
    <fo:inline>
      <xsl:if test="node()">
        <xsl:text>[</xsl:text>
        <xsl:apply-templates/>
        <xsl:text>]</xsl:text>
      </xsl:if>
    </fo:inline>
  </xsl:template>

  <xsl:template match="del">
    <fo:inline text-decoration="line-through">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>


  <xsl:template match="eg">
    <fo:block font-family="{$monospace.font.family}"
              white-space-treatment='preserve'
              white-space-collapse='false'
              linefeed-treatment="preserve"
              wrap-option="no-wrap"
              text-indent="0em"
              hyphenate="false"
              start-indent="{$exampleMargin}"
              text-align="start"
              font-size="{$exampleSize}"
              padding-top="4pt"
              padding-bottom="4pt">
      <xsl:if test="not($flowMarginLeft='')">
        <xsl:attribute name="padding-left">
          <xsl:value-of select="$exampleMargin"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:if test="parent::exemplum">
        <xsl:text>
</xsl:text>
      </xsl:if>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="seg">
    <fo:block font-family="{$monospace.font.family}"
        background-color="yellow"
        white-space-collapse="false"
        wrap-option="no-wrap"
        text-indent="0em"
        start-indent="{$exampleMargin}"
        text-align="start"
        font-size="{$exampleSize}"
        padding-top="8pt"
        padding-bottom="8pt"
        space-before.optimum="4pt"
        space-after.optimum="4pt">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="foreign">
    <fo:inline font-style="italic">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="gap">
    <fo:inline>
      <xsl:if test="@reason">
        <xsl:attribute name="border-color">#000000</xsl:attribute>
        <xsl:attribute name="border-width">5pt</xsl:attribute>
        <xsl:text>[</xsl:text>
        <xsl:value-of select="@reason"/>
        <xsl:text>]</xsl:text>
      </xsl:if>
    </fo:inline>
  </xsl:template>

  <xsl:template match="gi">
    <fo:inline hyphenate="false"
        color="{$giColor}"
        font-family="{$monospace.font.family}">
      <xsl:if test="node()">
        <xsl:text>&lt;</xsl:text>
        <xsl:apply-templates/>
        <xsl:text>&gt;</xsl:text>
      </xsl:if>
    </fo:inline>
  </xsl:template>


  <xsl:template match="gloss">
    <fo:inline font-style="italic">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="hi">
    <fo:inline>
      <xsl:call-template name="rend">
        <xsl:with-param name="defaultvalue" select="string('bold')"/>
        <xsl:with-param name="defaultstyle" select="string('font-weight')"/>
        <xsl:with-param name="rend" select="@rend"/>
      </xsl:call-template>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="ident">
    <fo:inline color="{$identColor}" font-family="{$sans.font.family}">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="kw">
    <fo:inline font-style="italic">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="mentioned">
    <fo:inline>
      <xsl:call-template name="rend">
        <xsl:with-param name="defaultvalue" select="string('italic')"/>
        <xsl:with-param name="defaultstyle" select="string('font-style')"/>
      </xsl:call-template>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="q">
    <xsl:choose>
      <xsl:when test="@rend='display'">
        <fo:block
                text-align="start"
                text-indent="0pt"
                end-indent="{$exampleMargin}"
                start-indent="{$exampleMargin}"
                font-size="{$exampleSize}"
                padding-top="{$exampleBefore}"
                padding-bottom="{$exampleAfter}">
          <xsl:apply-templates/>
        </fo:block>
      </xsl:when>
      <xsl:when test="@rend='eg'">
        <fo:block
                text-align="start"
                font-size="{$exampleSize}"
                padding-top="4pt"
                text-indent="0pt"
                padding-bottom="4pt"
                start-indent="{$exampleMargin}"
                font-family="{$monospace.font.family}">
          <xsl:apply-templates/>
        </fo:block>
      </xsl:when>
      <xsl:when test="@rend = 'qwic'">
        <fo:block
                padding-top="{$spaceAroundTable}"
                padding-bottom="{$spaceAroundTable}">
         <xsl:choose>
         <xsl:when test="q/term">
          <fo:block>
            <fo:table
                        font-size="{$exampleSize}"
                        font-family="{$monospace.font.family}"
                        start-indent="{$exampleMargin}">
              <fo:table-column column-number="1" column-width="" />
              <fo:table-column column-number="2" column-width="" />
              <fo:table-body>
                <xsl:for-each select="q">
                  <xsl:for-each select="term">
                    <fo:table-row>
                      <fo:table-cell>
                        <fo:block>
                          <xsl:apply-templates select="preceding-sibling::node()"/>
                        </fo:block>
                      </fo:table-cell>
                      <fo:table-cell>
                        <fo:block>
                          <xsl:apply-templates/>
                          <xsl:apply-templates select="following-sibling::node()"/>
                        </fo:block>
                      </fo:table-cell>
                    </fo:table-row>
                  </xsl:for-each>
                </xsl:for-each>
              </fo:table-body>
            </fo:table>
          </fo:block>
         </xsl:when>
         <xsl:otherwise>
           <xsl:apply-templates/>
         </xsl:otherwise>
         </xsl:choose>
        </fo:block>
      </xsl:when>
      <xsl:when test="starts-with(@rend,'kwic')">
        <fo:block
                padding-top="{$spaceAroundTable}"
                padding-bottom="{$spaceAroundTable}">
         <xsl:if test="term">
          <fo:table
                        font-size="{$exampleSize}"
                        start-indent="{$exampleMargin}"
                        font-family="{$monospace.font.family}">
            <fo:table-column column-number="1" column-width="" />
            <fo:table-column column-number="2" column-width="" />
            <fo:table-body>
              <xsl:for-each select="term">
                <fo:table-row>
                  <fo:table-cell>
                    <fo:block>
                      <xsl:value-of select="preceding-sibling::node()[1]"/>
                    </fo:block>
                  </fo:table-cell>
                  <fo:table-cell>
                    <fo:block>
                      <xsl:apply-templates/>
                      <xsl:value-of select="following-sibling::node()[1]"/>
                    </fo:block>
                  </fo:table-cell>
                </fo:table-row>
              </xsl:for-each>
            </fo:table-body>
          </fo:table>
         </xsl:if>
        </fo:block>
      </xsl:when>
      <xsl:when test="@rend='literal'">
        <fo:block
                white-space-collapse="false"
                wrap-option="no-wrap"
                font-size="{$exampleSize}"
                padding-top="4pt"
                padding-bottom="4pt"
                start-indent="{$exampleMargin}"
                font-family="{$monospace.font.family}">
          <xsl:apply-templates/>
        </fo:block>
      </xsl:when>
      <xsl:otherwise>
        <fo:inline>
          <xsl:if test="node()">
            <xsl:text>&#x201C;</xsl:text>
            <xsl:apply-templates/>
            <xsl:text>&#x201D;</xsl:text>
          </xsl:if>
        </fo:inline>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="epigraph/q">
    <fo:block
        padding-top="4pt"
        padding-bottom="4pt"
        start-indent="{$exampleMargin}">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="reg">
    <fo:inline font-family="{$sans.font.family}">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="soCalled">
    <fo:inline>
      <xsl:if test="node()">
        <xsl:text>'</xsl:text>
        <xsl:apply-templates/>
        <xsl:text>'</xsl:text>
      </xsl:if>
    </fo:inline>
  </xsl:template>


  <xsl:template match="term">
    <fo:inline font-style="italic">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="title">
    <xsl:choose>
      <xsl:when test="@level='a'">
        <fo:inline>
          <xsl:if test="node()">
            <xsl:text>'</xsl:text>
            <xsl:apply-templates/>
            <xsl:text>'</xsl:text>
          </xsl:if>
        </fo:inline>
      </xsl:when>
      <xsl:when test="@level='m'">
        <fo:inline font-style="italic">
          <xsl:apply-templates/>
        </fo:inline>
      </xsl:when>
      <xsl:when test="@level='s'">
        <fo:inline font-style="italic">
          <xsl:apply-templates/>
        </fo:inline>
      </xsl:when>
      <xsl:otherwise>
        <fo:inline font-style="italic">
          <xsl:apply-templates/>
        </fo:inline>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="unclear">
    <fo:inline text-decoration="blink" color="#ff0000" background-color="#0f0fff">
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="abbr">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="date">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="index">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="interp">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="interpGrp">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="rs">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="s">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="name">
    <fo:inline>
      <xsl:apply-templates/>
    </fo:inline>
  </xsl:template>

  <xsl:template match="opener">
    <fo:block>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

</xsl:stylesheet>
