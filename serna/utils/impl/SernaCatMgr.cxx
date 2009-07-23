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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#include "utils/SernaCatMgr.h"
#include "utils/Config.h"
#include "utils/Properties.h"
#include "utils/MsgBoxStream.h"

#include "proputils/PropertyAccessor.h"
#include "proputils/VariableResolver.h"

#include "grove/GroveBuilder.h"
#include "urimgr/Resource.h"

#include "common/Singleton.h"
#include "common/CommandEvent.h"
#include "common/PropertyTree.h"
#include "common/PropertyTreeEventFactory.h"
#include "common/PathName.h"
#include "common/Vector.h"
#include "common/RangeIterator.h"
#include "common/StringTokenizer.h"

#include "catmgr/CatalogManager.h"
#include <algorithm>
#include <iostream>

using CatMgr::CatalogManager;
using CatMgr::global_catalog;
using PropUtils::PropertyAccessor;
using namespace Common;

SernaCatMgr::SernaCatMgr() {}

class SernaCatMgrImpl : public SernaCatMgr, public CommandEventContext {
public:
    SernaCatMgrImpl();
    void updateCatalogs();

private:
    String  catList_;
};

SIMPLE_COMMAND_EVENT_IMPL(UpdateCatalogs, SernaCatMgrImpl)

bool UpdateCatalogs::doExecute(SernaCatMgrImpl* scm, EventData*)
{
    scm->updateCatalogs();
    return true;
}

static PropertyAccessor get_xcf_props()
{
    String xcpRootName(from_latin1(XmlCatalogs::XML_CATALOGS));
    PropertyNode* ptn = config().root()->getProperty(xcpRootName);
    if (0 == ptn)
        ptn = config().root()->makeDescendant(xcpRootName);
    return PropertyAccessor(ptn);
}

static const char XCF_VARREF[] = NOTR("$XML_CATALOG_FILES");

static bool subst(ustring& dst, const ustring& pat, const ustring& repl)
{
    unsigned pos = dst.find(pat);
    if (ustring::npos == pos)
        return false;
    unsigned const patlen = pat.size(), afterpat = pos + patlen;
    if (afterpat < dst.size() && isalnum(to_int_type(dst[afterpat])))
        return false;
    dst.replace(pos, patlen, repl);
    return true;
}

struct IsPathSep {
    bool operator()(Char c1, Char c2)
    {
        return c1 == c2 && PathName::PATH_SEP == c1;
    }
};

static Vector<String> make_catlist(const String& catalogs)
{
    ustring cats(catalogs);

    ustring varref(from_latin1(NOTR("$SERNA_XML")));
    ustring repl(config().getProperty(NOTR("vars/xml"))->getString());
    while (subst(cats, varref, repl));

    varref = to_lower<ustring>(varref);
    while (subst(cats, varref, repl));

    const Char* end = std::unique(cats.begin(), cats.end(), IsPathSep());
    if (cats.begin() != end && PathName::PATH_SEP == *(end - 1))
        --end;
    cats.resize(end - cats.begin());
    if (0 != cats.size() && PathName::PATH_SEP == cats[0])
        cats.erase(0, 1);

    Vector<String> catVec;
    SepRangeIter<Char> path_iter(make_range(cats), PathName::PATH_SEP);
    for (String uri; cats.end() != path_iter; ++path_iter) {
        uri.assign(path_iter->begin(), path_iter->size());
        if (XCF_VARREF == uri) {
            const char* xcfEnv = getenv(XCF_VARREF + 1);
            ustring xcfRepl(xcfEnv ? from_local_8bit(xcfEnv) : String::null());

            SepRangeIter<Char> xcfIter(make_range(xcfRepl), ' ');
            for (String xcf; xcfRepl.end() != xcfIter; ++xcfIter) {
                xcf.assign(xcfIter->begin(), xcfIter->size());
                catVec.push_back(xcf);
            }
        }
        else
            catVec.push_back(uri);
    }

    return catVec;
}

