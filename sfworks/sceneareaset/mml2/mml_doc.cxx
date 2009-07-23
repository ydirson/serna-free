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
 #include "common/XTreeNode.h"
#include "common/RefCntPtr.h"
#include "common/RefCounted.h"

#include "grove/Nodes.h"

class MmlNode;
typedef Common::RefCntPtr<MmlNode> MmlNodePtr;

#include "qtmmlwidget.cxx"

static bool is_empty_text(const GroveLib::Node* node)
{
    return (node->nodeType() == GroveLib::Node::TEXT_NODE &&
        CONST_TEXT_CAST(node)->data().simplifyWhiteSpace().isEmpty());
}

static int child_count(const GroveLib::Node* node)
{
    int child_cnt = 0;
    const GroveLib::Node* child = node->firstChild();
    for (; child; child = child->nextSibling())
        if (!is_empty_text(child))
            child_cnt++;
    return child_cnt;
}

Mml::NodeType groveToMmlNodeType(const GroveLib::Node* node)
{
    using namespace GroveLib;
    Mml::NodeType mml_type = Mml::NoNode;

    switch (node->nodeType()) {
	case Node::ELEMENT_NODE: {
	    QString tag = CONST_ELEMENT_CAST(node)->localName();
	    const NodeSpec* spec = mmlFindNodeSpec(tag);

	    // treat urecognised tags as mrow
	    if (spec == 0) {
                if (tag == "math")
                    mml_type = Mml::MrowNode;
                else
		    mml_type = Mml::UnknownNode;
            }
	    else
		mml_type = spec->type;

	    break;
	}
        case Node::TEXT_NODE:
	    mml_type = Mml::TextNode;
	    break;

        case Node::DOCUMENT_NODE:
	    mml_type = Mml::MrowNode;
	    break;

        default:
            break;
    }
    return mml_type;
}

bool MmlDocument::setContent(const GroveLib::Node* node, QString* errorMsg)
{
    clear();
    bool ok;
    MmlNodePtr root_node = groveToMml(node, &ok, errorMsg);
    if (!ok)
	return false;
    if (!root_node) {
	if (errorMsg != 0)
	    *errorMsg = "empty document";
	return false;
    }
    insertChild(0, root_node.pointer(), 0);
    layout();
    return true;
}

