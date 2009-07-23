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
#include "sapi/sapi_defs.h"

#ifndef SAPI_COMMON_CHAR_H_
#define SAPI_COMMON_CHAR_H_

class QChar;

namespace SernaApi {

class SAPI_EXPIMP Char {
public:
    /// Constructs a null Char (one that isNull()).
    Char() : ucs_(0) {}
    /// Constructs a Char corresponding to ASCII/Latin-1 character \a c.
    Char(char c) : ucs_(c) {}
    /// Constructs a QChar corresponding to ASCII/Latin-1 character \a c.
    Char(unsigned char c) : ucs_(c) {}
    /// Constructs a QChar for Unicode cell \a c in row \a r.
    Char(unsigned char col, unsigned char row) : ucs_((row << 8) | col) {}
    /// Constructs a QChar for the character with Unicode code point \a rc.
    Char(unsigned short c) : ucs_(c) {}
    /// Constructs a QChar for the character with Unicode code point \a rc.
    Char(short c) : ucs_((unsigned short) c) {}
    /// Constructs a QChar for the character with Unicode code point \a rc.
    Char(int c) : ucs_(c & 0xFFFF) {}
    /// Constructs a QChar for the character with Unicode code point \a rc.
    Char(unsigned int c) : ucs_(c & 0xFFFF) {}
    /*! Constructs a copy of \a c. This is a deep copy, if such a 
        lightweight object can be said to have deep copies.*/
    Char(const QChar&);

    static const Char null;
    static const Char replacement;
    static const Char byteOrderMark;
    static const Char byteOrderSwapped;
    static const Char nbsp;

    enum SpecialCharacter {
        Null = 0x0000,
        Nbsp = 0x00a0,
        ReplacementCharacter = 0xfffd,
        ObjectReplacementCharacter = 0xfffc,
        ByteOrderMark = 0xfeff,
        ByteOrderSwapped = 0xfffe,
        ParagraphSeparator = 0x2029,
        LineSeparator = 0x2028
    };
    Char(SpecialCharacter sc) : ucs_(sc) {}

    /// This enum maps the Unicode character categories.          
    enum Category
    {
        NoCategory,

        Mark_NonSpacing,          //   Mn
        Mark_SpacingCombining,    //   Mc
        Mark_Enclosing,           //   Me

        Number_DecimalDigit,      //   Nd
        Number_Letter,            //   Nl
        Number_Other,             //   No

        Separator_Space,          //   Zs
        Separator_Line,           //   Zl
        Separator_Paragraph,      //   Zp

        Other_Control,            //   Cc
        Other_Format,             //   Cf
        Other_Surrogate,          //   Cs
        Other_PrivateUse,         //   Co
        Other_NotAssigned,        //   Cn

        Letter_Uppercase,         //   Lu
        Letter_Lowercase,         //   Ll
        Letter_Titlecase,         //   Lt
        Letter_Modifier,          //   Lm
        Letter_Other,             //   Lo

        Punctuation_Connector,    //   Pc
        Punctuation_Dash,         //   Pd
        Punctuation_Dask = Punctuation_Dash, // oops
        Punctuation_Open,         //   Ps
        Punctuation_Close,        //   Pe
        Punctuation_InitialQuote, //   Pi
        Punctuation_FinalQuote,   //   Pf
        Punctuation_Other,        //   Po

        Symbol_Math,              //   Sm
        Symbol_Currency,          //   Sc
        Symbol_Modifier,          //   Sk
        Symbol_Other              //   So
    };
    ///  Returns the character category.
    Category category() const;

    ///  This enum type defines the Unicode direction attributes.
    enum Direction
    {
        DirL, DirR, DirEN, DirES, DirET, DirAN, DirCS, DirB, DirS, DirWS, DirON,
        DirLRE, DirLRO, DirAL, DirRLE, DirRLO, DirPDF, DirNSM, DirBN
    };
    /*!
        Returns the character's direction.
    */
    Direction direction() const;

    /// This enum type defines the Unicode decomposition attributes.
    enum Decomposition
    {
        Single, Canonical, Font, NoBreak, Initial, Medial,
        Final, Isolated, Circle, Super, Sub, Vertical,
        Wide, Narrow, Small, Square, Compat, Fraction
    };

    /*!
        Decomposes a character into its parts. Returns QString::null if no
        decomposition exists.
    */
    Decomposition decompositionTag() const;

    /// This enum type defines the Unicode joining attributes.
    enum Joining
    {
        OtherJoining, Dual, Right, Center
    };

    /*!
        Returns information about the joining properties of the character
        (needed for example, for Arabic).
    */
    Joining joining() const;

