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
#include "ui/UiItemSearch.h"
#include "utils/DocSrcInfo.h"
#include "common/PropertyTreeEventData.h"
#include "utils/Properties.h"
#include "common/Url.h"

#include "structeditor/StructEditor.h"
#include "structeditor/impl/XsUtils.h"

#include "groveeditor/GroveEditor.h"
#include "groveeditor/GroveCommand.h"
#include "groveeditor/GrovePos.h"
#include "grove/Grove.h"
#include "grove/IdManager.h"

#include "debug_cc.h"
#include "customcontent.h"
#include "xpathutils.h"

#include <QRegExp>
#include <QFileDialog>
#include <list>

using namespace Common;
using namespace GroveLib;
using namespace GroveEditor;
using namespace ContentProps;

// START_IGNORE_LITERALS
const uchar MARKED_BEFORE   = 1;
const uchar MARKED_AFTER    = 2;
const uchar MARKED_INSIDE   = 3;
const char  CURSOR_MARK[]   = "$(cursor)";
// STOP_IGNORE_LITERALS

////////////////////////////////////////////////////////////////////////////

typedef std::list<uint> IndexList;

static String remove_cursor_marks(const String& str, int& cursor,
                                  const char* pattern)
{
    cursor = -1;
    IndexList list;
    uint from_idx = 0;
    uint removed_len = 0;
    while (from_idx < str.length()) {
        int idx = str.find(pattern, from_idx);
        if (idx < 0)
            break;
        list.push_back(idx);
        if (idx > 0 && '$' == str[idx - 1]) {
            from_idx = idx + 1;
            removed_len++;
        }
        else {
            if (-1 == cursor)
                cursor = idx - removed_len;
            else
                removed_len += strlen(pattern);
            from_idx = idx + strlen(pattern);
        }
        list.push_back(from_idx);
    }
    if (list.empty()) //! Nothing to remove
        return str;
    //! Invert removal list
    if (0 == *(list.begin()))
        list.pop_front();
    else
        list.push_front(0);
    if (str.length() == *(list.rbegin()))
        list.pop_back();
    else
        list.push_back(str.length());
    if (list.empty()) //! Whole string removed
        return String();
    //! Collect inverted list
    String result;
    IndexList::const_iterator i = list.begin();
    while (i != list.end()) {
        int from = *(i++);
        int to = *(i++);
        if (to > from)
            result += str.mid(from, to - from);
    }
    return result;
}

static void substitute_cursor_marks(Node* node, GrovePos& pos)
{
    if (!node)
        return;
    if (Node::TEXT_NODE == node->nodeType()) {
        Text* text = static_cast<Text*>(node);
        int cursor_pos = -1;
        String stripped =
            remove_cursor_marks(text->data(), cursor_pos, CURSOR_MARK);
        if (stripped != text->data())
            text->setData(stripped);
        if (-1 != cursor_pos) {
            if (text->data().isEmpty()) {
                if (pos.isNull()) {
                    if (text->nextSibling())
                        pos = GrovePos(text->parent(), text->nextSibling());
                    else
                        pos = GrovePos(text->parent(), (Node*)(0));
                }
                text->remove();
            }
            else {
                if (pos.isNull())
                    pos = GrovePos(text, cursor_pos);
            }
        }
        else
            if (text->data().isEmpty())
                text->remove();
        return;
    }
    Node* child = node->firstChild();
    while (child) {
        Node* next = child->nextSibling();
        substitute_cursor_marks(child, pos);
        child = next;
    }
}

//////////////////////////////////////////////////////////////////////////

// START_IGNORE_LITERALS
const char ATTR_VAL[] = "\\s*((?:\"[^\"]*\")|(?:'[^']*'))";
const char ATTR_NAME[] = "\\s+([^=\\s]+)\\s*";
// STOP_IGNORE_LITERALS

static void parse_attrs(const String& attrString, PropertyNode* prop)
{
    QRegExp attr_expr(String(ATTR_NAME) + '=' + String(ATTR_VAL));
    int offset = 0;
    int attr_pos = -1;
    while (-1 < (attr_pos = attr_expr.search(attrString, offset))) {
        String value = attr_expr.cap(2);
        prop->makeDescendant(attr_expr.cap(1),
                             value.mid(1, value.length() - 2), true);
        if (attr_expr.matchedLength())
            offset += attr_expr.matchedLength();
        else
            offset++;
    }
}

