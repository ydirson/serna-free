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
#ifndef DOCVIEW_FINDER_H_
#define DOCVIEW_FINDER_H_

#include "docview/dv_defs.h"
#include "ui/LiquidItem.h"

#include "common/CommandEvent.h"
#include "common/OwnerPtr.h"
#include "common/PropertyTree.h"

#include "utils/SernaUiItems.h"

namespace Find
{

DOCVIEW_EXPIMP extern const char* const FIND_TEXT;
DOCVIEW_EXPIMP extern const char* const REPLACE_TEXT;
DOCVIEW_EXPIMP extern const char* const MATCH_CASE;
DOCVIEW_EXPIMP extern const char* const SEARCH_BACKWARDS;
DOCVIEW_EXPIMP extern const char* const SEARCH_TYPE;
DOCVIEW_EXPIMP extern const char* const REPLACE_MODE;

DOCVIEW_EXPIMP extern const char* const TEXT_TYPE;
DOCVIEW_EXPIMP extern const char* const ATTRVAL_TYPE;
DOCVIEW_EXPIMP extern const char* const ATTRNAME_TYPE;
DOCVIEW_EXPIMP extern const char* const COMMENT_TYPE;
DOCVIEW_EXPIMP extern const char* const PATTERN_TYPE;

}

class DOCVIEW_EXPIMP Finder : public Sui::LiquidItem {
public:
    Finder(PropertyNode* properties, bool isStruct);
    virtual ~Finder();

    static Finder*      findInstance(const Sui::Item* from);

    virtual String      itemClass() const { return Sui::FINDER; }

    void                setReplaceTab(bool isReplace);

    bool                find();
    bool                replace();
    bool                replaceAll();

    Common::PropertyNode* findProps() const { return findProps_.pointer(); }

    virtual bool        hasSelection() const = 0;

protected:
    virtual bool        doFind(bool startOver = false) = 0;
    virtual bool        replaceSelection() = 0;

    QWidget*            makeWidget(QWidget* parent, Type type);
    bool                showNotFound(bool) const;
    bool                showSearchFinished(bool) const;
    void                showChanged(int n);

    virtual const char* selfTypeId() const { return typeid(Finder).name(); }

protected:
    Common::PropertyNodePtr findProps_;
    bool                    isStruct_;
};

#endif // DOCVIEW_FINDER_H_
