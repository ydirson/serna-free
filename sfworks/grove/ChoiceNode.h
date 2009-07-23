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

#ifndef GROVE_CHOICE_NODE_H_
#define GROVE_CHOICE_NODE_H_

#include "grove/grove_defs.h"
#include "grove/Nodes.h"
#include "common/PropertyTree.h"
#include "common/CDList.h"

class XsNodeExt;

GROVE_NAMESPACE_BEGIN

class GROVE_EXPIMP ChoiceNode : public Element,
                                public Common::CDListItem<ChoiceNode> {
public:
    GROVE_OALLOC(ChoiceNode);
    ChoiceNode()
        : Element("#choice")
    {
        setNodeType(CHOICE_NODE);
    }
    virtual ~ChoiceNode() {}

    Common::PropertyNode* elemList() const { return elemList_.root(); }
    void            setElemList(Common::PropertyNode*);
    virtual void    dumpInherited() const;

    virtual void    saveAsXml(GroveSaverContext&, int) const;

private:
    friend class ::XsNodeExt;
    virtual Node*   copy(Node*) const;

    Common::PropertyTree elemList_;
};

typedef Common::CDList<ChoiceNode> ChoiceNodeList;

class GROVE_EXPIMP TextChoiceNode : public Text {
public:
    enum Type {
        SIMPLE_CHOICE,
        ENUM_CHOICE
    };
    TextChoiceNode(Type type)
        : type_(type), isChoice_(false) {}
    virtual ~TextChoiceNode() {}

    bool                  isChoice() const { return isChoice_; }
    void                  setChoice(bool v) { isChoice_ = v; }
    virtual void          dumpInherited() const {}
    virtual const String& nodeName() const;
    GROVE_OALLOC(TextChoiceNode);

private:
    friend class ::XsNodeExt;
    virtual Node*   copy(Node*) const;
    char            type_;
    bool            isChoice_;
};

GROVE_EXPIMP bool is_text_choice(const Text*);
GROVE_EXPIMP void set_text_choice(Text*, bool v);

GROVE_NAMESPACE_END

#endif // GROVE_CHOICE_NODE_H_
