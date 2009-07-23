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

#ifndef DAV_IOSTREAM_H_
#define DAV_IOSTREAM_H_

#include "dav/dav_defs.h"
#include "dav/types.h"
#include "common/RefCntPtr.h"

namespace Dav {

class IoRequestHandle;

// Protocol-independent I/O Stream
class DAV_EXPIMP IoStream {
public:
    OpStatus        status() const;
    OpStatus        close();
    int             readRaw(int n,  char* buf);
    int             writeRaw(int n, const char* buf);
    OpStatus        rewind();
    
    void            setEncoding(int);
    void            setAddCR(bool = false);
    void            encodeCharacterEntities(bool);
    void            setGenerateByteOrderMark(bool);
    bool            generateByteOrderMark() const;
    
    IoStream&       operator<<(const Common::String&);
    IoStream&       operator<<(const Common::Char);
    IoStream&       operator<<(char c) { return operator<<(Common::Char(c)); }
    IoStream&       operator<<(const char* s)
        { return operator<<(Common::String(s)); }
    
    IoStream&       operator>>(Common::Char&);
    IoStream&       operator>>(Common::String&);
    bool            atEof() const;

    //////////////////////////////////////////////////////////
    void            init(IoRequestHandle*);

    IoStream(IoRequestHandle* = 0);
    IoStream(Common::String& writeTo);
    IoStream(const IoStream&);
    IoStream& operator=(const IoStream&);
    ~IoStream();

private:
    class Impl;
    Common::RefCntPtr<Impl> impl_;
};

} // namespace Dav

#endif // DAV_IOSTREAM_H_
