<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:exsl="http://exslt.org/common"
  xmlns:estr="http://exslt.org/strings"
  exclude-result-prefixes="exsl estr" 
  extension-element-prefixes="exsl estr"
  xmlns:fo="http://www.w3.org/1999/XSL/Format"
  version="1.0">

 <xsl:key name="FILES"   match="dtdFrag[@file]"   use="@file"/>
 <xsl:key name="CONTIN"  match="dtdFrag[@contin]" use="@contin"/>
 <xsl:key name="IDS"     match="*[@id]"           use="@id"/>
 <xsl:key name="PARTS"   match="part"             use="name"/>
 <xsl:key name="CLAREFS" match="claDecl"          use="@classDoc"/>
 <xsl:key name="ENTREFS" match="entDecl"          use="@entDoc"/>
 <xsl:key name="TAGREFS" match="tagDecl"          use="@tagDoc"/>
 <xsl:key name="DTDREFS" match="dtdRef"           use="@dtdFrag"/>

 <xsl:param name="wrapLength">65</xsl:param>

 <!-- build a lookup table of class names and their members -->
 <xsl:variable name="classTable">
   <xsl:variable name="taglist">
    <racine>
     <xsl:apply-templates select="//tagDoc" mode="buildlist"/>
     <xsl:apply-templates select="//classDoc" mode="buildlist"/>
    </racine>
   </xsl:variable>

   <xsl:variable name="sortedtaglist">
    <racine>
         <xsl:for-each select="exsl:node-set($taglist)/racine/pair">
           <xsl:sort select="class/@id"/>
           <xsl:sort select="member/@id"/>
           <xsl:copy-of select="."/>
       </xsl:for-each>
   </racine>
 </xsl:variable>


 <xsl:variable name="temp">
 <racine>
  <xsl:for-each select="exsl:node-set($sortedtaglist)/racine/pair">
  <xsl:variable name="thisClass" select="class/@id"/>
  <xsl:if test="not(preceding-sibling::pair[1]/class/@id=$thisClass)"> 
  <class id="{$thisClass}" type="{class/@type}">
    <xsl:for-each select="exsl:node-set($sortedtaglist)/racine/pair[class/@id=$thisClass]">
          <xsl:copy-of select="member"/>
        </xsl:for-each>
      </class>
      </xsl:if>
    </xsl:for-each>  
  </racine>
 </xsl:variable>
 <memberClasses>
  <xsl:for-each select="exsl:node-set($temp)/racine/class">
    <class id="{@id}" type="{@type}">
        <xsl:for-each select="member">
          <xsl:if test="not(@id=preceding-sibling::member/@id)">
            <xsl:copy-of select="."/>
          </xsl:if>
        </xsl:for-each>
    </class>
  </xsl:for-each>    
 </memberClasses>
</xsl:variable>

<xsl:template name="processDtdFrag">
  <xsl:variable name="secnum">
    <xsl:call-template name="sectionNumber"/>
  </xsl:variable>
  <xsl:text>&lt;!-- [</xsl:text>
  <xsl:value-of select="@id"/>] <xsl:value-of select="$secnum"/>
  <xsl:text>: </xsl:text>
  <xsl:value-of select="@n"/> --&gt;
<xsl:message>    [dtdFrag <xsl:value-of select="@id"/>  </xsl:message>
    <xsl:apply-templates mode="dtd"/>
<!--
    <xsl:message> <xsl:value-of select="@id"/> has <xsl:value-of select="count(key('CONTIN',@id))"/> continuations</xsl:message>
-->
  <xsl:for-each select="key('CONTIN',@id)">
    <xsl:message>      continuation: <xsl:value-of select="@id"/>    </xsl:message>
    <xsl:choose>
      <xsl:when test="$oddmode='dtd'">
        <xsl:call-template name="processDtdFrag"/>
      </xsl:when>
      <xsl:when test="$oddmode='pdf'">
     &lt;!--continued in <fo:basic-link color="{$linkColor}">
        <xsl:attribute name="internal-destination">
           <xsl:value-of select="@id"/>
        </xsl:attribute>
    <xsl:value-of select="$secnum"/>: <xsl:value-of select="@n"/>
  </fo:basic-link>--&gt;
      </xsl:when>
      <xsl:otherwise>
[continued in <a>
     <xsl:attribute name="href">
       <xsl:apply-templates select="ancestor-or-self::div1" mode="ident"/>
	<xsl:text>.html#</xsl:text>
       <xsl:value-of select="@id"/>
     </xsl:attribute>
    <xsl:value-of select="$secnum"/>: <xsl:value-of select="@n"/>
  </a>]
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
  <xsl:message>    ] end of <xsl:value-of select="@id"/></xsl:message>
  <xsl:text>
