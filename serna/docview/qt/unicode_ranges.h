// 
// Copyright(c) 2009 Syntext, Inc. All Rights Reserved.
// Contact: info@syntext.com, http://www.syntext.com
// 
// This file is part of Syntext Serna XML Editor.
// 
// COMMERCIAL USAGE
// Licensees holding valid Syntext Serna commercial licenses may use this file
// in accordance with the Syntext Serna Commercial License Agreement provided
// with the software, or, alternatively, in accorance with the terms contained
// in a written agreement between you and Syntext, Inc.
// 
// GNU GENERAL PUBLIC LICENSE USAGE
// Alternatively, this file may be used under the terms of the GNU General 
// Public License versions 2.0 or 3.0 as published by the Free Software 
// Foundation and appearing in the file LICENSE.GPL included in the packaging 
// of this file. In addition, as a special exception, Syntext, Inc. gives you
// certain additional rights, which are described in the Syntext, Inc. GPL 
// Exception for Syntext Serna Free Edition, included in the file 
// GPL_EXCEPTION.txt in this package.
// 
// You should have received a copy of appropriate licenses along with this 
// package. If not, see <http://www.syntext.com/legal/>. If you are unsure
// which license is appropriate for your use, please contact the sales 
// department at sales@syntext.com.
// 
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
// 
//# Correlated with Unicode 4.0
//# Note: The casing of block names is not normative.
//#       For example, "Basic Latin" and "BASIC LATIN" are equivalent.
//#
//# Code points not explicitly listed in this file are given the value No_Block.
//#
//# Start Code..End Code; Block Name

#ifndef UNICODE_RANGES_H
#define UNICODE_RANGES_H

static char * unicode_ranges[102] = {
"0020..007F; Basic Latin",
"0080..00FF; Latin-1 Supplement",
"0100..017F; Latin Extended-A",
"0180..024F; Latin Extended-B",
"0250..02AF; IPA Extensions",
"02B0..02FF; Spacing Modifier Letters",
"0300..036F; Combining Diacritical Marks",
"0370..03FF; Greek and Coptic",
"0400..04FF; Cyrillic",
"0500..052F; Cyrillic Supplementary",
"0530..058F; Armenian",
"0590..05FF; Hebrew",
"0600..06FF; Arabic",
"0700..074F; Syriac",
"0780..07BF; Thaana",
"0900..097F; Devanagari",
"0980..09FF; Bengali",
"0A00..0A7F; Gurmukhi",
"0A80..0AFF; Gujarati",
"0B00..0B7F; Oriya",
"0B80..0BFF; Tamil",
"0C00..0C7F; Telugu",
"0C80..0CFF; Kannada",
"0D00..0D7F; Malayalam",
"0D80..0DFF; Sinhala",
"0E00..0E7F; Thai",
"0E80..0EFF; Lao",
"0F00..0FFF; Tibetan",
"1000..109F; Myanmar",
"10A0..10FF; Georgian",
"1100..11FF; Hangul Jamo",
"1200..137F; Ethiopic",
"13A0..13FF; Cherokee",
"1400..167F; Unified Canadian Aboriginal Syllabics",
"1680..169F; Ogham",
"16A0..16FF; Runic",
"1700..171F; Tagalog",
"1720..173F; Hanunoo",
"1740..175F; Buhid",
"1760..177F; Tagbanwa",
"1780..17FF; Khmer",
"1800..18AF; Mongolian",
"1900..194F; Limbu",
"1950..197F; Tai Le",
"19E0..19FF; Khmer Symbols",
"1D00..1D7F; Phonetic Extensions",
"1E00..1EFF; Latin Extended Additional",
"1F00..1FFF; Greek Extended",
"2000..206F; General Punctuation",
"2070..209F; Superscripts and Subscripts",
"20A0..20CF; Currency Symbols",
"20D0..20FF; Combining Diacritical Marks for Symbols",
"2100..214F; Letterlike Symbols",
"2150..218F; Number Forms",
"2190..21FF; Arrows",
"2200..22FF; Mathematical Operators",
"2300..23FF; Miscellaneous Technical",
"2400..243F; Control Pictures",
"2440..245F; Optical Character Recognition",
"2460..24FF; Enclosed Alphanumerics",
"2500..257F; Box Drawing",
"2580..259F; Block Elements",
"25A0..25FF; Geometric Shapes",
"2600..26FF; Miscellaneous Symbols",
"2700..27BF; Dingbats",
"27C0..27EF; Miscellaneous Mathematical Symbols-A",
"27F0..27FF; Supplemental Arrows-A",
"2800..28FF; Braille Patterns",
"2900..297F; Supplemental Arrows-B",
"2980..29FF; Miscellaneous Mathematical Symbols-B",
"2A00..2AFF; Supplemental Mathematical Operators",
"2B00..2BFF; Miscellaneous Symbols and Arrows",
"2E80..2EFF; CJK Radicals Supplement",
"2F00..2FDF; Kangxi Radicals",
"2FF0..2FFF; Ideographic Description Characters",
"3000..303F; CJK Symbols and Punctuation",
"3040..309F; Hiragana",
"30A0..30FF; Katakana",
"3100..312F; Bopomofo",
"3130..318F; Hangul Compatibility Jamo",
"3190..319F; Kanbun",
"31A0..31BF; Bopomofo Extended",
"31F0..31FF; Katakana Phonetic Extensions",
"3200..32FF; Enclosed CJK Letters and Months",
"3300..33FF; CJK Compatibility",
"3400..4DBF; CJK Unified Ideographs Extension A",
"4DC0..4DFF; Yijing Hexagram Symbols",
"4E00..9FFF; CJK Unified Ideographs",
"A000..A48F; Yi Syllables",
"A490..A4CF; Yi Radicals",
"AC00..D7AF; Hangul Syllables",
"E000..F8FF; Private Use Area",
"F900..FAFF; CJK Compatibility Ideographs",
"FB00..FB4F; Alphabetic Presentation Forms",
"FB50..FDFF; Arabic Presentation Forms-A",
"FE00..FE0F; Variation Selectors",
"FE20..FE2F; Combining Half Marks",
"FE30..FE4F; CJK Compatibility Forms",
"FE50..FE6F; Small Form Variants",
"FE70..FEFF; Arabic Presentation Forms-B",
"FF00..FFEF; Halfwidth and Fullwidth Forms",
"FFF0..FFFD; Specials"};

static char * favorites_default[23] = {
"20AC", "00A3", "00A5", "00A9", "00AE",
"2122", "00B1", "2260", "2264", "2265",
"00F7", "00D7", "221E", "00B5", "03B1",
"03B2", "03C0", "2126", "2211", "00A7",
"2026", "2105", "00E9"
};
#endif //UNICODE_RANGES_H