void SernaCatMgrImpl::updateCatalogs()
{
    using namespace XmlCatalogs;
    PropertyAccessor ph(get_xcf_props());
    bool useCatalogs = ph.getProp(USE_CATALOGS).getBool();

    if (useCatalogs) {
        bool ps = (NOTR("system") == ph.getProp(PREFER).getString());
        String prefStr(from_latin1(ps ? NOTR("system") : NOTR("public")));
        ph.getProp(PREFER).setString(prefStr);

        const String catList(ph.getProp(CAT_PATH).getString());
        if (catList_ != catList) {
            catList_ = catList;
            global_catalog().clearCatalogs();
            global_catalog().addCatalogList(make_catlist(catList_),
                                            msgbox_stream().getMessenger());
        }
        global_catalog().setOption(NOTR("prefer"),
                                   ps ? NOTR("system") : NOTR("public"));
        GroveLib::GroveBuilder::setDefaultCatalogManager(&global_catalog());
    }
    else
        GroveLib::GroveBuilder::setDefaultCatalogManager(0);
}

SernaCatMgrImpl::SernaCatMgrImpl()
{
    PropertyAccessor ph(get_xcf_props());
    String xcf(ph.getProp(XmlCatalogs::CAT_PATH).getString());
    if (xcf.empty()) {
        PathName cat(config().getProperty(NOTR("vars/xml"))->getString());
        cat.append(NOTR("catalog.xml"));
        if (cat.exists()) {
            xcf = from_latin1(NOTR("$SERNA_XML"));
            xcf.append(1, PathName::DIR_SEP).append(NOTR("catalog.xml"));
            xcf += PathName::PATH_SEP;
        }
        str_append(static_cast<ustring&>(xcf), XCF_VARREF);
        ph.getProp(XmlCatalogs::CAT_PATH).setString(xcf);
    }
    ustring dtdPath(ph.getProp(XmlCatalogs::DTD_PATH).getString());
    if (dtdPath.empty()) {
        dtdPath.assign(from_latin1(NOTR("$SERNA_XML/dtds")));
        ph.getProp(XmlCatalogs::DTD_PATH).setString(dtdPath);
    }

    bool useCatalogs = true;
    String useCatStr(ph.getProp(XmlCatalogs::USE_CATALOGS).getString());
    if (!useCatStr.empty() && !ph.getProp(XmlCatalogs::USE_CATALOGS).getBool())
        useCatalogs = false;
    ph.getProp(XmlCatalogs::USE_CATALOGS).setBool(useCatalogs);
    updateCatalogs();

    String subtreeName(from_latin1(XmlCatalogs::XML_CATALOGS));
    subtreeName.append(1, '/').append(1, '*');
    PropertyTreeEventFactory& ptef = config().eventFactory();
    ptef.subscribe(subtreeName, makeCommand<UpdateCatalogs>, this);
}

SernaCatMgr& SernaCatMgr::instance()
{
    return SingletonHolder<SernaCatMgrImpl, CreateUsingNew<SernaCatMgrImpl>,
                           NoDestroy<SernaCatMgrImpl> >::instance();
}

///////////////////////////////////////////////////////////////////

DocUriMapper::DocUriMapper()
{
    SernaCatMgr::instance();
    catMgr_ = global_catalog().copy();
}

void DocUriMapper::addCatalogs(const Common::String& moreCatalogs)
{
    catMgr_->addCatalogList(moreCatalogs, msgbox_stream().getMessenger());
}

void DocUriMapper::prependCatalogs(const Common::String& moreCatalogs)
{
    if (moreCatalogs.empty())
        return;

    Vector<String> pathVec;
    catMgr_->getCatalogList(pathVec);
    StringTokenizer st(moreCatalogs, NOTR(";"));
    while (st)
        pathVec.insert(pathVec.begin(), st.next());
    catMgr_->clearCatalogs();
    catMgr_->addCatalogList(pathVec, msgbox_stream().getMessenger());
}

Vector<String> DocUriMapper::getCatalogsList() const
{
    Vector<String> pathVec;
    catMgr_->getCatalogList(pathVec);
    return pathVec;
}

DocUriMapper::~DocUriMapper()
{
}

