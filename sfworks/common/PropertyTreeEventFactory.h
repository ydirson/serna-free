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
#ifndef _COMMON_PROPERTY_TREE_EVENT_FACTORY_H_
#define _COMMON_PROPERTY_TREE_EVENT_FACTORY_H_

#include "common/common_defs.h"
#include "common/CommandEvent.h"
#include "common/OwnerPtr.h"
#include "common/PropertyTree.h"

namespace Common {

/////////////////////////////////////////////////////////////////////////

class COMMON_EXPIMP PropertyNodePattern {
public:
    PropertyNodePattern(const String& pattern);
    PropertyNodePattern(const PropertyNodePattern& pattern);
    virtual ~PropertyNodePattern() {};
    
    bool            matches(const PropertyNode* prop) const;
    virtual void    dump() const;

private:
    Common::OwnerPtr<Vector<String> >   stringVector_;
};

// PropertyTreeEventFactory generates an event specified by EventMaker
// and dispatches it to appropriate subscribers when property tree gets
// modified. To dispatch pending events, update() must be called explicitly.
//
class COMMON_EXPIMP PropertyTreeEventFactory : private DynamicEventFactory,
                                               private PropertyNodeWatcher{
public:
    PropertyTreeEventFactory();
    virtual ~PropertyTreeEventFactory();

    /// Holds the root of the property tree
    PropertyNode*               root() const { return root_.pointer(); }

    /// Register the notification subscriber for the property tree.
    /// Pattern must consist of slash-separated property names; it is
    /// matched right-to-left when walking property tree upwards.
    void                        subscribe(const String& pattern,
                                          EventMakerPtr eventMaker,
                                          CommandEventContext* subscriber);

    using DynamicEventFactory::unsubscribe;

    /// Propagate changes (dispatch events)
    void                        update();

    /// Clear pending update events
    void                        clearUpdates();

    void                        dump();

private:
    class PatternMap;       // map of match patterns
    class PropertyNodeSet;  // set of property nodes

    virtual void                doUnsubscribe(CommandEventContext*);
    virtual void                propertyChanged(PropertyNode* property);

private:
    PropertyNodePtr             root_;
    OwnerPtr<PatternMap>        patterns_;
    OwnerPtr<PropertyNodeSet>   modifiedNodes_;
};

//////////////////////////////////////////////////////////////////////////

class PropertyTreeChangeEventData : public EventData {
public:
    const PropertyNode* propertyNode_;
};

}

#endif // _COMMON_PROPERTY_TREE_EVENT_FACTORY_H_

