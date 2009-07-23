<?xml version="1.0" encoding="UTF-8" ?>
<!--  domains2fo.xsl
 | DITA domains support for the demo set; extend as needed
 |
 | (C) Copyright IBM Corporation 2001, 2002, 2003. All Rights Reserved.
 | This file is part of the DITA package on IBM's developerWorks site.
 | See license.txt for disclaimers.
 +
 | updates:
 |   Added ui-d screen (new element)
 *-->

<xsl:transform version="1.0"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
                extension-element-prefixes="dtm"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

           
<!-- Start of UI domain -->
<dtm:doc dtm:elements="uicontrol" dtm:status="finished" dtm:idref="ui-d.uicontrol"/>
<xsl:template match="*[contains(@class,' ui-d/uicontrol ')]" dtm:id="ui-d.uicontrol">
  <!-- insert an arrow before all but the first uicontrol in a menucascade -->
  <xsl:if test="ancestor::*[contains(@class,' ui-d/menucascade ')]">
    <xsl:variable name="uicontrolcount"><xsl:number count="*[contains(@class,' ui-d/uicontrol ')]"/></xsl:variable>
    <xsl:if test="$uicontrolcount&gt;'1'">
      <xsl:text> --> </xsl:text>
    </xsl:if>
  </xsl:if>
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-weight="bold">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="wintitle" dtm:status="finished" dtm:idref="ui-d.wintitle"/>
<xsl:template match="*[contains(@class,' ui-d/wintitle ')]" dtm:id="ui-d.wintitle">
  <fo:inline border-left-width="0pt" border-right-width="0pt">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="menucascade" dtm:status="finished" dtm:idref="ui-d.menucascade"/>
<xsl:template match="*[contains(@class,' ui-d/menucascade ')]" dtm:id="ui-d.menucascade">
  <fo:inline border-left-width="0pt" border-right-width="0pt">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="shortcut" dtm:status="finished" dtm:idref="ui-d.shortcut"/>
<xsl:template match="*[contains(@class,' ui-d/shortcut ')]" dtm:id="ui-d.shortcut">
  <fo:inline border-left-width="0pt" border-right-width="0pt" text-decoration="underline">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="screen" dtm:status="finished" dtm:idref="ui-d.screen"/>
<xsl:template match="*[contains(@class,' ui-d/screen ')]" dtm:id="ui-d.screen">
  <xsl:call-template name="gen-att-label"/>
  <fo:block xsl:use-attribute-sets="pre">
    <!--xsl:call-template name="setscale"/-->
<!-- rules have to be applied within the scope of the PRE box; else they start from page margin! -->
    <xsl:if test="contains(@frame,'top')"><fo:block><fo:leader leader-pattern="rule" leader-length="5.65in" /></fo:block></xsl:if>
    <xsl:apply-templates/>
    <xsl:if test="contains(@frame,'bot')"><fo:block><fo:leader leader-pattern="rule" leader-length="5.65in" /></fo:block></xsl:if>
  </fo:block>
</xsl:template>


<!-- start of highlighting domain -->
<dtm:doc dtm:elements="b" dtm:status="finished" dtm:idref="hi-d.b"/>
<xsl:template match="*[contains(@class,' hi-d/b ')]" dtm:id="hi-d.b">
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-weight="bold">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="i" dtm:status="finished" dtm:idref="hi-d.i"/>
<xsl:template match="*[contains(@class,' hi-d/i ')]" dtm:id="hi-d.i">
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-style="italic">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="u" dtm:status="finished" dtm:idref="hi-d.u"/>
<xsl:template match="*[contains(@class,' hi-d/u ')]" dtm:id="hi-d.u">
  <fo:inline border-left-width="0pt" border-right-width="0pt" text-decoration="underline">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="tt" dtm:status="finished" dtm:idref="hi-d.tt"/>