static String get_open_file_name(const String& docPath, String& lastUrl, 
                                 QWidget* parent, PropertyNode* props)
{
    while (parent && !(parent->inherits(NOTR("QMainWindow")) ||
                       parent->inherits(NOTR("QDialog")) ||
                       parent->inherits(NOTR("QDockWindow"))))
        parent = parent->parentWidget();
    if (lastUrl.isEmpty())
        lastUrl = Url(docPath)[Url::COLLPATH];
    String path = QFileDialog::getOpenFileName(
        parent, 
        props->getSafeProperty("caption")->getString(),
        lastUrl,
        props->getSafeProperty("filter")->getString());
    if (path.isEmpty())
        return String();
    lastUrl = Url(path)[Url::COLLPATH];
    return docPath.isEmpty() ? path : String(Url(docPath).relativePath(path));
}

// START_IGNORE_LITERALS
const char* LAST_URL    = "last-url";
const char* CC_LAST_URL = "cc-last-url";
// STOP_IGNORE_LITERALS

String CustomContentPlugin::substitute_file_mark(const String& str,
                                                 const String& docPath)
{
    String attr = String(ATTR_NAME) + '=' + String(ATTR_VAL);
    QRegExp mark_expr(NOTR("\\$\\(file((?:") + attr + NOTR(")*)\\)"));
    String result = str;
    int offset = 0;
    int mark_pos = -1;
    while (-1 < (mark_pos = mark_expr.search(result, offset))) {
        String attr_string = mark_expr.cap(1);
        PropertyNode attr_prop;
        parse_attrs(attr_string, &attr_prop);

        String last_url = structEditor()->
            getDsi()->getSafeProperty(CC_LAST_URL)->getString();
        String path = get_open_file_name(docPath, last_url, 0, &attr_prop);
        if (path.isNull())
            throw CustomContentException(
                String(tr("Incorrect (empty) file path for $(file) mark")));
        structEditor()->getDsi()->
            makeDescendant(CC_LAST_URL)->setString(last_url);
        result.replace(mark_pos, mark_expr.matchedLength(), path);
        offset = mark_pos + path.length();
    }
    return result;
}

static bool substitute_attr_mark(String& str, String& caption)
{
    String attr = String(ATTR_NAME) + '=' + String(ATTR_VAL);
    QRegExp mark_expr(NOTR("\\$\\(attribute((?:") + attr + NOTR(")*)\\)"));
    int offset = 0;
    int mark_pos = mark_expr.search(str, offset);
    if (-1 == mark_pos)
        return false;
    String attr_string = mark_expr.cap(1);
    str.replace(mark_pos, mark_expr.matchedLength(), "");
    if (caption.isEmpty()) {
        PropertyNode attr_prop;
        parse_attrs(attr_string, &attr_prop);
        caption = attr_prop.getSafeProperty("caption")->getString();
    }
    return true;
}

static String substitute_genid_mark(const Attr* a, const StructEditor* se)
{
    const char genid_prefix[] = NOTR("$(generate-id");
    String val = a->value().left(sizeof(genid_prefix) - 1);
    if (val != genid_prefix)
        return a->value();
    const Char* cp = a->value().unicode() + val.length();
    const Char* ce = a->value().unicode() + a->value().length();
    val = String();
    for (; cp < ce; ++cp) {
        if (cp->isSpace() || cp->unicode() == ')')
            continue;
        val += cp->unicode();
    }
    return se->generateId(val);
}

class SetElementAttributes;

