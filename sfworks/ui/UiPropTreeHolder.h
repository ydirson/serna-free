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
#ifndef UI_PROP_TREE_HOLDER_H_
#define UI_PROP_TREE_HOLDER_H_

#include "ui/ui_defs.h"
#include "common/PropertyTree.h"

class QAction;

namespace Sui {

UI_EXPIMP Common::String get_translated(const char* context,
                                        const Common::String& str);

class UI_EXPIMP PropertyTreeHolder {
public:
    typedef Common::String          String;
    typedef Common::PropertyNode    PropertyNode;

    PropertyTreeHolder(PropertyNode*);
    virtual ~PropertyTreeHolder();

    // can be reimplemented for calling property updates
    PropertyNode*           properties() const { return properties_.pointer(); }
    virtual PropertyNode*   property(const String& name) const;

    const String&           get(const String& propName) const;
    String                  getTranslated(const String& propName) const;
    String                  getTranslated(const PropertyNode* prop) const;

    bool                    getBool(const String& propName) const;
    int                     getInt(const String& propName) const;
    double                  getDouble(const String& propName) const;

    void                    set(const String& propName, const String& value);
    void                    setBool(const String& propName, bool value);
    void                    setInt(const String& propName, int value);
    void                    setDouble(const String& propName, double value);
    virtual String          translationContext() const = 0;

    void                    setupSynchers(Item* item);

protected:
    void                    installSyncher(PropertyNode* prop);

private:
    class SyncherList;

    PropertyTreeHolder(const PropertyTreeHolder&);
    PropertyTreeHolder& operator=(const PropertyTreeHolder&);

    Common::OwnerPtr<SyncherList>   syncherList_;
    Common::PropertyNodePtr         properties_;
};
    
} // namespace Sui

#endif // UI_PROP_TREE_HOLDER_H_
