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
/*! \file
 */

#ifndef XSGROUP_H_
#define XSGROUP_H_

#include "xs/xs_defs.h"
#include "xs/Component.h"

XS_NAMESPACE_BEGIN

class Connector;

/*! Interface to implementation of named ModelGroup's.
 */
class XS_EXPIMP XsGroup : public Component {
public:

    virtual void    dump(int indent) const = 0;

    virtual Connector* connector(Schema* s) const = 0;

    static XsGroup* makeEmptyStub(const NcnCred&);

    XsGroup(const Origin& origin, const NcnCred& cred);

    virtual ~XsGroup();

    PRTTI_DECL(XsGroup);
};

XS_NAMESPACE_END

#endif // XSGROUP_H_
