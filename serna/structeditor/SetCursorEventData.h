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
#ifndef _STRUCTEDITOR_SET_CURSOR_EVENT_DATA_H_
#define _STRUCTEDITOR_SET_CURSOR_EVENT_DATA_H_

#include "structeditor/se_defs.h"
#include "common/CommandEvent.h"
#include "common/String.h"
#include "formatter/AreaPos.h"
#include "groveeditor/GrovePos.h"

class STRUCTEDITOR_EXPIMP SelectionEventData : public COMMON_NS::EventData {
public:
    SelectionEventData(const Formatter::ChainSelection& chainSel,
                       const GroveEditor::GroveSelection& groveSel)
        : chainSelection_(chainSel), 
          groveSelection_(groveSel), 
          showTop_(false) {}

    Formatter::ChainSelection   chainSelection_;
    GroveEditor::GroveSelection groveSelection_;
    bool                        showTop_;
};

#define SELECTION_EVENT_IMPL(eventName, contextType) \
class eventName : public COMMON_NS::CommandEvent<contextType> { \
public:                                                            \
    eventName(const Formatter::ChainSelection& chainSel,           \
              const GroveEditor::GroveSelection& groveSel)         \
        : chainSelection_(chainSel),                               \
          groveSelection_(groveSel) {}                             \
    virtual bool doExecute(contextType*, COMMON_NS::EventData*);   \
private:                                                            \
    Formatter::ChainSelection   chainSelection_;                   \
    GroveEditor::GroveSelection groveSelection_;                   \
};                                                                 \
COMMON_NS_BEGIN                                                    \
template<> CMD_MAKER_EXPORT COMMON_NS::CommandEventPtr                \
makeCommand<eventName>(const COMMON_NS::EventData* d)              \
{                                                                  \
    if (d) {                                                       \
        const SelectionEventData* sd =                              \
            static_cast<const SelectionEventData*>(d);              \
        return new eventName(sd->chainSelection_, sd->groveSelection_); \
    }                                                               \
    return new eventName(Formatter::ChainSelection(),               \
                         GroveEditor::GroveSelection());            \
} \
COMMON_NS_END

#endif // _STRUCTEDITOR_SET_CURSOR_EVENT_DATA_H_

