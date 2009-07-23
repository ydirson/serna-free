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
#ifndef SAPI_MESSAGE_VIEW_H_
#define SAPI_MESSAGE_VIEW_H_

#include "sapi/sapi_defs.h"
#include "sapi/common/SString.h"
#include "sapi/app/UiItem.h"

namespace SernaApi {

class DocumentPlugin;
class GroveNode;

/*! Interface to the Serna message window */
class SAPI_EXPIMP MessageView : public UiItem {
public:
    MessageView(SernaApiBase*, DocumentPlugin*);
    virtual ~MessageView();

    /// Specifies message facilitity (module of origin)
    enum MessageFacility {
        ALL_MSGS         = -1,  ///! All messages
        GROVE_MSGS       = 1,   ///! XML parser and grove messages    
        FORMATTER_MSGS   = 3,   ///! XSL-FO Formatter messages
        VALIDATOR_MSGS   = 6,   ///! XML Validator messages
        SERNA_MSGS       = 10   ///! Serna messages
    };
    /// Emit message to the Serna message window (at the bottom)
    void            emitMessage(const SString& msg,
                                const GroveNode* context = 0);
    void            clearMessages(int facility = SERNA_MSGS);
    
    // for internal use
    MessageView(const MessageView&);
    MessageView& operator=(const MessageView&);
    virtual void setRep(SernaApiBase*);

private:
    DocumentPlugin* dp_;
};

} // namespace SernaApi

#endif // SAPI_MESSAGE_VIEW_H_
