<?xml version="1.0" encoding="iso-8859-1"?>

<!--
de.xsl
Parameters for German resumes.

Copyright (c) 2001 Sean Kelly
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

$Id: de.xsl,v 1.2 2002/11/15 18:54:53 brandondoyle Exp $
-->

<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:param name="resume.word">Lebenslauf</xsl:param>
  <xsl:param name="page.word">Seite</xsl:param>
  <xsl:param name="contact.word">Personalien</xsl:param>
  <xsl:param name="objective.word">Ziele</xsl:param>
  <xsl:param name="history.word">Ausgeübte Tätigkeiten</xsl:param>
  <xsl:param name="academics.word">Akademische Ausbildung</xsl:param>
  <xsl:param name="publications.word">Veröffentlichungen</xsl:param>
  <xsl:param name="interests.word">Interessen</xsl:param>
  <xsl:param name="security-clearances.word">Leumundszeugnis</xsl:param>
  <xsl:param name="awards.word">Preise</xsl:param>

  <xsl:param name="miscellany.word">Verschiedenes</xsl:param>
  <xsl:param name="in.word">in</xsl:param>
  <xsl:param name="and.word">und</xsl:param>
  <xsl:param name="copyright.word">Copyright &#169;</xsl:param>
  <xsl:param name="by.word"></xsl:param>
  <xsl:param name="present.word">Gegenwart</xsl:param>
  <xsl:param name="achievements.word">Erreichte Ziele:</xsl:param>
  <xsl:param name="projects.word">Projekte:</xsl:param>
  <!-- Word to use for "minor" (lesser area of study), singluar and plural. -->
  <xsl:param name="minor.word">Nebenfach</xsl:param>
  <xsl:param name="minors.word">Nebenfächer</xsl:param>
  <xsl:param name="referees.word">Referenzen</xsl:param>
  <!-- Word to use for "Overall GPA", as in "*Overall GPA*: 3.3" -->
  <xsl:param name="overall-gpa.word">Notendurchschnitt</xsl:param>
  <!-- Word to use for "GPA in Major", as in "*GPA in Major*: 3.3" -->
  <xsl:param name="major-gpa.word">Notendurchschnitt der Hauptfächer</xsl:param>
  <!-- Text to use for "out of", as in "GPA: 3.71* out of *4.00" -->
  <xsl:param name="out-of.word">von</xsl:param>

  <!-- Phrase to display when referees are hidden. -->
  <xsl:param name="referees.hidden.phrase">Referenzen werden gerne auf Anfrage genannt.</xsl:param>
  <xsl:param name="last-modified.phrase">Letzte Aktualisierung:</xsl:param>

  <xsl:param name="phone.word">Telefon</xsl:param>
  <xsl:param name="fax.word">Fax</xsl:param>
  <xsl:param name="phone.home.phrase"><xsl:value-of select="$phone.word"/> Privat</xsl:param>
  <xsl:param name="phone.work.phrase"><xsl:value-of select="$phone.word"/> Geschäft</xsl:param>
  <xsl:param name="phone.mobile.phrase"><xsl:value-of select="$phone.word"/> Mobil</xsl:param>
  <xsl:param name="fax.home.phrase"><xsl:value-of select="$fax.word"/> Privat</xsl:param>
  <xsl:param name="fax.work.phrase"><xsl:value-of select="$fax.word"/> Geschäft</xsl:param>

  <xsl:param name="pager.word">Pager</xsl:param>
  <xsl:param name="email.word">Email</xsl:param>
  <xsl:param name="url.word">URL</xsl:param>

  <!-- Instant messenger service names -->
  <!-- (When you add or remove a service here, don't forget to update -->
  <!-- ../../lib/common.xsl and element.instantMessage.xml in the user guide.) -->
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
