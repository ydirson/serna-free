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
#include "common/PropertyTreeEventFactory.h"
#include "common/StringTokenizer.h"
#include "common/Vector.h"
#include <set>
#include <map>

#include <iostream>

namespace Common {

////////////////////////////////////////////////////////////////////////

typedef Vector<String> StringVector;
    
PropertyNodePattern::PropertyNodePattern(const String& pattern)
    : stringVector_(new StringVector)
{
    for (StringTokenizer tokenizer(pattern, "/"); tokenizer; )
        stringVector_->push_back(tokenizer.next());
}

PropertyNodePattern::PropertyNodePattern(const PropertyNodePattern& pattern)
    : stringVector_(new StringVector(*pattern.stringVector_.pointer()))
{
}

bool PropertyNodePattern::matches(const PropertyNode* prop) const
{
    for (int i = stringVector_->size() - 1; i >= 0; --i) {
        if (0 == prop)
            return false;
        const String& step = (*stringVector_.pointer())[i];
        unsigned const step_len = step.length();
        if (0 == step_len && !prop->parent())
            return true;
        if (prop->name() != step && !(1 == step_len && '*' == step[0]))
            return false;
        prop = prop->parent();
    }
    return true;
}

void PropertyNodePattern::dump() const
{
#ifndef NDEBUG
    for (uint i = 0; i < stringVector_->size(); i++) {
        std::cerr << (*stringVector_.pointer())[i];
        if (i != stringVector_->size() - 1)
            std::cerr << '/';
    }
    std::cerr << std::endl;
#endif // NDEBUG
}

/////////////////////////////////////////////////////////////////////////

class PatternData : public PropertyNodePattern {
public:
    PatternData(const String& pattern, EventMakerPtr p)
        : PropertyNodePattern(pattern),
          eventMaker_(p) {}
    PatternData(const PatternData& data)
        : PropertyNodePattern(data),
          eventMaker_(data.eventMaker_) {}

    EventMakerPtr eventMaker_;
};

class PropertyTreeEventFactory::PatternMap :
    public std::multimap<CommandEventContext*, PatternData> {};

class PropertyTreeEventFactory::PropertyNodeSet :
    public std::set<RefCntPtr<PropertyNode> > {};

////////////////////////////////////////////////////////////////////////

PropertyTreeEventFactory::PropertyTreeEventFactory()
    : root_(new PropertyNode()),
      patterns_(new PatternMap),
      modifiedNodes_(new PropertyNodeSet)
{
    root_->addWatcher(this);
}

PropertyTreeEventFactory::~PropertyTreeEventFactory()
{
}

void PropertyTreeEventFactory::subscribe(const String& pattern,
                                         EventMakerPtr eventMaker,
                                         CommandEventContext* subscriber)
{
    DynamicEventFactory::subscribe(eventMaker, subscriber);
    PatternMap::iterator ni = patterns_->insert(
        PatternMap::value_type(subscriber,
                               PatternData(pattern, eventMaker)));
}

void PropertyTreeEventFactory::propertyChanged(PropertyNode* property)
{
    //std::cerr << "property changed: " << property->name() << std::endl;
    modifiedNodes_->insert(property);
}

void PropertyTreeEventFactory::doUnsubscribe(CommandEventContext* subscriber)
{
    patterns_->erase(subscriber);
}

void PropertyTreeEventFactory::update()
{
    PropertyNodeSet::iterator i = modifiedNodes_->begin();
    PatternMap::const_iterator pmi;
    for (; i != modifiedNodes_->end(); ++i) {
        for (pmi = patterns_->begin(); pmi != patterns_->end(); ++pmi) {
            if (!pmi->second.matches(i->pointer()))
                continue;
            PropertyTreeChangeEventData ed;
            ed.propertyNode_ = i->pointer();
            CommandEventPtr event = (*pmi->second.eventMaker_)(&ed);
            event->execute(pmi->first, 0);
        }
    }
    clearUpdates();
}

void PropertyTreeEventFactory::dump()
{
#ifndef NDEBUG
    std::cerr << "PropertyTreeEventFactory::update()\n";
    std::cerr << "subscriptions:\n";
    for (PatternMap::const_iterator pmi = patterns_->begin();
         pmi != patterns_->end(); ++pmi)
        pmi->second.dump();
    std::cerr << "modifications:\n";

    for (PropertyNodeSet::iterator i = modifiedNodes_->begin();
         i != modifiedNodes_->end(); ++i)
        std::cerr << "    " << (*i)->name() << std::endl;
#endif // NDEBUG
}

void PropertyTreeEventFactory::clearUpdates()
{
    modifiedNodes_->clear();
}

}

