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
#include "grove/GroveBuilder.h"
#include "grove/grove_trace.h"
#include "grove/Decls.h"
#include "grove/Grove.h"
#include "grove/Node.h"
#include "grove/EntityDecl.h"
#include "grove/XmlValidator.h"
#include "grove/MatchPattern.h"
#include "common/Message.h"
#include "common/MessageUtils.h"
#include "common/PropertyTree.h"
#include <iostream>

USING_COMMON_NS

GROVE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////

class DefaultGroveMessenger : public Messenger {
public:
    virtual void        dispatch(RefCntPtr<Message>& message);
    virtual Messenger*  copy() const { return 0; }
};

void DefaultGroveMessenger::dispatch(RefCntPtr<Message>& msg)
{
    for (ulong i = 0; i < msg->nArgs(); ++i) {
        const MessageArgBase* a = msg->getArg(i);
        if (a->isContext())
            DDBG << a->format();
    }
    DDBG << msg->format(BuiltinMessageFetcher::instance()) << std::endl;
}

////////////////////////////////////////////////////////////////////////////

GrovePtr GroveBuilder::buildGrove(const Vector<String>& files, 
                                  const String&,
                                  bool dtdValidate)
{
    (void) dtdValidate;
    (void) files;
    return 0;
}

GrovePtr GroveBuilder::buildGrove(const String&, const String&, bool)
{
    return 0;
}

GrovePtr GroveBuilder::buildGroveFromFile(const String& fn, bool dtdValidate)
{
    DBG(GROVE.BUILDER) << "Building grove from file <" << fn << ">\n";
    Vector<String> v;
    v.push_back(fn);
    return buildGrove(v, String(), dtdValidate);
}

GroveBuilder& GroveBuilder::operator=(const GroveBuilder& o)
{
    messenger_ = o.messenger_;
    spath_     = o.spath_;
    encoding_  = o.encoding_;
    flags_     = o.flags_;
    numberOfElements_ = o.numberOfElements_;
    rootGrove_ = o.rootGrove_;
    validatorProvider_ = o.validatorProvider_;
    catMgr_ = o.catMgr_;
    matchPatternFactory_ = 
        o.matchPatternFactory_ ? o.matchPatternFactory_->copy() : 0;
    scopeDefs_ = o.scopeDefs_;
    nsMap_ = o.nsMap_;
    return *this;
}

GroveBuilder::GroveBuilder(const GroveBuilder& o)
{
    (void) operator=(o);
}

GroveBuilder* GroveBuilder::copy() const
{
    return new GroveBuilder(*this);
}

void GroveBuilder::setValidatorProvider(const XmlValidatorProvider* vp)
{
    validatorProvider_ = const_cast<XmlValidatorProvider*>(vp);
}

static CatMgr::CatalogManager* gb_catmgr;

void GroveBuilder::setDefaultCatalogManager(CatMgr::CatalogManager* catmgr)
{
    gb_catmgr = catmgr;
}

GroveBuilder::GroveBuilder(int f)
  : messenger_(new DefaultGroveMessenger),
    encoding_(Encodings::XML), flags_((Flags)f),
    numberOfElements_(0), catMgr_(gb_catmgr), 
    rootGrove_(0), validatorProvider_(0)
{
}

void GroveBuilder::setIdManagerParams(Common::PropertyNode* scopeDefs,
                                      Common::PropertyNode* nsMap,
                                      MatchPatternFactory* factory)
{
    scopeDefs_ = scopeDefs;
    nsMap_ = nsMap;
    matchPatternFactory_ = factory;
}

void GroveBuilder::setCatalogManager(CatMgr::CatalogManager* catmgr)
{
    catMgr_ = catmgr;
}

CatMgr::CatalogManager* GroveBuilder::getCatalogManager() const
{
    return catMgr_;
}

Common::String GroveBuilder::flagsAsString() const
{
    String flags;
#ifndef NDEBUG
    if (flags_ & SGML)
        flags += ",SGML";
    if (flags_ & noIdProcessing)
        flags += ",noIdProcessing";
    if (flags_ & noProlog)
        flags += ",noProlog";
    if (flags_ & noMarkedSections)
        flags += ",noMarkedSections";
    if (flags_ & noEntitySections)
        flags += ",noEntitySections";
    if (flags_ & noComments)
        flags += ",noComments";
    if (flags_ & noAttrGroups)
        flags += ",noAttrGroups";
    if (flags_ & noExtSubsetDecls)
        flags += ",noExtSubsetDecls";
    if (flags_ & lineInfo)
        flags += ",lineInfo";
    if (flags_ & processRedlines)
        flags += ",processRedlines";
    if (flags_ & processXinclude)
        flags += ",processXinclude";
    if (flags_ & disableXincludeBackrefs)
        flags += ",disableXincludeBackrefs";
    if (flags_ & checkLocks)
        flags += ",checkLocks";
    if (flags_ & doLocks)
        flags += ",doLocks";
    flags = flags.mid(1);
#endif
    return flags;
}

GroveBuilder::~GroveBuilder()
{
}

GROVE_NAMESPACE_END

