<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="prclist1">
    <xsl:if test="not(*/prcitem/processing-instruction('se:choice'))">
      <xsl:apply-templates select="*/prcitem/grphcref"/>
    </xsl:if>
    <fo:list-block padding-top="0.5em" padding-bottom="0em">
      <xsl:apply-templates/>
    </fo:list-block>
  </xsl:template>

  <xsl:template match="prclist2|prclist3|prclist4|prclist5|prclist6">
    <fo:list-item>
      <fo:list-item-label end-indent="label-end()"
                          text-align="end">
        <fo:block>
            <xsl:text> </xsl:text>
        </fo:block>
      </fo:list-item-label>
      <fo:list-item-body start-indent="body-start()">
        <fo:block>
          <xsl:if test="not(*/prcitem/processing-instruction('se:choice'))">
            <xsl:apply-templates select="*/prcitem/grphcref"/>
          </xsl:if>
          <fo:list-block padding-top="0.5em" padding-bottom="0em">
            <xsl:apply-templates/>
          </fo:list-block>
        </fo:block>
      </fo:list-item-body>
    </fo:list-item>
  </xsl:template>

  <xsl:template match="list1|list2|list3|list4|list5|list6|list7">
    <fo:list-block padding-top="0em" padding-bottom="0em">
      <xsl:apply-templates/>
    </fo:list-block>
  </xsl:template>

  <xsl:template match="prcitem1|prcitem2|prcitem3|prcitem4|prcitem5|prcitem6|prcitem7">
    <xsl:choose>
      <xsl:when test="prcitem">
        <xsl:apply-templates/>      
      </xsl:when>
      <xsl:otherwise>
        <fo:block>
          <xsl:apply-templates/>
        </fo:block>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="prcitem">
    <fo:list-item>
      <fo:list-item-label end-indent="label-end()"
                          text-align="end">
        <fo:block padding-top="2pt">
            <xsl:apply-templates select=".." mode="count"/>
            <xsl:text> </xsl:text>
        </fo:block>
      </fo:list-item-label>
      <fo:list-item-body start-indent="body-start()">
        <fo:block>
          <xsl:apply-templates select="*[not(self::grphcref)
            or self::processing-instruction('se:choice')]"/>
        </fo:block>
      </fo:list-item-body>
    </fo:list-item>
  </xsl:template>

  <xsl:template match="prcitem1|prcitem2|prcitem3|prcitem4|
                       prcitem5|prcitem6|prcitem7"
                mode="count">
    <xsl:choose>
      <xsl:when test="self::prcitem1">
        <xsl:number format="A. "/>
      </xsl:when>
      <xsl:when test="self::prcitem3">
        <xsl:text>(</xsl:text>
        <xsl:number format="a"/>
        <xsl:text>) </xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text>(</xsl:text>
        <xsl:number format="1"/>
        <xsl:text>) </xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>


  <xsl:template match="prcitem/grphcref">
    <fo:inline padding-left="2em"><xsl:apply-templates/></fo:inline>
  </xsl:template>


  <xsl:template match="l1item|l2item|l3item|l4item|l5item|l6item|l7item">
    <fo:list-item padding-top="0em">
      <fo:list-item-label end-indent="label-end()"
                          text-align="left">
        <fo:block padding-top="2pt">
          <xsl:choose>
            <xsl:when test="self::l1item">
              <xsl:number format="A. "/>
            </xsl:when>
            <xsl:when test="self::l3item">
              <xsl:text>(</xsl:text>
              <xsl:number format="a"/>
              <xsl:text>) </xsl:text>
            </xsl:when>
            <xsl:otherwise>
              <xsl:text>(</xsl:text>
              <xsl:number format="1"/>
              <xsl:text>) </xsl:text>
            </xsl:otherwise>
          </xsl:choose>
        </fo:block>
      </fo:list-item-label>
      <fo:list-item-body start-indent="body-start()">
        <fo:block padding-top="0em">
          <xsl:apply-templates/>
        </fo:block>
      </fo:list-item-body>
    </fo:list-item>
  </xsl:template>


  <xsl:template match="subtask/list1[count(l1item) = 1]/l1item|pretopic/list1/l1item">
    <fo:block padding-top="0em" padding-bottom="0em">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="unlist">
    <fo:list-block padding-top="0.5em" padding-bottom="0em"
                   provisional-label-separation="2pt" 
                   provisional-distance-between-starts="7pt">
      <xsl:apply-templates/>
    </fo:list-block>
  </xsl:template>

  <xsl:template match="unlist/unlitem">
    <fo:list-item>
      <fo:list-item-label end-indent="label-end()"
                          text-align="start">
        <fo:block padding-top="1pt">
          <xsl:text>-</xsl:text>
        </fo:block>
      </fo:list-item-label>
      <fo:list-item-body start-indent="body-start()">
        <fo:block>
          <xsl:apply-templates/>
        </fo:block>
      </fo:list-item-body>
    </fo:list-item>
  </xsl:template>
 
</xsl:stylesheet>
