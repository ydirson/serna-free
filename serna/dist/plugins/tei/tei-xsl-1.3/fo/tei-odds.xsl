<!-- $Date: 2003/11/24 14:49:48 $, $Revision: 1.1 $, $Author: ilia $

XSL stylesheet to format TEI ODDs to PDF

 Copyright 1999 Sebastian Rahtz/Oxford University  <sebastian.rahtz@oucs.ox.ac.uk>

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and any associated documentation files (the
 ``Software''), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:
 
 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.
--> 

<xsl:stylesheet 
  xmlns:fo="http://www.w3.org/1999/XSL/Format"
  xmlns:exsl="http://exslt.org/common"
  exclude-result-prefixes="exsl" 
  extension-element-prefixes="exsl"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  version="1.0"  >


<xsl:param name="Master"/>

<xsl:template match="att">
 <xsl:choose>
 <xsl:when test="parent::listAtts">
  <fo:list-item>
  <xsl:call-template name="listLabel">
  <xsl:with-param name="text"><fo:inline
 font-family="{$sansFont}"><xsl:apply-templates/></fo:inline></xsl:with-param>
      </xsl:call-template>
    </fo:list-item>
 </xsl:when>
 <xsl:otherwise>
   <fo:inline font-family="{$sansFont}" ><xsl:apply-templates/></fo:inline>
 </xsl:otherwise>
 </xsl:choose>
</xsl:template>


<xsl:template match="attDef" mode="inline"> 
  <fo:list-item>
    <xsl:apply-templates select="attName"/>
    <xsl:if test="valList|valDesc">
    <fo:list-block space-before="0pt">
     <xsl:apply-templates select="valList|valDesc"/>
    </fo:list-block>
    </xsl:if>
  </fo:list-item>
</xsl:template>

<xsl:template match="attDef"> 
<!-- attName, (name | rs)?, desc, datatype, (valList | valDesc)?, 
	default, eg?, remarks?, equiv*) -->
  <fo:list-item>
    <xsl:apply-templates select="attName"/>
    <xsl:if test="datatype|valList|valDesc|default|eg|remarks|equiv">
          <fo:list-block space-before="0pt">
     <xsl:apply-templates select="datatype|valList|valDesc|default|eg|remarks|equiv"/>
    </fo:list-block>
    </xsl:if>
  </fo:list-item>
</xsl:template>


<xsl:template match="attDef/eg">
<xsl:if test="not(text()='')">
 <fo:list-item>
  <xsl:call-template name="listLabel">
<xsl:with-param name="text"> <fo:inline font-style='italic'>Example</fo:inline>
</xsl:with-param>
  </xsl:call-template>
 <fo:list-item-body>
 <xsl:call-template name="verbatim">
  <xsl:with-param name="text">
  <xsl:if test="not($flowMarginLeft='')">
        <xsl:attribute name="padding-start">
         <xsl:value-of select="$exampleMargin"/>
        </xsl:attribute>
       </xsl:if>
  <xsl:choose>
  <xsl:when test ="@copyOf">
   <xsl:variable name="copyOfstr">
    <xsl:apply-templates select="id(@copyOf)" mode="copy"/>
   </xsl:variable>
     <xsl:choose>
      <xsl:when test="$copyOfstr">
      <xsl:value-of select="$copyOfstr"/>
      </xsl:when>
      <xsl:otherwise>
       <xsl:comment>* No stuff found for copyOf $copyOf *</xsl:comment>
      </xsl:otherwise>
     </xsl:choose>
  </xsl:when>
  <xsl:otherwise>
     <xsl:apply-templates/>
  </xsl:otherwise>
  </xsl:choose>
 </xsl:with-param>
 </xsl:call-template>

 </fo:list-item-body></fo:list-item>
    
  </xsl:if>
</xsl:template>

<xsl:template match="attList">
   <fo:list-item>
   <xsl:call-template name="listLabel">
       <xsl:with-param name="text">
          <fo:inline font-style='italic'>Attributes</fo:inline>
       </xsl:with-param>
   </xsl:call-template>
   <fo:list-item-body>
      <fo:list-block margin-right="{$listRightMargin}"
       margin-left="{$listLeftGlossInnerIndent}">
  <xsl:choose>
    <xsl:when test="*">
       <xsl:apply-templates/>
  </xsl:when>
  <xsl:otherwise>No attributes other than those globally
          available (see definition for a.global)
  </xsl:otherwise>
  </xsl:choose>
      </fo:list-block>
 </fo:list-item-body></fo:list-item>
</xsl:template>



