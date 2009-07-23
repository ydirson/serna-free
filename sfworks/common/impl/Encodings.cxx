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
#include "common/Encodings.h"
#include "common/String.h"

namespace {
    const char* names[] = {
        "ISO-8859-1",   "ISO-8859-2",   "ISO-8859-5",   "ISO-8859-6",
        "ISO-8859-7",   "ISO-8859-9",   "ISO-8859-15",   "KOI8-R",  
        "UTF-8", 
        "UTF-16",       "EUC-JP",       "EUC-KR",       "JIS",
        "shift_jis",    "windows-1250", "windows-1251", "windows-1252",
        "windows-1253", "windows-1254", "windows-1255", "windows-1256",
        "windows-1257", "windows-1258", "XML"           // XML
    };
    const char* qt_names[] = {
        "ISO 8859-1",   "ISO 8859-2",   "ISO 8859-5",   "ISO 8859-6",
        "ISO 8859-7",   "ISO 8859-9",   "ISO 8859-15",  "KOI8-R",   
        "UTF-8",
        "UTF-16",       "eucJP",        "eucKR",        "JIS7",
        "Shift-JIS",    "CP 1250",      "CP 1251",      "CP 1252",
        "CP 1253",      "CP 1254",      "CP 1255",      "CP 1256",
        "CP 1257",      "CP 1258",      "UTF-8"           // XML
    };
    static const int encodings_size = sizeof(names)/sizeof(char*);
};

namespace Common {

const String Encodings::encodingName(const Encoding e)
{
    return names[e];
}

const String Encodings::qtEncodingName(const Encoding e)
{
    return qt_names[e];
}

Encodings::Encoding Encodings::encodingByName(const String& n)
{
    String enc = n.lower();
    for (unsigned int i = FIRST_ENCODING; i < LAST_ENCODING; ++i)
        if (enc == String(names[i]).lower())
            return (Encoding) i;
    return XML;
}

} //  namespace Common

