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

#include "sapi/common/DavProtocol.h"
#include "sapi/common/SString.h"
#include "sapi/common/Url.h"
#include "sapi/common/PropertyNode.h"
#include "common/PropertyTree.h"
#include "dav/Protocol.h"
#include "dav/Session.h"
#include "dav/DavManager.h"
#include <iostream>
namespace {

class DavProtocolProxy : public Dav::Protocol {
public:
    DavProtocolProxy(SernaApi::DavProtocol* proxy)
        : proxy_(proxy) {}
    ~DavProtocolProxy() { delete proxy_; }

    virtual Common::String  name() const { return proxy_->name(); }
    virtual Dav::OpStatus   makeSession(Dav::UrlRef, Dav::SessionPtr&) const;
    virtual bool            persistentSessions() const 
        { return proxy_->persistentSessions(); }
    void                    reset() { proxy_ = 0; }

private:
    SernaApi::DavProtocol* proxy_;    
}; 

Dav::OpStatus DavProtocolProxy::makeSession(Dav::UrlRef url, 
                                            Dav::SessionPtr& sptr) const
{
    SernaApi::DavSession* session = proxy_->makeSession(url);
    if (!session)
        return Dav::DAV_RESULT_BAD_PROTOCOL;
    sptr = static_cast<Dav::Session*>(session->getRep());
    return Dav::DAV_RESULT_OK;
}

/////////////////////////////////////////////////////////////

class DavSessionProxy : public Dav::Session {
public:
    DavSessionProxy(SernaApi::DavSession* proxy)
        : proxy_(proxy) {}
    ~DavSessionProxy() { delete proxy_; }

    virtual Dav::OpStatus open(Dav::UrlRef url, int mode, 
                               Dav::IoRequestHandle**);
    virtual Dav::OpStatus listCollection(Dav::UrlRef url,
                               Common::PropertyNode* to);
    virtual Dav::OpStatus makeCollection(Dav::UrlRef url);
    virtual Dav::OpStatus getResourceInfo(Dav::UrlRef url, 
                                          Common::PropertyNode* to);
    virtual Dav::OpStatus lock(Dav::UrlRef url, Dav::LockOp op, int key);

private:
    SernaApi::DavSession* proxy_;
};

Dav::OpStatus DavSessionProxy::open(Dav::UrlRef url, int mode, 
                                    Dav::IoRequestHandle** h)
{
    SernaApi::DavManager::OpStatus status;
    SernaApi::DavIoRequestHandle* handle = proxy_->open(url, mode, status);
    if ((!status && !handle) || !h) 
        return Dav::DAV_RESULT_IO_ERROR;
    *h = static_cast<Dav::IoRequestHandle*>(handle->getRep());
    return (Dav::OpStatus) status;
}

Dav::OpStatus DavSessionProxy::listCollection(Dav::UrlRef url,
                               Common::PropertyNode* to)
{
    SernaApi::PropertyNode to_ptn(to);
    return (Dav::OpStatus) proxy_->listCollection(url, to_ptn);        
}

Dav::OpStatus DavSessionProxy::makeCollection(Dav::UrlRef url)
{
    return (Dav::OpStatus) proxy_->makeCollection(url);
}

Dav::OpStatus DavSessionProxy::getResourceInfo(Dav::UrlRef url, 
                                               Common::PropertyNode* to)
{
    SernaApi::PropertyNode to_ptn(to);
    return (Dav::OpStatus) proxy_->listCollection(url, to_ptn);        
}

Dav::OpStatus DavSessionProxy::lock(Dav::UrlRef url, Dav::LockOp op, int key)
{
    return (Dav::OpStatus) proxy_->lock(url,  
        (SernaApi::DavManager::LockOp) op, key);
}

/////////////////////////////////////////////////////////////

class IoRequestHandleProxy : public Dav::IoRequestHandle {
public:
    IoRequestHandleProxy(SernaApi::DavIoRequestHandle* proxy,
                         Dav::Session* session)
        : IoRequestHandle(session), proxy_(proxy) {}
    ~IoRequestHandleProxy() { delete proxy_; }

