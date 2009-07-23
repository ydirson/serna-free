<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <xsl:template match="chapx|cmmx|section|subject|
                       pgblk|tfmatr|mfmatr|intro|chapter|
                       vendlist|vendata|mfr|mad|dplist|figure|
                       iplintro|cmm-ipl">
     <fo:block>
        <xsl:apply-templates/>
     </fo:block>
  </xsl:template>

  <xsl:template match="task|subtask">
    <xsl:if test="compid|resource|note">
         <xsl:apply-templates select="compid|resource|note"/>
    </xsl:if>
    <xsl:if test="not(processing-instruction('se:choice'))">
      <fo:block> 
         <xsl:if test="self::task">
            <fo:block xsl:use-attribute-sets="h1">
              <xsl:text>TASK </xsl:text>
              <xsl:value-of select="@chapnbr"/>
              <xsl:text>-</xsl:text>
              <xsl:value-of select="@sectnbr"/>
              <xsl:text>-</xsl:text>
              <xsl:value-of select="@subjnbr"/>
              <xsl:text>-</xsl:text>
              <xsl:value-of select="@func"/>
              <xsl:text>-</xsl:text>
              <xsl:value-of select="@seq"/>
            </fo:block>
         </xsl:if>
         <xsl:apply-templates select="*[not(self::compid) and 
                        not(self::resource) and not(self::note)]"/>
      </fo:block>
    </xsl:if>
  </xsl:template>

  <xsl:template match="subtask/list1[count(l1item) = 1]|pretopic/list1">
    <xsl:if test="compid|resource|note">
       <xsl:apply-templates select="compid|resource|note"/>
    </xsl:if>
    <fo:list-block padding-top="0em" padding-bottom="0em"
                   padding-left="0em" padding-right="0em">
      <fo:list-item>
        <fo:list-item-label end-indent="label-end()">
          <fo:block padding-top="6pt">          
            <xsl:variable name="depth" select="count(parent::*/list1)" />  
            <xsl:choose>
              <xsl:when test="$depth = 1">
                <xsl:number format="A. " count="subtask|pretopic" level="multiple"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:number format="A. " count="list1" level="multiple"/>
              </xsl:otherwise>
            </xsl:choose>
          </fo:block>
        </fo:list-item-label>
        <fo:list-item-body start-indent="body-start()">
          <fo:block padding-top="0em" padding-bottom="0em"
                    padding-left="0em" padding-right="0em">
            <xsl:apply-templates select="*[not(self::compid) and 
                      not(self::resource) and not(self::note)]"/>
          </fo:block>
        </fo:list-item-body>
      </fo:list-item>
    </fo:list-block>
  </xsl:template>

  <xsl:template match="pretopic|topic">
     <fo:block>
       <xsl:apply-templates select="title"/>
       <fo:block start-indent="2em">
          <xsl:apply-templates select="*[not(self::title)]"/>
       </fo:block>
     </fo:block>
  </xsl:template>

  <xsl:template match="topic" mode="count">
    <xsl:choose>
      <xsl:when test="preceding-sibling::tfmatr">
        <xsl:variable name="topic_count">
          <xsl:number format="1" count="topic"/>
        </xsl:variable>
        <xsl:value-of select="$topic_count+1"/>
        <xsl:text>. </xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:number format="1. " count="topic"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="compid|resource">
     <xsl:choose>
       <xsl:when test="node()">
         <fo:block> 
           <xsl:apply-templates/>
         </fo:block>
       </xsl:when>
       <xsl:otherwise>
         <fo:inline>
           <xsl:text> </xsl:text>
         </fo:inline>
       </xsl:otherwise>
     </xsl:choose>
  </xsl:template>


  <xsl:template match="intro/title">
    <fo:block xsl:use-attribute-sets="h2" text-align="center">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="topic/title">
    <fo:block xsl:use-attribute-sets="h2">
      <fo:inline text-decoration="no-underline">
        <xsl:apply-templates select=".." mode="count"/>
      </fo:inline>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="pretopic/title">
    <fo:block xsl:use-attribute-sets="h2">
      <fo:inline text-decoration="no-underline">
        <xsl:text>1. </xsl:text>
      </fo:inline>
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="title|cmpnom">
    <fo:block xsl:use-attribute-sets="h1">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>



</xsl:stylesheet>
