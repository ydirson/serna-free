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
#ifndef SAPI_DOC_TEMPLATE_HOLDER_H_
#define SAPI_DOC_TEMPLATE_HOLDER_H_

#include "sapi/sapi_defs.h"
#include "sapi/common/SString.h"
#include "sapi/common/WrappedObject.h"

namespace SernaApi {

class PropertyNode;
    
class SAPI_EXPIMP DocTemplateHolder : public SimpleWrappedObject {
public:
    DocTemplateHolder(SernaApiBase* = 0);

    static DocTemplateHolder instance();
    
    /// Returns properties taken from document and matched template
    bool    getDocProperties(const SString& path, PropertyNode& root);

    /// Returns properties for all loaded document templates
    PropertyNode    getDocTemplates();
    
    /// Returns matched templates sorted by priority
    bool    findMatchedTemplates(PropertyNode& result,
                                 const PropertyNode& fileProps,
                                 const SString& fileName) const;
};

} // namespace SernaApi

#endif // SAPI_DOC_TEMPLATE_HOLDER_H_
