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

#include "dav/DavQIODevice.h"
#include "dav/Session.h"
#include "dav/impl/dav_debug.h"

namespace Dav {
    
DavQIODevice::DavQIODevice(IoRequestHandle* h)
    : handle_(h)
{
    open(ReadWrite);    // set permissive flags; the rest depends on iohandle
}
    
void DavQIODevice::close()
{
    DDBG << "DavQIODev: close\n";
    handle_->close();
    QIODevice::close();
}

Q_INT64 DavQIODevice::size() const
{
    return handle_->size();
}
    
Q_INT64 DavQIODevice::pos() const
{
    return handle_->position();
}

bool DavQIODevice::seek(Q_INT64 pos)
{
    QIODevice::seek(pos);
    DDBG << "DavQIODev: seek to=" << pos << std::endl;
    return (handle_->setPosition(pos) == DAV_RESULT_OK);    
}

bool DavQIODevice::isSequential() const 
{
    return false;
}

Q_INT64 DavQIODevice::readData(char* data, Q_INT64 maxlen)
{
    DDBG << "DavQIODev: readblock, maxlen=" << maxlen << " bytes\n";
    uint readBytes = 0;
    if (handle_->readRaw(maxlen, data, readBytes))
        return -1;
    DDBG << "DavQIODev: readblock: read " << readBytes << " bytes\n";
    return readBytes;
}

Q_INT64 DavQIODevice::writeData(const char* data, Q_INT64 len)
{
    DDBG << "DavQIODev: writeblock, len=" << len << std::endl;
    uint writtenBytes = 0;
    if (handle_->writeRaw(len, data, writtenBytes))
        return -1;
    return writtenBytes;
}
    
DavQIODevice::~DavQIODevice()
{
}

} // namespace Dav