&lt;!-- end of [</xsl:text>
  <xsl:value-of select="@id"/>]  <xsl:value-of select="$secnum"/>--&gt;
</xsl:template>

<xsl:template match="peRef" mode="dtd">
  <xsl:text>%</xsl:text>
  <xsl:value-of select="@n"/>
  <xsl:text>;
</xsl:text>
</xsl:template>

<xsl:template match="claDecl" mode="dtd">
  <xsl:message>    .. claDecl <xsl:value-of select="@classDoc"/>,<xsl:value-of select="@type"/>  </xsl:message>
 <xsl:choose>
  <xsl:when test="@type='atts'">    
   <xsl:apply-templates select="key('IDS',@classDoc)" mode="processAtts"/>
  </xsl:when>
  <xsl:when test="@type='model'">    
   <xsl:apply-templates select="key('IDS',@classDoc)" mode="processModel"/>
  </xsl:when>
  <xsl:otherwise>
   <xsl:apply-templates select="key('IDS',@classDoc)" mode="process"/>
  </xsl:otherwise>
 </xsl:choose>
</xsl:template>

<xsl:template match="entDecl" mode="dtd">
  <xsl:message>     .. entdecl <xsl:value-of select="@entDoc"/></xsl:message>
  <xsl:if test="preceding-sibling::entDecl">
    <xsl:text>
</xsl:text>
  </xsl:if>
  <xsl:apply-templates select="key('IDS',@entDoc)" mode="dtd"/>
</xsl:template>

<xsl:template match="entDoc" mode="dtd">
  <xsl:message>     .... entdoc <xsl:value-of select="entName"/></xsl:message>
  <xsl:text>&lt;!ENTITY </xsl:text>
 <xsl:if test="@type='pe'">
    <xsl:text>%</xsl:text>
 </xsl:if>
  <xsl:text> </xsl:text>
  <xsl:value-of select="entName"/>
  <xsl:text> </xsl:text>
  <xsl:value-of select="string"/>
  <xsl:text> &gt;</xsl:text>
</xsl:template>


<xsl:template match="tagDecl" mode="dtd">
  <xsl:message>     .. tagdecl <xsl:value-of select="@tagDoc"/></xsl:message>
  <xsl:apply-templates select="key('IDS',@tagDoc)" mode="dtd"/>
</xsl:template>


<xsl:template match="msection" mode="dtd">
  <xsl:message>     .... msection <xsl:value-of select="@keywords"/></xsl:message>
&lt;![<xsl:value-of select="@keywords"/><xsl:text>[</xsl:text>
   <xsl:if test="not(name(following-sibling::*[1])='commDecl')">
    <xsl:text>
</xsl:text>
  </xsl:if>
<xsl:apply-templates mode="dtd"/>
  <xsl:text>]]&gt;</xsl:text>
</xsl:template>

<xsl:template match="commDecl" mode="dtd">
    <xsl:text>
&lt;!--</xsl:text>
    <xsl:apply-templates/>
<xsl:text>--&gt;
</xsl:text>        
  </xsl:template>

<xsl:template match="classDoc" mode="processAtts">
    <xsl:call-template name="classAtt"/>
</xsl:template>

<xsl:template match="classDoc" mode="processModel">
    <xsl:call-template name="classModel"/>
</xsl:template>

<xsl:template match="classDoc" mode="process">
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
</xsl:template>

<xsl:template name="classModel">
<xsl:message>    .... class model <xsl:value-of select="class"/></xsl:message>
  <xsl:variable name="thisClass">
    <xsl:value-of select="class"/>   
  </xsl:variable>
    <xsl:text>