<xsl:template match="attName">
 <xsl:call-template name="listLabel">
   <xsl:with-param name="text"><fo:inline font-weight="bold" font-family="{$sansFont}"><xsl:apply-templates/></fo:inline></xsl:with-param>
  </xsl:call-template>
  <xsl:processing-instruction name="xmltex">
    <xsl:text>\index{</xsl:text>
    <xsl:value-of select="normalize-space(.)"/>
    <xsl:text>@\textsf{</xsl:text>
    <xsl:value-of select="normalize-space(.)"/>
    <xsl:text>}}</xsl:text>
  </xsl:processing-instruction>
 <xsl:apply-templates select="following-sibling::desc[1]" mode="show"/>
</xsl:template>

<xsl:template match="children">
 <fo:list-item>
 <xsl:call-template name="listLabel">
   <xsl:with-param name="text"><fo:inline font-style='italic'>Children</fo:inline></xsl:with-param></xsl:call-template>
<fo:list-item-body>
 <fo:inline font-size="8pt">
 <xsl:apply-templates/>
 </fo:inline>
</fo:list-item-body>
</fo:list-item>
</xsl:template>

<xsl:template match="class" mode="refdoc">
<fo:inline font-family="{$typewriterFont}"><xsl:apply-templates/></fo:inline>
</xsl:template>

<xsl:template match="class">
 <xsl:choose>
 <xsl:when test="parent::classDoc">
 <xsl:for-each select="..">
 <fo:list-item>      
 <xsl:call-template name="listLabel">
 <xsl:with-param name="text">
<fo:inline font-style='italic'>Declaration</fo:inline></xsl:with-param>
</xsl:call-template><fo:list-item-body>
 <xsl:call-template name="verbatim">
 <xsl:with-param name="startnewline">true</xsl:with-param>
 <xsl:with-param name="text">
   <xsl:choose>    
   <xsl:when test="@type='atts'">
    <xsl:call-template name="classAtt"/>
   </xsl:when>

  <xsl:when test="@type='model'">
    <xsl:call-template name="classModel"/>
  </xsl:when>

  <xsl:when test="@type='both'">
    <xsl:call-template name="classModel"/>
    <xsl:call-template name="classAtt"/>
  </xsl:when>
  </xsl:choose>
 </xsl:with-param>
 </xsl:call-template>
</fo:list-item-body>
</fo:list-item>    
    </xsl:for-each>
 </xsl:when>
 <xsl:otherwise>
   <fo:inline font-family="{$typewriterFont}" ><xsl:apply-templates/></fo:inline>
 </xsl:otherwise>
 </xsl:choose>
</xsl:template>



<xsl:template match="classDoc">   
<xsl:choose>
  <xsl:when test="$oddmode='html'">
    <a name="{@id}"/>
  </xsl:when>
  <xsl:when test="$oddmode='pdf'">
    <fo:block id="{@id}"/>
  </xsl:when>
</xsl:choose>
 <xsl:call-template name="refdoc">
   <xsl:with-param name="name">
     <xsl:apply-templates select="class[1]" mode="refdoc"/>
   </xsl:with-param>
   <xsl:with-param name="desc">
     <xsl:apply-templates select="desc[1]" mode="refdoc"/>
   </xsl:with-param>
 </xsl:call-template>
</xsl:template>


<xsl:template match="classes">
  <xsl:if test="@names">
     <fo:list-item><xsl:call-template name="listLabel">
 <xsl:with-param name="text">
  <fo:inline font-style='italic'>Class</fo:inline></xsl:with-param>
</xsl:call-template>
<fo:list-item-body>
  <xsl:call-template name="classSplitter">
   <xsl:with-param name="classes" 
     select="concat(normalize-space(@names),' ')"/>
   <xsl:with-param name="sep" select="''"/>
 </xsl:call-template>
 </fo:list-item-body>
</fo:list-item>
  </xsl:if>
</xsl:template>

<xsl:template name="classSplitter">
  <xsl:param name="classes"/>
  <xsl:param name="sep"/>
  <xsl:if test="not($classes='')">
   <xsl:value-of select="$sep"/>
   <xsl:for-each select="id(substring-before($classes,' '))">
     <fo:basic-link  color="{$linkColor}" 
            internal-destination="{@id}">
        <xsl:value-of select="class"/>
     </fo:basic-link>
    </xsl:for-each>
   <xsl:call-template name="classSplitter">
     <xsl:with-param name="classes" select="substring-after($classes,' ')"/>
     <xsl:with-param name="sep" select="'; '"/>
   </xsl:call-template>
  </xsl:if>
</xsl:template>