void CustomContentPlugin::substitute_marks(Node* node,
                                           const String& docPath,
                                           const GrovePos& pos)
{
    using namespace DocSrcInfo;
    using namespace AttributesSpace;

    if (Node::TEXT_NODE == node->nodeType()) {
        Text* text = static_cast<Text*>(node);
        String str = substitute_file_mark(text->data(), docPath);
        if (str != text->data())
            text->setData(str);
        return;
    }
    if (Node::ELEMENT_NODE == node->nodeType()) {
        Element* elem = static_cast<Element*>(node);
        PropertyTreeEventData attrs;
        PropertyNode* existing_attrs =
            attrs.root()->makeDescendant(EXISTING_ATTRS);
        PropertyNode* attrs_spec_list =
            attrs.root()->makeDescendant(ATTR_SPEC_LIST);
        get_schema_attributes(pos, elem->nodeName(), attrs_spec_list);

        const AttrList& attr_list = elem->attrs();
        String caption;
        for (Attr* a = attr_list.firstChild(); a; a = a->nextSibling()) {
            String value = substitute_genid_mark(a, structEditor());
            value = substitute_file_mark(value, docPath);
            if (substitute_attr_mark(value, caption))
                existing_attrs->makeDescendant(
                    a->nodeName())->setString(value);
            else {
                PropertyNode* attr_spec =
                    attrs_spec_list->getProperty(a->nodeName());
                if (attr_spec)
                    attr_spec->remove();
            }
            if (value != a->value())
                a->setValue(value);
        }
        attrs.root()->makeDescendant(
            ElementSpace::ELEMENT_NAME)->setString(elem->nodeName());
        if (caption.isEmpty())
            caption = tr("Set %1");
        attrs.root()->makeDescendant("caption")->setString(caption);

        //! Show attributes dialog
        attrs.root()->makeDescendant(LAST_URL)->setString(
            structEditor()->getDsi()->
                getSafeProperty(CC_LAST_URL)->getString());
        if (!makeCommand<SetElementAttributes>(&attrs)->
            execute(structEditor(), &attrs))
                throw CustomContentException(
                    String(tr("$(attribute) mark has not been substituted")));
        //! Remember last browsed url (if any)
        structEditor()->getDsi()->makeDescendant(CC_LAST_URL)->setString(
            attrs.root()->getSafeProperty(LAST_URL)->getString());

        for (PropertyNode* prop = existing_attrs->firstChild();
             prop; prop = prop->nextSibling()) {
            Attr* attr = elem->attrs().getAttribute(prop->name());
            if (attr)
                attr->setValue(prop->getString());
            else {
                attr = new Attr(prop->name());
                attr->setValue(prop->getString());
                PropertyNode* type = attrs_spec_list->getProperty(
                    prop->name() + '/' + ATTR_TYPE);
                if (type && type->getString() == NOTR("ID"))
                    attr->setIdClass(Attr::IS_ID);
                elem->attrs().appendChild(attr);
            }
        }
    }
    for (Node* c = node->firstChild(); c; c = c->nextSibling())
        substitute_marks(c, docPath, pos);
}

/////////////////////////////////////////////////////////////////////////////

static const char cc_nsuri[] = 
    NOTR("http://www.syntext.com/Extensions/CustomContent-1.0");

CustomContext::CustomContext(const GroveEditor::GrovePos& pos)
    : pos_(pos),
      nodesBeforeFunc_(this, false), 
      nodesAfterFunc_(this, true)
{
    resolver_ = new Element(NOTR("#cc-ns-resolver"));
    resolver_->addToPrefixMap(NOTR("cc"), cc_nsuri);
    for (Node* n = pos.node(); n; n = parentNode(n)) 
        if (n->nodeType() == Node::ELEMENT_NODE) {
            resolver_->setParent(n);
            break;
        }
}

CustomContext::~CustomContext()
{
    resolver_->setParent(0);
}

const Xpath::ExternalFunctionBase* 
CustomContext::getExternalFunction(const String& name, const String& ns) const
{
    if (ns != cc_nsuri)
        return 0;
    if (NOTR("nodes-before") == name)
        return &nodesBeforeFunc_;
    if (NOTR("nodes-after") == name)
        return &nodesAfterFunc_;
    return 0;
}

Xpath::ValueHolder*
NodesFunction::eval(const Xpath::ConstValuePtrList&) const
{
    Xpath::NodeSet result_set;
    Node* before = 0;
    if (ctx_->pos().type() == GrovePos::TEXT_POS)
        before = ctx_->pos().node();
    else
        before = ctx_->pos().before();
    if (isAfter_) {
        for (; before; before = before->nextSibling())
            result_set += before;
    } else {
        if (!before)
            before = ctx_->pos().node()->lastChild();
        else
            before = before->prevSibling();
        for (; before; before = before->prevSibling())
            result_set += before;
    }
    return Xpath::ConstValueHolder::makeNodeSet(result_set);
}

XpathLocator::XpathLocator(const String& str, const GrovePos& pos)
    : CustomContext(pos)
{
    try {
        expr_ = Xslt::PatternExpr::makeExpr(str, ns_node());
    }
    catch (...) {
        DBG(CC.TEST) << "Bad XPATH expression" << std::endl;
    }
}

