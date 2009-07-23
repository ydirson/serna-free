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
 *  Core of messaging framework: implementation (handling)
 */
#include "common/common_defs.h"
#include "common/Message.h"
#include "common/MessageUtils.h"
#include "common/String.h"
#include "common/DynamicLibrary.h"
#include "common/CompiledInStrings.h"
#include "common/OwnerPtr.h"
#include "common/Singleton.h"
#include "common/common_types.h"
#include <iostream>


namespace {

USING_COMMON_NS

class BuiltinMessageFetcherImpl : public BuiltinMessageFetcher {
public:
    virtual bool fetchString(uint32 fcl, uint32 id, String& os) const;
    virtual bool fetchString(uint32 fcl, const char* id, String& os) const;

    bool setLanguage(const char* lang);

    BuiltinMessageFetcherImpl();
    virtual ~BuiltinMessageFetcherImpl() {}

private:

    friend class BuiltinMessageFetcher;
    BuiltinMessageFetcherImpl(const BuiltinMessageFetcherImpl&);

    const char* lang_;
    OwnerVector<DynamicLibrary> libs_;
};

static inline BuiltinMessageFetcherImpl& inst()
{
    return SingletonHolder<BuiltinMessageFetcherImpl,
           CreateUsingNew<BuiltinMessageFetcherImpl>,
           PhoenixSingleton<BuiltinMessageFetcherImpl> >::instance();
}

} // namespace

COMMON_NS_BEGIN

void MessageListMessenger::dispatch(RefCntPtr<Message>& m)
{
    mvec_.push_back(m);
}

MessageListMessenger::~MessageListMessenger()
{
}

BuiltinMessageFetcher& BuiltinMessageFetcher::instance()
{
    return inst();
}

void BuiltinMessageFetcher::messageLibraryList(Vector<String>& v)
{
    v.clear();
    for (ulong i = 0; i < inst().libs_.size(); ++i)
        v.push_back(inst().libs_[i]->fileName(true));
}

void BuiltinMessageFetcher::unloadMessageLibrary(const String& path)
{
    Vector<DynamicLibrary*>& c = inst().libs_;
    ulong i = 0;
    for (; i < c.size(); ++i) {
        if (c[i]->fileName(true) == path)
            break;
    }
    if (i == c.size())
        return;
    delete c[i];
    c.erase(i);
}

bool BuiltinMessageFetcher::loadMessageLibrary(const String& path,
                                               bool forceAbs)
{
    OwnerPtr<DynamicLibrary> dl(new DynamicLibrary);
    if (!dl->load(path, forceAbs ? DynamicLibrary::FORCE_ABSPATH : 0))
        return false;
    inst().libs_.push_back(dl.release());
    return true;
}

BuiltinMessageFetcher::~BuiltinMessageFetcher()
{
}

void BuiltinMessageFetcher::listLanguages(Vector<const char*>& ll)
{
    CompiledInStrings::instance().listLanguages(ll);
}

bool BuiltinMessageFetcher::setLanguage(const char* lang)
{
    return inst().setLanguage(lang);
}

void DefaultMessenger::dispatch(RefCntPtr<Message>& mb)
{
    using namespace std;
    if (mb->severityLevel() > 0)
        cerr << mb->severityLevelName() << ": ";
    cerr << mb->format(BuiltinMessageFetcher::instance()).utf8() << endl;
}

void SilentMessenger::dispatch(RefCntPtr<Message>& mb)
{
    if (mb->severityLevel() > 0) {
        if (mb->severityLevel() == Message::L_FATAL) {
            std::cerr << "FATAL: "
                << mb->format(BuiltinMessageFetcher::instance()).utf8() << std::endl;
        }
    }
}

COMMON_NS_END

namespace {

BuiltinMessageFetcherImpl::BuiltinMessageFetcherImpl()
{
    setLanguage("en");
}

bool BuiltinMessageFetcherImpl::setLanguage(const char* lang)
{
    Vector<const char*> ll;
    uint32 i;
    CompiledInStrings::instance().listLanguages(ll);
    for (i = 0; i < ll.size(); ++i) {
        if (!strcmp(ll[i], lang))
            break;
    }
    if (i == ll.size())
        return false;       // attempt to set nonexistent language
    lang_ = ll[i];
    return true;
}

bool BuiltinMessageFetcherImpl::fetchString(uint32 facility,
                                            uint32 msgid,
                                            String& os) const
{
    unsigned int length;
    const CompiledInStrings::UCharType* uts;
    CompiledInStrings& cs = CompiledInStrings::instance();
    if (cs.getString(lang_, facility, msgid, uts, length)) {
        os.setUnicode((const Char*)uts, length);
        return true;
    }
    return false;
}

bool BuiltinMessageFetcherImpl::fetchString(uint32 /* facility */,
                                            const char* id,
                                            String& os) const
{
    os = id;
    return true;
}

};