<xsl:template match="*[contains(@class,' hi-d/tt ')]" dtm:id="hi-d.tt">
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-family="Courier">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="sup" dtm:status="finished" dtm:idref="hi-d.sup"/>
<xsl:template match="*[contains(@class,' hi-d/sup ')]" dtm:id="hi-d.sup">
  <fo:inline border-left-width="0pt" border-right-width="0pt" baseline-shift="super" font-size="75%">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="sub" dtm:status="finished" dtm:idref="hi-d.sub"/>
<xsl:template match="*[contains(@class,' hi-d/sub ')]" dtm:id="hi-d.sub">
  <fo:inline border-left-width="0pt" border-right-width="0pt" baseline-shift="sub" font-size="75%">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>


<!-- start of programming domain -->
<dtm:doc dtm:elements="codeph" dtm:status="finished" dtm:idref="pr-d.codeph"/>
<xsl:template match="*[contains(@class,' pr-d/codeph ')]" dtm:id="pr-d.codeph">
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-family="Courier">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="codeblock" dtm:status="finished" dtm:idref="pr-d.codeblock"/>
<xsl:template match="*[contains(@class,' pr-d/codeblock ')]" dtm:id="pr-d.codeblock">
  <xsl:call-template name="gen-att-label"/>
  <fo:block xsl:use-attribute-sets="pre">
    <!--xsl:call-template name="setscale"/-->
<!-- rules have to be applied within the scope of the PRE box; else they start from page margin! -->
    <xsl:if test="contains(@frame,'top')"><fo:block><fo:leader leader-pattern="rule" leader-length="5.65in" /></fo:block></xsl:if>
    <xsl:apply-templates/>
    <xsl:if test="contains(@frame,'bot')"><fo:block><fo:leader leader-pattern="rule" leader-length="5.65in" /></fo:block></xsl:if>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="option" dtm:status="finished" dtm:idref="pr-d.option"/>
<xsl:template match="*[contains(@class,' pr-d/option ')]" dtm:id="pr-d.option">
  <fo:inline border-left-width="0pt" border-right-width="0pt">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="var" dtm:status="finished" dtm:idref="pr-d.var"/>
<xsl:template match="*[contains(@class,' pr-d/var ')]" dtm:id="pr-d.var">
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-style="italic">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="parmname" dtm:status="finished" dtm:idref="pr-d.parmname"/>
<xsl:template match="*[contains(@class,' pr-d/parmname ')]" dtm:id="pr-d.parmname">
  <fo:inline font-family="Courier New" border-left-width="0pt" border-right-width="0pt">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="synph" dtm:status="finished" dtm:idref="pr-d.synph"/>
<xsl:template match="*[contains(@class,' pr-d/synph ')]" dtm:id="pr-d.synph">
  <fo:inline border-left-width="0pt" border-right-width="0pt">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="oper" dtm:status="finished" dtm:idref="pr-d.oper"/>
<xsl:template match="*[contains(@class,' pr-d/oper ')]" dtm:id="pr-d.oper">
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-family="Courier">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="delim" dtm:status="finished" dtm:idref="pr-d.delim"/>
<xsl:template match="*[contains(@class,' pr-d/delim ')]" dtm:id="pr-d.delim">
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-family="Courier">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="sep" dtm:status="finished" dtm:idref="pr-d.sep"/>
<xsl:template match="*[contains(@class,' pr-d/sep ')]" dtm:id="pr-d.sep">
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-family="Courier">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>


<dtm:doc dtm:elements="apiname" dtm:status="finished" dtm:idref="pr-d.apiname"/>
<xsl:template match="*[contains(@class,' pr-d/apiname ')]" dtm:id="pr-d.apiname">
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-family="Courier">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="parml" dtm:status="finished" dtm:idref="pr-d.parml"/>
<xsl:template match="*[contains(@class,' pr-d/parml ')]" dtm:id="pr-d.parml">
  <xsl:call-template name="gen-att-label"/>
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</xsl:template>

