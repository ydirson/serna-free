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
#ifndef SAPI_STRUCT_EDITOR_DATA_H_
#define SAPI_STRUCT_EDITOR_DATA_H_

#include "sapi/app/StructEditor.h"
#include "sapi/common/SimpleWatcher.h"
#include "common/CommandEvent.h"
#include "structeditor/StructEditor.h"

namespace SernaApi {
///  \internal This class is for internal use only.

/*!
  \internal
This class is for internal use only.
*/
class StructEditorData : public ::Common::RefCounted<>, 
                         public ::Common::CommandEventContext {
public:
    StructEditorData(::StructEditor* se)
        : posWatcher_(0), 
          selectionWatcher_(0), 
          doubleClickWatcher_(0), 
          tripleClickWatcher_(0), 
          se_(se) {}
    void    positionChanged() 
    {
        if (posWatcher_)
            posWatcher_->notifyChanged();
    }
    void    selectionChanged()
    {
        if (selectionWatcher_)
            selectionWatcher_->notifyChanged();
    }
    bool    doubleClick()
    {
        if (doubleClickWatcher_)
            return doubleClickWatcher_->notifyChanged();
        return true;
    }
    bool    tripleClick()
    {
        if (tripleClickWatcher_)
            return tripleClickWatcher_->notifyChanged();
        return true;
    }
    void    setPosWatcher(SimpleWatcher* spw);
    void    setSelectionWatcher(SimpleWatcher* spw);
    void    setDoubleClickWatcher(SimpleWatcher* spw);
    void    setTripleClickWatcher(SimpleWatcher* spw);

private:
    virtual const char* selfTypeId() const
    {
        return typeid(StructEditorData).name();
    }
    SimpleWatcher* posWatcher_;
    SimpleWatcher* selectionWatcher_;
    SimpleWatcher* doubleClickWatcher_;
    SimpleWatcher* tripleClickWatcher_;
    ::StructEditor*          se_;
};

} // namespace SernaApi

#endif // SAPI_STRUCT_EDITOR_DATA_H_
