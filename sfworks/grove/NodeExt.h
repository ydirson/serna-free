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
#ifndef NODE_EXT_H_
#define NODE_EXT_H_

#include "grove/grove_defs.h"
#include "common/prtti.h"
#include "common/RefCntPtr.h"
#include "common/RefCounted.h"

class XsNodeExt;    // for validator

GROVE_NAMESPACE_BEGIN

class EntityReferenceStart;
class OriginBase;
class LineLocExt;
class EntityNodeExt;

class GROVE_EXPIMP NodeExt {
public:
    virtual void dump() const {}
    virtual NodeExt* copy() const { return 0; }
    virtual ~NodeExt() {}

    GROVE_OALLOC(NodeExt);
    PRTTI_DECL(LineLocExt);
    PRTTI_DECL(EntityNodeExt);
    PRTTI_DECL(XsNodeExt);
};

class GROVE_EXPIMP EntityNodeExt : public NodeExt {
public:
    EntityReferenceStart*   getErs() const { return ers_; }
    void                    setErs(EntityReferenceStart* ers) { ers_ = ers; }

    EntityNodeExt()
        : ers_(0) {}
    virtual ~EntityNodeExt() {}
    virtual void dump() const;
    virtual NodeExt* copy() const;

    PRTTI_DECL(EntityNodeExt);

private:
    EntityReferenceStart*   ers_;
};

/*! Line origin - contains only line number and pointer to corresponding
 *  FileLocExt.
 */
class GROVE_EXPIMP LineLocExt : public EntityNodeExt {
public:

    LineLocExt(const OriginBase& loc);
    LineLocExt(long line = -1, long column = -1);

    //! Get line number
    long line() const { return line_; }
    //! Get column number
    long column() const { return column_; }

    void setLineInfo(long line, long col) { line_ = line; column_ = col; }

    bool isReadOnly() const { return readOnly_; }
    void setReadOnly(bool v) { readOnly_ = v; }

    // user-defined data
    uchar udata() const { return udata_; }
    void  setUdata(uchar v) { udata_ = v; }

    virtual void dump() const;
    virtual ~LineLocExt();
    virtual NodeExt* copy() const;

    PRTTI_DECL(LineLocExt);

private:
    long line_;
    int  column_   : 24;
    int  udata_    : 7;
    int  readOnly_ : 1;
};

GROVE_NAMESPACE_END

#endif // NODE_EXT_H_
