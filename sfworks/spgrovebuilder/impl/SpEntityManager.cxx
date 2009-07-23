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
#include "URLStorageMessages.h"
#include "SpUtils.h"
#include "dav/Session.h"
#include "dav/DavManager.h"

using namespace GroveLib;

namespace Sp {

static UnivCharsetDesc::Range range = { 0, 65536, 0 };
static CharsetInfo iso646Charset(UnivCharsetDesc(&range, 1));

class DavStorageManager : public IdStorageManager {
public:
    DavStorageManager(const char *type, int locksId)
        : IdStorageManager(&iso646Charset), 
          type_(type), locksId_(locksId) {}

    StorageObject *makeStorageObject(const StringC &id,
                                   const StringC &baseId,
                                   Boolean search,
                                   Boolean mayRewind,
                                   Messenger &,
                                   StringC &found);
    const char *type() const { return type_; }
    Boolean guessIsId(const StringC& id, const CharsetInfo &) const
    {
        Common::Url url(SpUtils::makeString(id));
        return url[Common::Url::PROTOCOL] == "http" || 
               url[Common::Url::PROTOCOL] == "https";
    }
    Boolean transformNeutral(StringC& str, Boolean fold, Messenger&) const
    {
        if (fold) {
            for (size_t i = 0; i < str.size(); i++) {
                Char c = str[i];
                if (c <= (unsigned char)-1)
                    str[i] = tolower(str[i]);
            }
        }
        return 1;
    }

private:
    Boolean resolveRelative(const StringC &base, StringC &, Boolean) const;
    DavStorageManager(const DavStorageManager &);

    void      operator=(const DavStorageManager &);
    const char *type_;
    int       locksId_;
};

class DavStorageObject : public RewindStorageObject {
public:
    DavStorageObject(Boolean mayRewind, Dav::IoRequestHandle* handle,
                     const StringC& id)
        : RewindStorageObject(mayRewind, 0),
          handle_(handle), id_(id) {}

    virtual Boolean read(char *buf, size_t bufSize,
                         Messenger &mgr, size_t &nread);
    Boolean seekToStart(Messenger &);

private:
  DavStorageObject(const DavStorageObject &); // undefined
  void operator=(const DavStorageObject &); // undefined

  Common::OwnerPtr<Dav::IoRequestHandle> handle_;
  Sp::StringC id_;
};

StorageObject*
DavStorageManager::makeStorageObject(const StringC &specId,
				     const StringC &baseId,
				     Boolean,
			             Boolean mayRewind,
				     Messenger &mgr,
				     StringC &id)
{
    using namespace Common;

    id = specId;
    resolveRelative(baseId, id, 0);
    Url url(SpUtils::makeString(id));
    if (url[Url::PROTOCOL] != "http" && url[Url::PROTOCOL] != "https") {
        mgr.message(DAVStorageMessages::onlyHTTP);
        return 0;
    }
    if (url[Url::HOST].isEmpty()) {
        mgr.message(DAVStorageMessages::emptyHost, StringMessageArg(id));
        return 0;
    }
        
    if (url[Url::PORT].toInt() <= 0)
        url.set(Url::PORT, url[Url::PROTOCOL] == "http" ? "80" : "443");
    Dav::IoRequestHandle* io_handle = 0;
    if (locksId_)
        Dav::DavManager::instance().lock(url, Dav::DAV_LOCK, locksId_);
    Dav::OpStatus op_status = Dav::DavManager::instance().open(url,
        Dav::DAV_OPEN_READ, &io_handle);
    if (op_status) {
        mgr.message(DAVStorageMessages::cannotConnect,
            StringMessageArg(id), StringMessageArg(id));
        return 0;
    }
    DavStorageObject* dav_obj = new DavStorageObject(mayRewind,
        io_handle, id);
    return dav_obj;
}

Boolean DavStorageObject::read(char *buf, size_t bufSize,
                               Messenger &mgr, size_t &nread)
{
    if (handle_.isNull())
        return 0;   // eof
    if (readSaved(buf, bufSize, nread))
        return 1;
    uint n = 0;
    if (handle_->readRaw(bufSize, buf, n)) {
        ParentLocationMessenger(mgr).message(DAVStorageMessages::readError,
				             StringMessageArg(id_),
					     StringMessageArg(StringC()));
        return 0;
    }
    if (n == 0) {
        handle_ = 0; // eof detected
        return 0;
    }
    nread = n;
    saveBytes(buf, n);
    return 1;
}

Boolean DavStorageObject::seekToStart(Messenger&)
{
    CANNOT_HAPPEN();
    return 0;
}

Boolean DavStorageManager::resolveRelative(const StringC &baseId,
					   StringC &id,
					   Boolean) const
{
  static const char schemeChars[] =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "01234567879"
    "+-.";
  size_t i;
  // If it has a scheme, it is absolute.
  for (i = 0; i < id.size(); i++) {
    if (id[i] == ':') {
      if (i == 0)
	break;
      else
	return 1;
    }
    else if (!strchr(schemeChars, id[i]))
      break;
  }
  for (i = 0; i < id.size(); i++) {
    if (id[i] != '/')
      break;
  }
  size_t slashCount = i;
  if (slashCount > 0) {
    Boolean foundSameSlash = 0;
    size_t sameSlashPos = 0;
    for (size_t j = 0; j < baseId.size(); j++) {
      size_t thisSlashCount = 0;
      for (size_t k = j; k < baseId.size() && baseId[k] == '/'; k++)
	thisSlashCount++;
      if (thisSlashCount == slashCount && !foundSameSlash) {
	foundSameSlash = 1;
	sameSlashPos = j;
      }
      else if (thisSlashCount > slashCount)
	foundSameSlash = 0;
    }
    if (foundSameSlash) {
      StringC tem(baseId.data(), sameSlashPos);
      tem += id;
      tem.swap(id);
    }
  }
  else {
    size_t j;
    for (j = baseId.size(); j > 0; j--)
      if (baseId[j - 1] == '/')
	break;
    if (j > 0) {
      StringC tem(baseId.data(), j);
      tem += id;
      tem.swap(id);
    }
  }
  // FIXME remove xxx/../, and /.
  return 1;
}

} // namespace Sp

namespace GroveLib {

void make_entity_manager(Sp::ParserApp* app, int locksId)
{
    app->entityManager()->registerStorageManager(
        new Sp::DavStorageManager("URL", locksId));
}

} // namespace GroveLib