<xsl:template match="code">
 <fo:inline font-family="{$typewriterFont}"><xsl:apply-templates/></fo:inline> 
</xsl:template>



<xsl:template match="dataDesc">
  <xsl-if test=".//text()">
 <fo:list-item><xsl:call-template name="listLabel">
 <xsl:with-param name="text"><fo:inline font-style='italic'>Content</fo:inline></xsl:with-param></xsl:call-template><fo:list-item-body> 
 <xsl:apply-templates/>
</fo:list-item-body>
</fo:list-item>
</xsl-if>
</xsl:template>


<xsl:template match="decl">
 <fo:list-item>
<xsl:call-template name="listLabel">
  <xsl:with-param name="text">
    <fo:inline font-style='italic'>Declaration</fo:inline></xsl:with-param></xsl:call-template>
<fo:list-item-body>
<fo:inline font-family="{$typewriterFont}" >
   <xsl:apply-templates/>
 </fo:inline>
 </fo:list-item-body>
</fo:list-item>
</xsl:template>




<xsl:template match="default">
 <fo:list-item>
<xsl:call-template name="listLabel">
  <xsl:with-param name="text"><fo:inline font-style='italic'>Default</fo:inline></xsl:with-param></xsl:call-template>
 <fo:list-item-body>
   <fo:inline font-family="{$typewriterFont}">
     <xsl:apply-templates/>
   </fo:inline>
</fo:list-item-body>
</fo:list-item>
</xsl:template>

<xsl:template match="dataType|datatype">
 <fo:list-item>
<xsl:call-template name="listLabel">
  <xsl:with-param name="text"><fo:inline font-style='italic'>Datatype</fo:inline></xsl:with-param></xsl:call-template>
 <fo:list-item-body>
  <fo:inline font-family="{$sansFont}" >
   <xsl:apply-templates/>
  </fo:inline>
</fo:list-item-body>
</fo:list-item>
</xsl:template>

<xsl:template match="valDesc">
 <fo:list-item>
<xsl:call-template name="listLabel">
  <xsl:with-param name="text">
   <fo:inline font-style='italic'>Values</fo:inline></xsl:with-param></xsl:call-template>
 <fo:list-item-body>
 <xsl:apply-templates/>
</fo:list-item-body>
</fo:list-item>
</xsl:template>


<xsl:template match="name" mode="show">
  <xsl:text> (</xsl:text><xsl:apply-templates/><xsl:text>) </xsl:text>
</xsl:template>



<xsl:template match="desc" mode="show">
 <fo:list-item-body>
 <xsl:apply-templates select="preceding-sibling::rs"/>
 <xsl:apply-templates select="preceding-sibling::name" mode="show"/>
 <xsl:choose>
  <xsl:when test="@copyOf">
   <xsl:variable name="copyOfstr">
    <xsl:apply-templates select="id(@copyOf)" mode="copy"/>
   </xsl:variable>
     <xsl:choose>
     <xsl:when test="$copyOfstr">
      <xsl:value-of select="$copyOfstr"/>
     </xsl:when>
     <xsl:otherwise>
       <xsl:comment>* No element found for copyOf $copyOf *</xsl:comment>
     </xsl:otherwise>
     </xsl:choose>
  </xsl:when>
<!--
  <xsl:otherwise>
    <xsl:comment>* No copyOf found and no content for $theName*</xsl:comment>
  </xsl:otherwise>
-->
 </xsl:choose>
 <xsl:apply-templates/>
</fo:list-item-body>
</xsl:template>

<xsl:template match="desc" mode="refdoc">
 <xsl:apply-templates select="preceding-sibling::rs"/>
 <xsl:apply-templates select="preceding-sibling::name" mode="show"/>
 <xsl:choose>
  <xsl:when test="@copyOf">
   <xsl:variable name="copyOfstr">
    <xsl:apply-templates select="id(@copyOf)" mode="copy"/>
   </xsl:variable>
     <xsl:choose>
     <xsl:when test="$copyOfstr">
      <xsl:value-of select="$copyOfstr"/>
     </xsl:when>
     <xsl:otherwise>
       <xsl:comment>* No element found for copyOf $copyOf *</xsl:comment>
     </xsl:otherwise>
     </xsl:choose>
  </xsl:when>
 </xsl:choose>
 <xsl:apply-templates/>
</xsl:template>

<xsl:template match="desc"/>

<xsl:template match="name"/>