<dtm:doc dtm:elements="plentry" dtm:status="finished" dtm:idref="pr-d.plentry"/>
<xsl:template match="*[contains(@class,' pr-d/plentry ')]" dtm:id="pr-d.plentry">
 <fo:block>
  <xsl:apply-templates/>
 </fo:block>
</xsl:template>

<dtm:doc dtm:elements="pt" dtm:status="finished" dtm:idref="pr-d.pt"/>
<xsl:template match="*[contains(@class,' pr-d/pt ')]" dtm:id="pr-d.pt">
 <fo:block xsl:use-attribute-sets="dt">
    <xsl:choose>
      <xsl:when test="*"> <!-- tagged content - do not default to bold -->
        <xsl:apply-templates/>
      </xsl:when>
      <xsl:otherwise>
        <fo:inline font-weight="bold"><xsl:apply-templates/></fo:inline> <!-- text only - bold it -->
      </xsl:otherwise>
    </xsl:choose>
 </fo:block>
</xsl:template>

<dtm:doc dtm:elements="pd" dtm:status="finished" dtm:idref="pr-d.pd"/>
<xsl:template match="*[contains(@class,' pr-d/pd ')]" dtm:id="pr-d.pd">
  <fo:block xsl:use-attribute-sets="dd">
   <xsl:apply-templates />
  </fo:block>
</xsl:template>


<!-- syntax diagram -->
<dtm:doc dtm:elements="synblk" dtm:status="finished" dtm:idref="pr-d.synblk"/>
<xsl:template match="*[contains(@class,' pr-d/synblk ')]" dtm:id="pr-d.synblk">
  <fo:inline border-left-width="0pt" border-right-width="0pt">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="synblk/title" dtm:status="finished" dtm:idref="pr-d.synblk.title"/>
<xsl:template match="*[contains(@class,' pr-d/synblk ')]/*[contains(@class,' topic/title ')]" dtm:id="pr-d.synblk.title">
<fo:block font-weight="bold">
  <xsl:apply-templates/>
</fo:block>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="synnotes.generate"/>
<xsl:template name="gen-synnotes" dtm:id="synnotes.generate">
  <fo:block font-weight="bold">Notes:</fo:block>
  <xsl:for-each select="//*[contains(@class,' pr-d/synnote ')]">
    <xsl:call-template name="dosynnt"/>
  </xsl:for-each>
</xsl:template>

<dtm:doc dtm:status="testing" dtm:idref="dosynnt"/>
<xsl:template name="dosynnt" dtm:id="dosynnt"> <!-- creates a list of endnotes of synnt content -->
 <xsl:variable name="callout">
  <xsl:choose>
   <xsl:when test="@callout"><xsl:value-of select="@callout"/></xsl:when>
   <xsl:otherwise><xsl:value-of select="@id"/></xsl:otherwise>
  </xsl:choose>
 </xsl:variable>
 <!--a name="{@id}"-->{<xsl:value-of select="$callout"/>}<!--/a-->
<!--
 <table border="1" cellpadding="6">
   <tr><td bgcolor="LightGrey">
     <xsl:apply-templates/>
   </td></tr>
 </table>
-->
 <fo:block><xsl:apply-templates/></fo:block>
</xsl:template>

<dtm:doc dtm:elements="synnoteref" dtm:status="finished" dtm:idref="pr-d.synnoteref"/>
<xsl:template match="*[contains(@class,' pr-d/synnoteref ')]" dtm:id="pr-d.synnoteref">
<fo:inline border-left-width="0pt" border-right-width="0pt" baseline-shift="super" font-size="75%" href="#FNsrc_{@refid}">
  <xsl:choose>
    <xsl:when test="@href">
      [<xsl:value-of select="@href"/>]
    </xsl:when>
    <xsl:otherwise>
      [<fo:block><xsl:apply-templates/></fo:block>]
    </xsl:otherwise>
  </xsl:choose>
