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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.
//
#ifndef UTILS_GROVE_POS_EVENT_DATA_
#define UTILS_GROVE_POS_EVENT_DATA_

#include "common/PropertyTreeEventData.h"
#include "groveeditor/GrovePos.h"

class GrovePosEventData : public PropertyTreeEventData {
public:
    GrovePosEventData(COMMON_NS::PropertyNode* n)
        : PropertyTreeEventData(n) {}
    GrovePosEventData(const GroveEditor::GrovePos& pos)
        : pos_(pos) {}
    GrovePosEventData() {}

    GroveEditor::GrovePos pos_;
};

#define GROVEPOS_EVENT_IMPL(eventName, contextType) \
class eventName : public COMMON_NS::CommandEvent<contextType> { \
public:                                                            \
    eventName(const COMMON_NS::PropertyNode* ed,               \
              const GroveEditor::GrovePos& gp)                     \
        : ed_(ed), pos_(gp) {}                                     \
    virtual bool doExecute(contextType*, COMMON_NS::EventData*);   \
private:                                                            \
    const COMMON_NS::PropertyNode*      ed_;                        \
    GroveEditor::GrovePos               pos_;                       \
};                                                                 \
COMMON_NS_BEGIN                                                    \
template<> CMD_MAKER_EXPORT COMMON_NS::CommandEventPtr             \
makeCommand<eventName>(const COMMON_NS::EventData* d)              \
{                                                                  \
    if (d) {                                                       \
        const GrovePosEventData* gd =                              \
            static_cast<const GrovePosEventData*>(d);              \
        return new eventName(gd->root(), gd->pos_);        \
    }                                                              \
    return new eventName(0, GroveEditor::GrovePos());              \
} \
COMMON_NS_END

#endif // UTILS_GROVE_POS_EVENT_DATA_
