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
#ifndef GROVE_ORIGIN_H_
#define GROVE_ORIGIN_H_

#include "grove/grove_defs.h"
#include "grove/Node.h"
#include "common/RefCntPtr.h"
#include "common/Message.h"
#include "common/SernaApiRefCounted.h"

namespace GroveLib {

class GROVE_EXPIMP OriginBase : public Common::SernaApiRefCounted {
public:
    virtual const Node*     node() const = 0;
    virtual Common::String  fileName() const = 0;
    virtual int             line() const = 0;
    virtual int             column() const = 0;
    virtual Common::String  asString() const = 0;

    virtual ~OriginBase() {}
};

class GROVE_EXPIMP NodeOrigin : public OriginBase {
public:
    NodeOrigin(const Node* node)
        : node_(node) {}

    virtual const Node*     node() const { return node_.pointer(); }
    virtual Common::String  fileName() const;
    virtual int             line() const;
    virtual int             column() const;
    virtual Common::String  asString() const;

    virtual ~NodeOrigin() {}

private:
    Common::RefCntPtr<const Node> node_;
};

class GROVE_EXPIMP PlainOrigin : public OriginBase {
public:
    PlainOrigin(const Common::String& filename,
                int line = -1, int col = -1)
        : fileName_(filename), line_(line), column_(col) {}

    virtual const Node*     node() const { return 0; }
    virtual Common::String  fileName() const { return fileName_; }
    virtual int             line() const { return line_; }
    virtual int             column() const { return column_; }
    virtual Common::String  asString() const;
    virtual ~PlainOrigin() {}

private:
    Common::String  fileName_;
    int             line_;
    int             column_;
};

class GROVE_EXPIMP CompositeOrigin {
public:
    enum { messageArgType = COMMON_NS::MessageArgBase::USER_DEFINED + 1 };

    CompositeOrigin(OriginBase* docOrigin, OriginBase* modOrigin = 0)
        : docOrigin_(docOrigin), moduleOrigin_(modOrigin) {}

    const OriginBase* moduleOrigin() const { return moduleOrigin_.pointer(); }
    const OriginBase* docOrigin() const { return docOrigin_.pointer(); }

    virtual ~CompositeOrigin() {}

private:
    Common::RefCntPtr<OriginBase> docOrigin_;
    Common::RefCntPtr<OriginBase> moduleOrigin_;
};

GROVE_EXPIMP Common::String nodePosToString(const Node*, int textIdx = -1);
GROVE_EXPIMP
    const CompositeOrigin* getCompositeOrigin(const Common::Message* m);

} // namespace GroveLib

#endif // GROVE_ORIGIN_H_