</fo:inline>
</xsl:template>

<dtm:doc dtm:elements="synnote" dtm:status="finished" dtm:idref="pr-d.synnote"/>
<xsl:template match="*[contains(@class,' pr-d/synnote ')]" dtm:id="pr-d.synnote">
<fo:inline border-left-width="0pt" border-right-width="0pt" baseline-shift="super" font-size="75%">
  <xsl:choose>
    <xsl:when test="not(@id='')"> <!-- case of an explicit id -->
            <xsl:value-of select="@id"/>
            <xsl:text>: </xsl:text>
    </xsl:when>
    <xsl:when test="not(@callout='')"> <!-- case of an explicit callout (presume id for now) -->
            <xsl:value-of select="@callout"/>
            <xsl:text> </xsl:text>
    </xsl:when>
    <xsl:otherwise>
          <xsl:text>*</xsl:text>
          <xsl:text> </xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <fo:block>
    <xsl:apply-templates/>
  </fo:block>
</fo:inline>
</xsl:template>

<dtm:doc dtm:elements="syntaxdiagram" dtm:status="finished" dtm:idref="pr-d.syntaxdiagram"/>
<xsl:template match="*[contains(@class,' pr-d/syntaxdiagram ')]" dtm:id="pr-d.syntaxdiagram">
<fo:block>
  <xsl:apply-templates/>
</fo:block>
</xsl:template>

<dtm:doc dtm:elements="fragment" dtm:status="finished" dtm:idref="pr-d.fragment"/>
<xsl:template match="*[contains(@class,' pr-d/fragment ')]" dtm:id="pr-d.fragment">
<fo:block>
  <xsl:value-of select="*[contains(@class,' topic/title ')]"/><xsl:text> </xsl:text>
  <fo:block><xsl:apply-templates/></fo:block>
</fo:block>
</xsl:template>

<!-- Title is optional-->
<dtm:doc dtm:elements="syntaxdiagram/title" dtm:status="finished" dtm:idref="pr-d.syntaxdiagram.title"/>
<xsl:template match="*[contains(@class,' pr-d/syntaxdiagram ')]/*[contains(@class,' topic/title ')]" dtm:id="pr-d.syntaxdiagram.title">
<fo:block font-weight="bold">
  <xsl:apply-templates/>
</fo:block>
</xsl:template>

<dtm:doc dtm:elements="fragment/title" dtm:status="finished" dtm:idref="pr-d.fragment.title"/>
<xsl:template match="*[contains(@class,' pr-d/fragment ')]/*[contains(@class,' topic/title ')]" dtm:id="pr-d.fragment.title">
	<fo:block font-weight="bold"><xsl:apply-templates/></fo:block>
</xsl:template>

<!-- We would hide this content in real output, but leave it editable here. -->
<dtm:doc dtm:elements="repsep" dtm:status="finished" dtm:idref="pr-d.repsep"/>
<xsl:template match="*[contains(@class,' pr-d/repsep ')]" dtm:id="pr-d.repsep">
 <fo:inline border-left-width="0pt" border-right-width="0pt" font-size="4pt">
  <xsl:apply-templates/>
 </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="kwd" dtm:status="finished" dtm:idref="pr-d.kwd"/>
<xsl:template match="*[contains(@class,' pr-d/kwd ')]" dtm:id="pr-d.kwd">
<fo:inline border-left-width="0pt" border-right-width="0pt" font-family="Courier">
  <xsl:if test="parent::*[contains(@class,' pr-d/groupchoice ')]"><xsl:if test="count(preceding-sibling::*)!=0"> | </xsl:if></xsl:if>
  <xsl:if test="@importance='optional'"> [</xsl:if>
  <xsl:choose>
    <xsl:when test="@importance='default'"><fo:inline text-decoration="underline"><xsl:apply-templates/></fo:inline></xsl:when>
    <xsl:otherwise><xsl:apply-templates/></xsl:otherwise>
  </xsl:choose>
  <xsl:if test="@importance='optional'">] </xsl:if>
