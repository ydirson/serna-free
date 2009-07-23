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
#include "sapi/common/DavManager.h"
#include "sapi/common/PropertyNode.h"
#include "sapi/common/Url.h"

#include "common/PropertyTree.h"
#include "dav/DavManager.h"
#include "dav/IoStream.h"
#include "dav/types.h"
#include "dav/DavQIODevice.h"

#include <vector>

#define PN_IMPL(pn) static_cast<Common::PropertyNode*>(pn.getRep())

namespace SernaApi {

#define SELF static_cast<Dav::DavManager*>(getRep())

DavManager::DavManager(SernaApiBase* rep)
    :SimpleWrappedObject(rep)
{}
    
DavManager DavManager::instance()
{
    return DavManager(&Dav::DavManager::instance());
}
  
DavManager::OpStatus DavManager::listCollection(const Url& url,
                                                PropertyNode& result)
{
    if (result.isNull())
        return (OpStatus)(-1);
    return (OpStatus)(SELF->listCollection(url, PN_IMPL(result)));
}
    
DavManager::OpStatus DavManager::makeCollection(const Url& url)
{
    return (OpStatus)(SELF->makeCollection(url));
}
    
DavManager::OpStatus DavManager::getResourceInfo(const Url& url, 
                                                 PropertyNode& result)
{
    if (result.isNull())
        return (OpStatus)(-1);
    return (OpStatus)(SELF->getResourceInfo(url, PN_IMPL(result)));
}
    
DavManager::OpStatus DavManager::lock(const Url& url, LockOp op, int key)
{
    return (OpStatus)(SELF->lock(url, (Dav::LockOp)(op), key));
}

DavManager::OpStatus 
DavManager::open(const Url& url, int openFlags, QIODevice*& qdev)
{
    Dav::IoRequestHandle* io_handle = 0;
    Dav::OpStatus res = SELF->open(url, openFlags, &io_handle);
    if (res != Dav::DAV_RESULT_OK)
        return (DavManager::OpStatus) res;
    if (!io_handle)
        return (DavManager::OpStatus)(-1);
    qdev = new Dav::DavQIODevice(io_handle);
    return (DavManager::OpStatus) res;
}

DavManager::OpStatus DavManager::copy(const Url& src_url, const Url& dst_url)
{
    std::vector<char> io_buffer(1024*64);
    Dav::DavManager& dav_mgr = Dav::DavManager::instance();
    Dav::IoStream ifs, ofs;
    int res = 0;
    res = dav_mgr.open(src_url, Dav::DAV_OPEN_READ, ifs);
    if (res != Dav::DAV_RESULT_OK)
        return (OpStatus)res;
    res = dav_mgr.open(dst_url, Dav::DAV_OPEN_WRITE, ofs);
    if (res != Dav::DAV_RESULT_OK)
        return (OpStatus)res;
    int n = 0;
    do {
        n = ifs.readRaw(io_buffer.size(), &io_buffer[0]);
        if (n < 0)
            return DAV_RESULT_IO_ERROR;
        if (n != 0 && n != ofs.writeRaw(n, &io_buffer[0]))
            return DAV_RESULT_IO_ERROR;
    } while (n);
    return (OpStatus) ofs.close();
}

void DavManager::closeSessions()
{
    SELF->closeSessions();
}
    
SString DavManager::lastError() const
{
    return SELF->lastError();
}    

} 

