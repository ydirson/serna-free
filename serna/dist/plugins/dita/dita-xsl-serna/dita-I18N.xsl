<?xml version='1.0'?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:fo="http://www.w3.org/1999/XSL/Format"
                xmlns:se="http://www.syntext.com/XSL/Format-1.0"
                xmlns:dtm="http://syntext.com/Extensions/DocumentTypeMetadata-1.0"
                extension-element-prefixes="dtm"                
                version='1.0'>


<!-- basis for string localization -->
<dtm:doc dtm:status="testing" dtm:idref="xgetString"/>
<xsl:template name="xgetString" dtm:id="xgetString">
  <xsl:param name="stringName"/>
  <xsl:value-of select="$stringName"/>
</xsl:template>

<!-- language support (from an algorithm described in XSL FAQ) -->
<!--
        given the info needed to produce a set of candidates ($str),
           pick the best of the bunch:
           1. $str[lang($Lang)][1]
           2. $str[lang($PrimaryLang)][1]
           3. $str[1]
           4. if not($str) then issue warning to STDERR
   - -->
<dtm:doc dtm:status="testing" dtm:idref="getString"/>
<xsl:template name="getString" dtm:id="getString">
  <xsl:param name="stringName"/>
  <xsl:variable name="str" 
     select="$StringFile/strings/str[@name=$stringName]"/>
  <xsl:choose>
    <xsl:when test="$str[lang($Lang)]">
      <xsl:value-of select="$str[lang($Lang)][1]"/>
    </xsl:when>
    <xsl:when test="$str[lang($PrimaryLang)]">
      <xsl:value-of select="$str[lang($PrimaryLang)][1]"/>
    </xsl:when>
    <xsl:when test="$str">
      <xsl:value-of select="$str[1]"/>
    </xsl:when>
    <xsl:otherwise>
       [<xsl:value-of select="$stringName"/>]
    </xsl:otherwise>
  </xsl:choose>

</xsl:template>


<!-- =================== end of I18N generated text support ====================== -->



</xsl:stylesheet>