</fo:inline>
</xsl:template>

<!-- This should test to see if there's a fragment with matching title 
and if so, produce an associative link. -->
<dtm:doc dtm:elements="fragref" dtm:status="finished" dtm:idref="pr-d.fragref"/>
<xsl:template match="*[contains(@class,' pr-d/fragref ')]" priority="100" dtm:id="pr-d.fragref">
<fo:inline border-left-width="0pt" border-right-width="0pt" font-family="Courier">
      <!--a><xsl:attribute name="href">#<xsl:value-of select="."/></xsl:attribute-->
  &lt;<fo:block><xsl:apply-templates/></fo:block>&gt;<!--/a-->
</fo:inline>
</xsl:template>

<!-- Where is the template for var with a priority of 50? -->
<dtm:doc dtm:status="finished" dtm:idref="var"/>
<xsl:template match="var" priority="51" dtm:id="var">
 <fo:inline border-left-width="0pt" border-right-width="0pt" font-style="italic">
  <xsl:if test="parent::*[contains(@class,' pr-d/groupchoice ')]"> | </xsl:if>
  <xsl:if test="@importance='optional'"> [</xsl:if>
  <xsl:choose>
    <xsl:when test="@importance='default'"><fo:inline text-decoration="underscore"><xsl:apply-templates/></fo:inline></xsl:when>
    <xsl:otherwise><xsl:apply-templates/></xsl:otherwise>
  </xsl:choose>
  <xsl:if test="@importance='optional'">] </xsl:if>
 </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="fragment/groupcomp|fragment/groupchoice|fragment/groupseq" dtm:status="finished" dtm:idref="pr-d.fragment.groupcomp|groupchoice|groupseq"/>
<xsl:template match="*[contains(@class,' pr-d/fragment ')]/*[contains(@class,' pr-d/groupcomp ')]|*[contains(@class,' pr-d/fragment ')]/*[contains(@class,' pr-d/groupchoice ')]|*[contains(@class,' pr-d/fragment ')]/*[contains(@class,' pr-d/groupseq ')]" dtm:id="pr-d.fragment.groupcomp|groupchoice|groupseq">
	<fo:block><!--indent this?-->
	<xsl:call-template name="dogroup"/>
	</fo:block>
</xsl:template>

<dtm:doc dtm:elements="syntaxdiagram/groupcomp|syntaxdiagram/groupchoice|syntaxdiagram/groupseq" dtm:status="finished" dtm:idref="pr-d.syntaxdiagram.groupcomp|groupchoice|groupseq"/>
<xsl:template match="*[contains(@class,' pr-d/syntaxdiagram ')]/*[contains(@class,' pr-d/groupcomp ')]|*[contains(@class,' pr-d/syntaxdiagram ')]/*[contains(@class,' pr-d/groupseq ')]|*[contains(@class,' pr-d/syntaxdiagram ')]/*[contains(@class,' pr-d/groupchoice ')]" dtm:id="pr-d.syntaxdiagram.groupcomp|groupchoice|groupseq">
  <xsl:call-template name="dogroup"/>
</xsl:template>


<!-- okay, here we have to work each permutation because figgroup/figroup fallback is too general -->
<dtm:doc dtm:elements="groupcomp/groupcomp" dtm:status="finished" dtm:idref="pr-d.groupcomp"/>
<xsl:template match="*[contains(@class,' pr-d/groupcomp ')]/*[contains(@class,' pr-d/groupcomp ')]" dtm:id="pr-d.groupcomp">
	<xsl:call-template name="dogroup"/>
</xsl:template>

