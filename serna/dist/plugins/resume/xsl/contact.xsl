<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
  xmlns:r="http://xmlresume.sourceforge.net/resume/0.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template name="PhoneLocation">
  <xsl:param name="Location"/>
  <xsl:choose>
    <xsl:when test="$Location = 'home'">
      <xsl:text>Home Phone</xsl:text>
    </xsl:when>
    <xsl:when test="$Location = 'work'">
      <xsl:text>Work Phone</xsl:text>
    </xsl:when>
    <xsl:when test="$Location = 'mobile'">
      <xsl:text>Mobile Phone</xsl:text>
    </xsl:when>
    <xsl:when test="string-length($Location) > 0">
      <xsl:message>
        <xsl:text>***** WARNING: Unknown phone location: '</xsl:text>
        <xsl:value-of select="$Location"/>
        <xsl:text>' (inserting literally into output)</xsl:text>
      </xsl:message>
      <xsl:value-of select="$Location"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>Phone</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="FaxLocation">
  <xsl:param name="Location" /> 
  <xsl:choose>
    <xsl:when test="$Location = 'home'">
      <xsl:text>Home Fax</xsl:text> 
    </xsl:when>
    <xsl:when test="$Location = 'work'">
      <xsl:text>Work Fax</xsl:text>  
    </xsl:when>
    <xsl:when test="string-length($Location) > 0">
      <xsl:message>
        <xsl:text>***** WARNING: Unknown fax location: '</xsl:text> 
        <xsl:value-of select="$Location" /> 
        <xsl:text>' (inserting literally into output)</xsl:text> 
      </xsl:message>
      <xsl:value-of select="$Location" /> 
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>Fax</xsl:text> 
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="IMServiceName">
  <xsl:param name="Service" /> 
  <xsl:choose>
    <xsl:when test="$Service = 'aim'">
      <xsl:text>AIM</xsl:text> 
    </xsl:when>
    <xsl:when test="$Service = 'icq'">
      <xsl:text>ICQ</xsl:text>  
    </xsl:when>
    <xsl:when test="$Service = 'irc'">
      <xsl:text>IRC</xsl:text> 
    </xsl:when>
    <xsl:when test="$Service = 'jabber'">
      <xsl:text>Jabber</xsl:text> 
    </xsl:when>
    <xsl:when test="$Service = 'msn'">
      <xsl:text>MSN Messenger</xsl:text> 
    </xsl:when>
    <xsl:when test="$Service = 'yahoo'">
      <xsl:text>Yahoo! Messenger</xsl:text>  
    </xsl:when>
    <xsl:when test="string-length($Service) > 0">
      <xsl:message>
        <xsl:text>***** WARNING: Unknown instantMessage service: '</xsl:text> 
        <xsl:value-of select="$Service" /> 
        <xsl:text>' (inserting literally into output)</xsl:text> 
      </xsl:message>
      <xsl:value-of select="$Service" /> 
    </xsl:when>
    <xsl:otherwise /> 
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
