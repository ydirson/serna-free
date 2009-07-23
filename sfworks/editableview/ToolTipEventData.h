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

#ifndef EDITABLEVIEW_TOOLTIP_EVENT_DATA_
#define EDITABLEVIEW_TOOLTIP_EVENT_DATA_

#include "common/CommandEvent.h"
#include "editableview/TooltipView.h"
#include "groveeditor/GrovePos.h"

#include <QRect>

class QWidget;

class ToolTipEventData : public Common::EventData {
public:
    ToolTipEventData(const GroveEditor::GrovePos& foPos,
                     TooltipView::RegionType t,
                     QWidget* widget,
                     const QRect& rect)
        : foPos_(foPos), type_(t), widget_(widget), rect_(rect) {}

    const GroveEditor::GrovePos&    foPos() const { return foPos_; }
    const TooltipView::RegionType   type() const { return type_; }
    QWidget*                        widget() const { return widget_; }
    const QRect&                    rect() const { return rect_; }

private:
    GroveEditor::GrovePos   foPos_;
    TooltipView::RegionType type_;
    QWidget*                widget_;
    QRect                   rect_;
};

#define TOOLTIP_EVENT_IMPL(eventName, contextType)                 \
class eventName : public Common::CommandEvent<contextType> {       \
public:                                                            \
    eventName(const ToolTipEventData& ed)                          \
        : ed_(ed) {}                                               \
    virtual bool doExecute(contextType*, Common::EventData* ed);   \
private:                                                           \
    const ToolTipEventData& ed_;                                   \
};                                                                 \
namespace Common {                                                 \
template<> CMD_MAKER_EXPORT Common::CommandEventPtr                \
makeCommand<eventName>(const Common::EventData* d)                 \
{                                                                  \
    return new eventName(static_cast<const ToolTipEventData&>(*d));\
} \
} // namespace Common

#endif // EDITABLEVIEW_TOOLTIP_EVENT_DATA_
