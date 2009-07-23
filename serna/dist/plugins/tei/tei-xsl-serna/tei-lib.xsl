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

<xsl:template name="rend">
  <xsl:param name="defaultvalue"/>
  <xsl:param name="defaultstyle"/>
  <xsl:param name="rend"/>
  <xsl:choose>
    <xsl:when test="$rend=''">
       <xsl:attribute name="{$defaultstyle}">
           <xsl:value-of select="$defaultvalue"/>
      </xsl:attribute>  
    </xsl:when>
    <xsl:when test="contains($rend,';')">
      <xsl:call-template name="applyRend">
        <xsl:with-param name="rendvalue" select="substring-before($rend,';')"/>
      </xsl:call-template>
      <xsl:call-template name="rend">
        <xsl:with-param name="rend" select="substring-after($rend,';')"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="applyRend">
       <xsl:with-param name="rendvalue" select="$rend"/>
      </xsl:call-template>   
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="applyRend">
  <xsl:param name="rendvalue"/>
   <xsl:choose>
   <xsl:when test="$rendvalue='gothic'">
     <xsl:attribute name="font-family">fantasy</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='calligraphic'">
     <xsl:attribute name="font-family">cursive</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='ital' or $rendvalue='italic' or $rendvalue='it' or $rendvalue='i'">
     <xsl:attribute name="font-style">italic</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='sc'">
     <xsl:attribute name="font-variant">small-caps</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='code'">
     <xsl:attribute name="font-family">
       <xsl:value-of select="$monospace.font.family"/>
     </xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='bo' or $rendvalue='bold'">
     <xsl:attribute name="font-weight">bold</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='BO'">
     <xsl:attribute name="font-style">italic</xsl:attribute>
     <xsl:attribute name="text-decoration">underline</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='UL' or $rendvalue='ul' or $rendvalue='underscore'">
     <xsl:attribute name="text-decoration">underline</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='sub' or $rendvalue='subscript'">
     <xsl:attribute name="baseline-shift">sub</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='small'">
     <xsl:attribute name="font-size">small</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='strike'">
     <xsl:attribute name="text-decoration">line-through</xsl:attribute>
   </xsl:when>
   <xsl:when test="$rendvalue='sup' or $rendvalue='superscript'">
     <xsl:attribute name="baseline-shift">super</xsl:attribute>
   </xsl:when>
</xsl:choose>
</xsl:template>

<xsl:template name="addID">
  <xsl:attribute name="id">
    <xsl:call-template name="idLabel"/>
  </xsl:attribute>
</xsl:template>

<xsl:template name="idLabel">
   <xsl:choose>
       <xsl:when test="@id">
         <xsl:value-of select="translate(@id,'_','-')"/>
       </xsl:when>
       <xsl:otherwise>
          <xsl:value-of select="generate-id()"/>
       </xsl:otherwise>
   </xsl:choose>
</xsl:template>

</xsl:stylesheet>
