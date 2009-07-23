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
#include "docview/dv_utils.h"
#include "docview/Finder.h"
#include "docview/SernaDoc.h"

#include "ui/UiItemSearch.h"

#include "common/PropertyTreeEventData.h"

#include "structeditor/StructFinder.h"
#include "structeditor/StructEditor.h"
#include "structeditor/InsertTextEventData.h"
#include "structeditor/impl/debug_se.h"

#include "groveeditor/GrovePos.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/GroveCommand.h"

#include "common/XTreeIterator.h"
#include "common/OwnerPtr.h"
#include "editableview/EditableView.h"

#include "xslt/ResultOrigin.h"
#include "xslt/PatternExpr.h"

#include "formatter/XslFoExt.h"
#include "formatter/impl/TerminalFos.h"

#include "xpath/ValueHolder.h"
#include "xpath/ExprContext.h"
#include "xpath/NodeSet.h"

#include <qapplication.h>

USING_COMMON_NS;
using namespace GroveEditor;
using namespace GroveLib;
using namespace Formatter;

class SetAttribute;
class Attributes;
class ShowElementAttribute;
class InsertText;
class EditComment;

GrovePos next_pos(const GrovePos& pos)
{
    if (pos.isNull())
        return GrovePos();
    if (GrovePos::TEXT_POS == pos.type())
        return GrovePos(pos.node()->parent(), pos.node()->nextSibling());
    if (pos.before())
        return GrovePos(pos.before(), pos.before()->firstChild());
    return GrovePos(pos.node()->parent(), pos.node()->nextSibling());
}

GrovePos prev_pos(const GrovePos& pos)
{
    if (pos.isNull())
        return GrovePos();
    if (GrovePos::TEXT_POS == pos.type())
        return GrovePos(pos.node()->parent(), pos.node());
    const Node* after = (pos.before()) 
        ? pos.before()->prevSibling() : pos.node()->lastChild();
    if (after)
        return GrovePos(after);
    return GrovePos(pos.node()->parent(), pos.node());
}

