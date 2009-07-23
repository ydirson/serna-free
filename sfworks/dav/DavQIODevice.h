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

#ifndef DAV_DAV_QIODEVICE_H_
#define DAV_DAV_QIODEVICE_H_

#include "dav/dav_defs.h"
#include "dav/types.h"
#include "common/OwnerPtr.h"
#include <QIODevice>

namespace Dav {

class IoRequestHandle;

class DAV_EXPIMP DavQIODevice : public QIODevice {
public:
    DavQIODevice(IoRequestHandle*);

    virtual void close();
    
    virtual Q_INT64 readData(char* data, Q_INT64 maxlen);
    virtual Q_INT64 writeData(const char* data, Q_INT64 len);
    
    virtual bool    seek (Q_INT64 pos);  
    virtual Q_INT64 pos() const;
    virtual Q_INT64 size() const;
    virtual bool    isSequential() const;

    virtual ~DavQIODevice();

private:
    DavQIODevice(const DavQIODevice&);
    DavQIODevice& operator=(const DavQIODevice&);
    Common::OwnerPtr<IoRequestHandle> handle_;
};

} // namespace Dav

#endif // DAV_DAV_QIODEVICE_H_

