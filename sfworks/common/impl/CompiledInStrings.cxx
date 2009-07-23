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
 *  Compiled-in string dictionary: implementation
 */

#include "common/common_defs.h"
#include "common/CompiledInStrings.h"
#include "common/Singleton.h"
#include "common/String.h"
#include "common/Vector.h"
#include "common/StrdupNew.h"
#include "common_debug.h"
#include <assert.h>
#include <string.h>


COMMON_NS_BEGIN

CompiledInStrings::CompiledInStrings()
  : lastitem_(0)
{
}

CompiledInStrings::~CompiledInStrings()
{
    for (uint32 i = 0; i < messages_.size(); ++i) {
        delete[] messages_[i].lang;
        delete messages_[i].segset;
    }
}

struct CompiledInStringsImpl : CompiledInStrings {};

CompiledInStrings& CompiledInStrings::instance()
{
    return SingletonHolder<CompiledInStringsImpl,
      CreateUsingNew<CompiledInStringsImpl>,
      PhoenixSingleton<CompiledInStringsImpl > >::instance();
}

CompiledInStrings::LangItem*
CompiledInStrings::find_lang(const char* lang) const
{
    for (uint32 i = 0; i < messages_.size(); ++i) {
        if (!strcmp(messages_[i].lang, lang))
            return &messages_[i];
    }
    return 0;
}

bool CompiledInStrings::getString(const char* lang,
                                  uint32 facility,
                                  uint32 msgid,
                                  const UCharType*& str,
                                  uint& length) const
{
    if (0 == lang)
        return false;
    if (0 == lastitem_ || lang != lastitem_->lang) {
        if (!(lastitem_ = find_lang(lang)))
            return false;
    }
    const _seg* s = lastitem_->segset->get(facility);
    if (0 == s || msgid > (unsigned)s->nmsg)
        return false;
    const _msg* m = s->msgv[msgid - 1];
    str = m->msg;
    length = m->len;
    return true;
}

CompiledInStrings::LangItem::LangItem(const char* language)
 :  lang(strdup_new(language)), segset(new SegSet)
{
}

using namespace std;

void
CompiledInStrings::addSeg(const char* lang, uint32 facility, const _seg* seg)
{
    DBG(COMMON.MSG) << "adding facility:";
    DBG(COMMON.MSG) << sbr(facility);
    DBG(COMMON.MSG) << ", lang:";
    DBG(COMMON.MSG) << (lang ? lang : "");
    DBG(COMMON.MSG) << ", segment:";
    DBG(COMMON.MSG) << abr(seg);
    DBG(COMMON.MSG) << endl;
    DDINDENT
    if (0 != lang && 0 != seg) {
        LangItem* li = find_lang(lang);
        if (0 == li)
            li = messages_.push_back(LangItem(lang));
        assert(0 == li->segset->get(facility));
        li->segset->set(seg, facility);
    }
    DBG(COMMON.MSG) << "messages_.size():" << sbr(messages_.size()) << endl;
}

void
CompiledInStrings::delSeg(const char* lang, uint32 facility)
{
    DBG(COMMON.MSG) << "deleting facility:" << sbr(facility)
                    << ", lang:" << (lang ? lang : "") << endl;
    DDINDENT
    if (LangItem* li = find_lang(lang)) {
        DBG(COMMON.MSG) << "found lang" << endl;
        li->segset->remove(facility);
        if (li->segset->isEmpty()) {
            DBG(COMMON.MSG) << "segset " << abr(li->segset) << " empty" << endl;
            if (lastitem_ && lastitem_->lang == li->lang)
                lastitem_ = 0;
            delete li->segset;
            delete[] li->lang;
            messages_.erase(li, li + 1);
        }
    }
    DBG(COMMON.MSG) << "messages_.size():" << sbr(messages_.size()) << endl;
}

void CompiledInStrings::listLanguages(Vector<const char*>& ll) const
{
    for (uint32 i = 0; i < messages_.size(); ++i)
        ll.push_back(messages_[i].lang);
}

CompiledInStrings::Registrar::Registrar(const char* lang,
                                        uint32 facility,
                                        const _seg* seg)
  : facility_(facility), lang_(lang)
{
    instance().addSeg(lang, facility, seg);
}

CompiledInStrings::Registrar::~Registrar()
{
    instance().delSeg(lang_, facility_);
}

COMMON_NS_END