    /// This enum type defines names for some of the Unicode combining classes.
    enum CombiningClass
    {
        Combining_BelowLeftAttached       = 200,
        Combining_BelowAttached           = 202,
        Combining_BelowRightAttached      = 204,
        Combining_LeftAttached            = 208,
        Combining_RightAttached           = 210,
        Combining_AboveLeftAttached       = 212,
        Combining_AboveAttached           = 214,
        Combining_AboveRightAttached      = 216,

        Combining_BelowLeft               = 218,
        Combining_Below                   = 220,
        Combining_BelowRight              = 222,
        Combining_Left                    = 224,
        Combining_Right                   = 226,
        Combining_AboveLeft               = 228,
        Combining_Above                   = 230,
        Combining_AboveRight              = 232,

        Combining_DoubleBelow             = 233,
        Combining_DoubleAbove             = 234,
        Combining_IotaSubscript           = 240
    };

    /*!
        Returns the combining class for the character as defined in the
        Unicode standard. This is mainly useful as a positioning hint for
        marks attached to a base character.
    */
    unsigned char combiningClass() const;

    ///////////////////////////////////////////////////////////////

    /// Returns the numeric value of the digit or -1 if the character is not 
    /// a digit.
    int         digitValue() const;

    /// Returns the lowercase equivalent when the character is uppercase. 
    /// Otherwise returns the character itself.
    Char        lower() const;

    /// Returns the uppercase equivalent when the character is lowercase. 
    /// Otherwise returns the character itself.
    Char        upper() const;

    /// Returns TRUE if the character is a mirrored character (should be 
    /// reversed if the text direction is reversed).
    bool        mirrored() const;

    /// Returns the mirrored character if this character is a mirrored character. 
    /// Otherwise returns the character itself.
    Char        mirroredChar() const;

    /// Returns the Latin-1 value of this character, or 0 if it cannot be 
    /// represented in Latin-1.
    unsigned char latin1() const { return ucs_ & 0xFF; }

    /// Returns the numeric Unicode value.
    unsigned short unicode() const { return ucs_; }

    /// Same as latin1()
    operator unsigned char() const { return latin1(); }

    /// Same as latin1()
    operator char() const { return latin1(); }

    /// Returns TRUE if the character is the Unicode character 
    /// 0x0000 (ASCII NUL). Otherwise returns FALSE.
    bool isNull() const { return !ucs_; }

    /// Returns TRUE if the character is a printable character. 
    /// Otherwise returns FALSE.
    bool isPrint() const;

    /// Returns TRUE if the character is a punctuation mark.
    bool isPunct() const;

    /// Returns TRUE if the character is a separator character.
    bool isSpace() const;

    /// Returns TRUE if the character is a mark.
    bool isMark() const;

    /// Returns TRUE if the character is a letter.
    bool isLetter() const;

    /// Returns TRUE if the character is a number.
    bool isNumber() const;

    /// Returns TRUE if the character is a letter or number.
    bool isLetterOrNumber() const;

    /// Returns TRUE if the character is a decimal digit.
    bool isDigit() const;

    /// Returns TRUE if the character is a symbol (Symbol_* categories).
    bool isSymbol() const;

    /// Returns the cell (least significant byte) of the Unicode character.
    unsigned char cell() const { return ((unsigned char) ucs_ & 0xff); }

    /// Returns the row (most significant byte) of the Unicode character.
    unsigned char row() const { return ((unsigned char) (ucs_ >> 8) & 0xff); }

    ///  Returns TRUE if the compared are the same Unicode characte
    bool operator==(Char ch) const { return ch.ucs_ == ucs_; }

    /// Returns TRUE if the compared are not the same Unicode character.
    bool operator!=(Char ch) const { return !(*this == ch); }

    /// Returns TRUE if the numeric Unicode value of the left is less than 
    /// that of the right or they are the same Unicode character
    bool operator<=(Char ch) const { return ucs_ <= ch.ucs_; }

    /// Returns TRUE if the numeric Unicode value of the left is greater than 
    /// that of the right or they are the same Unicode character
    bool operator>=(Char ch) const { return ucs_ >= ch.ucs_; }

    /// Returns TRUE if the numeric Unicode value of the left is less than 
    /// that of the right.
    bool operator<(Char ch) const { return ucs_ < ch.ucs_; }

    /// Returns TRUE if the numeric Unicode value of the left is greater than 
    /// that of the right.
    bool operator>(Char ch) const { return ucs_ > ch.ucs_; }

    /// A conversion to QChar.
    operator QChar() const;

private:
    unsigned short ucs_;
};

} // namespace SernaApi

#endif // SAPI_COMMON_CHAR_H_

