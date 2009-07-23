<!-- 
RCS: $Date: 2003/11/24 14:50:28 $, $Revision: 1.1 $, $Author: ilia $

XSL stylesheet to format TEI ODDs to HTML

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
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  xmlns:estr="http://exslt.org/strings"
  exclude-result-prefixes="estr" 
  extension-element-prefixes="estr"
  version="1.0"  >

<xsl:variable name="linkColor"/>

<xsl:template match="att">
 <xsl:choose>
 <xsl:when test="parent::listAtts">
    <tr><td valign='top'><tt><b><xsl:apply-templates/></b></tt></td></tr>
 </xsl:when>
 <xsl:otherwise>
   <tt><b><xsl:apply-templates/></b></tt>  
 </xsl:otherwise>
 </xsl:choose>
</xsl:template>


<xsl:template match="attDef"> 
  <tr><xsl:apply-templates/></tr>
</xsl:template>


<xsl:template match="attDef/eg">
 <tr>
 <td></td>
 <td valign="top" colspan='2'>
 <i>Example: </i>
 <xsl:call-template name="verbatim">
  <xsl:with-param name="text">
  <xsl:variable name="content"><xsl:apply-templates/></xsl:variable>
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
  <xsl:value-of select="$content"/>
  </xsl:otherwise>
  </xsl:choose>
</xsl:with-param></xsl:call-template>
 </td></tr>
</xsl:template>

<xsl:template match="attList">
 <tr><td valign="top"><i>Attributes </i></td><td>
  <xsl:choose>
    <xsl:when test="*">
  <table>
       <xsl:apply-templates/>
  </table>
  </xsl:when>
  <xsl:otherwise>No attributes other than those globally
          available (see definition for a.global)
  </xsl:otherwise>
  </xsl:choose>
  </td></tr>
</xsl:template>


<xsl:template match="attName">
 <td valign='top'><tt><b>
 <xsl:apply-templates/>
 </b></tt></td>
 <xsl:apply-templates select="following-sibling::desc[1]" mode="show"/>
</xsl:template>

<xsl:template match="children">
 <tr><td valign="top"><i>Children</i></td><td colspan="2"><small>
 <xsl:apply-templates/>
 </small></td></tr>
</xsl:template>


<xsl:template match="class">
 <xsl:choose>
 <xsl:when test="parent::classDoc">
    <tr>
     <td valign='top'><tt><b><xsl:apply-templates/></b></tt></td>
     <xsl:apply-templates select="following-sibling::desc[1]" mode="show"/>
    </tr>
    <xsl:for-each select="..">
 <tr><td valign='top'><i>Declaration</i></td><td colspan='2'>
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
    <!--    <xsl:call-template name="classAtt"/>-->
  </xsl:when>
  </xsl:choose>
</xsl:with-param></xsl:call-template>
</td></tr>    
    </xsl:for-each>
 </xsl:when>
 <xsl:otherwise>
   <tt><b><xsl:apply-templates/></b></tt>  
 </xsl:otherwise>
 </xsl:choose>
</xsl:template>



<xsl:template match="classDoc">     
 <xsl:call-template name="refdoc">
   <xsl:with-param name="name"><xsl:value-of select="class"/></xsl:with-param>
 </xsl:call-template>
</xsl:template>


<xsl:template match="classes">
<xsl:if test="@names">
 <tr><td valign="top"><i>Class</i></td><td colspan="2">
 <xsl:call-template name="classSplitter">
   <xsl:with-param name="classes" 
     select="concat(normalize-space(@names),' ')"/>
   <xsl:with-param name="sep" select="''"/>
 </xsl:call-template>
 </td></tr>
</xsl:if>
</xsl:template>

<xsl:template name="classSplitter">
  <xsl:param name="classes"/>
  <xsl:param name="sep"/>
  <xsl:if test="not($classes='')">
   <xsl:value-of select="$sep"/>
   <xsl:for-each select="id(substring-before($classes,' '))">
     <a href="ref-{@id}.html"><xsl:value-of select="class"/></a>
    </xsl:for-each>
   <xsl:call-template name="classSplitter">
     <xsl:with-param name="classes" select="substring-after($classes,' ')"/>
     <xsl:with-param name="sep" select="'; '"/>
   </xsl:call-template>
  </xsl:if>
