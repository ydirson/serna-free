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

#ifndef OUTPUT_PROPERTIES_H
#define OUTPUT_PROPERTIES_H

#include "common/RefCounted.h"
#include "common/common_defs.h"
#include "grove/grove_defs.h"
#include "grove/grove_exports.h"
#include "grove/XmlName.h"
#include <set>

GROVE_NAMESPACE_BEGIN

class GROVE_EXPIMP OutputProperties : public COMMON_NS::RefCounted<> {
public:
    //KLUDGE:: temporally to make safe transformation of using XmlName instead previous QName
    // type of method and cdataSectionElements (Expanded) changed to Qualified
    //TODO: change method and cdataSectionElementstype to ExpandedName
    QualifiedName   method;
    Common::String  version;
    Common::String  encoding;
    bool            omitXmlDeclaration;
    bool            standalone;
    Common::String  doctypePublic;
    Common::String  doctypeSystem;
    bool            indent;
    Common::String  mediaType;

    std::set<QualifiedName> cdataSectionElements;
    void            merge(OutputProperties& output);

    OutputProperties()
      : method("default"), version("1.0"), encoding("UTF8"),
        omitXmlDeclaration(false), standalone(false), indent(false),
        mediaType("text/html") {}

    GROVE_OALLOC(OutputProp);
};

GROVE_NAMESPACE_END

#endif // OUTPUT_PROPERTIES_H