    Dav::OpStatus    readRaw(uint n,  char* buf, uint& readBytes);
    Dav::OpStatus    writeRaw(uint n, const char* buf, uint& writtenBytes);
    Dav::OpStatus    close() { return (Dav::OpStatus) proxy_->close(); }
    uint             position() const { return proxy_->position(); }
    Dav::OpStatus    setPosition(uint n) 
        { return (Dav::OpStatus) proxy_->setPosition(n); }
    uint             size() const { return proxy_->size(); }

private:
    SernaApi::DavIoRequestHandle* proxy_;
};

Dav::OpStatus 
IoRequestHandleProxy::readRaw(uint n,  char* buf, uint& readBytes)
{ 
    SernaApi::int64 res = proxy_->readRaw(buf, n);
    if (res < 0) {
        readBytes = 0;
        return Dav::DAV_RESULT_IO_ERROR;
    } 
    readBytes = res;
    return Dav::DAV_RESULT_OK;
}

Dav::OpStatus 
IoRequestHandleProxy::writeRaw(uint n, const char* buf, uint& writtenBytes)
{ 
    SernaApi::int64 res = proxy_->writeRaw(buf, n);
    writtenBytes = (res < 0) ? 0 : res;
    return  (writtenBytes == n) ? 
        Dav::DAV_RESULT_OK : Dav::DAV_RESULT_IO_ERROR;
}

} // namespace

/////////////////////////////////////////////////////////////

namespace SernaApi {

void DavProtocol::registerProtocol(DavProtocol* proto)
{
    DavProtocolProxy* proxy = new DavProtocolProxy(proto);
    proto->setRep(proxy);
    Dav::DavManager::registerProtocol(proxy);
}

void DavProtocol::deregisterProtocol(DavProtocol* proto)
{
    Dav::DavManager::deregisterProtocol(
        static_cast<Dav::Protocol*>(proto->getRep()));
}

DavProtocol::~DavProtocol()
{   
    static_cast<DavProtocolProxy*>(getRep())->reset();
}

/////////////////////////////////////////////////////////////

DavSession::DavSession()
    : SimpleWrappedObject(new DavSessionProxy(this))
{
}

DavManager::OpStatus DavSession::listCollection(const Url&,
                                       const PropertyNode&)
{
    return DavManager::DAV_RESULT_BAD_PROTOCOL;
}

DavManager::OpStatus DavSession::makeCollection(const Url&)
{
    return DavManager::DAV_RESULT_BAD_PROTOCOL;
}

DavManager::OpStatus 
DavSession::getResourceInfo(const Url&, const PropertyNode&)
{
    return DavManager::DAV_RESULT_BAD_PROTOCOL;
}

DavManager::OpStatus DavSession::lock(const Url&, DavManager::LockOp, int)
{
    return DavManager::DAV_RESULT_OK;
}

DavSession::~DavSession()
{
}

DavIoRequestHandle::DavIoRequestHandle(const DavSession& sess)
    : SimpleWrappedObject(new IoRequestHandleProxy(this, 
        static_cast<Dav::Session*>(sess.getRep())))
{
}

int64 DavIoRequestHandle::writeRaw(const char*, int64)
{
    return -1;
}

int64 DavIoRequestHandle::readRaw(char*, int64)
{
    return 0;
}

DavManager::OpStatus DavIoRequestHandle::close()
{ 
    return DavManager::DAV_RESULT_OK;
}

int64 DavIoRequestHandle::position() const
{
    return 0; 
}

DavManager::OpStatus DavIoRequestHandle::setPosition(int64)
{
    return DavManager::DAV_RESULT_OK;
}

int64 DavIoRequestHandle::size() const 
{
    return 0; 
}

DavIoRequestHandle::~DavIoRequestHandle()
{
}

/////////////////////////////////////////////////////////////

} // namespace SernaApi
