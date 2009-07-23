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

#ifndef XMLNS_H_
#define XMLNS_H_

#include "common/String.h"
#include "common/Vector.h"
#include "grove/Decls.h"

GROVE_NAMESPACE_BEGIN

namespace XmlNs
{
    /*! Default namespace if it is not defined  otherwise - empty
     * (non-null!) string
     */
    GROVE_EXPIMP const COMMON_NS::String&  defaultNs();

    ///XML namespace mapping attribute name
    GROVE_EXPIMP const COMMON_NS::String&  xmlnsatt();

    /// Pre-defined 'xml' namespace prefix
    GROVE_EXPIMP const COMMON_NS::String&  xmlpref();

    /// Xinclude namespace
    GROVE_EXPIMP const COMMON_NS::String&  xincludeNs();

    /// For grove-builders use only
    GROVE_EXPIMP bool    parseXmlNsAtt(const COMMON_NS::String& attname,
                                             COMMON_NS::String& prefix);
    /// For grove-builders use only
    GROVE_EXPIMP void    makePredefinedNamespaces(GROVE_NAMESPACE::Grove*);
}

GROVE_NAMESPACE_END

#endif // XMLNS_H_