&lt;!ENTITY % x.</xsl:text><xsl:value-of select="$thisClass"/> "" &gt;<xsl:text>
&lt;!ENTITY % m.</xsl:text>
    <xsl:value-of select="$thisClass"/>
    <xsl:text> "%x.</xsl:text>
    <xsl:value-of select="$thisClass"/>
    <xsl:text>; </xsl:text>
    <xsl:for-each select="exsl:node-set($classTable)/memberClasses/class[@id=$thisClass]/member">
      <xsl:text>%</xsl:text><xsl:value-of select="@prefix"/>.<xsl:value-of select="@id"/><xsl:text>;</xsl:text> 
      <xsl:if test="position() &lt; last()"> | </xsl:if>
    </xsl:for-each>
    <xsl:text>"&gt; </xsl:text>
</xsl:template>

<xsl:template name="classAtt">
<xsl:message>    .... class attributes <xsl:value-of select="class"/></xsl:message>  
 <xsl:variable name="thisClass">
    <xsl:value-of select="class"/>   
  </xsl:variable>
      <xsl:text>
&lt;!ENTITY % a.</xsl:text>
    <xsl:value-of select="$thisClass"/>
      <xsl:text> '</xsl:text>
      <xsl:call-template name="attclasses">
        <xsl:with-param name="classes">
          <xsl:value-of select="concat(classes/@names,' ')"/>
        </xsl:with-param>
      </xsl:call-template>
    <xsl:for-each select="attList/attDef" >
      <xsl:text>
      </xsl:text>
      <xsl:value-of select="normalize-space(attName)"/>
      <xsl:text> </xsl:text>
      <xsl:value-of select="normalize-space(datatype)"/>
      <xsl:text> </xsl:text>
 <xsl:if test='not(starts-with(default,"#")) and
      not(starts-with(default,"%")) and
      not(starts-with(default,"&#39;"))'>"</xsl:if>
      <xsl:value-of select="normalize-space(default)"/>
 <xsl:if test='not(starts-with(default,"#")) and
      not(starts-with(default,"%")) and
      not(starts-with(default,"&#39;"))'>"</xsl:if>
    </xsl:for-each>
    <xsl:text>'&gt; </xsl:text>
</xsl:template>

<xsl:template match="classDoc" mode="tagatts">
 <xsl:if test="@type='atts' or @type='both'">
<xsl:message>      .... added contents of [%a.<xsl:value-of select="class"/>;]</xsl:message>
<xsl:choose>
  <xsl:when test="$oddmode='dtd'">
   <xsl:text>
      %a.</xsl:text><xsl:value-of select="class"/><xsl:text>;</xsl:text>
  </xsl:when>
  <xsl:when test="$oddmode='html'">
   <xsl:text>
      %a.</xsl:text>
     <a>
       <xsl:attribute name="href">
         <xsl:text>ref-</xsl:text>
         <xsl:value-of select="@id"/>
         <xsl:text>.html</xsl:text>
       </xsl:attribute>
      <xsl:value-of select="class"/>
    </a>
     <xsl:text>;</xsl:text>
   </xsl:when>
   <xsl:when test="$oddmode='pdf'">
   <xsl:text>
      %a.</xsl:text>
     <fo:basic-link color="{$linkColor}">
       <xsl:attribute name="internal-destination">
         <xsl:value-of select="@id"/>
       </xsl:attribute>
      <xsl:value-of select="class"/>
    </fo:basic-link>
     <xsl:text>;</xsl:text>
   </xsl:when>
  </xsl:choose>
 </xsl:if>
</xsl:template>

<xsl:template match="tagDoc" mode="dtd">
  <xsl:message>     .... tagdoc <xsl:value-of select="gi"/></xsl:message>
  <xsl:if test="$oddmode='dtd' and not(starts-with(gi,'%'))">
    <xsl:text>&lt;!ENTITY % </xsl:text>
    <xsl:value-of select="gi"/> 'INCLUDE' &gt;
