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
#include "csl/Instructions.h"
#include "csl/Exception.h"
#include "csl/csl_debug.h"
#include "csl/Stylesheet.h"
#include "csl/Template.h"

#include "common/String.h"
#include "common/StringTokenizer.h"
#include "common/Message.h"
#include "common/MessageUtils.h"
#include "utils/tr.h"

#include "xpath/Expr.h"
#include "xpath/Engine.h"
#include "xpath/XpathFunctionFactory.h"
#include "xslt/PatternExpr.h"
#include "grove/Nodes.h"
#include <map>

using namespace Common;

// START_IGNORE_LITERALS
static const uint DEFAULT_COLLECT_TEXT_LENGTH = 80;
static const uint DEFAULT_CUTOFF_LEVEL = 5;
static const char CSL_XMLNS[] = 
    "http://www.syntext.com/Extensions/CSL-1.0";
static const char DEFAULT_PROFILE_NAME[] = "default";
// STOP_IGNORE_LITERALS

namespace Csl {

static String get_attr(const GroveLib::Element* elem, 
                       const String& name, bool required)
{
    if (0 == elem)
        return String::null();
    const GroveLib::Attr* a = elem->attrs().getAttribute(name);
    if (0 == a) {
        if (required)
            throw CslException(elem, 
                tr("Attribute '%0' is required").arg(name));
        return String::null();
    }
    return a->value();
}

static bool get_bool_attr(const GroveLib::Element* elem,
                          const String& name, 
                          bool required, bool default_value = false)
{
    String v = get_attr(elem, name, required);
    if (v.isEmpty())
        return default_value;
    if (v == NOTR("yes") || v == NOTR("true"))      
        return true;
    if (v == NOTR("no")  || v == NOTR("false"))
        return false;
    throw CslException(elem, 
        tr("Bad value for boolean attribute '%0'").arg(name));
}

////////////////////////////////////////////////////////////////////

Text::Text(const GroveLib::Element* elem)
{
    const GroveLib::Node* n = elem->firstChild();
    for (; n; n = n->nextSibling()) 
        if (n->nodeType() == GroveLib::Node::TEXT_NODE)
            text_ += static_cast<const GroveLib::Text*>(n)->data();
}

void Text::dump() const 
{
    DDBG << "\tTextInstruction:<" << text_ << ">\n";
}
    
////////////////////////////////////////////////////////////////////

CollectText::CollectText(const GroveLib::Element* elem)
{
    String lstr = get_attr(elem, NOTR("max-length"), false);
    maxLength_ = lstr.isEmpty() ? 0 : lstr.toUInt();
    if (0 == maxLength_)
        maxLength_ = DEFAULT_COLLECT_TEXT_LENGTH; 
    String sel_attr = get_attr(elem, NOTR("select"), false);
    if (sel_attr.isEmpty())
        sel_attr = ".";
    if (elem->localName() == NOTR("value-of")) 
        selectExpr_ = Xpath::Engine::makeExpr(NOTR("string(") + sel_attr + NOTR(")"), elem);
    else
        selectExpr_ = Xpath::Engine::makeExpr(sel_attr, elem);
    firstNodeOnly_ = get_bool_attr(elem, NOTR("first-node-only"), false, false);
}

void CollectText::dump() const
{
    DDBG << "\tCollectText: maxlen=" << maxLength() << ", selectExpr dump:\n";
    DBG_IF(CSL.TEST) selectExpr().dump();
}

void ValueOf::dump() const
{
    DDBG << "\tValueOf: maxlen=" << maxLength() << ", selectExpr dump:\n";
    DBG_IF(CSL.TEST) selectExpr().dump();
}

////////////////////////////////////////////////////////////////////

Profile::Profile(const GroveLib::Element* elem)
    : name_(get_attr(elem, NOTR("name"), false)),
      inscription_(get_attr(elem, NOTR("inscription"), false)),
      icon_(get_attr(elem, NOTR("icon"), false))
{
    if (name_.isEmpty())
        name_ = DEFAULT_PROFILE_NAME;
    showAttributes_    = get_bool_attr(elem, NOTR("show-attribtues"), false, false);
    showCursorBetween_ = get_bool_attr(elem, NOTR("show-cursor-between-elements"),
        false, true);
    String s = get_attr(elem, NOTR("cutoff-level"), false);
    cutoffLevel_ = s.isEmpty() ? 0 : s.toUInt();
    if (0 == cutoffLevel_)
        cutoffLevel_ = DEFAULT_CUTOFF_LEVEL;
    if (inscription_.isEmpty())
        inscription_ = tr("Default");
    DBG(CSL.LIFE) << "CSL: Profile cons: " << this << std::endl;
}

void Profile::dump() const
{
    DBG_IF(CSL.TEST) {
        DDBG << "Profile <" << name_ << ">, active for: ";
        for (uint i = 0; i < templates().size(); ++i)
            DDBG << '<' << templates()[i] << "> ";
        DDBG << "\n  show-attributes=" << showAttributes_
            << ", show-cursor-between-elements=" << showCursorBetween_ << "\n";
        DDBG << "  cutoff-level=" << cutoffLevel_ << std::endl;
        DDBG << "  inscription=" << inscription_ << std::endl;
        DDBG << "  icon=" << icon_ << std::endl;
    }
}

Profile::~Profile()
{
    DBG(CSL.LIFE) << "~Profile:<" << name_ << ">:" << this << "\n";
}

////////////////////////////////////////////////////////////////////

static String get_instruction(const GroveLib::Node* n)
{
    switch (n->nodeType()) {
        case GroveLib::Node::ELEMENT_NODE: {
            const GroveLib::Element* e = 
                static_cast<const GroveLib::Element*>(n);
            if (CSL_XMLNS == e->xmlNsUri())
                return e->localName();
            return String::null();
        }
        case GroveLib::Node::TEXT_NODE: {
            const GroveLib::Text* t = 
                static_cast<const GroveLib::Text*>(n);
            if (t->isEmpty())
                return String::null();
            const GroveLib::Element* e = CONST_ELEMENT_CAST(parentNode(n));
            throw CslException(e,
                tr("Element '%0' contains junk text").arg(e->nodeName()));
        }
        default:
            return String::null();
    }
}

class Stylesheet::DefinesMap :
    public std::map<String, const GroveLib::Element*> {};

class ArgSubstMap : public std::map<String, String> {
public:
    String get(const String& key) const 
    {
        const_iterator it = find(key);
        return (it != end()) ? it->second : String::null();
    }
};

static String subst_str(const ArgSubstMap& smap, const String& istr)
{
    if (istr.isEmpty())
        return String::null();
    String result;
    result.reserve(istr.length());
    const Char* cp = istr.unicode();
    const Char* ce = cp + istr.length();
    while (cp < ce) {
        if (*cp != '%') {
            result += *cp++;
            continue;
        }
        const Char* tok = ++cp;
        if (tok >= ce)
            break;
        if (*tok == '%') {
            result += *tok;
            ++cp;
            continue;
        }
        if (tok->isLetter()) { // valid name start
            cp = tok;
            while (cp < ce && (cp->isLetter() || cp->isDigit()))
                ++cp;
            result += smap.get(String(tok, cp - tok));
            continue;
        }
        result += '%';
        result += *cp++;
    }
    return result;
}

static void subst_tree(GroveLib::Node* n, const ArgSubstMap& smap)
{
    switch (n->nodeType()) {
        case GroveLib::Node::TEXT_NODE: {
            GroveLib::Text* t = TEXT_CAST(n);
            t->setData(subst_str(smap, t->data()));
            return;
        }
        case GroveLib::Node::ELEMENT_NODE: {
            GroveLib::Element* e = ELEMENT_CAST(n);
            GroveLib::Attr* a = e->attrs().firstChild();
            for (; a; a = a->nextSibling()) {
                if (!a->xmlNsPrefix().isEmpty())
                    continue;
                a->setValueNoNotify(subst_str(smap, a->value()));
            }
            for (n = n->firstChild(); n; n = n->nextSibling())
                subst_tree(n, smap);
            return;
        }
        default:
            return;
    }
}

GroveLib::DocumentFragment*
Stylesheet::processUse(const GroveLib::Element* use_elem) const
{
    String ref = get_attr(use_elem, NOTR("ref"), true);
    DefinesMap::const_iterator it = definesMap_->find(ref);
    if (it == definesMap_->end())
        throw CslException(use_elem, 
            tr("Reference to undeclared csl:define %0").arg(ref));
    GroveLib::NodePtr np(it->second->cloneNode(true));
    ArgSubstMap smap;
    const GroveLib::Attr* a = use_elem->attrs().firstChild();
    for (; a; a = a->nextSibling()) 
        if (a->localName() != NOTR("ref"))
            smap[a->localName()] = a->value();
    subst_tree(np.pointer(), smap);
    GroveLib::DocumentFragment* df = np->firstChild() 
        ? np->firstChild()->takeAsFragment(np->lastChild()) 
        : new GroveLib::DocumentFragment;
    return df;
}

Stylesheet::Stylesheet(const GroveLib::Element* elem)
    : templateList_(new TemplateList),
      definesMap_(new DefinesMap)
{
    DBG(CSL.LIFE) << "Stylesheet Cons: " << this << std::endl;
    GroveLib::Node* n = elem->firstChild();
    while (n) {
        String instr_name = get_instruction(n);
        if (instr_name.isEmpty()) {
            n = n->nextSibling();
            continue;
        }
        const GroveLib::Element* ielem = CONST_ELEMENT_CAST(n);
        if (NOTR("use") == instr_name) {
            n->insertAfter(processUse(ielem));
            GroveLib::Node* next = n->nextSibling();
            n->remove();
            n = next;
            continue;
        }
        if (NOTR("profile") == instr_name)
            profileList_.appendChild(new Profile(ielem));
        else if (NOTR("template") == instr_name) 
            templateList_->appendChild(new Template(ielem));
        else if (NOTR("define") == instr_name) 
            (*definesMap_)[get_attr(ielem, NOTR("name"), true)] = ielem;
        else
            throw CslException(ielem,
                tr("Invalid top-level instruction: %0").arg(instr_name));
        n = n->nextSibling();
    }
    // create default template and profile
    if (!findProfile(DEFAULT_PROFILE_NAME))
        if (profileList_.firstChild())
            profileList_.firstChild()->insertBefore(new Profile(0));
        else
            profileList_.appendChild(new Profile(0));
    // for each profile, find appropriate template
    Profile* prof = profileList_.firstChild();
    for (; prof; prof = prof->nextSibling()) {
        const Template* t = templateList_->firstChild();
        for (; t; t = t->nextSibling()) {
            StringTokenizer st(t->profiles());
            while (st)
                if (st.next() == prof->name())
                    prof->addTemplate(t);
        }
    }
}

void Stylesheet::dump() const
{
    DBG_IF(CSL.TEST) {
        DDBG << "************** CSL STYLESHEET PROFILES:\n";
        const Profile* p = profiles().firstChild();
        for (; p; p = p->nextSibling())
            p->dump();
        DDBG << "CSL STYLESHEET TEMPLATES:\n";
        const Template* t = templates().firstChild();
        for (; t; t = t->nextSibling())
            t->dump();
        DDBG << "***************************************\n";
    }
}
    
const Profile* Stylesheet::findProfile(const String& name) const
{
    const Profile* p = profiles().firstChild();
    for (; p; p = p->nextSibling())
        if (p->name() == name)
            return p;
    return 0;
}

Stylesheet::~Stylesheet()
{
    DBG(CSL.LIFE) << "CSL: ~Stylesheet: " << this << std::endl;
}

////////////////////////////////////////////////////////////////////

// START_IGNORE_LITERALS
static const char* cursor_placements[] = {
    "cursor-inside-end", "cursor-inside-begin",
    "cursor-before", "cursor-after", 0
};
// STOP_IGNORE_LITERALS

static Instruction* make_instruction(const String& instr_name,   
                                     const GroveLib::Node* n)
{
// START_IGNORE_LITERALS
    if (instr_name == "text")
        return new Text(CONST_ELEMENT_CAST(n));
    else if (instr_name == "collect-text")
        return new CollectText(CONST_ELEMENT_CAST(n));
    else if (instr_name == "value-of")
        return new ValueOf(CONST_ELEMENT_CAST(n));
    else if (instr_name == "number")
        return new Number(CONST_ELEMENT_CAST(n));
    else
        throw CslException(CONST_ELEMENT_CAST(n),
            tr("Unknown CSL instruction: %0").arg(instr_name));
// STOP_IGNORE_LITERALS
}

Template::Template(const GroveLib::Element* elem)
    : cursPlacement_(0),
      doFold_(get_bool_attr(elem, NOTR("fold"), false, false)),
      mixedOnly_(get_bool_attr(elem, NOTR("mixed-only"), false, false))
{
    fontStyle_ = get_attr(elem, NOTR("font-style"), false);
    fontWeight_ = get_attr(elem, NOTR("font-weight"), false);
    fontDecoration_ = get_attr(elem, NOTR("text-decoration"), false);

    String lstr = get_attr(elem, NOTR("max-length"), false);
    maxLength_ = lstr.isEmpty() ? 0 : lstr.toUInt();
    if (0 == maxLength_)
        maxLength_ = DEFAULT_COLLECT_TEXT_LENGTH; 

    String match_str = get_attr(elem, NOTR("match"), false);
    if (match_str.isEmpty())
        match_str = "*";    // match all elements
    matchPattern_ = Xslt::PatternExpr::makeSimplePattern(match_str, elem);
    profiles_ = get_attr(elem, NOTR("profiles"), false);
    if (profiles_.isEmpty())
        profiles_ = DEFAULT_PROFILE_NAME;
    String cur_str = get_attr(elem, NOTR("cursor-placement"), false);
    for (const char** cv = cursor_placements; *cv; ++cv) {
        if (*cv == cur_str) {
            cursPlacement_ = cv - cursor_placements;
            break;
        }
    }
    cur_str = get_attr(elem, NOTR("cursor-xpath"), false);
    if (!cur_str.isEmpty())
        cursorExpr_ = Xpath::Engine::makeExpr(cur_str, elem);
    const GroveLib::Node* n = elem->firstChild();
    Instruction* instr = 0;
    for (; n; n = n->nextSibling()) {
        String instr_name = get_instruction(n);
        if (instr_name.isEmpty())
            continue;
        if (instr_name == NOTR("when-closed")) {
            const GroveLib::Node* n2 = n->firstChild();
            for (; n2; n2 = n2->nextSibling()) {
                instr_name = get_instruction(n2);
                if (instr_name.isEmpty())
                    continue;
                instr = make_instruction(instr_name, n2);
                if (instr)
                    closedInstructions_.appendChild(instr);
            }
            continue;
        }
        instr = make_instruction(instr_name, n);
        if (instr)
            instructions_.appendChild(instr);
    }
}

Template::Template()
    : doFold_(false), mixedOnly_(false), 
      maxLength_(DEFAULT_COLLECT_TEXT_LENGTH)
{
    RefCntPtr<GroveLib::Element> e = new GroveLib::Element(NOTR("csl:template"));
    matchPattern_ =
        Xslt::PatternExpr::makeSimplePattern(NOTR("node()[0]"), e.pointer());
    DBG(CSL.LIFE) << "CSL: Empty template cons:" << this << std::endl;
}

void Template::dump() const
{
    DBG_IF(CSL.TEST) {
        DDBG << "Template " << this << ", fold=" << doFold() 
            << ", MP dump: \n";
        matchPattern().dump();
        if (!profiles_.isEmpty())
            DDBG << "  profiles: " << profiles_ << std::endl;
        DDBG << " sub-instructions:\n";
        const Instruction* i = instructions(false).firstChild();
        for (; i; i = i->nextSibling())
            i->dump();
        DDBG << " CLOSED sub-instructions:\n";
        i = instructions(true).firstChild();
        for (; i; i = i->nextSibling())
            i->dump();
        DDBG << "----------------\n";
    }
}

Template::~Template()
{
    DBG(CSL.LIFE) << "CSL: ~Template: " << this << std::endl;
}

///////////////////////////////////////////////////////////////

class DitaFunctionFactory : public Xpath::XpathFunctionFactory {
public:
    virtual int optimizationFlags() const { return OPT_NORMAL|OPT_DITA; }
};

static const Xpath::FunctionFactory* 
func_factory(const GroveLib::Node* node)
{
    static Xpath::XpathFunctionFactory xpath_fact;
    static DitaFunctionFactory dita_fact;
    const GroveLib::Element* elem = 
        node->grove()->document()->documentElement();
    if (0 == elem)
        return &xpath_fact;
    String opt = get_attr(elem, NOTR("optimization"), false);
    if (opt.contains(NOTR("dita")))
        return &dita_fact;
    return &xpath_fact;
}

Number::Number(const GroveLib::Element* elem)
    : NumberInstructionBase(elem, func_factory(elem))
{
    if (!errorId())
        return;
    UintIdMessage msg(errorId(), 5);
    throw CslException(elem, msg.format(BuiltinMessageFetcher::instance()));
}

void Number::dump() const 
{
    NumberInstructionBase::dump();
}

///////////////////////////////////////////////////////////////

} // namespace Csl

