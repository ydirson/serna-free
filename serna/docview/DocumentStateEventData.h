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
#ifndef DOCVIEW_DOCUMENT_STATE_EVENT_DATA_H_
#define DOCVIEW_DOCUMENT_STATE_EVENT_DATA_H_

#include "docview/dv_defs.h"
#include "common/CommandEvent.h"
#include "common/PropertyTree.h"

class SernaDoc;

class DOCVIEW_EXPIMP DocumentStateEventData : public Common::EventData {
public:
    enum State {
        NEW_DOCUMENT_GROVE,             //! when the new document is created
        BEFORE_TRANSFORM,               //! just before XSLT transform is made
        BUILD_INTERFACE,                //! building interface stage
        POST_INIT,                      //! post-initialization
        REMOVE_DOC_SPECIFIC_FILES,      //! for removing temporary files
        SAVE_PROPERTIES,
        RESTORE_PROPERTIES,
        GET_PROP_TREE_SUFFIXES,
        ABOUT_TO_SAVE,                  //! called immeditately before save
        DOCUMENT_SAVED,                 //! called after successful save
        PRE_CLOSE                       //! called on close attempt
    };
    DocumentStateEventData(State state, SernaDoc* sernaDoc = 0)
        : state_(state), sernaDoc_(sernaDoc) {}

    State                   state_;
    SernaDoc*               sernaDoc_;
    Common::PropertyNodePtr prop_;
};

#endif // DOCVIEW_DOCUMENT_STATE_EVENT_DATA_H_