&lt;![ %<xsl:value-of select="gi"/>; [
</xsl:if>
<xsl:text>&lt;!ELEMENT </xsl:text>
<xsl:if test="not(starts-with(gi,'%'))">%n.</xsl:if>
    <xsl:value-of select="gi"/>    
<xsl:if test="not(starts-with(gi,'%'))">;</xsl:if> 
    <xsl:value-of select="elemDecl"/>
    <xsl:text>&gt; </xsl:text>
          <xsl:text>
&lt;!ATTLIST </xsl:text>
<xsl:if test="not(starts-with(gi,'%'))">%n.</xsl:if> 
    <xsl:value-of select="gi"/>
<xsl:if test="not(starts-with(gi,'%'))">;</xsl:if> 
      <xsl:text>
      %a.global;</xsl:text>
     <xsl:if test="classes/@names">
      <xsl:call-template name="splitAtts">
       <xsl:with-param name="words">
         <xsl:value-of select="concat(classes/@names,' ') "/>
       </xsl:with-param>
      </xsl:call-template>
     </xsl:if>
    <xsl:for-each select="attList/attDef" >
      <xsl:text>
      </xsl:text>
      <xsl:value-of select="normalize-space(attName)"/>
      <xsl:text> </xsl:text>
      <xsl:value-of select="normalize-space(datatype)"/>
      <xsl:text> </xsl:text> 
 <xsl:if test='not(starts-with(default,"#")) and
      not(starts-with(default,"%")) and
      not(starts-with(default,"&#39;"))'>"</xsl:if>
      <xsl:value-of select="normalize-space(default)"/>
 <xsl:if test='not(starts-with(default,"#")) and
      not(starts-with(default,"%")) and
      not(starts-with(default,"&#39;"))'>"</xsl:if>
</xsl:for-each>
<xsl:if test="not(starts-with(gi,'%'))">
      TEIform CDATA '<xsl:value-of select="gi"/><xsl:text>' </xsl:text>
</xsl:if>
<xsl:text> &gt;</xsl:text>
<xsl:if test="$oddmode='dtd' and not(starts-with(gi,'%'))">
]]&gt;
  </xsl:if>
</xsl:template>


<xsl:template match="tagDoc" mode="buildlist">
  <xsl:if test="classes/@names">
          <xsl:call-template name="splitter">
         <xsl:with-param name="words">
     <xsl:value-of select="concat(classes/@names, ' ')"/>
   </xsl:with-param>
         <xsl:with-param name="id">
           <xsl:value-of select="gi"/>
        </xsl:with-param>
         <xsl:with-param name="origid">
           <xsl:value-of select="@id"/>
        </xsl:with-param>
        <xsl:with-param name="prefix">n</xsl:with-param>
       </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template match="classDoc" mode="buildlist">
  <xsl:if test="classes/@names">
   <xsl:call-template name="splitter">
    <xsl:with-param name="words">
         <xsl:value-of select="concat(classes/@names, ' ')"/>
    </xsl:with-param>
    <xsl:with-param name="origid">
           <xsl:value-of select="@id"/>
    </xsl:with-param>
    <xsl:with-param name="id">
        <xsl:value-of select="class"/>
    </xsl:with-param>
    <xsl:with-param name="prefix">m</xsl:with-param>
   </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template name="splitter">
 <xsl:param name="words"/>
 <xsl:param name="id"/>
 <xsl:param name="origid"/>
 <xsl:param name="prefix"/>
 <xsl:if test="not($words='')">
   <xsl:variable name="current">
      <xsl:value-of select="substring-before($words,' ')"/>
   </xsl:variable>
   <!--
   <xsl:message>Member: origid=<xsl:value-of select="$origid"/>, prefix=<xsl:value-of select="$prefix"/>, id=<xsl:value-of select="$id"/>. Class: type=<xsl:value-of select="key('IDS',$current)/@type"/>, id=<xsl:value-of select="key('IDS',$current)/class"/>.</xsl:message>
-->
    <pair>
     <member origid="{$origid}" prefix="{$prefix}" id="{$id}"/>
     <class  type="{key('IDS',$current)/@type}" id="{key('IDS',$current)/class}"/>
    </pair>
    <xsl:call-template name="splitter">
     <xsl:with-param name="words">
         <xsl:value-of select="substring-after($words,' ') "/>
     </xsl:with-param>
     <xsl:with-param name="id" select="$id"/>
     <xsl:with-param name="origid" select="$origid"/>
     <xsl:with-param name="prefix" select="$prefix"/>
   </xsl:call-template>
 </xsl:if>