</xsl:template>



<xsl:template match="code">
 <tt><xsl:apply-templates/></tt> 
</xsl:template>



<xsl:template match="dataDesc">
<xsl-if test=".//text()">
 <tr><td valign="top"><i>Content</i></td><td colspan="2"><small>
 <xsl:apply-templates/>
 </small></td></tr>
</xsl-if>
</xsl:template>


<xsl:template match="dataType">
 <tr><td></td><td valign="top" colspan='2'><i>Datatype: </i>
 <xsl:apply-templates/>
 </td></tr>
</xsl:template>



<xsl:template match="decl">
 <tr><td valign='top'><i>Declaration</i></td><td colspan='2'><tt><b>
   <xsl:apply-templates/>
 </b></tt></td></tr>
</xsl:template>




<xsl:template match="default">
 <tr><td></td><td valign="top" colspan='2'><i>Default: </i>
 <xsl:apply-templates/>
 </td></tr>
</xsl:template>

<xsl:template match="datatype">
 <tr><td></td><td valign="top" colspan='2'><i>Datatype: </i>
 <xsl:apply-templates/>
 </td></tr>
</xsl:template>


<xsl:template match="name" mode="show">
  (<xsl:apply-templates/>)
</xsl:template>



<xsl:template match="desc" mode="show">
 <td colspan='2'> 
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
</td>
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
 </xsl:choose>
   <xsl:apply-templates/>
 </xsl:with-param></xsl:call-template>
</xsl:template>


<xsl:template match="attlDecl"/>

<xsl:template match="attlDecl" mode="show">
 <xsl:apply-templates/>
</xsl:template>

<xsl:template match="elemDecl">
 <tr><td valign='top'><i>Declaration</i></td><td colspan='2'>
 <xsl:call-template name="verbatim">
  <xsl:with-param name="startnewline">true</xsl:with-param>
  <xsl:with-param name="text">&lt;!ELEMENT <xsl:value-of select="parent::tagDoc/gi"/>
    <xsl:value-of select="."/>
    <xsl:text>&gt; 
&lt;!ATTLIST </xsl:text><xsl:value-of select="parent::tagDoc/gi"/>
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
      <xsl:value-of select="normalize-space(datatype)"/>
      <xsl:text> </xsl:text> 
      <xsl:if test="not(starts-with(default,'#')) and not(starts-with(default,'%'))">"</xsl:if>
      <xsl:value-of select="normalize-space(default)"/>
      <xsl:if test="not(starts-with(default,'#')) and not(starts-with(default,'%'))">"</xsl:if>
    </xsl:for-each>
    <xsl:text>&gt;
</xsl:text>
</xsl:with-param></xsl:call-template>
 </td>
 </tr>

</xsl:template>






<xsl:template match="entDoc">     
 <xsl:call-template name="refdoc">
   <xsl:with-param name="name">
     <xsl:text>%</xsl:text><xsl:value-of select="entName"/>
   </xsl:with-param>
 </xsl:call-template>
</xsl:template>



<xsl:template match="entName">
 <xsl:choose>
 <xsl:when test="parent::entDoc">
    <tr><td valign='top'>
    <tt><b><xsl:apply-templates/></b></tt></td>
    <xsl:apply-templates select="following-sibling::desc" mode="show"/>
    </tr>
 </xsl:when>
 <xsl:otherwise>
   <tt><xsl:apply-templates/></tt>  
 </xsl:otherwise>
 </xsl:choose>
</xsl:template>





<xsl:template match="entName|class|gi|scrap|rs" mode="copy">
 <xsl:apply-templates/>
</xsl:template>



<xsl:template match="exemplum">
 <tr><td valign='top'><i>Example</i></td><td colspan='2'>
  <xsl:apply-templates/>
 </td></tr>
</xsl:template>






<xsl:template match="figdesc">
<p><i><small>[<xsl:apply-templates/>]</small></i></p>
</xsl:template>


<xsl:template match="files">
  <tr>
   <td valign="top"><i>Filename</i></td>
   <td colspan="2"><tt><xsl:value-of select="@names"/></tt></td>
 </tr>
