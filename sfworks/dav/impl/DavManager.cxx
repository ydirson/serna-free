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

#include "dav/DavManager.h"
#include "dav/impl/FileProtocol.h"
#include "dav/IoStream.h"
#include "dav/impl/dav_debug.h"
#include "common/Singleton.h"
#include <map>

using namespace Common;

namespace Dav {

class DavManagerImpl : public DavManager {
public:
    DavManagerImpl();

    OpStatus        open(UrlRef url, int openFlags, IoStream&);
    OpStatus        open(UrlRef url, int openFlags, IoRequestHandle**);
    OpStatus        listCollection(UrlRef url, Common::PropertyNode* to);
    OpStatus        makeCollection(UrlRef url);
    OpStatus        getResourceInfo(UrlRef url, Common::PropertyNode* to);
    OpStatus        lock(UrlRef url, LockOp op, int key = 0);

    void            closeSessions();

    void            doRegisterProtocol(Protocol*);
    void            doDeregisterProtocol(Protocol*);
    
    const String&   lastError() const { return lastError_; }
    virtual void    setLastError(const Common::String& s) { lastError_ = s; }

private:
    typedef std::map<String, SessionPtr> SessionMap; 
    typedef std::map<String, Protocol*>  ProtocolMap;

    OpStatus    get_session(UrlRef url, SessionPtr& session);  
    
    ProtocolMap protocols_;
    SessionMap  sessions_;
    String      lastError_;
};

static String mangle_host_port(const Url& url)
{
    return url[Url::PROTOCOL] + ":" + url[Url::HOST] + ":" + url[Url::PORT];
}

OpStatus DavManagerImpl::open(UrlRef url, int openFlags, IoRequestHandle** h)
{
    SessionPtr session;
    OpStatus op_status = get_session(url, session);
    if (op_status)
        return op_status;
    IoRequestHandle* io_handle = 0;
    op_status = session->open(url, openFlags, &io_handle);
    if (op_status)
        return op_status;
    *h = io_handle;
    return DAV_RESULT_OK;
}

OpStatus DavManagerImpl::open(UrlRef url, int openFlags, IoStream& ustream)
{
    IoRequestHandle* io_handle = 0;
    OpStatus op_status = open(url, openFlags, &io_handle);
    if (op_status)
        return op_status;
    ustream.init(io_handle);
    return DAV_RESULT_OK;
}

OpStatus DavManagerImpl::listCollection(UrlRef url, PropertyNode* to)
{
    SessionPtr session;
    OpStatus op_status = get_session(url, session);
    if (op_status)
        return op_status;
    return session->listCollection(url, to);
}

OpStatus DavManagerImpl::makeCollection(UrlRef url)
{
    SessionPtr session;
    OpStatus op_status = get_session(url, session);
    if (op_status)
        return op_status;
    return session->makeCollection(url);
}
    
OpStatus DavManagerImpl::getResourceInfo(UrlRef url, Common::PropertyNode* to)
{
    SessionPtr session;
    OpStatus op_status = get_session(url, session);
    if (op_status)
        return op_status;
    return session->getResourceInfo(url, to);
}

OpStatus DavManagerImpl::lock(UrlRef url, LockOp op, int key)
{
    SessionPtr session;
    OpStatus op_status = get_session(url, session);
    if (op_status)
        return op_status;
    return session->lock(url, op, key);
}

OpStatus DavManagerImpl::get_session(UrlRef url, SessionPtr& session)
{
    String protocol(url[Url::PROTOCOL]);
    if (protocol.isEmpty())
        protocol = "file";
    ProtocolMap::const_iterator proto_iter =
        protocols_.find(url[Url::PROTOCOL]);
    if (proto_iter == protocols_.end()) {
        setLastError("bad protocol: " + url[Url::PROTOCOL]);
        return DAV_RESULT_BAD_PROTOCOL;
    }
    String sess_id = mangle_host_port(url);
    if (proto_iter->second->persistentSessions()) {
        SessionMap::const_iterator sess_iter = sessions_.find(sess_id);
        if (sess_iter != sessions_.end()) {
            session = sess_iter->second;
            DDBG << "DAV: Found session for: " << String(url) << std::endl;
            return DAV_RESULT_OK;
        }
    }
    DDBG << "DAV: Existing session for " << String(url) << " not found\n";
    SessionPtr new_session;
    OpStatus status = proto_iter->second->makeSession(url, new_session);
    if (status)
        return status;
    session = new_session;
    if (proto_iter->second->persistentSessions())
        sessions_[sess_id] = new_session;
    DDBG << "DAV: Created session for " << String(url) << std::endl;
    return DAV_RESULT_OK;
}

void DavManagerImpl::doRegisterProtocol(Protocol* protocol)
{
    if (protocols_.find(protocol->name()) != protocols_.end())
        return; // already registered
    DDBG << "DAV: Register protocol: " << protocol->name() << std::endl;
    protocols_[protocol->name()] = protocol;
}

void DavManagerImpl::doDeregisterProtocol(Protocol* protocol)
{
    DDBG << "DAV: De-Register protocol: " << protocol->name() << std::endl;
    ProtocolMap::iterator proto_iter = protocols_.find(protocol->name());
    if (proto_iter != protocols_.end())
        protocols_.erase(proto_iter);
}

void DavManagerImpl::closeSessions()
{
    DDBG << "DAV: Closing sessions\n";
    sessions_.clear();
}

DavManagerImpl::DavManagerImpl()
{
    doRegisterProtocol(new FileProtocol);
}

typedef SingletonHolder<DavManagerImpl> DavSingleton;

void DavManager::registerProtocol(Protocol* protocol)
{
    static_cast<DavManagerImpl&>(instance()).doRegisterProtocol(protocol);
}

void DavManager::deregisterProtocol(Protocol* protocol)
{
    if (DavSingleton::isDead())
        return;    
    static_cast<DavManagerImpl&>(instance()).doRegisterProtocol(protocol);
}

DavManager& DavManager::instance()
{
    return DavSingleton::instance();
}

} // namespace Dav
