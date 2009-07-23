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
#include "common/String.h"
#include "common/StringCvt.h"
#include "common/XTreeIterator.h"
#include "grove/Nodes.h"
#include "grove/PrologNodes.h"
#include "utils/DocTemplate.h"
#include "utils/utils_debug.h"
#include "utils/DocSrcInfo.h"
#include "dav/DavManager.h"
#include "dav/IoStream.h"

#include <stdexcept>
#include <string>

USING_COMMON_NS

using namespace DocSrcInfo;

UTILS_EXPIMP nstring detect_encoding(const String& filename);

namespace DocTemplate {

const char* const SERNA_PI_NAME     = NOTR("syntext-serna");

class DtPrologParser {
public:
    class Eof {};
    DtPrologParser(PropertyNode* ptree)
        : ptree_(ptree) {}

    bool            parse(const String& ifn);

private:
    Dav::IoStream&  is() { return is_; }
    Char            get();
    void            skip_ws();
    void            check(char c);
    bool            parse_prolog_stmt();
    void            parse_pi();
    void            parse_serna_pi();
    void            parse_xml_pi();
    void            parse_xml_stylesheet_pi();
    bool            parse_av(String& name, String& val, bool& additive);
    void            parse_comment_or_dt();
    void            parse_dt();
    static bool     isxmlnc(Char c)
    {
        return (c.isLetterOrNumber() || c == '-' || c == '_' || c == ':');
    }
    PropertyNode*           ptree_;
    Dav::IoStream           is_;
    Char                    lastChar_;
};

PiReader::PiReader(const String& filename)
{
    DtPrologParser pp(this->root());
    if (!pp.parse(filename))
        return;
}

static void make_pi(GroveLib::Grove* g, GroveLib::Node* targ,
                    const String& target, const String& data)
{
    RefCntPtr<GroveLib::ProcessingInstruction> np =
        new GroveLib::ProcessingInstruction;
    np->setTarget(target);
    np->setData(data);
    RefCntPtr<GroveLib::SSepNode> sn = new GroveLib::SSepNode;
    sn->setData("\n");
    if (targ) {
        targ->insertBefore(np.pointer());
        targ->insertBefore(sn.pointer());
    } else {
        g->prolog()->appendChild(np.pointer());
        g->prolog()->appendChild(sn.pointer());
    }
}

void PiReader::updatePi(GroveLib::Grove* g, const PropertyNode* fromSet)
{
    // first, remove all known PI's
    GroveLib::Node* n = g->prolog()->firstChild();
    GroveLib::Node* xmlTarg = 0;
    while (n) {
        if (n->nodeType() == GroveLib::Node::COMMENT_NODE
            || n->nodeType() == GroveLib::Node::SSEP_NODE) {
                n = n->nextSibling();
                continue;
        }
        if (n->nodeType() != GroveLib::Node::PI_NODE)
            break;
        GroveLib::ProcessingInstruction* pn =
            static_cast<GroveLib::ProcessingInstruction*>(n);
        if (pn->target() == NOTR("xml")) {
            xmlTarg = pn->nextSibling(); // mark location to insert before
            n = n->nextSibling();
            continue;
        }
        if (SERNA_PI_NAME == pn->target()) {
            GroveLib::Node* nn = n->nextSibling();
            n->remove();
            n = nn;
        }
    }
    if (!xmlTarg)
        xmlTarg = g->prolog()->firstChild();

    // traverse property set bottom-up, make PI's
    XTreeDfsIterator <const PropertyNode> iter(fromSet);
    String av;
    for (;;) {
        while (iter.node() && iter.node()->firstChild())
            ++iter;
        if (0 == iter.node())
            break;
        String pname;
        const PropertyNode* pn = iter.node();
        for (; pn; pn = pn->parent()) {
            if (pn->parent() && pn->parent()->parent())
                pname.prepend('/');
            pname.prepend(pn->name());
        }
        const String& pval = iter.node()->getString();
        if ((av.length() + pname.length() + pval.length()) > 58) {
            make_pi(g, xmlTarg, SERNA_PI_NAME, av);
            av = NOTR("");
        }
        /* TODO:
                check_literals.py BUG. It can not correctly works
                with strings which holding escaped quotas.
        */
        // START_IGNORE_LITERALS
        av += String(av.length() ? " " : "") + pname + "=\"" + pval + "\"";
        // STOP_IGNORE_LITERALS
    }
    if (av.length())
        make_pi(g, xmlTarg, SERNA_PI_NAME, av);
    if (xmlTarg && xmlTarg->nodeType() == GroveLib::Node::SSEP_NODE)
        static_cast<GroveLib::SSepNode*>(xmlTarg)->setData(NOTR("\n"));
}

PiReader::~PiReader()
{
}

//////////////////////////////////////////////////////////////////

typedef Common::Encodings ENC;

bool DtPrologParser::parse(const COMMON_NS::String& ifn)
{
    if (Dav::DavManager::instance().open(ifn, Dav::DAV_OPEN_READ, is_) !=
        Dav::DAV_RESULT_OK) {
            ptree_->makeDescendant("#url-open-failed", "true", true);
            return false;
    }
    is_.setEncoding(Encodings::UTF_8);  // UTF-16 autodetected
    String temp_enc;                    
    try {
        if (parse_prolog_stmt()) {
            temp_enc = ptree_->getSafeProperty(DOC_ENCODING)->getString();
            Encodings::Encoding enc = Encodings::encodingByName(temp_enc);
            if (enc != Encodings::XML && enc != Encodings::UTF_8
                && enc != Encodings::UTF_16) {
                    is_.setEncoding(enc);
            }
            temp_enc = Encodings::encodingName(enc);
        }
        ptree_->makeDescendant(GENERATE_UNICODE_BOM)->setBool(
            is_.generateByteOrderMark());
        is_.rewind();       // rewind the stream
        lastChar_ = 0;
        while (parse_prolog_stmt())
            ;
    }
    catch (Eof&) {}
    if (!temp_enc.isEmpty())
        ptree_->makeDescendant(DOC_ENCODING, temp_enc);
    return true;
}

Char DtPrologParser::get()
{
    Char c;
    if (to_int_type(lastChar_)) {
        c = lastChar_;
        lastChar_ = 0;
    } else {

        if (is_.atEof())
            throw Eof();
        is_ >> c;
    }
    return c;
}

void DtPrologParser::skip_ws()
{
    Char c;
    while ((c = get()).isSpace())
        ;
    lastChar_ = c;
}

void DtPrologParser::check(char c)
{
    Char ic = get();
    if (ic != c)
        throw Eof();
}

bool DtPrologParser::parse_prolog_stmt()
{
    skip_ws();
    Char c = get();
    if (c != '<')           // each prolog stmt must start with '<'
        return false;
    c = get();
    if (c == '?')
        parse_pi();
    else if (c == '!')
        parse_comment_or_dt();
    else {
        String docelem;
        if (!isxmlnc(c))
            return false;
        docelem += c;
        for (;;) {
            c = get();
            if (!isxmlnc(c))
                break;
            docelem += c;
        }
        const PropertyNode* dp = ptree_->getProperty(DOCINFO_DOCELEM);
        if (dp)
            docelem = dp->getString();
        DBG(UTILS.DT) << "making docelem=" << docelem << std::endl;
        ptree_->makeDescendant(DOCINFO_DOCELEM, docelem);
        bool additive = false;
        for (;;) {
            String attname, attval, sloc;
            bool finished = parse_av(attname, attval, additive);
            DBG(UTILS.DT) << "parsed docelem av: "
                << attname << "=<" << attval << ">\n";
            GroveLib::QualifiedName qname;
            qname.parse(attname);
            if (qname.localName() == NOTR("noNamespaceSchemaLocation")) {
                ptree_->makeDescendant(NONS_SCHEMA_PATH, attval, true);
                break;
            }
            if (finished)
                throw Eof();
        }
        return false;
    }
    skip_ws();
    check('>'); // end of statement
    return true;
}

// parse: pi-name att="val" att="val" ?
void DtPrologParser::parse_pi()
{
    DBG(UTILS.DT) << "parse_pi entered" << std::endl;
    Char c;
    String pi_name;
    for (;;) {
        c = get();
        if (isxmlnc(c))
            pi_name += c;
        else
            break;
    }
    if (pi_name == SERNA_PI_NAME)
        parse_serna_pi();
    else if (pi_name == NOTR("xml"))
        parse_xml_pi();
    else {
        while ((c = get()) != '?') // skip unknown PI's
            ;
        lastChar_ = 0;
        DBG(UTILS.DT) << "Unknown PI parsed: " << pi_name << std::endl;
    }
}

void DtPrologParser::parse_serna_pi()
{
    String attname, attval;
    for (;;) {
        bool additive = false;
        bool last = parse_av(attname, attval, additive);
        PropertyNode* pn = ptree_->makeDescendant(attname, attval);
        if (additive)
            pn->makeDescendant("##additive##");
        if (last)
            break;
    }
    DBG(UTILS.DT) << "Serna PI parsed OK" << std::endl;
}

void DtPrologParser::parse_xml_stylesheet_pi()
{
    String attname, attval, href;
    for (;;) {
        bool additive;
        bool last = parse_av(attname, attval, additive);
        if (attname == NOTR("href"))
            href = attval;
        if (last)
            break;
    }
    DBG(UTILS.DT) << "xml-stylesheet PI parsed OK" << std::endl;
}

void DtPrologParser::parse_xml_pi()
{
    String attname, attval, encoding;
    for (;;) {
        bool additive;
        bool last = parse_av(attname, attval, additive);
        if (attname == NOTR("encoding"))
            encoding = attval;
        if (last)
            break;
    }
    if (!encoding.isEmpty())
        ptree_->makeDescendant(DOC_ENCODING, encoding);
    DBG(UTILS.DT) << "xml-pi parsed: encoding: " << encoding
        << std::endl;
}

bool DtPrologParser::parse_av(String& attname,
                                      String& attval,
                                      bool& additive)
{
    Char c;
    attname = attval = "";
    skip_ws();
    while (isxmlnc(c = get()))
        attname += c;
    if (c == '+') {
        additive = true;
        c = get();
    } else
        additive = false;
    lastChar_ = c;
    check('=');
    skip_ws();
    Char quote = get();
    if (quote != '"' && quote != '\'') {
        lastChar_ = quote;
        check('"');
    }
    for (;;) {
        c = get();
        if (c == quote)
            break;
        attval += c;
    }
    DBG(UTILS.DT) << "Parsed A/V: <" << attname << ">/<"
        << attval << ">" << std::endl;
    skip_ws();
    c = get();
    if (c == '?' || c == '>')
        return true;        // last
    lastChar_ = c;
    return false;           // not last
}

void DtPrologParser::parse_comment_or_dt()
{
    skip_ws();
    Char c = get();
    if (c == '-') {
        check('-'); // <!--
        while ((c = get()) != '-')
            ;
        lastChar_ = c;
        for (;;) {
            c = get();
            if (c == '-') {
                c = get();
                lastChar_ = c;
                if (c == '>')      // --> - end of comment
                    return;
            }
        }
    }
    if (c == 'd' || c == 'D') {
        String ds;
        while ((c = get()).isLetter())
            ds += c;
        if (ds.lower() != NOTR("octype"))
            throw Eof();
        skip_ws();
        parse_dt();
        return;
    }
    throw Eof();
}

void DtPrologParser::parse_dt()
{
    Char c;
    String dtString;
    while ((c = get()) != '[' && c != '>')
        dtString += c;
    lastChar_ = c;
    ptree_->makeDescendant(DOCINFO_DOCTYPE, dtString);

    const Char* cp = dtString.unicode();
    const Char* ce = cp + dtString.length();
    const Char* cp1;
    bool quot = false;
    String s1, pubid, sysid;
    while (cp < ce && cp->isSpace())
        ++cp;
    cp1 = cp;
    while (cp < ce && !cp->isSpace())
        ++cp;
    ptree_->makeDescendant(DOCINFO_DOCELEM, String(cp1, cp - cp1));
    while (cp < ce && cp->isSpace())
        ++cp;
    if (cp >= ce)
        goto dt_done;
    cp1 = cp;
    while (cp < ce && cp->isLetter())
        ++cp;
    if (cp <= cp1)
        goto dt_done;
    s1 = String(cp1, cp - cp1);
    if (s1.upper() == NOTR("PUBLIC")) {
        while (cp < ce && cp->isSpace())
            ++cp;
        if (cp >= ce || *cp++ != '"')
            goto dt_done;
        while (cp < ce && *cp != '"')
            pubid += *cp++;
        ++cp;
    }
    ptree_->makeDescendant(DOCINFO_PUBID, pubid);
    while (cp < ce && cp->isSpace())
        ++cp;
    if (cp >= ce || *cp == '>')
        goto dt_done;
    if (*cp == '"') {
        ++cp;
        quot = true;
    }
    while (cp < ce) {
        if (quot && *cp == '"')
            break;
        sysid += *cp;
        ++cp;
    }
    ptree_->makeDescendant(DOCINFO_SYSID, sysid);
dt_done:
    DBG(UTILS.DT) << "dtString='" << dtString << "'\n";
    skip_ws();
    c = get();
    if (c == '>') {
        lastChar_ = c;
        return;
    }
    if (c == '[') {
        for (;;) {
            while ((c = get()) != ']')
                ;
            c = get();
            if (c == '>') {
                lastChar_ = c;
                break;
            }
        }
    }
}

}

UTILS_EXPIMP nstring detect_encoding(const String& filename)
{
    using namespace DocTemplate;
    PropertyNodePtr pnPtr(new PropertyNode);
    DtPrologParser pp(&*pnPtr);
    nstring encoding;
    if (pp.parse(filename)) {
        const PropertyNode* enc_pn = pnPtr->getSafeProperty(DOC_ENCODING);
        encoding = to_string<nstring>(enc_pn->getString());
    }
    return encoding;
}