bool struct_find(StructFindFunctor* findFunctor, StructEditor* se, bool start)
{
    const GrovePos& pos = se->editViewFoPos();
    const Node* node = 0;
    if (pos.type() == GrovePos::ELEMENT_POS) {
        if (pos.before())
            node = findFunctor->isReverse() 
                ? pos.before()->prevSibling() : pos.before();
        else
            node = pos.node();
    } else
        node = pos.node();
    if (0 == node || start)
        node = se->fot()->document()->documentElement();
    const Node* n = 0;
    XTreeDocOrderIterator<const Node> it(node);
    for (; it.node(); findFunctor->isReverse() ? --it : ++it) {
        n = Xslt::resultOrigin(it.node());
        if (0 == n || n->grove() != se->grove())
            continue;
        if (findFunctor->isMatched(it.node(), n))
            return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////

namespace {

class StructFindFunctorImpl : public StructFindFunctor {
public:
    StructFindFunctorImpl(StructEditor* se, const PropertyNode* props)
        : se_(se), oldAreaPos_(se->editableView().context().areaPos())
    {
        matchCase_  = props->getSafeProperty(Find::MATCH_CASE)->getBool();
        reverse_ = props->getSafeProperty(Find::SEARCH_BACKWARDS)->getBool();
        whatString_ = props->getSafeProperty(Find::FIND_TEXT)->getString();
        replaceText_ = props->getSafeProperty(Find::REPLACE_TEXT)->getString();
        DBG(SE.FIND) << "FindFunctor: case=" << matchCase_ << " reverse="
                     << reverse_ << " find_text<" << whatString_ << ">\n";
    }
    virtual ~StructFindFunctorImpl() {}

    virtual bool    isReverse() const { return reverse_; }
    const String&   whatString() const { return whatString_; }
    virtual bool    replace() = 0;
    virtual bool    hasSelection() const = 0;

    StructEditor*   structEditor() const { return se_; }

    static StructFindFunctorImpl* make(StructEditor* se, const PropertyNode*);
    
protected:
    StructEditor*   se_;
    bool            matchCase_;
    bool            reverse_;
    String          whatString_;
    String          replaceText_;
    AreaPos         oldAreaPos_;
};

/////////////////////////////////////////////////////////////////////////////

class FindText : public StructFindFunctorImpl {
public:
    FindText(StructEditor* se, const PropertyNode* props)
        : StructFindFunctorImpl(se, props), pos_(se->editViewFoPos()) {}

    virtual bool isMatched(const Node* foNode, const Node* n) const
    {
        (void) n; // to remove warning when debug is off
        if (Node::TEXT_NODE != foNode->nodeType())
            return false;
        const Text* text_node = static_cast<const Text*>(foNode);
        const TextFo* text_fo = 
            dynamic_cast<const TextFo*>(XslFoExt::areaChain(foNode));
        String stripped_text = text_fo->strippedText();

        int start_idx = reverse_ ? -1 : 0;
        if (foNode == pos_.node() && GrovePos::TEXT_POS == pos_.type())
            start_idx = pos_.idx();

        if (reverse_) 
            stripped_text = stripped_text.left(start_idx);

        //! For debugging
        DBG_IF(SE.FIND) {
            String fo_text = text_node->data();
            fo_text.replace(" ", "$").replace("\t ", "$").replace("\n", "#");
            String src_text = CONST_TEXT_CAST(n)->data();
            src_text.replace(" ", "$").replace("\t ", "$").replace("\n", "#");
            DBG(SE.FIND) << "      match: foNode=posNode:" 
                         << (foNode == pos_.node())
                         << " idx=" << start_idx 
                         << "\n             fo_text<" 
                         << fo_text << ">\n"
                         << "             src_text=<" << src_text << ">\n"
                         << "             stripped_text=<"  
                         << stripped_text << ">\n";
        }
        //! Finding string in stripped text and converting to source position
        DBG(SE.FIND) << "find in:<" << stripped_text << "> what:<"
            << whatString_ << "> start_idx="
            << start_idx << ", match_case=" << matchCase_ << std::endl;
        start_idx = reverse_ 
            ? stripped_text.findRev(whatString_, start_idx - 1, matchCase_)
            : stripped_text.find(whatString_, start_idx, matchCase_);
        DBG(SE.FIND) << "after find start_idx = " << start_idx << std::endl;
        if (start_idx < 0)
            return false;
        DBG_IF(SE.TEST) text_node->dump();

        int start_src = text_fo->convertPos(start_idx, false);
        int end_src = text_fo->convertPos(start_idx + whatString_.length(), 
                                          false);

        DBG(SE.FIND) << "matched start_src:" << start_src 
                     << " end_src:" << end_src 
                     << " start_idx:" << start_idx 
                     << " end_idx:" << start_idx + whatString_.length() 
                     << std::endl;

        //! Calculating positions to set selection
        AreaPos new_start = se_->toAreaPos(GrovePos(text_node, start_src));
        DBG_IF(SE.TEST) new_start.dump();
        AreaPos new_end = se_->toAreaPos(GrovePos(text_node, end_src));
        if (new_start.isNull() || new_end.isNull()) 
            return false;                                
        if ((reverse_ ? new_start : new_end) == oldAreaPos_)
            return false;

        GrovePos src_start = EditContext::getSrcPos(
            (reverse_) ? new_end : new_start);
        GrovePos src_end = EditContext::getSrcPos(
            (reverse_) ? new_start : new_end);

        ChainSelection chain_sel(reverse_ ? ChainSelection(new_end, new_start) 
            : ChainSelection(new_start, new_end));
        GroveSelection grove_sel(src_start, src_end);

        se_->setSelection(chain_sel, GroveSelection(src_start, src_end));
        se_->editableView().adjustView((reverse_) ? new_start : new_end, true);
        return true;
    }
    virtual bool replace()
    {
        if (se_->editableView().getSelection().src_.isEmpty())
            return false;
        GrovePos from, to;
        if (StructEditor::POS_OK != se_->getSelection(
                from, to, StructEditor::TEXT_OP))
            return false;
        DBG(SE.FIND) << "replace: from_idx=" << from.idx()
            << ", to_idx=" << to.idx() << std::endl;
        CommandPtr cmd = se_->groveEditor()->replaceText(
            from, to.idx() - from.idx(), replaceText_);
        if (reverse_) {
            RefCntPtr<GroveBatchCommand> batch_cmd = new GroveBatchCommand;
            batch_cmd->setSuggestedPos(from);
            batch_cmd->executeAndAdd(cmd.pointer());
            cmd = batch_cmd.pointer();
        } 
        bool ok = se_->executeAndUpdate(cmd.pointer());
        DBG(SE.FIND) << "ok = " << ok << ", sposidx=" 
            << dynamic_cast<GroveEditor::CommandContext&>(*cmd).pos().idx() 
            << ", now fopos.idx=" << pos_.idx() 
            << ", spos.idx=" << se_->editViewSrcPos().idx() << std::endl;
        return ok;
    }
    virtual bool hasSelection() const
    {
        return !se_->editableView().getSelection().src_.isEmpty();
    }
private:
    const GrovePos& pos_;
};

/////////////////////////////////////////////////////////////////////////////

class FindAttr : public StructFindFunctorImpl {
public:
    FindAttr(StructEditor* se, const PropertyNode* props)
        : StructFindFunctorImpl(se, props)
    {
        se->removeSelection();
    }
    virtual bool isMatched(const Node* foNode, const Node* srcNode) const
    {
        if (srcNode->nodeType() != Node::ELEMENT_NODE)
            return false;
        DBG_IF(SE.FIND) {
            DBG(SE.FIND) << "srcNode: " << srcNode << std::endl;
        }
        const Element* e = static_cast<const Element*>(srcNode);
        const Attr* attr = e->attrs().firstChild();
        for (; attr; attr = attr->nextSibling()) {
            if (matchAttr(attr)) {
                AreaPos new_pos = se_->toAreaPos(GrovePos(foNode));
                if (new_pos.isNull() || new_pos == oldAreaPos_)
                    return false;
                if (se_->setCursor(new_pos, true)) {
                    PropertyTreeEventData attr_name;
                    attr_name.root()->appendChild(
                        new PropertyNode(attr->qualifiedName().asString()));
                    return makeCommand<ShowElementAttribute>(
                        &attr_name)->execute(se_);
                }
                return false;
            }
        }
        return false;
    }
    virtual bool hasSelection() const
    {
        return !!findAttr();
    }
    GroveLib::Attr* findAttr() const
    {
        GrovePos pos;
        if (!se_->getCheckedPos(pos))
            return 0;
        const GroveLib::Node* srcNode = pos.node();
        if (0 == srcNode)
            return 0;
        if (srcNode->nodeType() == Node::TEXT_NODE)
            srcNode = parentNode(srcNode);
        if (!srcNode || srcNode->nodeType() != Node::ELEMENT_NODE)
            return false;
        const Element* e = static_cast<const Element*>(srcNode);
        Attr* attr = e->attrs().firstChild();
        for (; attr; attr = attr->nextSibling())
            if (matchAttr(attr))
                return attr;
        return 0;
    }
    virtual bool matchAttr(const Attr* attr) const = 0;
};

class FindAttrValue : public FindAttr {
public:
    FindAttrValue(StructEditor* se, const PropertyNode* props)
        : FindAttr(se, props) {}

    virtual bool matchAttr(const Attr* attr) const
    {
        return attr->value().find(whatString_, 0, matchCase_) >= 0;
    }
    virtual bool replace()
    {
        GroveLib::Attr* attr = findAttr();
        if (0 == attr)
            return false;
        int i = attr->value().find(whatString_, 0, matchCase_);
        if (i < 0) 
            return false;
        String new_value = attr->value();
        new_value.replace(i, whatString_.length(), replaceText_);
        return se_->executeAndUpdate(se_->groveEditor()->
                                     setAttribute(attr, new_value));
    }
};

class FindAttrName : public FindAttr {
public:
    FindAttrName(StructEditor* se, const PropertyNode* props)
        : FindAttr(se, props) {}

    virtual bool matchAttr(const Attr* attr) const
    {
        return attr->nodeName().find(whatString_, 0, matchCase_) >= 0;
    }
    virtual bool replace()
    {
        GroveLib::Attr* attr = findAttr();
        if (0 == attr)
            return false;
        PropertyNode attr_spec(replaceText_, attr->value());
        return se_->executeAndUpdate(se_->groveEditor()->
            setAttribute(attr, replaceText_));
    }
};

/////////////////////////////////////////////////////////////////////////////

class FindComment : public StructFindFunctorImpl {
public:
    FindComment(StructEditor* se, const PropertyNode* props)
        : StructFindFunctorImpl(se, props)
    {
        se->removeSelection();
    }
    virtual bool isMatched(const Node* foNode, const Node* srcNode) const
    {
        if (!matchComment(srcNode))
            return false;
        AreaPos new_pos = se_->toAreaPos(GrovePos(foNode));
        if (new_pos.isNull() || new_pos == oldAreaPos_)
            return false;
        if (se_->setCursor(new_pos, true)) {
            makeCommand<EditComment>()->execute(se_);
            return true;
        }
        return false;
    }
    virtual bool replace()
    {
        GrovePos pos;
        if (!se_->getCheckedPos(pos))
            return false;
        Comment* comment = matchComment(pos.node());
        if (0 == comment)
            return false;
        String newText = comment->comment();
        newText.replace(whatString_, replaceText_, matchCase_);
        return se_->executeAndUpdate
            (se_->groveEditor()->changeComment(pos, newText));
    }
    virtual bool hasSelection() const
    {
        return matchComment(se_->editViewSrcPos().node());
    }
    Comment* matchComment(const Node* node) const
    {
        if (!node || node->nodeType() != Node::COMMENT_NODE)
            return 0;
        const Comment* comment = static_cast<const Comment*>(node);
        if (comment->comment().find(whatString_, 0, matchCase_) >= 0)
            return const_cast<Comment*>(comment);
        return 0;
    }
};

/////////////////////////////////////////////////////////////////////////////

class FindPattern : public StructFindFunctorImpl,
                    public Xpath::ExprContext {
public:
    FindPattern(StructEditor* se, const PropertyNode* props)
        : StructFindFunctorImpl(se, props)
    {
        se->removeSelection();
        pattern_ = Xslt::PatternExpr::makeSimplePattern(whatString_,
            se->grove()->document()->documentElement());
    }
    virtual bool isMatched(const Node* foNode, const Node* srcNode) const
    {
        Xpath::ValueHolderPtr vh;
        try {
            Xpath::NodeSetItem nsi(const_cast<Node*>(srcNode));
            vh = pattern_->makeInst(nsi, *this);
        } catch (...) {
            return false;
        }
        if (vh.isNull() || !vh->value()->getBool())
            return false;
        AreaPos new_pos = se_->toAreaPos(GrovePos(foNode));
        if (new_pos.isNull() || new_pos == oldAreaPos_)
            return false;
        return se_->setCursor(new_pos, true);
    }
    virtual bool    replace() { return true; }
    virtual bool    hasSelection() const { return false; }

// reimplemented from ExprContext
    virtual String  contextString() const { return ""; }
    virtual void    registerVisitor(const Node*, NodeVisitor*, short) const {}

private:
    Xslt::PatternExprPtr pattern_;
};

/////////////////////////////////////////////////////////////////////////////

StructFindFunctorImpl* StructFindFunctorImpl::make(StructEditor* se,
                                                   const PropertyNode* props)
{
    const String& type =
        props->getSafeProperty(Find::SEARCH_TYPE)->getString();
    try {
        if (type.isEmpty() || type == Find::TEXT_TYPE)
            return new FindText(se, props);
        if (type == Find::ATTRVAL_TYPE)
            return new FindAttrValue(se, props);
        if (type == Find::ATTRNAME_TYPE)
            return new FindAttrName(se, props);
        if (type == Find::COMMENT_TYPE)
            return new FindComment(se, props);
        if (type == Find::PATTERN_TYPE)
            return new FindPattern(se, props);
    }
    catch (Common::Exception& exc) {
        se->sernaDoc()->showMessageBox(
            SernaDoc::MB_CRITICAL,
            qApp->translate("StructFinder", "Bad XSLT Pattern"),
            qApp->translate("StructFinder", 
                            "Invalid XSLT pattern (wrong syntax)"), 
            tr("&Ok"));
    }
    return new FindText(se, props);
}

} // namespace

///////////////////////////////////////////////////////////////////

#define MAKE_FIND_FUNCTOR \
    if (0 == structEditor_) \
        return false; \
    OwnerPtr<StructFindFunctorImpl> findFunctor \
        (StructFindFunctorImpl::make(structEditor_, findProps()));

class StructFinder : public Finder {
public:
    StructFinder(Sui::Action*, PropertyNode* properties)
        : Finder(properties, true),
          structEditor_(0)
    {
    }

private:
    virtual bool doFind(bool start)
    {
        MAKE_FIND_FUNCTOR;
        if (findFunctor->whatString().isEmpty())
            return false;
        return struct_find(findFunctor.pointer(), structEditor_, start);
    }
    virtual bool replaceSelection()
    {
        MAKE_FIND_FUNCTOR;
        findFunctor->replace();
        return true;
    }
    virtual bool hasSelection() const
    {
        MAKE_FIND_FUNCTOR;
        return findFunctor->hasSelection();
    }
    //! Tells that this item is inserted/removed from parent
    virtual void inserted()
    {
        Sui::Item* doc = documentItem();
        if (0 == doc)
            return;
        structEditor_ = dynamic_cast<StructEditor*>(
            doc->findItem(Sui::ItemClass(Sui::STRUCT_EDITOR)));
        if (structEditor_) {
            findProps_ = structEditor_->getDsi()->makeDescendant("recentFind");
            findProps_->makeDescendant(Find::SEARCH_BACKWARDS, "false", false);
            findProps_->makeDescendant(Find::MATCH_CASE, "false", false);
        }
    };
    virtual void removed() { structEditor_ = 0; }

private:
    StructEditor* structEditor_;
};

namespace Sui {
    CUSTOM_ITEM_MAKER(StructFinder, StructFinder)
}

///////////////////////////////////////////////////////////////////////

static void make_struct_finder(StructEditor* se, bool isReplace)
{

    PropertyNodePtr prop = new PropertyNode(Sui::FINDER);
    prop->makeDescendant(Sui::ITEM_PROPS + String("/") +
                         Sui::NAME)->setString(Sui::FINDER);
    Finder* finder = static_cast<Finder*>(restore_ui_item(se->sernaDoc(),
                                                          prop.pointer(), 0));
    se->sernaDoc()->appendChild(finder);
    finder->buildChildren(&restore_ui_item);
    finder->attach();
    finder->setReplaceTab(isReplace);
}

SIMPLE_COMMAND_EVENT_IMPL(CallStructFind, StructEditor)

bool CallStructFind::doExecute(StructEditor* se, EventData*)
{
    Finder* finder = Finder::findInstance(se->sernaDoc());
    if (finder) {
        finder->setReplaceTab(false);
        finder->grabFocus();
        return true;
    }
    make_struct_finder(se, false);
    return true;
}

SIMPLE_COMMAND_EVENT_IMPL(CallStructFindReplace, StructEditor)

bool CallStructFindReplace::doExecute(StructEditor* se, EventData*)
{
    Finder* finder = Finder::findInstance(se->sernaDoc());
    if (finder) {
        finder->setReplaceTab(true);
        finder->grabFocus();
        return true;
    }
    make_struct_finder(se, true);
    return true;
}
