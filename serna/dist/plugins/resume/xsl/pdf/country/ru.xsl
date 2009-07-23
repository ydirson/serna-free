<?xml version='1.0' encoding='UTF-8'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:param name="resume.word">ó€òµò·óŽò¼òµ</xsl:param>
  <xsl:param name="page.word">óó‚ó€</xsl:param>
<!-- Word to use for "Contact Information" -->  <xsl:param name="contact.word">òšò¾ò½ó‚ò°òºó‚ó‹</xsl:param>
  <xsl:param name="objective.word">ò–òµò»ò°òµò¼ò°ó ò¿ò¾ò·ò¸ó†ò¸ó</xsl:param>
<!-- Word to use for "Employment History" -->  <xsl:param name="history.word">òŸó€ò¾ó„òµóóò¸ò¾ò½ò°ò»óŒò½ó‹ò¹ ò¾ò¿ó‹ó‚</xsl:param>
  <xsl:param name="academics.word">òžò±ó€ò°ò·ò¾ò²ò°ò½ò¸òµ</xsl:param>
  <xsl:param name="publications.word">òŸóƒò±ò»ò¸òºò°ó†ò¸ò¸</xsl:param>
  <xsl:param name="interests.word">ò˜ò½ó‚òµó€òµóó‹</xsl:param>
  <xsl:param name="security-clearances.word">ò”ò¾ò¿óƒóòºò¸ òº óòµòºó€òµó‚ò½ò¾óó‚ò¸</xsl:param>
  <xsl:param name="awards.word">òò°ò³ó€ò°ò´ó‹</xsl:param>
  <xsl:param name="miscellany.word">ò ò°ò·ò½ò¾òµ</xsl:param>
<!-- Word to use for "in", as in "bachelor degree *in* political science" -->  <xsl:param name="in.word"/>
<!-- Word to use for "and", as in "Minors in political science, English, *and*
  business" -->  <xsl:param name="and.word">ò¸</xsl:param>
<!-- Word to use for "Copyright (c)" -->  <xsl:param name="copyright.word">Copyright ÷©</xsl:param>
<!-- Word to use for "by", as in "Copyright by Joe Doom" -->  <xsl:param name="by.word"/>
<!-- Word to use for "present", as in "Period worked: August 1999-Present" -->  <xsl:param name="present.word">ò½ò°óó‚ò¾óó‰òµòµ ò²ó€òµò¼ó</xsl:param>
  <xsl:param name="achievements.word">ò´ò¾óó‚ò¸ò¶òµò½ò¸ó:</xsl:param>
  <xsl:param name="projects.word">òŸó€ò¾òµòºó‚ó‹:</xsl:param>
<!-- Word to use for "minor" (lesser area of study), singluar and plural. -->  <xsl:param name="minor.word">óò¿òµó†ò¸ò°ò»ò¸ò·ò°ó†ò¸ó</xsl:param>
  <xsl:param name="minors.word">ò´ò¾ò¿. óò¿òµó†ò¸ò°ò»ò¸ò·ò°ó†ò¸ò¸</xsl:param>
  <xsl:param name="referees.word">ò òµòºò¾ò¼òµò½ò´ò°ó†ò¸ò¸</xsl:param>
<!-- Word to use for "Overall GPA", as in "*Overall GPA*: 3.3" -->  <xsl:param name="overall-gpa.word">ò¾ò±ó‰ò¸ò¹ óó€òµò´ò½ò¸ò¹ ò±ò°ò»</xsl:param>
<!-- Word to use for "GPA in Major", as in "*GPA in Major*: 3.3" -->  <xsl:param name="major-gpa.word">óò¿ó€òµò´ò½ò¸ò¹ ò±ò°ò» ò¿ò¾ óò¿òµó†ò¸ò°ò»ò¸ò·ò°ó†ò¸ò¸</xsl:param>
<!-- Text to use for "out of", as in "GPA: 3.71* out of *4.00" -->  <xsl:param name="out-of.word"> ò¸ò· </xsl:param>
<!-- Phrase to display when referees are hidden. -->  <xsl:param name="referees.hidden.phrase">ò¿ò¾ ò·ò°ò¿ó€ò¾óóƒ</xsl:param>
  <xsl:param name="last-modified.phrase">òŸò¾óò»òµò´ò½òµòµ ò¾ò±ò½ò¾ò²ò»òµò½ò¸òµ:</xsl:param>
  <xsl:param name="phone.word">ó‚òµò».</xsl:param>
  <xsl:param name="fax.word">ò¤òòšò¡</xsl:param>
  <xsl:param name="phone.home.phrase">ò”ò¾ò¼ò°óˆò½ò¸ò¹ <xsl:value-of select="$phone.word"/></xsl:param>
  <xsl:param name="phone.work.phrase">ò ò°ò±ò¾ó‡ò¸ò¹ <xsl:value-of select="$phone.word"/></xsl:param>
  <xsl:param name="phone.mobile.phrase">òœò¾ò±ò¸ò»óŒò½ó‹ò¹ <xsl:value-of select="$phone.word"/></xsl:param>
  <xsl:param name="fax.home.phrase">ò”ò¾ò¼ò°óˆò½ò¸ò¹ <xsl:value-of select="$fax.word"/></xsl:param>
  <xsl:param name="fax.work.phrase">ò ò°ò±ò¾ó‡ò¸ò¹ <xsl:value-of select="$fax.word"/></xsl:param>
  <xsl:param name="pager.word">òŸòµò¹ò´ò¶òµó€</xsl:param>
  <xsl:param name="email.word">Email</xsl:param>
  <xsl:param name="url.word">URL</xsl:param>
<!-- Instant messenger service names --><!-- (When you add or remove a service here, don't forget to update
  ../../lib/common.xsl and element.instantMessage.xml in the user guide.)
  -->  <xsl:param name="im.aim.service">AIM</xsl:param>
  <xsl:param name="im.icq.service">ICQ</xsl:param>
  <xsl:param name="im.irc.service">IRC</xsl:param>
  <xsl:param name="im.jabber.service">Jabber</xsl:param>
  <xsl:param name="im.msn.service">MSN Messenger</xsl:param>
  <xsl:param name="im.yahoo.service">Yahoo! Messenger</xsl:param>
</xsl:stylesheet>
