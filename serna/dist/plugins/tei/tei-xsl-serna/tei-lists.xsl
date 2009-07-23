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

<xsl:template match="list[@type='catalogue']">
  <fo:block
        padding-top="{$spaceAroundTable}"
        padding-bottom="{$spaceAroundTable}">
 <fo:table>
   <fo:table-column column-number="1" column-width="20%"/>
   <fo:table-column column-number="2" column-width="80%"/>
   <fo:table-body>
     <xsl:for-each select="item">
       <fo:table-row>
         <xsl:apply-templates select="." mode="catalogue"/>
       </fo:table-row>
      </xsl:for-each>
   </fo:table-body>
 </fo:table>
  </fo:block>
</xsl:template>

<xsl:template match="item" mode="catalogue">
    <fo:table-cell>
        <fo:block>
            <xsl:choose>
                <xsl:when test="label">
                    <fo:inline font-weight='bold'>
                        <xsl:apply-templates select="label" mode="print"/>
                    </fo:inline>
                </xsl:when>
                <xsl:otherwise>
                    <fo:inline font-weight='bold'>
                        <xsl:apply-templates mode="print" 
                            select="preceding-sibling::*[1]"/>
                    </fo:inline>
                </xsl:otherwise>
            </xsl:choose>
        </fo:block>
    </fo:table-cell>
    <fo:table-cell>
        <fo:block><xsl:apply-templates/></fo:block>
    </fo:table-cell>
</xsl:template>

<xsl:template mode="print" match="label">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="head" mode="list">
  <fo:block font-style="italic" 
    text-align="start"
    padding-top="4pt">
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<xsl:template match="list">
    <fo:block>
    <xsl:if test="head">
      <xsl:apply-templates select="head" mode="list"/>
    </xsl:if>

    <xsl:if test="label|item">
    <fo:list-block padding-right="{$listRightMargin}">

      <!-- Setting appropriate indent for the list -->
      <xsl:call-template name="setListIndents"/>
      <xsl:choose>
        <xsl:when test="label">
          <xsl:attribute name="provisional-label-separation">
             <xsl:value-of select="$listGlossSeparation"/>          
          </xsl:attribute>
          <xsl:choose>
            <xsl:when test="starts-with(@rend,'indent(')">
              <xsl:attribute name="provisional-distance-between-starts">
                <xsl:value-of 
                  select="concat(substring-before(substring-after(@rend,'('),')'),'em')"/>
              </xsl:attribute>
            </xsl:when>
            <xsl:otherwise>
              <xsl:attribute name="provisional-distance-between-starts">
                <xsl:value-of select="$listGlossWidth"/>
              </xsl:attribute>
            </xsl:otherwise>
          </xsl:choose>
          <xsl:attribute name="padding-left">
            <xsl:choose>
              <xsl:when test="ancestor::list">
                <xsl:value-of select="$listLeftGlossInnerIndent"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="$listLeftGlossIndent"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="provisional-label-separation">
             <xsl:value-of select="$listLabelSeparation"/>
          </xsl:attribute>
          <xsl:attribute name="provisional-distance-between-starts">
             <xsl:value-of select="$listLabelWidth"/>
          </xsl:attribute>
          <xsl:attribute name="padding-left">
            <xsl:value-of select="$listLeftIndent"/>
          </xsl:attribute>
        </xsl:otherwise> 
      </xsl:choose>
      
      <!-- Rendering the content -->
      <xsl:choose>
        <xsl:when test="label">
          <!-- Common list-item fo for label and item -->
          <xsl:apply-templates select="label" mode="label-item-pair"/>
        </xsl:when>
       <xsl:otherwise>
         <!-- Rendering "item" sequence -->
         <xsl:apply-templates select="item" mode="single-items"/>
       </xsl:otherwise>
     </xsl:choose>    
   </fo:list-block>
    </xsl:if>
 </fo:block>
</xsl:template>

<xsl:template match="item" mode="single-items">
  <fo:list-item>
    <xsl:if test="not(parent::note[@place='foot'])">
      <xsl:attribute name="padding-top">
        <xsl:value-of select="$listItemsep"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:call-template name="labelItem">
      <xsl:with-param name="item" select="."/>
    </xsl:call-template>
    <xsl:apply-templates select="."/>                        
  </fo:list-item>
</xsl:template>

<xsl:template match="label" mode="label-item-pair">
  <!-- We call this pseodo-wrapper in order to show "fo:list-item" 
       element in content of "list" element. -->
  <xsl:apply-templates select="ancestor::list[1]" mode="pseodo-wrapper">
    <xsl:with-param name="content">
      <xsl:apply-templates select="."/>
      <xsl:apply-templates select="following-sibling::*[1][self::item]"/>
    </xsl:with-param>
  </xsl:apply-templates>
</xsl:template>

<xsl:template match="list" mode="pseodo-wrapper">
  <xsl:param name="content" select="."/>
  <fo:list-item>
    <xsl:if test="not(note[@place='foot'])">
      <xsl:attribute name="padding-top">
        <xsl:value-of select="$listItemsep"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:copy-of select="$content"/>
  </fo:list-item>
</xsl:template>

<xsl:template match="label">
  <xsl:call-template name="labelItem"/>
</xsl:template>

