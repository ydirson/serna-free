<?xml version="1.0" encoding="UTF-8"?>

<!--
textlayout.xsl
Library of plain-text layout helper templates.

 - This library differs from text.xsl in that it has no XML Resume-specific
   code.
 - This library differs from string.xsl in that string.xsl provides templates
   that are useful to any application that may need to do text processing; this
   library is likely to be useful only when doing plain text output.
 - This library could forseeably be used in a different project that needs to do
   text layout in XSLT.

Copyright (c) 2000-2002 by Vlad Korolev, Sean Kelly, and Bruce Christensen

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

$Id: textlayout.xsl,v 1.3 2002/11/25 02:35:22 brandondoyle Exp $
-->

<xsl:stylesheet version="1.0"
         xmlns:r="http://xmlresume.sourceforge.net/resume/0.0"
         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:template name="NewLine">
    <xsl:text>&#xA;</xsl:text>
  </xsl:template>
  
  <!-- Create a heading. -->
  <xsl:template name="Heading">
    <xsl:param name="Text"/>

    <xsl:call-template name="NewLine"/>
    <xsl:call-template name="NewLine"/>
    <xsl:value-of select="$Text"/><xsl:text>:</xsl:text>
    <xsl:call-template name="NewLine"/>
    <xsl:call-template name="NewLine"/>
  </xsl:template>
  
  <xsl:template name="NSpace">
    <xsl:param name="n" select="0"/>
    <xsl:if test="$n &gt; 0">
      <xsl:text> </xsl:text>
      <xsl:call-template name="NSpace">
        <xsl:with-param name="n" select="$n - 1" />
      </xsl:call-template>
    </xsl:if>
  </xsl:template>

  <xsl:template name="Indent">
    <xsl:param name="Length" select="$text.indent.width"/>
    <xsl:param name="Text"/>

    <!-- Put Space -->
    <xsl:call-template name="NSpace">
      <xsl:with-param name="n" select="$Length"/>
    </xsl:call-template>

    <!-- Display One Line -->
    <xsl:choose>
      <xsl:when test="contains($Text,'&#xA;')">
        <xsl:value-of select="substring-before($Text,'&#xA;')"/>
        <xsl:call-template name="NewLine"/>
        <xsl:call-template name="Indent">
          <xsl:with-param name="Length" select="$Length"/>
          <xsl:with-param name="Text" select="substring-after($Text,'&#xA;')"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$Text"/>
      </xsl:otherwise>
    </xsl:choose>

    <!-- Continue with the rest -->
  </xsl:template>
  
  <!-- Center a multi-line block of text -->
  <xsl:template name="CenterBlock">
    <xsl:param name="Width" select="$text.width"/>
    <xsl:param name="Text" />
    <xsl:choose>
      <xsl:when test="contains($Text,'&#xA;')">
        <xsl:call-template name="Center">
          <xsl:with-param name="Text" select="substring-before($Text,'&#xA;')"/>
          <xsl:with-param name="Width" select="$Width"/>
        </xsl:call-template>
        <xsl:call-template name="NewLine"/>
        <xsl:call-template name="CenterBlock">
          <xsl:with-param name="Width" select="$Width"/>
          <xsl:with-param name="Text" select="substring-after($Text,'&#xA;')"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="Center">
          <xsl:with-param name="Text" select="$Text"/>
          <xsl:with-param name="Width" select="$Width"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Center a single line of text -->
  <xsl:template name="Center">
    <xsl:param name="Width" select="$text.width"/>
    <xsl:param name="Text" />

    <xsl:if test="string-length($Text) &gt; 0">
      <xsl:call-template name="NSpace">
        <xsl:with-param
          name="n"
          select="($Width - string-length($Text)) div 2" />
      </xsl:call-template>
      <xsl:value-of select="$Text"/>
    </xsl:if>
  </xsl:template>

  <!-- Named template for formatting a generic bullet list item *SE* -->
  <!-- Simply calls Wrap to do a hanging indent of the bullet character and
  $Text. -->
  <xsl:template name="FormatBulletListItem" >
    <xsl:param name="Text"/>
    <!-- Note: This is the max width of the lines outputted, *including the
    bullet* -->
    <xsl:param name="Width" select="$text.width - $text.indent.width"/>

    <xsl:call-template name="Wrap">
      <xsl:with-param name="Text">
        <xsl:value-of select="$text.bullet.prefix"/>
        <xsl:value-of select="normalize-space($Text)"/>
      </xsl:with-param>
      <xsl:with-param name="Width" select="$Width"/>
      <xsl:with-param name="FirstIndent" select="0"/>
      <xsl:with-param name="Indent" select="string-length($text.bullet.prefix)"/>
      <xsl:with-param name="KeepNewlines" select="1"/>
    </xsl:call-template>

  </xsl:template>

  <!-- Wraps $Text to $Width characters(*). Whitespace in input text is
  normalized before processing, with the exception of newlines if $KeepNewlines
  is true.
  
  The first line outputted will be indented $FirstIndent spaces (Default:
  $Indent.)

  All subsequent lines will be indented $Indent spaces. (Default: 0.)

  $FirstIndent and $Indent are both measured from column zero; that is, they're
  independent of each other. If $FirstIndent is 0, and $Indent is 3, you'll get
  text shaped like this:

  a b c d e f g h i j
     k l m n o p q r s
     t u v w x y z

  Spaces used for indenting count toward $Width.

  (*) If $Text contains a word that's longer than $Width, that word will be
  output on a line by itself, causing the width of the outputted text to be
  greater than $Width. Words are separated by spaces. -->
  <xsl:template name="Wrap">
    <!-- Text to wrap -->
    <xsl:param name="Text"/>
    <!-- Maximum line length; lines longer than this get wrapped -->
    <xsl:param name="Width" select="$text.width - $text.indent.width"/>
    <!-- Whether newlines in $Text should be kept. -->
    <xsl:param name="KeepNewlines" select="0"/>
    <!-- Number of characters to indent each line. If $FirstIndent is specified,
    the first line is indented that many characters instead. -->
    <xsl:param name="Indent" select="0"/>
    <!-- Number of characters to indent first line -->
    <xsl:param name="FirstIndent" select="$Indent"/>

    <!-- Number of characters outputted on the current line -->
    <xsl:param name="CPos" select="0"/>
    <!-- Is the current line the first line? -->
    <xsl:param name="IsFirstLine" select="1"/>
    <!-- Has $Text been normalized? -->
    <xsl:param name="IsNormalized" select="0"/>

    <xsl:choose>

      <xsl:when test="string-length($Text) = 0">
        <!-- Basis case; we're done. -->
      </xsl:when>

      <!-- Keep newlines if requested. (Basic idea is to split on newlines, do
      wrapping on each part, and then join results with newlines.) -->
      <xsl:when test="$KeepNewlines and contains($Text, '&#xA;')">

        <xsl:variable name="Before" select="substring-before($Text, '&#xA;')"/>
        <xsl:variable name="After" select="substring-after($Text, '&#xA;')"/>

        <xsl:call-template name="Wrap">
          <xsl:with-param name="Text" select="$Before"/>
          <xsl:with-param name="Width" select="$Width"/>
          <xsl:with-param name="FirstIndent" select="$FirstIndent"/>
          <xsl:with-param name="Indent" select="$Indent"/>
          <xsl:with-param name="KeepNewlines" select="1"/>
          <xsl:with-param name="IsFirstLine" select="$IsFirstLine"/>
        </xsl:call-template>

        <xsl:call-template name="NewLine"/>

        <xsl:call-template name="Wrap">
          <xsl:with-param name="Text" select="$After"/>
          <xsl:with-param name="Width" select="$Width"/>
          <xsl:with-param name="FirstIndent" select="$FirstIndent"/>
          <xsl:with-param name="Indent" select="$Indent"/>
          <xsl:with-param name="KeepNewlines" select="1"/>
          <xsl:with-param name="IsFirstLine" select="0"/>
        </xsl:call-template>

      </xsl:when>

      <xsl:when test="not($IsNormalized)">
        <xsl:call-template name="Wrap">
          <xsl:with-param name="Text" select="normalize-space($Text)"/>
          <xsl:with-param name="Width" select="$Width"/>
          <xsl:with-param name="FirstIndent" select="$FirstIndent"/>
          <xsl:with-param name="Indent" select="$Indent"/>
          <xsl:with-param name="CPos" select="0"/>
          <xsl:with-param name="IsFirstLine" select="$IsFirstLine"/>
          <xsl:with-param name="IsNormalized" select="1"/>
        </xsl:call-template>
      </xsl:when>

      <!-- If we're here, text non-empty, normalized, and newline-free. We just
      have to output the next word, or, if that would cause the line to become
      too long, output a newline. -->
      <xsl:otherwise>

        <xsl:variable name="Word">
          <xsl:choose>
            <xsl:when test="contains($Text,' ')">
              <xsl:value-of select="substring-before($Text,' ')"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:value-of select="$Text"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:variable>
        <xsl:variable name="WordWidth" select="string-length($Word)"/>

        <!-- Number of spaces to output before $Word -->
        <xsl:variable name="SpaceWidth">
          <!-- We're at the beginning of the line, so we may need to indent -->
          <xsl:choose>
            <xsl:when test="$CPos=0">
              <xsl:choose>
                <!-- We're on the first line -->
                <xsl:when test="$IsFirstLine">
                  <xsl:value-of select="$FirstIndent"/>
                </xsl:when>
                <!-- We're on a following line -->
                <xsl:otherwise>
                  <xsl:value-of select="$Indent"/>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:when>
            <!-- We're not at the beginning of the line, so we'll need just one
            space to separate this word from the previous one -->
            <xsl:otherwise>1</xsl:otherwise>
          </xsl:choose>
        </xsl:variable>

        <xsl:choose>

          <!-- If the word length is longer than the allowed line length, and
          we're at the start of the line, then output the word. (Without this
          case, we'd get stuck in endless recursion.) -->
          <xsl:when test="($CPos = 0) and ($SpaceWidth + $WordWidth &gt; $Width)">

            <xsl:call-template name="NSpace">
              <xsl:with-param name="n" select="$SpaceWidth"/>
            </xsl:call-template>

            <xsl:value-of select="$Word"/>

            <xsl:call-template name="Wrap">
              <xsl:with-param name="Text" select="substring-after($Text, ' ')"/>
              <xsl:with-param name="Width" select="$Width"/>
              <xsl:with-param name="FirstIndent" select="$FirstIndent"/>
              <xsl:with-param name="Indent" select="$Indent"/>
              <xsl:with-param name="CPos" select="$WordWidth"/>
              <xsl:with-param name="IsFirstLine" select="$IsFirstLine"/>
              <xsl:with-param name="IsNormalized" select="1"/>
            </xsl:call-template>

          </xsl:when>

          <!-- If the word will make this line too long, make a new line and
          try again -->
          <xsl:when test="$CPos + $SpaceWidth + $WordWidth &gt; $Width">

            <xsl:call-template name="NewLine"/>

            <xsl:call-template name="Wrap">
              <xsl:with-param name="Text" select="$Text"/>
              <xsl:with-param name="Width" select="$Width"/>
              <xsl:with-param name="FirstIndent" select="$FirstIndent"/>
              <xsl:with-param name="Indent" select="$Indent"/>
              <xsl:with-param name="CPos" select="0"/>
              <xsl:with-param name="IsFirstLine" select="0"/>
              <xsl:with-param name="IsNormalized" select="1"/>
            </xsl:call-template>

          </xsl:when>

          <!-- There's room on this line for the word, so output it, possibly
          prepended with a space -->
          <xsl:otherwise>

            <xsl:call-template name="NSpace">
              <xsl:with-param name="n" select="$SpaceWidth"/>
            </xsl:call-template>

            <xsl:value-of select="$Word"/>

            <xsl:call-template name="Wrap">
              <xsl:with-param name="Text" select="substring-after($Text, ' ')"/>
              <xsl:with-param name="Width" select="$Width"/>
              <xsl:with-param name="FirstIndent" select="$FirstIndent"/>
              <xsl:with-param name="Indent" select="$Indent"/>
              <xsl:with-param name="CPos"
                select="$CPos + $SpaceWidth + $WordWidth"/>
              <xsl:with-param name="IsFirstLine" select="$IsFirstLine"/>
              <xsl:with-param name="IsNormalized" select="1"/>
            </xsl:call-template>

          </xsl:otherwise>

        </xsl:choose>

      </xsl:otherwise>

    </xsl:choose>

  </xsl:template>

</xsl:stylesheet>