<dtm:doc dtm:elements="groupchoice/groupchoice" dtm:status="finished" dtm:idref="pr-d.groupchoice"/>
<xsl:template match="*[contains(@class,' pr-d/groupchoice ')]/*[contains(@class,' pr-d/groupchoice ')]" dtm:id="pr-d.groupchoice">
	<xsl:call-template name="dogroup"/>
</xsl:template>

<dtm:doc dtm:elements="groupseq/groupseq" dtm:status="finished" dtm:idref="pr-d.groupseq"/>
<xsl:template match="*[contains(@class,' pr-d/groupseq ')]/*[contains(@class,' pr-d/groupseq ')]" dtm:id="pr-d.groupseq">
	<xsl:call-template name="dogroup"/>
</xsl:template>

<dtm:doc dtm:elements="groupchoice/groupcomp" dtm:status="finished" dtm:idref="pr-d.groupchoice.groupcomp"/>
<xsl:template match="*[contains(@class,' pr-d/groupchoice ')]/*[contains(@class,' pr-d/groupcomp ')]" dtm:id="pr-d.groupchoice.groupcomp">
	<xsl:call-template name="dogroup"/>
</xsl:template>

<dtm:doc dtm:elements="groupchoice/groupseq" dtm:status="finished" dtm:idref="pr-d.groupchoice.groupseq"/>
<xsl:template match="*[contains(@class,' pr-d/groupchoice ')]/*[contains(@class,' pr-d/groupseq ')]" dtm:id="pr-d.groupchoice.groupseq">
	<xsl:call-template name="dogroup"/>
</xsl:template>

<dtm:doc dtm:elements="groupcomp/groupchoice" dtm:status="finished" dtm:idref="pr-d.groupcomp.groupchoice"/>
<xsl:template match="*[contains(@class,' pr-d/groupcomp ')]/*[contains(@class,' pr-d/groupchoice ')]" dtm:id="pr-d.groupcomp.groupchoice">
	<xsl:call-template name="dogroup"/>
</xsl:template>

<dtm:doc dtm:elements="groupcomp/groupseq" dtm:status="finished" dtm:idref="pr-d.groupcomp.groupseq"/>
<xsl:template match="*[contains(@class,' pr-d/groupcomp ')]/*[contains(@class,' pr-d/groupseq ')]" dtm:id="pr-d.groupcomp.groupseq">
	<xsl:call-template name="dogroup"/>
</xsl:template>

<dtm:doc dtm:elements="groupseq/groupchoice" dtm:status="finished" dtm:idref="pr-d.groupseq.groupchoice"/>
<xsl:template match="*[contains(@class,' pr-d/groupseq ')]/*[contains(@class,' pr-d/groupchoice ')]" dtm:id="pr-d.groupseq.groupchoice">
	<xsl:call-template name="dogroup"/>
</xsl:template>

<dtm:doc dtm:elements="groupseq/groupcomp" dtm:status="finished" dtm:idref="pr-d.groupseq.groupcomp"/>
<xsl:template match="*[contains(@class,' pr-d/groupseq ')]/*[contains(@class,' pr-d/groupcomp ')]" dtm:id="pr-d.groupseq.groupcomp">
	<xsl:call-template name="dogroup"/>
</xsl:template>

<!--
<xsl:template match="*[contains(@class,' topic/figgroup ')]/*[contains(@class,' topic/figgroup ')]">
	<xsl:call-template name="dogroup"/>
</xsl:template>
-->


	<!--xsl:if test="../@choiceseq='choice'"> | </xsl:if-->
<dtm:doc dtm:status="testing" dtm:idref="doggroup"/>
<xsl:template name="dogroup" dtm:id="doggroup">
	<xsl:if test="parent::*[contains(@class,' pr-d/groupchoice ')]">
		<xsl:if test="count(preceding-sibling::*)!=0"> | </xsl:if>
	</xsl:if>
	<xsl:if test="@importance='optional'">[</xsl:if>
	<xsl:if test="name()='groupchoice'">{</xsl:if>
	  <xsl:text> </xsl:text><fo:inline><xsl:apply-templates/></fo:inline><xsl:text> </xsl:text>
<!-- repid processed here before -->
	<xsl:if test="name()='groupchoice'">}</xsl:if>
	<xsl:if test="@importance='optional'">]</xsl:if>
