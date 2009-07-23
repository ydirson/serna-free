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
/*! \file
 */

#ifndef COMMON_ENCODINGS_H
#define COMMON_ENCODINGS_H

#include "common/common_defs.h"
#include "common/StringDecl.h"

namespace Common {

/*! This class defines mapping between internal names of encodings and
    encoding names used elsewhere (currently Qt and SP).
    By default, we use SP names.
 */
class COMMON_EXPIMP Encodings {
public:
    enum Encoding {
        FIRST_ENCODING,
        ISO_8859_1 = FIRST_ENCODING,
        ISO_8859_2, ISO_8859_5,ISO_8859_6, ISO_8859_7,  ISO_8859_9,
        ISO_8859_15, KOI8_R, UTF_8, UTF_16, EUC_JP, EUC_KR, JIS, SJIS,
        CP_1250, CP_1251, CP_1252, CP_1253, CP_1254, CP_1255,
        CP_1256, CP_1257, CP_1258, XML,
        LAST_ENCODING
    };
    /*! Return encoding name by encoding ID */
    static const COMMON_NS::String encodingName(const Encoding);

    /*! Obtain encoding Id by it's name */
    static Encoding  encodingByName(const COMMON_NS::String&);

    /*! Qt: get Qt encoding name */
    static const COMMON_NS::String qtEncodingName(const Encoding);
};

} // namespace Common

#endif // COMMON_ENCODINGS_H