</xsl:template>

<xsl:template name="splitAtts">
 <xsl:param name="words"/>
 <xsl:if test="not($words='')">
   <xsl:variable name="current">
      <xsl:value-of select="substring-before($words,' ')"/>
   </xsl:variable>
   <xsl:for-each select="key('IDS',$current)">
     <xsl:message>tagatt <xsl:value-of select="name(.)"/>_<xsl:value-of select="@id"/></xsl:message>
     <xsl:apply-templates  select="." mode="tagatts"/>
   </xsl:for-each>
   <xsl:call-template name="splitAtts">
     <xsl:with-param name="words">
         <xsl:value-of select="substring-after($words,' ') "/>
     </xsl:with-param>
   </xsl:call-template>
 </xsl:if>
</xsl:template>

<xsl:template name="attclasses">
 <xsl:param name="classes"/>
 <xsl:if test="not($classes='') and not($classes=' ')">
   <xsl:variable name="class" select="key('IDS',substring-before($classes,' '))"/>
   <!--
   <xsl:message>     look up <xsl:value-of select="$class/@type"/>, <xsl:value-of select="$class/class"/>   </xsl:message>
-->
   <xsl:if test="$class/@type='atts'">
      %a.<xsl:value-of select="$class/class"/>
      <xsl:text>;</xsl:text>
   </xsl:if>
   <xsl:call-template name="attclasses">
     <xsl:with-param name="classes">
         <xsl:value-of select="substring-after($classes,' ') "/>
     </xsl:with-param>
   </xsl:call-template>
 </xsl:if>
</xsl:template>

<xsl:template name="sectionNumber">
  <xsl:for-each select="(ancestor::div1|ancestor::div2|ancestor::div3|ancestor::div4)[last()]">
    <xsl:for-each select="ancestor-or-self::div1">
     <xsl:number from="body" level="any" /><xsl:text>.</xsl:text>
    </xsl:for-each>
    <xsl:number level="multiple" count="div2|div3|div4" from="div1"/>
  </xsl:for-each>
</xsl:template>


<xsl:template match="dtdRef" mode="dtd">
 <xsl:choose>
 <xsl:when test="$oddmode='dtd'">
  <xsl:apply-templates select="key('IDS',@dtdFrag)" mode="ok"/>
</xsl:when>
<xsl:when test="$oddmode='html'">
  <xsl:text>[definitions from </xsl:text>
  <xsl:for-each select="key('IDS',@dtdFrag)">
   <xsl:variable name="secnum">
    <xsl:call-template name="sectionNumber"/>
   </xsl:variable>
   <a>
     <xsl:attribute name="href">
       <xsl:apply-templates select="ancestor-or-self::div1" mode="ident"/>
	<xsl:text>.html#</xsl:text>
       <xsl:value-of select="@id"/>
     </xsl:attribute>
    <xsl:value-of select="$secnum"/>: <xsl:value-of select="@n"/>
  </a>
 <xsl:text> inserted here ]
</xsl:text>
  </xsl:for-each>
</xsl:when>
<xsl:when test="$oddmode='pdf'">
  <xsl:text>&lt;!--definitions from </xsl:text>
  <xsl:for-each select="key('IDS',@dtdFrag)">
   <xsl:variable name="secnum">
    <xsl:call-template name="sectionNumber"/>
   </xsl:variable>
   <fo:basic-link color="{$linkColor}">
     <xsl:attribute name="internal-destination">
       <xsl:value-of select="@id"/>
     </xsl:attribute>
    <xsl:value-of select="$secnum"/>: <xsl:value-of select="@n"/>
  </fo:basic-link>
 <xsl:text> inserted here --&gt;</xsl:text>
  </xsl:for-each>
</xsl:when>
</xsl:choose>
</xsl:template>

