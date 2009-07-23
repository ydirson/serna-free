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
#ifndef _DOCVIEW_GO_TO_ORIGIN_EVENT_DATA_H_
#define _DOCVIEW_GO_TO_ORIGIN_EVENT_DATA_H_

#include "docview/dv_defs.h"
#include "common/CommandEvent.h"
#include "grove/Origin.h"

class DOCVIEW_EXPIMP GoToOriginEventData : public COMMON_NS::EventData {
public:
    GoToOriginEventData(const GroveLib::CompositeOrigin& o)
        : origin_(o) {}

    GroveLib::CompositeOrigin origin_;
};

#define GOTO_ORIGIN_EVENT_IMPL(eventName, contextType) \
class COMMON_EXPORT eventName : public COMMON_NS::CommandEvent<contextType> { \
public:                                                            \
    eventName(const GroveLib::CompositeOrigin& o)                  \
        : origin_(o) {}                                            \
    virtual bool doExecute(contextType*, COMMON_NS::EventData*);   \
private:                                                           \
    GroveLib::CompositeOrigin origin_;                             \
};                                                                 \
COMMON_NS_BEGIN                                                    \
template<> COMMON_EXPORT COMMON_NS::CommandEventPtr                \
makeCommand<eventName>(const COMMON_NS::EventData* d)              \
{                                                                  \
    if (d)                                                         \
        return new eventName(static_cast                           \
            <const GoToOriginEventData*>(d)->origin_);             \
    return new eventName(0);                                       \
} \
COMMON_NS_END


#endif // _DOCVIEW_GO_TO_ORIGIN_EVENT_DATA_H_

