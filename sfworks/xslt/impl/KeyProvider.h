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

#ifndef XSLT_KEY_PROVIDER_H
#define XSLT_KEY_PROVIDER_H

#include "xslt/xslt_defs.h"
#include "common/String.h"
#include "common/SubscriberPtr.h"
#include <list>

namespace GroveLib
{
    class Node;
}

namespace Xslt {

class KeyMap {
public:
    XSLT_OALLOC(KeyMap);
    //!
    void        addNode(GroveLib::Node*, const COMMON_NS::String&) {};
    //!
    void        removeNode(GroveLib::Node*) {};
    //!
    void        getNodes(const COMMON_NS::String&, std::list<GroveLib::Node*>&) const {};
    //!
    void        dump() const;
};

/*! \brief KeyProvider
 */
class KeyProvider : public COMMON_NS::RefCounted<>,
                                public COMMON_NS::SubscriberPtrPublisher {
public:
    XSLT_OALLOC(KeyProvider);
    virtual ~KeyProvider() {};
    //!
    void        addNode(const COMMON_NS::String& name,
                        GroveLib::Node* node,
                        const COMMON_NS::String& value);
    //!
    void        removeNode(GroveLib::Node*) {};
    //!
    void        getNodes(const COMMON_NS::String& name,
                         const COMMON_NS::String& value,
                         std::list<GroveLib::Node*>& nodes) const;
    //!
    void        dump() const;
protected:
    KeyMap*     getMap(const COMMON_NS::String&) const {return 0;}
};

} // namespace Xslt

#endif // XSLT_KEY_PROVIDER_H