<xsl:template match="entDoc|classDoc|tagDoc|dtdFrag" mode="findfile">
  <xsl:choose>
    <xsl:when test="@file">
      <xsl:text>Declared in file </xsl:text>
      <xsl:value-of select="@file"/>
      <xsl:text>; </xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:choose>
      <xsl:when test="@contin">
         <xsl:apply-templates select="key('IDS',@contin)" mode="findfile"/>
      </xsl:when>
      <xsl:otherwise>
       <xsl:choose>
       <xsl:when test="count(key('DTDREFS',@id))&gt;0">
        <xsl:for-each select="key('DTDREFS',@id)">
         <xsl:apply-templates select=".." mode="findfile"/>
        </xsl:for-each>
       </xsl:when>
       <xsl:when test="count(key('TAGREFS',@id))&gt;0">
        <xsl:for-each select="key('TAGREFS',@id)">
         <xsl:apply-templates select=".." mode="findfile"/>
        </xsl:for-each>
      </xsl:when>
      <xsl:otherwise>NO FILE</xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
 </xsl:otherwise>
 </xsl:choose>
</xsl:template>

<xsl:template match="*" mode="dtd"/>

<xsl:template name="makeTagsetInfo">
<xsl:choose>
  <xsl:when test="text()">
       <xsl:apply-templates/>
  </xsl:when>
  <xsl:otherwise>
   <xsl:variable name="partLook">
       <xsl:value-of select="@name"/>
       <xsl:value-of select="@type"/>
   </xsl:variable>
   <xsl:variable name="tagLook">
            <xsl:choose>
              <xsl:when test="$partLook='NDtop'">tag-names</xsl:when>
              <xsl:when test="$partLook='anatop'">tag-ana</xsl:when>
              <xsl:when test="$partLook='certtop'">tag-cert</xsl:when>
              <xsl:when test="$partLook='corebase'">tag-core</xsl:when>
              <xsl:when test="$partLook='coretop'">tag-core</xsl:when>
              <xsl:when test="$partLook='corporaaux'">tag-corp</xsl:when>
              <xsl:when test="$partLook='dictbase'">tag-dict</xsl:when>
              <xsl:when test="$partLook='fstop'">tag-fs</xsl:when>
              <xsl:when test="$partLook='fsdaux'">tag-fs</xsl:when>
              <xsl:when test="$partLook='fttop'">tag-ftab</xsl:when>
              <xsl:when test="$partLook='graphstop'">tag-graph</xsl:when>
              <xsl:when test="$partLook='hdraux'">tag-core</xsl:when>
              <xsl:when test="$partLook='ihsaux'">tag-ihs</xsl:when>
              <xsl:when test="$partLook='linktop'">tag-link</xsl:when>
              <xsl:when test="$partLook='msstop'">tag-mss</xsl:when>
              <xsl:when test="$partLook='namestop'">tag-names</xsl:when>
              <xsl:when test="$partLook='officetop'">tag-names</xsl:when>
              <xsl:when test="$partLook='perfbase'">tag-perf</xsl:when>
              <xsl:when test="$partLook='printbase'">tag-con</xsl:when>
              <xsl:when test="$partLook='tctop'">tag-tc</xsl:when>
              <xsl:when test="$partLook='termbase'">tag-term</xsl:when>
              <xsl:when test="$partLook='tsbase'">tag-spok</xsl:when>
              <xsl:when test="$partLook='tsdaux'">tag-tsd</xsl:when>
              <xsl:when test="$partLook='versebase'">tag-ve</xsl:when>
              <xsl:when test="$partLook='wsdaux'">tag-wsd</xsl:when>
                <xsl:otherwise>DISASTER</xsl:otherwise>
     </xsl:choose>
   </xsl:variable>
   <xsl:variable name="File">
    <xsl:choose>
     <xsl:when test="parent::tagDoc">
      <xsl:for-each select="key('TAGREFS',../@id)">
       <xsl:apply-templates select=".." mode="findfile"/>
      </xsl:for-each>
     </xsl:when>
    <xsl:when test="parent::entDoc">
    <xsl:for-each select="key('ENTREFS',../@id)">
     <xsl:apply-templates select=".." mode="findfile"/>
    </xsl:for-each>
  </xsl:when>
  <xsl:when test="parent::classDoc">
   <xsl:for-each select="key('CLAREFS',../@id)">
     <xsl:apply-templates select=".." mode="findfile"/>
   </xsl:for-each>
  </xsl:when>
  <xsl:when test="ancestor::dtdFrag/@file">
    <xsl:value-of select="ancestor:dtdFrag/@file"/>
  </xsl:when>
  <xsl:otherwise>
   <xsl:for-each select="key('TAGREFS',../@id)">
     <xsl:apply-templates select=".." mode="findfile"/>
   </xsl:for-each>
 </xsl:otherwise>