<xsl:template match="eg">
<xsl:call-template name="verbatim">
  <xsl:with-param name="autowrap">false</xsl:with-param>
 <xsl:with-param name="startnewline">
  <xsl:if test="parent::exemplum">true</xsl:if>
 </xsl:with-param>    
 <xsl:with-param name="text">
 <xsl:choose>
  <xsl:when test="@copyOf">
   <xsl:variable name="copyOfstr">
    <xsl:apply-templates select="id(@copyOf)" mode="copy"/>
   </xsl:variable>
   <xsl:choose>
     <xsl:when test="$copyOfstr">
      <xsl:value-of select="$copyOfstr"/>
     </xsl:when>
     <xsl:otherwise>
       <xsl:comment>* No element found for copyOf $copyOf *</xsl:comment>
     </xsl:otherwise>
     </xsl:choose>
  </xsl:when>
<!--
  <xsl:otherwise>
    <xsl:comment>* No copyOf found and no content for $theName*</xsl:comment>
  </xsl:otherwise>
-->
 </xsl:choose>
   <xsl:apply-templates/>
 </xsl:with-param>
</xsl:call-template>
</xsl:template>


<xsl:template match="attlDecl"/>

<xsl:template match="attlDecl" mode="show">
 <xsl:apply-templates/>
</xsl:template>

<xsl:template match="elemDecl">
 <fo:list-item>
<xsl:call-template name="listLabel">
  <xsl:with-param name="text">
  <fo:inline font-style='italic'>Declaration</fo:inline></xsl:with-param>
 </xsl:call-template>
  <fo:list-item-body>
 <xsl:call-template name="verbatim">
  <xsl:with-param name="startnewline">true</xsl:with-param>
  <xsl:with-param name="text">
       <xsl:if test="not($Master)">
         <xsl:text>&lt;!ELEMENT  </xsl:text>
<xsl:value-of select="parent::tagDoc/gi"/>
       </xsl:if>
    <xsl:value-of select="."/>
       <xsl:if test="not($Master)">
    <xsl:text>&gt;
</xsl:text>
       </xsl:if>
<xsl:text>&lt;!ATTLIST </xsl:text><xsl:value-of select="parent::tagDoc/gi"/>
      <xsl:text>  
      %a.global;</xsl:text>
      <xsl:if test="../classes/@names">
      <xsl:call-template name="splitAtts">
       <xsl:with-param name="words">
         <xsl:value-of select="concat(../classes/@names,' ') "/>
       </xsl:with-param>
      </xsl:call-template>
     </xsl:if>
    <xsl:for-each select="parent::tagDoc/attList/attDef" >
      <xsl:text>
      </xsl:text>
      <xsl:value-of select="normalize-space(attName)"/>
      <xsl:text> </xsl:text>
      <xsl:value-of select="normalize-space(dataType|datatype)"/>
      <xsl:text> </xsl:text> 
      <xsl:if test="not(starts-with(default,'#')) and not(starts-with(default,'%'))">"</xsl:if>
      <xsl:value-of select="normalize-space(default)"/>
      <xsl:if test="not(starts-with(default,'#')) and not(starts-with(default,'%'))">"</xsl:if>
    </xsl:for-each>
    <xsl:text>&gt;
</xsl:text>
</xsl:with-param></xsl:call-template>
  </fo:list-item-body>
 </fo:list-item>
 <!--
 <fo:list-item>
<xsl:call-template name="listLabel">
 <xsl:with-param name="text">
<fo:inline font-style='italic'>Parents</fo:inline>
</xsl:with-param></xsl:call-template>
<fo:list-item-body/>
 </fo:list-item>
-->
</xsl:template>


<xsl:template match="entDoc">     
 <xsl:call-template name="refdoc">
   <xsl:with-param name="name">
     <xsl:apply-templates select="entName[1]" mode="refdoc"/>
   </xsl:with-param>
   <xsl:with-param name="desc">
     <xsl:apply-templates select="desc[1]" mode="refdoc"/>
   </xsl:with-param>
 </xsl:call-template>
</xsl:template>



<xsl:template match="entName" mode="refdoc">
 <fo:inline font-family="{$typewriterFont}" >
   <xsl:apply-templates/>
 </fo:inline>
</xsl:template>


<xsl:template match="entName">
 <xsl:choose>
   <xsl:when test="parent::entDoc"></xsl:when>
 <xsl:otherwise>
 <fo:inline font-family="{$typewriterFont}"><xsl:apply-templates/></fo:inline>  </xsl:otherwise>
 </xsl:choose>
</xsl:template>


<xsl:template match="entName|class|gi|scrap|rs" mode="copy">
 <xsl:apply-templates/>
</xsl:template>