<xsl:template name="labelItem">
    <fo:list-item-label end-indent="label-end()">
        <xsl:if test="@id">
            <xsl:attribute name="id"><xsl:value-of select="@id"/></xsl:attribute>
        </xsl:if>
        <fo:block>
            <xsl:attribute name="padding-right">2.5pt</xsl:attribute>
            <xsl:choose>
                <xsl:when test="local-name()='item' and @n">
                    <xsl:attribute name="text-align">end</xsl:attribute>
                    <xsl:value-of select="@n"/>
                </xsl:when>
                <xsl:when test="../@type='bibliography'">
                    <xsl:attribute name="text-align">end</xsl:attribute>
                    <xsl:apply-templates mode="xref" select="."/>
                </xsl:when>
                <xsl:when test="../@type='ordered'">
                    <xsl:attribute name="text-align">end</xsl:attribute>
                    <xsl:apply-templates mode="xref" select="."/>
                    <xsl:text>.</xsl:text>
                </xsl:when>
                <xsl:when test="local-name()='label'">
                    <xsl:choose>
                        <xsl:when test="../@type='gloss'">
                            <xsl:attribute name="text-align">start</xsl:attribute>
                            <xsl:attribute name="font-weight">bold</xsl:attribute>                    
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:attribute name="text-align">end</xsl:attribute>
                        </xsl:otherwise>
                    </xsl:choose>
                    <xsl:apply-templates/>
                </xsl:when>
                <xsl:when test="local-name()='item' and label and not(@n='')">
                    <xsl:attribute name="font-weight">bold</xsl:attribute>
                    <xsl:choose>
                        <xsl:when test="../@type='gloss'">
                            <xsl:attribute name="text-align">start</xsl:attribute>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:attribute name="text-align">end</xsl:attribute>
                        </xsl:otherwise>
                    </xsl:choose>
                    <xsl:apply-templates/>
                </xsl:when>
                <xsl:otherwise> 
                    <xsl:attribute name="text-align">center</xsl:attribute>
                    <xsl:variable name="listdepth" select="count(ancestor::list)"/>
                    <xsl:choose>
                        <xsl:when test="$listdepth=1">
                            <xsl:value-of select="$bulletOne"/>
                        </xsl:when>
                        <xsl:when test="$listdepth=2">
                            <xsl:value-of select="$bulletTwo"/>
                        </xsl:when>
                        <xsl:when test="$listdepth=3">
                            <xsl:value-of select="$bulletThree"/>
                        </xsl:when>
                        <xsl:when test="$listdepth=4">
                            <xsl:value-of select="$bulletFour"/>
                        </xsl:when>
                    </xsl:choose>
                </xsl:otherwise>
            </xsl:choose>
        </fo:block>
    </fo:list-item-label>
</xsl:template>

<xsl:template match="item">
  <fo:list-item-body start-indent="body-start()">
    <fo:block font-weight="normal"><xsl:apply-templates/></fo:block>
  </fo:list-item-body>
</xsl:template>

<xsl:template match="item" mode="xref">
    <xsl:variable name="listdepth" select="count(ancestor::list)"/>
    <xsl:if test="parent::list[@type='bibliography']">
        <xsl:text> [</xsl:text>
    </xsl:if>
    
    <xsl:choose>
      <xsl:when test="$listdepth=1">
        <xsl:number format="1"/>
      </xsl:when>
      <xsl:when test="$listdepth=2">
        <xsl:number format="i"/>
      </xsl:when>
      <xsl:when test="$listdepth=3">
        <xsl:number format="a"/>
      </xsl:when>
      <xsl:when test="$listdepth=4">
        <xsl:number format="I"/>
      </xsl:when>
    </xsl:choose>

    <xsl:if test="parent::list[@type='bibliography']">
      <xsl:text>]</xsl:text>
    </xsl:if>
</xsl:template>

<xsl:template name="setListIndents">
    <xsl:variable name="listdepth" select="count(ancestor::list)"/>
    <xsl:choose>
        <xsl:when test="$listdepth=0">
            <xsl:attribute name="padding-top">
                <xsl:value-of select="$listAbove-1"/>
            </xsl:attribute>
            <xsl:attribute name="padding-bottom">
                <xsl:value-of select="$listBelow-1"/>
            </xsl:attribute>
        </xsl:when>
        <xsl:when test="$listdepth=1">
            <xsl:attribute name="padding-top">
                <xsl:value-of select="$listAbove-2"/>
            </xsl:attribute>
            <xsl:attribute name="padding-bottom">
                <xsl:value-of select="$listBelow-2"/>
            </xsl:attribute>
        </xsl:when>
        <xsl:when test="$listdepth=2">
            <xsl:attribute name="padding-top">
                <xsl:value-of select="$listAbove-3"/>
            </xsl:attribute>
            <xsl:attribute name="padding-bottom">
                <xsl:value-of select="$listBelow-3"/>
            </xsl:attribute>
        </xsl:when>
        <xsl:when test="$listdepth=3">
            <xsl:attribute name="padding-top">
                <xsl:value-of select="$listAbove-4"/>
            </xsl:attribute>
            <xsl:attribute name="padding-bottom">
                <xsl:value-of select="$listBelow-4"/>
            </xsl:attribute>
        </xsl:when>
    </xsl:choose>
</xsl:template>

</xsl:stylesheet>