</xsl:template>

<xsl:template match="gi">
 <xsl:choose>
 <xsl:when test="parent::listElements">
    <tr>
     <td valign='top'><tt>&lt;<xsl:apply-templates/>&gt;</tt></td>
     <xsl:apply-templates select="following-sibling::desc[1]" mode="show"/>
    </tr>
 </xsl:when>
 <xsl:when test="parent::tagDoc">
<tr bgcolor='C0C0C0'>
    <td valign='top'><b><tt>&lt;<xsl:apply-templates/>&gt;</tt></b></td>
    <xsl:apply-templates select="following-sibling::desc[1]" mode="show"/>
</tr>
 </xsl:when>
 <xsl:otherwise>
   <xsl:call-template name="makeAnchor">
    <xsl:with-param name="name">GDX-<xsl:number level="any"/></xsl:with-param>
   </xsl:call-template>
   <b><tt>&lt;<xsl:apply-templates/>&gt;</tt></b>
 </xsl:otherwise>
 </xsl:choose>
</xsl:template>

<xsl:template match="index">
 <xsl:variable name="idxnum"><xsl:number level="any"/></xsl:variable>
 <a href='IDX{$idxnum}'></a>
</xsl:template>


<xsl:template match="item"> 
 <xsl:choose>
   <xsl:when test="parent::list[@type='gloss']"> 
     &#9;<xsl:apply-templates/>
   </xsl:when>
   <xsl:when test="parent::list[@type='elementlist']"> 
     &#9;<xsl:apply-templates/>
   </xsl:when>
   <xsl:otherwise>
     <li><xsl:apply-templates/></li>
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
  <tr><td valign="top"><i>Attributes include</i></td><td><table>
   <xsl:apply-templates/>
   <xsl:call-template name="processatts">
    <xsl:with-param name="values"><xsl:value-of select="concat(@atts,' ')"/>
    </xsl:with-param>
   </xsl:call-template>
  </table></td></tr>
 </xsl:when>
 <xsl:otherwise>
   <table width='75%' cellpadding='1' cellspacing='1'>
      <xsl:apply-templates/>
   <xsl:call-template name="processatts">
    <xsl:with-param name="values"><xsl:value-of select="concat(@atts,' ')"/>
    </xsl:with-param>
   </xsl:call-template>
   </table>
 </xsl:otherwise>
 </xsl:choose>
</xsl:template>



<xsl:template match="listElements">
 <table width='75%' cellpadding='1' cellspacing='1'>
  <xsl:apply-templates/>
 </table>
</xsl:template>


<xsl:template match="members">
 <tr><td valign="top"><i>Members</i></td><td colspan="2"><small>
 <xsl:apply-templates/>
 </small></td></tr>
</xsl:template>


<xsl:template match="parents">
 <tr><td valign="top"><i>Parents</i></td><td colspan="2"><small>
 <xsl:apply-templates/>
 </small></td></tr>
</xsl:template>



<xsl:template match="part">
 <tr><td valign="top"><i>Tagset</i></td><td colspan="2">
  <xsl:call-template name="makeTagsetInfo"/>
 </td></tr>
</xsl:template>


<xsl:template match="refby">
 <tr><td valign="top"><i>Referenced by</i></td><td colspan="2">
 <xsl:apply-templates/>
 </td></tr>
</xsl:template>



<xsl:template match="remarks">
<xsl:if test="*//text()">
 <tr><td valign="top"><i>Note</i></td><td colspan="2">
 <xsl:apply-templates/>
 </td></tr>
</xsl:if>
</xsl:template>


<xsl:template match="revisionDesc//date">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="rs"/>

<xsl:template match="scrap"/>

<xsl:template match="dtdFrag">
  <a name="{@id}"/>
 <xsl:call-template name="verbatim">
  <xsl:with-param name="text">
  <xsl:call-template name="processDtdFrag"/>
</xsl:with-param></xsl:call-template>
</xsl:template>