Node* XpathLocator::locate() const
{
    if (expr_.isNull())
        return 0;
    Xpath::ValueHolderPtr vh;
    try {
        Xpath::NodeSetItem nsi(pos_.node());
        vh = expr_->makeInst(nsi, *this);
    }
    catch (...) {
        return 0;
    }
    if (vh.isNull() || Xpath::Value::NODESET != vh->value()->type())
        return 0;
    Xpath::NodeSetItem* nsi = vh->value()->getNodeSet().first();
    if (nsi)
        return nsi->node();
    return 0;
}

PatternMatcher::PatternMatcher(const String& str, const GrovePos& pos)
    : CustomContext(pos)
{
    try {
        expr_ = Xslt::PatternExpr::makeSimplePattern(str, ns_node());
    }
    catch (Common::Exception& e) {
        DBG(CC.TEST) << "Bad XSLT Pattern" << std::endl;
    }
}

bool PatternMatcher::matches() const 
{
    if (expr_.isNull())
        return false;
    Xpath::ValueHolderPtr vh;
    try {
        Xpath::NodeSetItem nsi(pos_.node());
        vh = expr_->makeInst(nsi, *this);
    }
    catch (...) {
        return false;
    }
    return (!vh.isNull() && vh->value()->getBool());
}

static bool is_pattern_matches(const GrovePos& pos, const String& patternStr)
{
    if (pos.isNull())
        return false;
    if (patternStr.isEmpty())
        return true;
    return PatternMatcher(patternStr, pos).matches();
}

static GrovePos find_context_pos(GrovePos pos, String xpathLoc,
                                 RelativeLocation l)
{
    //std::cerr << "find_context_pos: " << xpathLoc << std::endl;
    if (pos.isNull())
        return GrovePos();
    pos = pos.adjustChoice();
    if (xpathLoc.isEmpty()) 
        return pos;
    Node* node = XpathLocator(xpathLoc, pos).locate();
    if (!node)
        return GrovePos();
    GrovePos context_pos;
    switch (l) {
        case INSERT_BEFORE_ :
            //std::cerr << "#before\n";
            context_pos = GrovePos(node->parent(), node);
            break;
        case INSERT_AFTER_ :
            //std::cerr << "#after\n";
            context_pos = GrovePos(node->parent(), node->nextSibling());
            break;
        default:
            //std::cerr << "#at\n";
            context_pos = GrovePos(node, (Node*)(0));
            break;
    }
    if (!context_pos.node()->parent())
        context_pos = GrovePos();
    return context_pos;
}

////////////////////////////////////////////////////////////////////////////

class GroveTreeloc : public std::list<ulong> {
public:
    typedef std::list<ulong>::const_iterator    CIterator;

    GroveTreeloc(const GrovePos& pos, const Node* root = 0)
    {
        TreelocRep treeloc;
        if (pos.isNull())
            return;
        pos.node()->treeLoc(treeloc, root);
        if (GrovePos::TEXT_POS == pos.type())
            treeloc.append(pos.idx());
        else
            treeloc.append((pos.before())
                           ? pos.before()->siblingIndex()
                           : pos.node()->countChildren());
        for (ulong c = 0; c < treeloc.length(); c++)
            push_back(treeloc[c]);
    }

    void        append(const GroveTreeloc& other)
    {
        for (CIterator i = other.begin(); i != other.end(); i++)
            push_back(*i);
    }
    GrovePos    grovePos(const Node* root) const
    {
        TreelocRep loc;
        for (CIterator i = begin(); i != end(); i++)
            loc.append((*i));
        TreelocRep treeloc;
        for (ulong c = 0; c < loc.length() - 1; c++)
            treeloc.append(loc[c]);
        Node* node = root->getByTreeloc(treeloc);
        if (!node)
            return GrovePos();
        if (Node::TEXT_NODE == node->nodeType())
            return GrovePos(node, back());
        return GrovePos(node, node->getChild(back()));
    }
    void        dump() const
    {
#ifndef NDEBUG
        std::cerr << NOTR("GroveTreeloc: ");
        for (CIterator i = begin(); i != end(); i++)
            std::cerr << (*i) << " ";
        std::cerr << std::endl;
#endif // NDEBUG
    }
};