<xsl:template match="exemplum">
  <xsl:if test="not(normalize-space(string(.)) = '')">
   <fo:list-item>
    <xsl:call-template name="listLabel">
     <xsl:with-param name="text">
      <fo:inline font-style='italic'>Example</fo:inline></xsl:with-param></xsl:call-template>
     <fo:list-item-body>
        <xsl:apply-templates/>
     </fo:list-item-body>
   </fo:list-item>
  </xsl:if>
</xsl:template>


<xsl:template match="figdesc">
<p><fo:inline font-style='italic'><small>[<xsl:apply-templates/>]</small></fo:inline></p>
</xsl:template>


<xsl:template match="files">
  <fo:list-item>
     <xsl:call-template name="listLabel">
       <xsl:with-param name="text"><fo:inline font-style='italic'>Filename</fo:inline></xsl:with-param></xsl:call-template>
   <fo:list-item-body><fo:inline font-family="{$typewriterFont}"><xsl:value-of select="@names"/></fo:inline></fo:list-item-body>
 </fo:list-item>
</xsl:template>

<xsl:template match="gi" mode="refdoc">
      <fo:inline font-weight="bold" 
  font-family="{$typewriterFont}">&lt;<xsl:apply-templates/>&gt;</fo:inline>
</xsl:template>

<xsl:template match="gi">
 <xsl:choose>
 <xsl:when test="parent::tagDoc">
 </xsl:when>
 <xsl:when test="parent::listElements">
<fo:list-item>
  <xsl:call-template name="listLabel"><xsl:with-param name="text">
   <fo:inline font-weight="bold" font-family="{$typewriterFont}">&lt;<xsl:apply-templates/>&gt;</fo:inline>
   </xsl:with-param>
    </xsl:call-template>
    <xsl:apply-templates select="following-sibling::desc[1]" mode="show"/>
</fo:list-item>
 </xsl:when>
 <xsl:otherwise>
   <fo:inline font-family="{$typewriterFont}">&lt;<xsl:apply-templates/>&gt;</fo:inline>
   <xsl:if test="not(@TEI='no')">
       <xsl:processing-instruction name="xmltex">
    <xsl:text>\index{</xsl:text>
    <xsl:value-of select="normalize-space(.)"/>
    <xsl:text>}</xsl:text>
  </xsl:processing-instruction>
   </xsl:if>
 </xsl:otherwise>
 </xsl:choose>
</xsl:template>

<xsl:template match="item"> 
 <xsl:choose>
   <xsl:when test="parent::list[@type='elementlist']"> 
     &#9;<xsl:apply-templates/>
   </xsl:when>
   <xsl:otherwise>
     <xsl:call-template name="makeItem"/>
   </xsl:otherwise>
 </xsl:choose>
</xsl:template>




<xsl:template name="processatts">
 <xsl:param name="values"/>
 <xsl:if test="not($values = '')">
   <xsl:apply-templates 
      select="id(substring-before($values,' '))"/>
   <xsl:call-template name="processatts">
   <xsl:with-param name="values" select="substring-after($values,' ')"/>
  </xsl:call-template>
 </xsl:if>
</xsl:template>

<xsl:template match="listAtts">
 <xsl:choose>
 <xsl:when test="parent::listElements">
 <fo:block start-indent="{$exampleMargin}">
  <fo:inline font-style='italic'>Attributes include</fo:inline>
   <xsl:comment>listAtts</xsl:comment>
 <fo:list-block margin-right="{$listRightMargin}"
     margin-left="{$listLeftGlossInnerIndent}">
   <xsl:apply-templates/>
   <xsl:call-template name="processatts">
    <xsl:with-param name="values"><xsl:value-of select="concat(@atts,' ')"/>
    </xsl:with-param>
   </xsl:call-template>
  </fo:list-block>
 </fo:block>
 </xsl:when>
 <xsl:otherwise>
   <xsl:comment>listAtts 2</xsl:comment>
 <fo:list-block margin-right="{$listRightMargin}"
     margin-left="{$listLeftGlossInnerIndent}">
      <xsl:apply-templates/>
   <xsl:call-template name="processatts">
    <xsl:with-param name="values"><xsl:value-of select="concat(@atts,' ')"/>
    </xsl:with-param>
   </xsl:call-template>
   </fo:list-block>
 </xsl:otherwise>
 </xsl:choose>
</xsl:template>



<xsl:template match="listElements">
   <xsl:comment>listElements</xsl:comment>
 <fo:list-block space-before="12pt" 
     margin-right="{$listRightMargin}"
     margin-left="{$listLeftGlossInnerIndent}">
  <xsl:apply-templates/>
  </fo:list-block>