</xsl:template>

<dtm:doc dtm:elements="groupcomp/title|groupseq/title|groupchoice/title" dtm:status="finished" dtm:idref="pr-d.groupcomp|groupchoice|groupseq.title"/>
<xsl:template match="*[contains(@class,' pr-d/groupcomp ')]/title|*[contains(@class,' pr-d/groupseq ')]/title|*[contains(@class,' pr-d/groupchoice ')]/title" dtm:id="pr-d.groupcomp|groupchoice|groupseq.title">  <!-- Consume title -->
  <xsl:text> </xsl:text>
  <fo:inline> <xsl:apply-templates/></fo:inline>
  <xsl:text> </xsl:text>
</xsl:template>

<!-- start of software domain elements -->
<dtm:doc dtm:elements="msgph" dtm:status="finished" dtm:idref="sw-d.msgph"/>
<xsl:template match="*[contains(@class,' sw-d/msgph ')]" dtm:id="sw-d.msgph">
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-family="Courier">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>


<dtm:doc dtm:elements="msgblock" dtm:status="finished" dtm:idref="sw-d.msgblock"/>
<xsl:template match="*[contains(@class,' sw-d/msgblock ')]" dtm:id="sw-d.msgblock">
  <xsl:if test="contains(@frame,'top')"><hr /></xsl:if>
  <xsl:call-template name="gen-att-label"/>
  <fo:block> <!-- use pre style -->
    <xsl:if test="@scale">
      <!--xsl:attribute name="style">font-size: <xsl:value-of select="@scale"/>%;</xsl:attribute-->
    </xsl:if>
    <xsl:apply-templates/>
  </fo:block>
  <xsl:if test="contains(@frame,'bot')"><hr /></xsl:if>
</xsl:template>

<dtm:doc dtm:elements="msgnum" dtm:status="finished" dtm:idref="sw-d.msgnum"/>
<xsl:template match="*[contains(@class,' sw-d/msgnum ')]" dtm:id="sw-d.msgnum">
  <fo:inline border-left-width="0pt" border-right-width="0pt">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="cmdname" dtm:status="finished" dtm:idref="sw-d.cmdname"/>
<xsl:template match="*[contains(@class,' sw-d/cmdname ')]" dtm:id="sw-d.cmdname">
  <fo:inline border-left-width="0pt" border-right-width="0pt">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="varname" dtm:status="finished" dtm:idref="sw-d.varname"/>
<xsl:template match="*[contains(@class,' sw-d/varname ')]" dtm:id="sw-d.varname">
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-style="italic">
    <xsl:apply-templates/>
  </fo:inline>
  <!-- <xsl:text> </xsl:text> //visual "kerning" for on-screen italics -->
</xsl:template>

<dtm:doc dtm:elements="filepath" dtm:status="finished" dtm:idref="sw-d.filepath"/>
<xsl:template match="*[contains(@class,' sw-d/filepath ')]" dtm:id="sw-d.filepath">
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-family="Courier">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="userinput" dtm:status="finished" dtm:idref="sw-d.userinput"/>
<xsl:template match="*[contains(@class,' sw-d/userinput ')]" dtm:id="sw-d.userinput">
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-family="Courier">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>

<dtm:doc dtm:elements="systemoutput" dtm:status="finished" dtm:idref="sw-d.systemoutput"/>
<xsl:template match="*[contains(@class,' sw-d/systemoutput ')]" dtm:id="sw-d.systemoutput">
  <fo:inline border-left-width="0pt" border-right-width="0pt" font-family="Courier">
    <xsl:apply-templates/>
  </fo:inline>
</xsl:template>


</xsl:transform>