</xsl:choose>
</xsl:variable>
<xsl:message>  <xsl:value-of select="../@id"/>: <xsl:value-of select="$File"/></xsl:message>
<xsl:value-of select="$File"/>
  <xsl:apply-templates select="id($tagLook)/head[1]"/>
 </xsl:otherwise>
</xsl:choose>
</xsl:template>


<xsl:template match="token" mode="commentline">
<xsl:if test="preceding-sibling::token">
 <xsl:text>
</xsl:text>
</xsl:if>
  <xsl:call-template name="italicize">
      <xsl:with-param name="text">
        <xsl:value-of select="translate(.,'&#10;','')"/>
      </xsl:with-param>
  </xsl:call-template>
  <xsl:if test="following-sibling::token">
  <xsl:choose>
    <xsl:when test="contains(.,'--&gt;')">
         <xsl:apply-templates select="following-sibling::token[1]" 
                 mode="normalline"/>  
    </xsl:when>
    <xsl:otherwise>
 <xsl:apply-templates select="following-sibling::token[1]" mode="commentline"/>
      </xsl:otherwise>
   </xsl:choose>
 </xsl:if>
</xsl:template>

<xsl:template match="token" mode="normalline">
<xsl:if test="preceding-sibling::token">
 <xsl:text>
</xsl:text>
</xsl:if>
<xsl:choose>
  <xsl:when test="contains(.,'&lt;!--')">
    <xsl:call-template name="italicize">
      <xsl:with-param name="text">
      <xsl:value-of select="translate(.,'&#10;','')"/>
      </xsl:with-param>
    </xsl:call-template>

  <xsl:if test="following-sibling::token">
  <xsl:choose>
    <xsl:when test="contains(.,'--&gt;')">
         <xsl:apply-templates select="following-sibling::token[1]" 
                 mode="normalline"/>  
    </xsl:when>
    <xsl:otherwise>
 <xsl:apply-templates select="following-sibling::token[1]" mode="commentline"/>
      </xsl:otherwise>
   </xsl:choose>
 </xsl:if>
</xsl:when>
<xsl:otherwise>
  <xsl:call-template name="linebreak"/>  
  <xsl:if test="following-sibling::token">
    <xsl:apply-templates select="following-sibling::token[1]" 
      mode="normalline"/>
  </xsl:if>
</xsl:otherwise>
</xsl:choose>
</xsl:template>

<xsl:template name="linebreak">
 <xsl:choose>
  <xsl:when test="string-length(.)&lt;$wrapLength">
      <xsl:value-of select="."/>
  </xsl:when>
  <xsl:otherwise>
    <xsl:variable name="words" select="estr:tokenize(.)"/>
    <xsl:apply-templates select="$words[1]" mode="word">
      <xsl:with-param name="len" select="0"/>
    </xsl:apply-templates>
  </xsl:otherwise>
 </xsl:choose>
</xsl:template>

<xsl:template match="token" mode="word">
  <xsl:param name="len"/>
    <xsl:choose>
      <xsl:when test="$len +string-length(.) &gt; $wrapLength">
        <xsl:text>
         </xsl:text>
       <xsl:value-of select="."/><xsl:text> </xsl:text>
       <xsl:if test="following-sibling::token">
         <xsl:apply-templates select="following-sibling::token[1]" mode="word">
           <xsl:with-param name="len" select="8"/>
         </xsl:apply-templates>
       </xsl:if>
      </xsl:when>
      <xsl:otherwise>
       <xsl:value-of select="."/><xsl:text> </xsl:text>
       <xsl:if test="following-sibling::token">
         <xsl:apply-templates select="following-sibling::token[1]" mode="word">
           <xsl:with-param name="len" select="$len + string-length(.)"/>
         </xsl:apply-templates>
       </xsl:if>
      </xsl:otherwise>
    </xsl:choose>
</xsl:template>


</xsl:stylesheet>