static GroveTreeloc get_treeloc(const GrovePos& insPos, 
                                const GrovePos& fragPos,
                                const Node* fragRoot)
{
    GroveTreeloc frag_treeloc(fragPos, fragRoot);
    //Node::dumpSubtree(fragRoot);
    //frag_treeloc.dump();
    Node* first_child =  fragRoot->firstChild();
    Node* child =  fragRoot->getChild(frag_treeloc.front());
    GrovePos ins_pos = insPos;
    if (GrovePos::TEXT_POS == ins_pos.type()) {
        if (first_child && Node::TEXT_NODE != first_child->nodeType()) {
            //! No text join, insert in split point
            ins_pos = GrovePos(ins_pos.node()->parent(),
                               (ins_pos.idx()) ? ins_pos.node()->nextSibling()
                                               : ins_pos.node());
        }
        else { //! Text join
            if (child && !child->prevSibling() &&
                Node::TEXT_NODE  == child->nodeType())
                //! Cursor is inside joined text
                frag_treeloc.pop_front();
            else {
                //! Cursor is outside of joined text
                ins_pos = GrovePos(ins_pos.node()->parent(),
                                   ins_pos.node()->nextSibling());
                frag_treeloc.front()--;
            }
        }
    }
    GroveTreeloc ins_treeloc(ins_pos);
    //frag_treeloc.dump();
    //ins_treeloc.dump();
    ulong ins_idx = ins_treeloc.back();
    ins_idx += frag_treeloc.front();
    ins_treeloc.pop_back();
    frag_treeloc.pop_front();
    ins_treeloc.push_back(ins_idx);
    ins_treeloc.append(frag_treeloc);
    //ins_treeloc.dump();
    return ins_treeloc;
}

/////////////////////////////////////////////////////////////////////////

typedef Common::RefCntPtr<DocumentFragment> FragmentPtr;

SIMPLE_PLUGIN_UI_EXECUTOR_IMPL(InsertCustomContent, CustomContentPlugin)

#ifdef DEBUG_CC_POS
# define DUMP_POS(p, t) dump_pos(p, t)
static void dump_pos(const GrovePos& pos, const String& text)
{
    std::cerr << text << std::endl;
    if (pos.isNull()) {
        std::cerr << NOTR("NULL pos\n");
        return;
    }
    pos.dump();
    if (pos.node()->parent())
        Node::dumpSubtree(pos.node()->parent());
    else
        Node::dumpSubtree(pos.node());
}
#else  // DEBUG_CC_POS
# define DUMP_POS(p, t)
#endif // DEBUG_CC_POS

