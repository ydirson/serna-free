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
#include "xslt/xslt_defs.h"
#ifdef USE_XPATH3
#include "xslt/PatternExpr.h"
#include "xslt/XsltResource.h"
#include "xslt/DocumentCache.h"
#include "xslt/impl/FunctionExprImpl.h"
#include "xslt/impl/DocumentHolder.h"
#include "xslt/impl/WhitespaceStripper.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xslt/impl/instances/TopStylesheetInst.h"
#include "xslt/impl/debug.h"
#include "xslt/impl/utils.h"
#include "xslt/impl/xpath_values.h"
#include "grove/Nodes.h"
#include "grove/Grove.h"
#include "grove/XmlValidator.h"
#include "grove/MatchPattern.h"
#include "common/StringTokenizer.h"
#include "common/Vector.h"
#include "spgrovebuilder/SpGroveBuilder.h"

using namespace Common;

namespace Xslt {

typedef Common::Vector<CachedDocumentPtr> CachedDocVec;

////////////////////////////////////////////////////////////////////

// trick (apg): we release cached documents only after update finish,
// to avoid premature deletion of cached document grove

class DocumentFuncSubinst : public Xpath::ExprSubInst {
public:
    virtual int type() const { return XSLT_SUBINST + XsltFunctionId::Document; }
    CachedDocVec instanceDocs_;
};

static Xpath::ConstValueImplPtr grove_result(GroveLib::Grove* grove)
{
    Xpath::NodeSet node_set;
    if (grove && grove->document())
        node_set += grove->document();
    return new Xpath::NodeSetValue(node_set);
}

template<> Xpath::ConstValueImplPtr
FunctionExprImpl<XsltFunctionId::Document>::eval(const Xpath::NodeSetItem& nsi,
                                             Xpath::ExprInst& ei) const
{
    const Instance& ci = static_cast<const Instance&>(ei.exprContext());
    DocumentCache* dc = ci.topStylesheetInst()->documentCache();
    DocumentFuncSubinst* subInst = static_cast<DocumentFuncSubinst*>
        (ei.findSubInst(Xpath::ExprSubInst::XSLT_SUBINST + 
            XsltFunctionId::Document));
    if (0 == subInst) {
        subInst = new DocumentFuncSubinst;
        ei.appendSubInst(subInst);
    }
    Xpath::FuncArgEvaluator fargs(nsi, ei, this);
    const GroveLib::Element* instrNode = ci.instruction()->element();
    const GroveLib::Node* originNode = instrNode;
    if (fargs.argNum() < 1 || fargs.argNum() > 2)
        throw Xslt::Exception(XsltMessages::xpathEvalDocArgs,
                              ei.exprContext().contextString());
    String uri = fargs.arg(0)->to(Xpath::Value::STRING, &ei)->getString();
    if (uri.isEmpty()) 
        return grove_result(originNode->grove());
    if (fargs.argNum() == 2) {
        if (fargs.arg(1)->type() != Xpath::Value::NODESET)
            throw Xslt::Exception(XsltMessages::xpathEvalDocParam,
                                  ei.exprContext().contextString());
        const Xpath::NodeSet& ns = fargs.arg(1)->getNodeSet();
        if (ns.first())
            originNode = fargs.arg(1)->getNodeSet().first()->node();
    }

    const TopStylesheet* ts = ci.styleContext().topStylesheet();
    String baseUri;
    if (!originNode->grove())
        baseUri = *ts->docHolder()->uriStack().begin();
    else if (originNode->grove())
        baseUri = originNode->grove()->topSysid();
    else
        baseUri = ts->docHolder()->findUri(originNode);
    DBG(XSLT.DOCFUNC) << "document(): origin->sysid=" 
        << originNode->grove()->topSysid() << std::endl;
    baseUri = getRelativePath(uri, baseUri);
    DBG(XSLT.DOCFUNC) << "document(): calculated URI=" << baseUri << std::endl;

    // check if document() points to some already loaded document grove
    GroveLib::Grove* gp = ci.topStylesheetInst()->nsiContext().node()->grove();
    if (gp->topSysid() == baseUri) {
        DBG(XSLT.DOCFUNC) << "document(): Returned main grove\n";
        return grove_result(gp);
    }
    for (gp = gp->firstChild(); gp; gp = gp->nextSibling()) {
        if (gp->topSysid() == baseUri) {
            DBG(XSLT.DOCFUNC) << "document(): Returned subgrove\n";
            return grove_result(gp);
        }
    }
    CachedDocumentPtr d = 
        dc->getDoc(baseUri, instrNode, nsi.node(), false);
    if (d.isNull()) 
        d = dc->getDoc(baseUri, instrNode, nsi.node(), true);
    if (d.isNull())
        return grove_result(0);
    CachedDocVec& dv = subInst->instanceDocs_;
    // do not release docs so they may be reused later
    uint i = 0;
    for (; i < dv.size(); ++i)
        if (dv[i].pointer() == d.pointer())
            break;
    if (i == dv.size())
        subInst->instanceDocs_.push_back(d);
    return grove_result(d->grove());
}

///////////////////////////////////////////////////////////////////

CachedDocumentPtr DocumentCache::getDoc(const COMMON_NS::String& baseUri,
                                        const GroveLib::Element* instrNode,
                                        const GroveLib::Node* contextNode,
                                        bool doLoad)
{
    const GroveLib::Grove* top_grove = 
        topInst_->nsiContext().node()->grove();
    bool ignore_errors = false;
    bool hide_progress = false;
    bool read_write = false;
    bool validate = false;
    bool process_xinclude = top_grove->groveBuilder()->flags() & 
        GroveLib::GroveBuilder::processXinclude;
    const GroveLib::Attr* mode_attr = 
        instrNode->attrs().getAttribute(NOTR("xse:document-mode"));
    if (mode_attr) {
        StringTokenizer st(mode_attr->value());
        while (st) {
            String s = st.next();
            if (NOTR("ignore-errors") == s)
                ignore_errors = true;
            else if (NOTR("hide-progress") == s)
                hide_progress = true;
            else if (NOTR("read-write") == s)
                read_write = true;
            else if (NOTR("validate") == s)
                validate = true;
            else if (NOTR("process-xinclude") == s)
                process_xinclude = true;
            else if (NOTR("dont-process-xinclude") == s)
                process_xinclude = false;
        }
    }
    DBG(XSLT.DOCFUNC) << "document()/getDoc: doLoad = " << doLoad 
        << "> baseUri=<" << baseUri << ">\n";

    for (CachedDocument* cd = docs_.firstChild(); cd; cd = cd->nextSibling()) {
        if (baseUri == cd->uri()) {
            DBG(XSLT.DOCFUNC) << "DocFunc: found in cache: "
                << baseUri << std::endl;
            return cd;
        }
    }
    if (!doLoad)
        return 0;
    const TopStylesheet* ts = topInst_->styleContext().topStylesheet();
    GroveLib::SpGroveBuilder* grb =
        new GroveLib::SpGroveBuilder(GroveLib::GroveBuilder::pureXmlData |
            (process_xinclude ? GroveLib::GroveBuilder::processXinclude : 0));
    if (ignore_errors) 
        grb->setMessenger(silentMessenger_.pointer());
    else
        grb->setMessenger(topInst_->mstream()->getMessenger());
    GroveLib::GroveBuilder* top_builder = top_grove->groveBuilder();
    grb->setCatalogManager(top_builder->getCatalogManager());
    DBG(XSLT.DOCFUNC) << "document(): sdefs=" 
        << top_builder->scopeDefs() << ", nsMap="
        << top_builder->nsMap() << ", mpf="
        << top_builder->matchPatternFactory() << std::endl;
    grb->setIdManagerParams(top_builder->scopeDefs(), 
        top_builder->nsMap(), top_builder->matchPatternFactory() ? 
            top_builder->matchPatternFactory()->copy() : 0);
    const GroveLib::XmlValidatorProvider* xvp = 0;
    if (validate) {
        xvp = contextNode->grove()->groveBuilder()->getValidatorProvider();
        DBG(XSLT.DOCFUNC) << "document(): Validate ON, xvp=" 
            << xvp << std::endl;
        grb->setValidatorProvider(xvp);
    }
    MessageStream* const pstream = hide_progress ? 0 : topInst_->pstream();
    if (pstream) 
        *pstream << String("XSLT: Parsing " + baseUri);
    static const char transform_msg[] = "XSLT: Transforming document...";
    GroveLib::GrovePtr g = grb->buildGroveFromFile(baseUri);
    if (!g.isNull() && g->document() && 
        (ignore_errors || g->document()->documentElement())) {
        if (!read_write)
            g->setRegisterVisitors(false);
        docs_.appendChild(new CachedDocument(g.pointer(), baseUri));
        if (g->document()->documentElement()) {
            ts->whitespaceStripper()->stripTree
                (g->document()->documentElement());
            if (xvp) {
                GroveLib::XmlValidatorPtr xv =
                    xvp->getValidator(g->document()->grove());
                int vmode = GroveLib::XmlValidator::OPEN_MODE;
                if (hide_progress)
                    vmode = GroveLib::XmlValidator::HIDE_PROGRESS;
                if (xv)
                    xv->validate(g->document()->documentElement(), vmode);
            }
        }
        if (pstream)
            *pstream << String(transform_msg);
        return docs_.lastChild();
    }
    if (pstream)
        *pstream << String(transform_msg);
    return 0;
}

DocumentCache::DocumentCache(TopStylesheetInst* tsi)
    : topInst_(tsi),
      silentMessenger_(new SilentMessenger)
{
}

CachedDocument::CachedDocument(GroveLib::Grove* g,
                               const String& uri)
    : grove_(g), uri_(uri)
{
}

CachedDocument::~CachedDocument()
{
    DBG(XSLT.DOCFUNC) << "~CachedDocument: " << uri_ << std::endl;
}

DocumentCache::~DocumentCache()
{
}

} // namespace Xslt

#endif // USE_XPATH3