</xsl:template>


<xsl:template match="members">
  <fo:list-item><xsl:call-template name="listLabel"><xsl:with-param name="text"><fo:inline font-style='italic'>Members</fo:inline></xsl:with-param></xsl:call-template><fo:list-item-body>
 <xsl:apply-templates/>
</fo:list-item-body></fo:list-item>
</xsl:template>


<xsl:template match="parents">
  <fo:list-item><xsl:call-template name="listLabel"><xsl:with-param name="text"><fo:inline font-style='italic'>Parents</fo:inline></xsl:with-param></xsl:call-template><fo:list-item-body>
 <xsl:apply-templates/>
</fo:list-item-body></fo:list-item>
</xsl:template>



<xsl:template match="part">
  <fo:list-item><xsl:call-template name="listLabel"><xsl:with-param name="text"><fo:inline font-style='italic'>Tagset</fo:inline></xsl:with-param></xsl:call-template><fo:list-item-body>
  <xsl:call-template name="makeTagsetInfo"/>
</fo:list-item-body></fo:list-item>
</xsl:template>


<xsl:template match="refby">
  <fo:list-item><xsl:call-template name="listLabel"><xsl:with-param name="text"><fo:inline font-style='italic'>Referenced by</fo:inline></xsl:with-param></xsl:call-template><fo:list-item-body>
 <xsl:apply-templates/>
 </fo:list-item-body></fo:list-item>
</xsl:template>



<xsl:template match="remarks">
  <xsl:if test="*//text()">
  <fo:list-item>
   <xsl:call-template name="listLabel">
    <xsl:with-param name="text">
         <fo:inline font-style='italic'>Note</fo:inline>
    </xsl:with-param>
   </xsl:call-template>
   <list-item-body>
      <xsl:apply-templates/>
   </list-item-body>
  </fo:list-item>
</xsl:if>
</xsl:template>


<xsl:template match="revisionDesc//date">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="rs"/>

<xsl:template match="scrap">
 <xsl:call-template name="verbatim">
  <xsl:with-param name="text">
       <xsl:apply-templates/>
     </xsl:with-param></xsl:call-template>
</xsl:template>

<xsl:template match="dtdFrag">
 <xsl:call-template name="verbatim">
  <xsl:with-param name="text">
       <xsl:call-template name="processDtdFrag"/>
  </xsl:with-param>
 </xsl:call-template>
</xsl:template>