void InsertCustomContent::execute()
{
    StructEditor* const se = plugin()->structEditor();
    if (!se)
        return;
    GrovePos pos;
    if (!se->getCheckedPos(pos) || !pos.node())
        return;
    if (Node::CHOICE_NODE == pos.node()->nodeType())
        pos = GrovePos(pos.node()->parent(), pos.node());

    //! Make batch command
    GrovePos initial_pos = pos;
    RefCntPtr<GroveBatchCommand> batch_cmd = new GroveBatchCommand;
    batch_cmd->setInfo(uiAction()->get(INSCRIPTION));

    try {
        //! Cut Selection if any
        GrovePos from, to;
        FragmentPtr selection_fragment = new DocumentFragment;
        if (StructEditor::POS_OK == se->getSelection(from, to)) {
            se->removeSelection();
            se->groveEditor()->copy(from, to, selection_fragment);
            Command* cut_cmd = se->groveEditor()->cut(from, to).pointer();
            if (cut_cmd) {
                batch_cmd->executeAndAdd(
                    se->groveEditor()->cut(from, to).pointer());
                pos = batch_cmd->pos();
            }
        }
        //! Choose appropriate content and adjust insertion position
        String content;
        const PropertyNode* c_specs =
            uiAction()->properties()->getSafeProperty(CONTEXT_SPECS);
        GrovePos context_pos;
        for (PropertyNode* c = c_specs->firstChild(); 
                c ; c = c->nextSibling()) {
            if (!is_pattern_matches(
                    pos, c->getSafeProperty(PATTERN_STR)->getString()))
                continue;
            context_pos = find_context_pos(
                pos, c->getSafeProperty(XPATH_LOCATION)->getString(),
                location(c->getSafeProperty(REL_LOCATION)->getString()));
            if (!context_pos.isNull()) {
                content = NOTR("<CONTENT-TOP-ELEMENT>") +
                    c->getSafeProperty(TEXT)->getString() +
                    NOTR("</CONTENT-TOP-ELEMENT>");
                break;
            }
        }
        if (context_pos.isNull()) {
            se->sernaDoc()->showMessageBox(
                SernaDoc::MB_INFO, tr("Custom Content Insertion"),
                tr("No context position to insert content"),
                tr("&Close"));
            return;
        }
        pos = context_pos;

        //! Parse custom content data
        Grove* grove = pos.node()->grove();
        GrovePtr content_grove =
            grove->groveBuilder()->copy()->buildGrove(content);
        if (content_grove.isNull()) {
            se->sernaDoc()->showMessageBox(
                SernaDoc::MB_INFO, tr("Custom Content Insertion"),
                tr("Error parsing custom content: No grove built"),
                tr("&Close"));
            return;
        }
        Node* top_element = content_grove->document()->documentElement();
        if (top_element)
            se->stripInfo()->strip(top_element);
        if (!(top_element && top_element->firstChild())) {
            se->sernaDoc()->showMessageBox(
                SernaDoc::MB_INFO, tr("Custom Content Insertion"),
                tr("Error parsing custom content: Empty grove built"),
                tr("&Close"));
            return;
        }
        FragmentPtr fragment =
            top_element->firstChild()->takeAsFragment(top_element->lastChild());
        fragment->setGrove(top_element->grove());

        //! Find cursor mark and get GrivePos for cursor setting
        GrovePos fragment_pos;
        if (fragment.pointer()) {
            substitute_cursor_marks(fragment.pointer(), fragment_pos);
            if (fragment_pos.isNull()) {
                Node* child = fragment.pointer();
                while (child->firstChild())
                    child = child->firstChild();
                if (child) {
                    if (Node::TEXT_NODE  == child->nodeType())
                        fragment_pos = GrovePos(
                            child, static_cast<Text*>(child)->data().length());
                    else
                        fragment_pos = GrovePos(child, (Node*)(0));
                }
            }
        }
        GroveTreeloc cursor_treeloc = get_treeloc(
            pos, fragment_pos, fragment.pointer());

        //! Replace special marks with user-defined data
        String grove_path = (grove && !grove->topSysid().isEmpty())
            ? String(QFileInfo(grove->topSysid()).dir().canonicalPath())
            : DocSrcInfo::myDocumentsPath();
        try {
            plugin()->substitute_marks(fragment.pointer(), grove_path, pos);
        }
        catch (...) {
            return;
        }
        DUMP_POS(pos, NOTR("Insert custom content:"));
        //! Insert custom content
        CommandPtr insert_cmd = 
            se->groveEditor()->paste(fragment, pos, 0, true);
        if (insert_cmd.isNull()) {
            se->sernaDoc()->showMessageBox(SernaDoc::MB_CRITICAL,
                tr("Cannot insert Custom Content"),
                String(tr("Cannot insert Custom Content: ")) +
                    se->groveEditor()->errorMessage(), tr("&Ok"));
            return;
        }
        batch_cmd->executeAndAdd(insert_cmd.pointer());
        DUMP_POS(batch_cmd->pos(), NOTR("batch_cmd->pos():"));

        if (!pos.node()->parent())
            pos = (batch_cmd->lastChild())
                ? cmd_pos(batch_cmd->lastChild()) : GrovePos();
        DUMP_POS(pos, NOTR("pos:"));
        GrovePos cursor_pos = (!pos.isNull())
            ? cursor_treeloc.grovePos(pos.node()->root()) : GrovePos();
        DUMP_POS(cursor_pos, NOTR("cursor_pos:"));

        //! Paste selection
        if (selection_fragment->firstChild()) {
            GrovePos paste_pos = cursor_pos;
            if (paste_pos.isNull())
                paste_pos = (batch_cmd->lastChild())
                    ? cmd_pos(batch_cmd->lastChild()) : GrovePos();
            DUMP_POS(paste_pos, NOTR("paste_pos:"));
            if (!paste_pos.isNull())
                batch_cmd->executeAndAdd(
                    se->groveEditor()->paste(
                        selection_fragment, (!cursor_pos.isNull())
                        ? cursor_pos : batch_cmd->pos()).pointer());
        }
        else
            if (!cursor_pos.isNull())
                batch_cmd->setSuggestedPos(cursor_pos);
        batch_cmd->setUndoPos(initial_pos);
        se->executeAndUpdate(batch_cmd.pointer());
        se->grabFocus();
    } catch(...) {
        se->sernaDoc()->showMessageBox(
            SernaDoc::MB_INFO, tr("Custom Content Insertion"),
            tr("Command execution error while executing custom content"),
            tr("&Ok"));
    }
}

