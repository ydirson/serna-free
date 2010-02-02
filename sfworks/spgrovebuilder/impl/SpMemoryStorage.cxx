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
#include "sp/config.h"
#include "sp/ParserApp.h"
#include "sp/StorageManager.h"
#include "sp/WinInetStorage.h"
#include "sp/URLStorage.h"
#include "sp/RewindStorageObject.h"
#include "sp/ExtendEntityManager.h"
#include "sp/MessageArg.h"
#include "sp/MessageBuilder.h"
#include "sp/macros.h"
#include "SpUtils.h"

#include "sp_debug.h"

#include "common/String.h"
#include "common/Url.h"
#include "common/StringCvt.h"

#include <string>

#include "stdio.h"

#ifdef DBG_DEFAULT_TAG
 #undef DBG_DEFAULT_TAG
#endif

#define DBG_DEFAULT_TAG SPGB.MEMSM

//static const char STR_ID[] = "string://";
static const char STR_ID[] = "mem://";

template<typename T> inline
bool starts_with_str_id(const T* str, unsigned len)
{
    if (len < sizeof(STR_ID) - 1)
        return false;

    return (0 == Common::compare(str, sizeof(STR_ID) - 1, &STR_ID[0]));
}

inline bool starts_with_str_id(const Sp::Char* str, unsigned len)
{
    using Common::Char;
    return starts_with_str_id<Char>(reinterpret_cast<const Char*>(str), len);
}

namespace Sp {

USING_GROVE_NAMESPACE

static UnivCharsetDesc::Range range = { 0, 128, 0 };
static CharsetInfo iso646Charset(UnivCharsetDesc(&range, 1));

static UnivCharsetDesc::Range range10646 = { 0, 65536, 0 };
static CharsetInfo iso10646Charset(UnivCharsetDesc(&range10646, 1));

class MemoryStorageManager : public IdStorageManager {
public:
    MemoryStorageManager(const char *type)
        : IdStorageManager(&iso10646Charset), type_(type) {}

    StorageObject *makeStorageObject(const StringC &id,
                                     const StringC &baseId,
                                     Boolean search,
                                     Boolean mayRewind,
                                     Messenger &,
                                     StringC &found);
    const char *type() const { return type_; }
    Boolean guessIsId(const StringC& id, const CharsetInfo &) const
    {
        Boolean result = starts_with_str_id(id.data(), id.size());
        DDBG << "Got id = " << GROVE_NAMESPACE::SpUtils::makeString(id)
             << ", result = " << result << std::endl;
        return result;
    }

private:
    Boolean resolveRelative(const StringC &base, StringC &, Boolean) const;
    MemoryStorageManager(const MemoryStorageManager &);

    void      operator=(const MemoryStorageManager &);
    const char *type_;
};

class StringStorageObject : public RewindStorageObject {
public:
    StringStorageObject(Boolean mayRewind, const StringC& id)
     :  RewindStorageObject(mayRewind, 0),
        begin_(&STR_ID[0]), end_(begin_), cur_(begin_)
    {
        if (starts_with_str_id(id.data(), id.size())) {
            using Common::Char;
            using Common::String;
            using namespace Common;

            const int str_id_size = sizeof(STR_ID) - 1;
            const Char* loc_addr = reinterpret_cast<const Char*>(id.data());
            loc_addr += str_id_size;

            nstring loc(latin1(String(loc_addr, id.size() - str_id_size)));
            unsigned int len = 0;
            const void* p = 0;
            if (2 == sscanf(loc.c_str(), "%p,%x", &p, &len)) {
//                cur_ = begin_ = reinterpret_cast<const char*>(p);
                const ustring& str = *(reinterpret_cast<const ustring*>(p));
                buf_ = Common::utf8(str);
                cur_ = begin_ = buf_.data();
                end_ = begin_ + buf_.size();
            }
        }
        DDBG << "MemStorageObject ctor: buf_ = '" << buf_
             << ", len = " << end_ - begin_
             << std::endl;
    }

    virtual Boolean read(char *buf, size_t bufSize,
                         Messenger &mgr, size_t &nread);
    Boolean seekToStart(Messenger &);

private:
    StringStorageObject(const StringStorageObject &); // undefined
    void operator=(const StringStorageObject &); // undefined

    Common::nstring buf_;
    const char*     begin_;
    const char*     end_;
    const char*     cur_;
};

StorageObject*
MemoryStorageManager::makeStorageObject(const StringC &specId,
                                        const StringC &,
                                        Boolean,
                                        Boolean mayRewind,
                                        Messenger &,
                                        StringC &id)
{
    id = specId;
    DDBG << "makeStorageObject: id = '"
         << GROVE_NAMESPACE::SpUtils::makeString(specId)
         << '\'' << std::endl;
    StringStorageObject* mem_obj = new StringStorageObject(mayRewind, id);
    return mem_obj;
}

Boolean StringStorageObject::read(char *buf, size_t bufSize,
                                  Messenger &, size_t &nread)
{
    if (readSaved(buf, bufSize, nread))
        return 1;

    unsigned to_read = bufSize;
    if (end_ > cur_) {
        unsigned remaining = end_ - cur_;
        to_read = remaining > bufSize ? bufSize : remaining;
    }
    else
        return 0;

    if (0 < to_read)
        memmove(buf, cur_, to_read);
    DDBG << "MemStorageObject: read " << to_read << " bytes: '"
         << std::string(buf, to_read) << '\'' << std::endl;
    saveBytes(buf, to_read);
    cur_ += to_read;
    nread = to_read;
    return 1;
}

Boolean StringStorageObject::seekToStart(Messenger&)
{
    cur_ = begin_;
    return true;
}

Boolean MemoryStorageManager::resolveRelative(const StringC& base,
                                              StringC& specId,
                                              Boolean search) const
{
    DDBG << "resolveRelative: base = '"
         << GROVE_NAMESPACE::SpUtils::makeString(base)
         << "', specId = '" << GROVE_NAMESPACE::SpUtils::makeString(specId)
         << "', search = " << search
         << '\'' << std::endl;

    using namespace Common;
    Common::String id(GROVE_NAMESPACE::SpUtils::makeString(specId));
    if (to_lower<ustring>(id) == "catalog")
        return 0;

    Url url(id);
    Common::String abs_path(url.absolute());
    if (url.isLocal())
        abs_path.insert(0, "file://");
    specId.assign(reinterpret_cast<const Sp::Char*>(abs_path.data()),
                  abs_path.size());

    DDBG << "resolveRelative: specId on exit = '" << abs_path
         << '\'' << std::endl;
    return 1;
}

} // namespace Sp

namespace GroveLib {

void make_mem_storage_manager(Sp::ParserApp* app)
{
    Sp::StorageManager* sm = new Sp::MemoryStorageManager("STRING");
    app->entityManager()->registerStorageManager(sm);
}

} // namespace GroveLib

