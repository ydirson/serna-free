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

#ifndef SAPI_DAV_PROTOCOL_H_
#define SAPI_DAV_PROTOCOL_H_

#include "sapi/common/WrappedObject.h"
#include "sapi/common/DavManager.h"
#include <sys/types.h>

namespace SernaApi {

class PropertyNode;
class Url;
class DavSession;
class DavIoRequestHandle;

/*! Implements session factory for custom protocol. */
class SAPI_EXPIMP DavProtocol : public SimpleWrappedObject {
public:
    /// Must reutrn protocol name (part of URL), such as 'http'
    virtual SString             name() const = 0;
    
    /// Session factory
    virtual DavSession*         makeSession(const Url&) const { return 0; }

    /// Specifies whether this protocol supports persistent sessions.
    /// If sessions are persistent, they are kept in session list (for reuse)
    /// until DavManager::closeSessions() is called. 
    /// Sessions are mapped in session table by host+port.
    virtual bool                persistentSessions() const { return false; }

    /// Registers new protocol. Note that this function should be
    /// called only once for single protocol. Ownership of protocol
    /// object is passed to Serna.
    static  void                registerProtocol(DavProtocol*);

    /// This function should be called for explicit de-registering
    /// of the protocol, if necessary. DavProtocol object will be
    /// destroyed.
    static  void                deregisterProtocol(DavProtocol*);

    virtual ~DavProtocol();
};

/*! Implements session. If session persistence is enabled for protocol,
    session can be re-used for multiple requests.
 */
class SAPI_EXPIMP DavSession : public SimpleWrappedObject {
public:
    typedef DavManager::OpStatus OpStatus;

    /// An I/O handle factory. I/O handle is required for read/write ops.
    virtual DavIoRequestHandle* open(const Url&, 
                                     int /*mode*/, OpStatus&) { return 0; }
    /// Implements collection listing
    virtual OpStatus    listCollection(const Url& url,
                                       const PropertyNode& to);
    /// Collection create request
    virtual OpStatus    makeCollection(const Url& url);

    /// Information about the collection (collection type, entries)
    virtual OpStatus    getResourceInfo(const Url& url, 
                                        const PropertyNode& to);
    /// Implements locking, if supported
    virtual OpStatus    lock(const Url& url,
                             DavManager::LockOp op, int key);
    DavSession();
    virtual ~DavSession();

private:
    DavSession(const DavSession&);
    DavSession& operator=(const DavSession&);
};

class SAPI_EXPIMP DavIoRequestHandle : public SimpleWrappedObject {
public:
    typedef DavManager::OpStatus OpStatus;
    
    DavIoRequestHandle(const DavSession&);
    
    /// Read request. Default implementation returns 0 bytes
    virtual int64       readRaw(char* buf, int64 bytesToRead);

    /// Write request. Default implementation returns error
    virtual int64       writeRaw(const char* buf, int64 maxBytes);

    /// Close operations (e.g. file handles should be closed here)
    virtual OpStatus    close();

    /// Must return current seek current seek position
    virtual int64       position() const;

    /// Must set seek position, if supported
    virtual OpStatus    setPosition(int64);

    /// Must return resource size
    virtual int64 size() const;
    
    virtual ~DavIoRequestHandle();

private:
    DavIoRequestHandle(const DavIoRequestHandle&);
    DavIoRequestHandle& operator=(const DavIoRequestHandle&);
};

} // namespace SernaApi

#endif // SAPI_DAV_PROTOCOL_H_