<xsl:template match="soCalled">
 <xsl:text>`</xsl:text>
 <xsl:apply-templates/>
 <xsl:text>'</xsl:text>
</xsl:template>



<xsl:template match="string">
  <fo:list-item><xsl:call-template name="listLabel">
   <xsl:with-param name="text">
   <fo:inline font-style='italic'>Declaration</fo:inline>
   </xsl:with-param>
  </xsl:call-template>
<list-item-body>
   <fo:inline font-family="{$typewriterFont}">
   <xsl:text>&lt;!ENTITY % </xsl:text>
   <xsl:choose>
    <xsl:when test="preceding-sibling::entName">
     <xsl:apply-templates select="preceding-sibling::entName" mode="copy"/>
    </xsl:when>
    <xsl:when test="preceding-sibling::gi">
     <xsl:apply-templates select="preceding-sibling::gi" mode="copy"/>
    </xsl:when>
    <xsl:when test="preceding-sibling::class">
     <xsl:apply-templates select="preceding-sibling::class" mode="copy"/>
    </xsl:when>
   </xsl:choose>
   <xsl:text> </xsl:text>
   <xsl:apply-templates/>
   &gt;</fo:inline></list-item-body></fo:list-item>
</xsl:template>


<xsl:template match="tag">
  <fo:inline font-family="{$typewriterFont}">&lt;<xsl:apply-templates/>&gt;</fo:inline>
</xsl:template>




<xsl:template match="tagDoc">     
 <xsl:call-template name="refdoc">
   <xsl:with-param name="name">
     <xsl:apply-templates select="gi[1]" mode="refdoc"/>
   </xsl:with-param>
   <xsl:with-param name="desc">
     <xsl:apply-templates select="desc[1]" mode="refdoc"/>
   </xsl:with-param>
 </xsl:call-template>
</xsl:template>


<xsl:template match="ptr">
<xsl:choose>
 <xsl:when test="parent::entDoc or parent::classDoc or parent::tagDoc">
  <xsl:if test="count(preceding-sibling::ptr)=0">
    <fo:list-item><xsl:call-template name="listLabel"><xsl:with-param name="text"><fo:inline font-style='italic'>See further</fo:inline></xsl:with-param></xsl:call-template><fo:list-item-body>
  <xsl:apply-templates select="../ptr" mode="useme"/>
  </fo:list-item-body></fo:list-item>
  </xsl:if>
 </xsl:when>
 <xsl:otherwise>
 <fo:basic-link  color="{$linkColor}" internal-destination="{@target}">
      <xsl:apply-templates mode="xref" select="key('IDS',@target)"/>
 </fo:basic-link>
  </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="ptr" mode="useme">
 <xsl:if test="count(preceding-sibling::ptr)&gt;0">; </xsl:if>
 <fo:basic-link  color="{$linkColor}" internal-destination="{@target}">
      <xsl:apply-templates mode="xref" select="key('IDS',@target)"/>
 </fo:basic-link>
</xsl:template>

<xsl:template match="valList">
 <xsl:choose>
   <xsl:when test="ancestor::tagDoc or ancestor::classDoc or ancestor::entDoc">
     <fo:list-item>
     <xsl:call-template name="listLabel">
     <xsl:with-param name="text">
      <fo:inline font-style="italic">
      <xsl:choose>
        <xsl:when test="@type='semi'">Suggested values include:</xsl:when>
        <xsl:when test="@type='open'">Sample values include:</xsl:when>
        <xsl:when test="@type='closed'">Legal values are:</xsl:when>
        <xsl:otherwise>Values are:</xsl:otherwise>
      </xsl:choose>
      </fo:inline>
    </xsl:with-param>
    </xsl:call-template><fo:list-item-body>
    <xsl:apply-templates select="." mode="contents"/>
    </fo:list-item-body></fo:list-item>
 </xsl:when>
 <xsl:otherwise>
   <xsl:apply-templates select="." mode="contents"/>
  </xsl:otherwise>
 </xsl:choose>
</xsl:template>

<xsl:template match="valList" mode="intext">
  <xsl:text> </xsl:text>
      <xsl:choose>
            <xsl:when test="@type='semi'">Suggested values are:</xsl:when>
            <xsl:when test="@type='open'">Possible values are:</xsl:when>
            <xsl:when test="@type='closed'">Legal values are:</xsl:when>
            <xsl:otherwise>Values are:</xsl:otherwise>
      </xsl:choose>
    <xsl:apply-templates select="." mode="contents"/>
</xsl:template>

<xsl:template match="valList" mode="contents">
   <xsl:comment>valList</xsl:comment>
 <fo:list-block  space-before="0pt" space-after="0pt"
    margin-right="{$listRightMargin}"
     margin-left="6pt">
       <xsl:for-each select="val">
         <fo:list-item>
           <xsl:call-template name="listLabel">
             <xsl:with-param name="text">
               <fo:inline font-family="{$typewriterFont}">
                   <xsl:value-of select="."/>
               </fo:inline>
              </xsl:with-param>
           </xsl:call-template>
         <fo:list-item-body>
               <xsl:value-of select="following-sibling::desc[1]"/>
          </fo:list-item-body>
         </fo:list-item>
        </xsl:for-each>
      </fo:list-block>
</xsl:template>

<xsl:template name="refdoc">
 <xsl:param name="name"/>
 <xsl:param name="desc"/>
 <fo:block   padding="4pt" text-align="start"
    space-before="12pt" background-color="yellow">
   <xsl:copy-of select="$name"/>&#160;
   <xsl:copy-of select="$desc"/>
 </fo:block>
  <xsl:processing-instruction name="xmltex">
    <xsl:text>\index{</xsl:text>
    <xsl:value-of select="translate($name,'&lt;&gt;','')"/>
    <xsl:text>@\textbf{</xsl:text>
    <xsl:copy-of select="$name"/>
    <xsl:text>}}</xsl:text>
  </xsl:processing-instruction>
   <xsl:comment>refdoc</xsl:comment>
 <fo:list-block space-before="6pt" 
     margin-right="{$listRightMargin}"
     margin-left="{$listLeftGlossInnerIndent}">
     <xsl:apply-templates select="*[not(self::gi)]"/>
  </fo:list-block>
</xsl:template>

<xsl:template match="divGen[@type='index']">
<fo:block>
<xsl:processing-instruction name="xmltex">
  <xsl:text>\printindex</xsl:text>
</xsl:processing-instruction>
</fo:block>
</xsl:template>


<xsl:template match="tagList">
   <xsl:comment>tagList</xsl:comment>
<fo:list-block   margin-right="{$listRightMargin}"
     margin-left="{$listLeftGlossInnerIndent}">
  <xsl:for-each select="tagDesc">
     <xsl:apply-templates select="key('IDS',@tagDoc)" mode="show"/>
  </xsl:for-each>
</fo:list-block>
</xsl:template>

<xsl:template match="tagDoc" mode="show">
 <fo:list-item>
  <xsl:call-template name="listLabel">
           <xsl:with-param name="text">
              <fo:inline font-weight='bold'>
                <xsl:text>&lt;</xsl:text>
               <xsl:value-of select="gi"/>&gt; </fo:inline>
            </xsl:with-param>
  </xsl:call-template>
  <fo:list-item-body>
  <xsl:value-of select="desc"/>
  <xsl:if test="attList/attDef">
    <xsl:text> </xsl:text> Attributes include:
    <fo:list-block margin-right="{$listRightMargin}"
     margin-left="{$listLeftGlossInnerIndent}">
      <xsl:apply-templates  select="attList/attDef" mode="inline"/>
    </fo:list-block>
  </xsl:if>
  </fo:list-item-body>
 </fo:list-item>
</xsl:template>

<xsl:template match="adList">
   <xsl:comment>adList</xsl:comment>
 <fo:list-block margin-right="{$listRightMargin}"
     margin-left="{$listLeftGlossInnerIndent}">
   <xsl:call-template name="adlistprocess">
    <xsl:with-param name="class" select="@classDoc"/>
    <xsl:with-param name="atts" select="concat(@atts,' ')"/>
  </xsl:call-template>
  </fo:list-block>
</xsl:template>


<xsl:template name="adlistprocess">
  <xsl:param name="class"/>
  <xsl:param name="atts"/>
  <xsl:if test="not($atts='')">
    <xsl:variable name="this" select="substring-before($atts,' ')"/>
    <xsl:for-each select="key('IDS',$class)">
      <fo:list-item><xsl:call-template name="listLabel"><xsl:with-param name="text"><fo:inline font-family="{$sansFont}"><xsl:value-of select="$this"/></fo:inline></xsl:with-param></xsl:call-template>
     <fo:list-item-body>
      <xsl:for-each select="attList/attDef[attName=$this]/desc">
       <xsl:apply-templates/>
       <xsl:apply-templates select="following-sibling::valList" mode="intext"/>
     </xsl:for-each>
     </fo:list-item-body>
        </fo:list-item>
    </xsl:for-each>
  <xsl:call-template name="adlistprocess">
    <xsl:with-param name="class" select="$class"/>
    <xsl:with-param name="atts" select="substring-after($atts,' ')"/>
  </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template name="listLabel">
 <xsl:param name="text"/>
  <fo:list-item-label>
    <fo:block>
      <xsl:attribute name="margin-right">2.5pt</xsl:attribute>
      <xsl:attribute name="text-align">start</xsl:attribute>
      <xsl:copy-of select="$text"/>
   </fo:block>
</fo:list-item-label>
</xsl:template>

<xsl:template name="verbatim">
  <xsl:param name="text"/>
  <xsl:param name="startnewline">false</xsl:param>
  <xsl:param name="autowrap">true</xsl:param>
     <fo:block 
        font-family="{$typewriterFont}" 
	white-space-collapse="false" 
	wrap-option="no-wrap" 
	text-indent="0em"
	start-indent="{$exampleMargin}"
	text-align="start"
	font-size="{$exampleSize}"
	space-before.optimum="4pt"
	space-after.optimum="4pt">      
       <xsl:if test="not($flowMarginLeft='')">
        <xsl:attribute name="padding-start">
         <xsl:value-of select="$exampleMargin"/>
        </xsl:attribute>
       </xsl:if>
        <xsl:if test="$startnewline='true'">
         <xsl:text>
</xsl:text>
       </xsl:if>
       <xsl:choose>
         <xsl:when test="$autowrap='false'">
           <xsl:value-of select="."/>
         </xsl:when>
       <xsl:otherwise>           
       <xsl:variable name="lines" select="estr:tokenize($text,'&#10;')"/>
           <xsl:apply-templates select="$lines[1]" 
                mode="normalline"/>
     </xsl:otherwise>
   </xsl:choose>
 </fo:block>
</xsl:template>

<xsl:template match="ident[@type='pe']">
<fo:inline font-family="{$sansFont}">%<xsl:apply-templates/>;</fo:inline>
</xsl:template>

<xsl:template name="italicize">
  <xsl:param name="text"/>
  <fo:inline font-style="italic">
    <xsl:copy-of select="$text"/>
 </fo:inline>
</xsl:template>

</xsl:stylesheet>