<xsl:template match="soCalled">
 <xsl:text>`</xsl:text>
 <xsl:apply-templates/>
 <xsl:text>'</xsl:text>
</xsl:template>



<xsl:template match="string">
   <tr><td valign='top'><i>Declaration</i></td><td colspan='2'><tt>
   <b><xsl:text>&lt;!ENTITY % </xsl:text>
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
   &gt;</b></tt></td></tr>
</xsl:template>


<xsl:template match="tag">
  <tt>&lt;<xsl:apply-templates/>&gt;</tt>
</xsl:template>

<xsl:template match="tagDoc">     
 <xsl:call-template name="refdoc">
   <xsl:with-param name="name">
     <xsl:text>&lt;</xsl:text>
     <xsl:value-of select="gi[1]"/>
     <xsl:text>&gt;</xsl:text>
   </xsl:with-param>
 </xsl:call-template>
</xsl:template>


<xsl:template match="ptr">
<xsl:choose>
 <xsl:when test="parent::entDoc or parent::classDoc or parent::tagDoc">
  <xsl:if test="count(preceding-sibling::ptr)=0">
  <tr><td valign="top"><i>See further</i></td><td colspan="2">
  <xsl:apply-templates select="../ptr" mode="useme"/>
  </td></tr>
  </xsl:if>
 </xsl:when>
 <xsl:otherwise>
 <a class="ptr">
 <xsl:attribute name="href">
      <xsl:apply-templates mode="xrefheader" select="key('IDS',@target)"/>
 </xsl:attribute>
 <xsl:variable name="xx">
  <xsl:apply-templates mode="header" select="key('IDS',@target)">
    <xsl:with-param name="minimal" select="$minimalCrossRef"/>
 </xsl:apply-templates>
 </xsl:variable>
 <xsl:value-of select="normalize-space($xx)"/>
 </a>
  </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="ptr" mode="useme">
 <xsl:if test="count(preceding-sibling::ptr)&gt;0">; </xsl:if>
 <xsl:variable name="xx">
   <xsl:apply-templates mode="header" select="key('IDS',@target)">
    <xsl:with-param name="minimal" select="$minimalCrossRef"/>
   </xsl:apply-templates>
  </xsl:variable>
 <a class="ptr">
  <xsl:attribute name="href">
      <xsl:apply-templates mode="xrefheader" select="key('IDS',@target)"/>
  </xsl:attribute>
  <xsl:value-of select="normalize-space($xx)"/>
 </a>
</xsl:template>

<xsl:template match="valList">
 <xsl:choose>
 <xsl:when test="ancestor::tagDoc or ancestor::classDoc or ancestor::entDoc">
    <tr><td></td><td valign="top">
    <xsl:apply-templates select="." mode="contents"/>
    </td></tr>
 </xsl:when>
 <xsl:otherwise>
   <xsl:apply-templates select="." mode="contents"/>
  </xsl:otherwise>
 </xsl:choose>
</xsl:template>

<xsl:template match="valList" mode="contents">
      <xsl:choose>
        <xsl:when test="@type='semi'">Suggested values include:</xsl:when>
        <xsl:when test="@type='open'">Sample values include:</xsl:when>
        <xsl:when test="@type='closed'">Legal values are:</xsl:when>
        <xsl:otherwise>Values are:</xsl:otherwise>
      </xsl:choose>
      <table class="valList">
       <xsl:for-each select="val">
         <tr><td valign="top"><b><xsl:value-of select="."/></b></td>
         <td valign="top">
               <xsl:value-of select="following-sibling::desc[1]"/></td>
         </tr>
        </xsl:for-each>
      </table>
</xsl:template>



<xsl:template name="refdoc">
 <xsl:param name="name"/>
 <xsl:message>   refdoc for <xsl:value-of select="name(.)"/> - <xsl:value-of select="@id"/> </xsl:message>
 [<a href="ref-{@id}.html"><xsl:value-of select="$name"/></a>]
 <xsl:variable name="BaseFile">
 <xsl:value-of select="$masterFile"/>
  <xsl:if test="ancestor::teiCorpus.2">
    <xsl:text>-</xsl:text>
     <xsl:choose>
      <xsl:when test="@id"><xsl:value-of select="@id"/></xsl:when> 
      <xsl:otherwise><xsl:number/></xsl:otherwise>
     </xsl:choose>
  </xsl:if>
 </xsl:variable>
 <xsl:call-template name="outputChunk">
  <xsl:with-param name="ident">
   <xsl:text>ref-</xsl:text><xsl:apply-templates select="@id"/>
  </xsl:with-param>
  <xsl:with-param name="content">
 <html>
<xsl:comment>THIS IS A GENERATED FILE. DO NOT EDIT</xsl:comment>
<head><title><xsl:value-of select="$name"/></title>
         <xsl:if test="not($cssFile = '')">
           <link rel="stylesheet" type="text/css" href="{$cssFile}"/>
         </xsl:if>
</head>
<body>
<xsl:call-template name="bodyHook"/>
 <a name="TOP"/>
 <div  class="teidiv">
  <xsl:call-template name="stdheader">
   <xsl:with-param name="title">
     <xsl:value-of select="$name"/>
   </xsl:with-param>
  </xsl:call-template>
  <p><a name='{@id}'></a>
  <table border='1'>
  <xsl:apply-templates/>
  </table></p>
  <p align="{$alignNavigationPanel}">
 <i><xsl:text> </xsl:text> <xsl:value-of select="$upWord"/>: </i>
    <a  class="navlink"> 
    <xsl:attribute name="href">
      <xsl:apply-templates select="ancestor::div1" mode="ident"/>
      <xsl:text>.html</xsl:text>
    </xsl:attribute>
     <xsl:apply-templates select="ancestor::div1" mode="header"/>
    </a>
  </p>
  </div>
 </body></html>
  </xsl:with-param>
</xsl:call-template>
</xsl:template>

<xsl:template match="divGen[@type='index']">
     <h2>Index</h2>
</xsl:template>

<xsl:template match="tagList">
<ul class="tagList">
  <xsl:apply-templates/>
</ul>
</xsl:template>

<xsl:template match="tagDesc">
  <li>  <xsl:apply-templates select="key('IDS',@tagDoc)" mode="show"/></li>
</xsl:template>


<xsl:template match="tagDoc" mode="show">
  <b>&lt;<xsl:value-of select="gi"/>&gt; </b>
  <xsl:value-of select="desc"/>
  <xsl:choose>
    <xsl:when test="attList/*">
    <table class="attList">
      <xsl:for-each  select="attList/attDef">
        <tr>
        <td valign="top"><b><xsl:value-of select="attName"/></b></td>
        <xsl:apply-templates select="desc" mode="show"/>
      </tr>
    </xsl:for-each>
  </table>
  </xsl:when>
  <xsl:otherwise>
    <table class="attList">
        <tr>
        <td valign="top" colspan='2'>
No attributes other than those globally
          available (see definition for a.global)</td>
      </tr>
    </table>    
  </xsl:otherwise>
</xsl:choose>
</xsl:template>

<xsl:template match="adList">
  <table class="adList">
  <xsl:call-template name="adlistprocess">
    <xsl:with-param name="class" select="@classDoc"/>
    <xsl:with-param name="atts" select="concat(@atts,' ')"/>
  </xsl:call-template>
  </table>
</xsl:template>


<xsl:template name="adlistprocess">
  <xsl:param name="class"/>
  <xsl:param name="atts"/>
  <xsl:if test="not($atts='')">
    <xsl:variable name="this" select="substring-before($atts,' ')"/>
    <xsl:for-each select="key('IDS',$class)">
      <tr><td valign="top"><xsl:value-of select="$this"/></td>
      <td  valign="top">
          <xsl:value-of select="attList/attDef[attName=$this]/desc"/></td>
        </tr>
    </xsl:for-each>
  <xsl:call-template name="adlistprocess">
    <xsl:with-param name="class" select="$class"/>
    <xsl:with-param name="atts" select="substring-after($atts,' ')"/>
  </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template name="verbatim">
  <xsl:param name="text"/>
  <xsl:param name="startnewline">false</xsl:param>
  <xsl:param name="autowrap">true</xsl:param>
     <pre class="eg">
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
 </pre>
</xsl:template>

<xsl:template name="italicize">
  <xsl:param name="text"/>
  <i><xsl:copy-of select="$text"/></i>
</xsl:template>

</xsl:stylesheet>
