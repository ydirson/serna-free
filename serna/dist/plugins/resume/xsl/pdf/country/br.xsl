<?xml version="1.0" encoding="UTF-8"?>

<!--
br.xsl
Parameters for Brazilian resumes.

Copyright (c) 2002 Felipe Leme
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the
   distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

$Id: br.xsl,v 1.4 2002/11/26 18:39:27 brandondoyle Exp $
-->

<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:param name="resume.word">Curriculum Vitae</xsl:param>
  <xsl:param name="page.word">p&#x00e1;gina</xsl:param>
  <!-- Word to use for "Contact Information" -->
  <xsl:param name="contact.word">Dados para Contato</xsl:param>
  <xsl:param name="objective.word">Objetivo Profissional</xsl:param>
  <!-- Word to use for "Employment History" -->
  <xsl:param name="history.word">Experi&#x00ea;ncia Profissional</xsl:param>
  <xsl:param name="academics.word">Forma&#x00e7;&#x00e3;o Acad&#x00ea;mica</xsl:param>
  <xsl:param name="publications.word">Publica&#x00e7;&#x00f5;es</xsl:param>
  <xsl:param name="interests.word">Interesses</xsl:param>
  <xsl:param name="security-clearances.word">Autoriza&#x00e7;&#x00f5;es</xsl:param>
  <xsl:param name="awards.word">Pr&#x00E8;mios</xsl:param>
  <xsl:param name="miscellany.word">Diversos</xsl:param>
  <!-- Word to use for "in", as in "bachelor degree *in* political science" -->
  <xsl:param name="in.word">em</xsl:param>
  <!-- Word to use for "and", as in "Minors in political science, 
	English, *and* business" -->
  <xsl:param name="and.word">e</xsl:param>
  <!-- Word to use for "Copyright (c)" -->
  <xsl:param name="copyright.word">Copyright &#169;</xsl:param>
  <!-- Word to use for "by", as in "Copyright by Joe Doom" -->
  <xsl:param name="by.word">por</xsl:param>
  <!-- Word to use for "present", as in "Period worked: August 1999-Present" -->
  <xsl:param name="present.word">presente data</xsl:param>
  <xsl:param name="achievements.word">Realiza&#x00e7;&#x00f5;es:</xsl:param>
  <xsl:param name="projects.word">Projetos:</xsl:param>

  <!-- Word to use for "minor" (lesser area of study), singluar and plural. -->
  <xsl:param name="minor.word">minorit&#x00E1;rio</xsl:param>
  <xsl:param name="minors.word">minorit&#x00E1;rios</xsl:param>
  <xsl:param name="referees.word">Refer&#x00ea;ncias</xsl:param>
  <!-- Word to use for "Overall GPA", as in "*Overall GPA*: 3.3" -->
  <xsl:param name="overall-gpa.word">M&#x00E9;dia Geral</xsl:param>
  <!-- Word to use for "GPA in Major", as in "*GPA in Major*: 3.3" -->
  <xsl:param name="major-gpa.word">M&#x00E9;dia Principal</xsl:param>
  <!-- Text to use for "out of", as in "GPA: 3.71* out of *4.00" -->
  <xsl:param name="out-of.word">de um total de</xsl:param>

  <!-- Phrase to display when referees are hidden. -->
  <xsl:param name="referees.hidden.phrase">Dispon&#x00ED;vel sob pedido.</xsl:param>
  <xsl:param name="last-modified.phrase">&#x00DA;ltima modifica&#x00e7;&#x00f5;</xsl:param>

  <xsl:param name="phone.word">Telefone</xsl:param>
  <xsl:param name="fax.word">Fax</xsl:param>
  <xsl:param name="pager.word">Pager</xsl:param>
  <xsl:param name="email.word">Endere&#x00e7;o Eletr&#x00f4;nico</xsl:param>
  <xsl:param name="url.word">URL</xsl:param>

  <!-- Words for phone and fax locations, as in "Home Phone", or "Work Fax" -->

  <xsl:param name="phone.home.phrase"><xsl:value-of select="$phone.word"/> Residencial</xsl:param>
  <xsl:param name="phone.work.phrase"><xsl:value-of select="$phone.word"/> Comercial</xsl:param>
  <xsl:param name="phone.mobile.phrase"><xsl:value-of select="$phone.word"/> Celular</xsl:param>
  <xsl:param name="fax.home.phrase"><xsl:value-of select="$fax.word"/> Residencial</xsl:param>
  <xsl:param name="fax.work.phrase"><xsl:value-of select="$fax.word"/> Comercial</xsl:param>

  <!-- Instant messenger service names -->
  <!-- (When you add or remove a service here, don't forget to update
  ../../lib/common.xsl and element.instantMessage.xml in the user guide.)
  -->
  <xsl:param name="im.aim.service">AIM</xsl:param>
  <xsl:param name="im.icq.service">ICQ</xsl:param>
  <xsl:param name="im.irc.service">IRC</xsl:param>
  <xsl:param name="im.jabber.service">Jabber</xsl:param>
  <xsl:param name="im.msn.service">MSN Messenger</xsl:param>
  <xsl:param name="im.yahoo.service">Yahoo! Messenger</xsl:param>

  <!-- default is European address formatting.  For countries other -->
  <!-- than France (e.g. Canada) "standard" formatting may be more correct. -->
  <xsl:param name="address.format">european</xsl:param>

</xsl:stylesheet>