MmlNode* MmlDocument::groveToMml(const GroveLib::Node* node,
                                 bool* ok, QString* errorMsg)
{
    Q_ASSERT(ok);
    NodeType mml_type = groveToMmlNodeType(node);
    if (mml_type == NoNode) {
        *ok = true;
        return 0;
    }
    MmlAttributeMap mml_attr;
    if (node->nodeType() == GroveLib::Node::ELEMENT_NODE) {
        const GroveLib::Element* elem = CONST_ELEMENT_CAST(node);
        const GroveLib::Attr* a = elem->attrs().firstChild();
        for (; a; a = a->nextSibling())
            mml_attr[a->localName()] = a->value();
    }
    QString mml_value;
    if (mml_type == TextNode)
        mml_value = CONST_TEXT_CAST(node)->data();
    Common::OwnerPtr<MmlNode> mml_node(createNode(mml_type,
        mml_attr, mml_value, errorMsg));
    if (!mml_node) {
	*ok = false;
	return 0;
    }
    // create the node's children according to the child_spec

    const NodeSpec* spec = mmlFindNodeSpec(mml_type);
    const GroveLib::Node* child = node->firstChild();
    int child_cnt = child_count(node);
    MmlNodePtr mml_child;

    QString separator_list;
    if (mml_type == MfencedNode)
	separator_list = mml_node->explicitAttribute("separators", ",");

    switch (spec->child_spec) {
	case NodeSpec::ChildIgnore:
	    break;

	case NodeSpec::ImplicitMrow:

	    if (child_cnt) {
		mml_child = grove_createImplicitMrowNode(node, ok, errorMsg);
		if (!*ok)
		    return 0;
		if (!insertChild(mml_node.pointer(),
                                 mml_child.pointer(), errorMsg)) {
		    *ok = false;
		    return 0;
		}
	    }
	    break;

	default:
	    // exact ammount of children specified - check...
	    if (spec->child_spec != child_cnt &&
                !(!child_cnt && mml_type == MmlNode::MtextNode)) {
		if (errorMsg != 0)
		    *errorMsg = QString("element ")
			+ spec->tag
			+ " requires exactly "
			+ QString::number(spec->child_spec)
			+ " arguments, got "
			+ QString::number(child_cnt);
		*ok = false;
		return 0;
	}
	// ...and continue just as in ChildAny

	case NodeSpec::ChildAny:
	    if (mml_type == MfencedNode)
		insertOperator(mml_node.pointer(),
                               mml_node->explicitAttribute("open", "("));
            int i = 0;
            for (; child; child = child->nextSibling()) {
                if (is_empty_text(child))
                    continue;
		mml_child = groveToMml(child, ok, errorMsg);
		if (!*ok)
                    return 0;
		if (mml_type == MtableNode &&
                    mml_child->nodeType() != MtrNode) {
		    MmlNodePtr mtr_node = createNode(MtrNode,
                        MmlAttributeMap(), QString::null, 0);
		    insertChild(mml_node.pointer(), mtr_node.pointer(), 0);
		    if (!insertChild(mtr_node.pointer(),
                                     mml_child.pointer(), errorMsg)) {
			*ok = false;
			return 0;
		    }
		}
		else if (mml_type == MtrNode &&
                         mml_child->nodeType() != MtdNode) {
		    MmlNodePtr mtd_node = createNode(MtdNode,
                        MmlAttributeMap(), QString::null, 0);
		    insertChild(mml_node.pointer(), mtd_node.pointer(), 0);
		    if (!insertChild(mtd_node.pointer(),
                                     mml_child.pointer(), errorMsg)) {
			*ok = false;
			return 0;
		    }
		}
		else {
		    if (!insertChild(mml_node.pointer(),
                            mml_child.pointer(), errorMsg)) {
			*ok = false;
			return 0;
		    }
		}
		if (i < child_cnt - 1 &&
                    mml_type == MfencedNode && !separator_list.isEmpty()) {
		    QChar separator;
		    if (i >= (int)separator_list.length())
			separator =
                            separator_list.at(separator_list.length() - 1);
		    else
			separator = separator_list[i];
		    insertOperator(mml_node.pointer(), QString(separator));
		}
                ++i;
	    }
	    if (mml_type == MfencedNode)
		insertOperator(mml_node.pointer(),
                               mml_node->explicitAttribute("close", ")"));
	    break;
    }
    *ok = true;
    return mml_node.release();
}

MmlNode* MmlDocument::grove_createImplicitMrowNode(const GroveLib::Node* node,
                                                     bool* ok,
	                                             QString* errorMsg)
{
    const GroveLib::Node* child = node->firstChild();
    int child_cnt = child_count(node);

    if (child_cnt == 0) {
	*ok = true;
	return 0;
    }
    Common::OwnerPtr<MmlNode> mml_node;
    if (child_cnt == 1) {
        while (child && is_empty_text(child))
            child = child->nextSibling();
        if (0 == child) {
            *ok = true;
            return 0;
        }
	return groveToMml(child, ok, errorMsg);
    }
    mml_node = createNode(MrowNode, MmlAttributeMap(),
			  QString::null, errorMsg);
    for (; child; child = child->nextSibling()) {
        if (is_empty_text(child))
            continue;
	MmlNodePtr mml_child = groveToMml(child, ok, errorMsg);
	if (!*ok)
	    return 0;
	if (!insertChild(mml_node.pointer(), mml_child.pointer(), errorMsg)) {
	    *ok = false;
	    return 0;
	}
    }
    return mml_node.release();
}

void QtMmlDocument::dump() const
{
    m_doc->dump();
}

void QtMmlDocument::setDrawFrames(bool b)
{
    g_draw_frames = true;
}

bool QtMmlDocument::drawFrames() const
{
    return g_draw_frames;
}

