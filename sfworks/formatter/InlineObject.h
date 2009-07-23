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
#ifndef FORMATTER_INLINE_OBJECT_H_
#define FORMATTER_INLINE_OBJECT_H_

#include "formatter/formatter_defs.h"
#include "formatter/types.h"
#include "common/String.h"

namespace GroveLib {
    class Node;
}

namespace Formatter {

class FORMATTER_EXPIMP InlineObject {
public:
    FORMATTER_OALLOC(InlineObject)
    
    virtual CRange size() const = 0;
    virtual ~InlineObject() {}
};

class InlineObjectData {
public:
    InlineObjectData(const GroveLib::Node* origin,
                     const Rgb& contColor,
                     const Common::String& fontFamily,
                     const CType& fontSize) 
        : origin_(origin), contColor_(contColor), 
          fontFamily_(fontFamily), fontSize_(fontSize) {}
    const GroveLib::Node* origin_;
    Rgb                   contColor_;
    Common::String        fontFamily_;
    CType                 fontSize_;
};    

class FORMATTER_EXPIMP InlineObjectFactory {
public:
    virtual InlineObject* make(const InlineObjectData&) const = 0;
    virtual ~InlineObjectFactory() {}
};

} // namespace Formatter

#endif // FORMATTER_INLINE_OBJECT_H_
