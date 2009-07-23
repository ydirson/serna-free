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

#ifndef IMPORT_MAP_H_
#define IMPORT_MAP_H_

#include "xs/xs_defs.h"
#include "xs/Schema.h"
#include "common/RefCntPtr.h"
#include "common/RefCounted.h"
#include "common/String.h"
#include "common/Vector.h"

XS_NAMESPACE_BEGIN

using COMMON_NS::Vector;
using COMMON_NS::String;

class ImportMap {
public:
    enum LoadStatus {
        NOT_LOADED, LOADED_OK, LOAD_FAILURE
    };
    struct ImportMapItem {
        Schema*           schema;
        COMMON_NS::RefCntPtr<Schema> schemaRef;  // used only for refcounting
        String            uri;
        String            location;
        LoadStatus        status;
        ImportMapItem()
            : status(NOT_LOADED) {}
    };
    bool                isMapped(const String& uri) const;
    Schema*             ischema(const String& uri);
    void                addImportItem(const String& uri, const String& loc);
    void                appendMySchema(const String& myTarget,
                                       Schema* mySchema);
    void                importedSchemas(Vector<Schema*>&) const;
    void                loadAll();

    ImportMap(Schema* s)
        : schema_(s) {}

    ~ImportMap();

    XS_OALLOC(ImportMap);

private:
    ImportMapItem*          find(const String& uri) const;
    Schema*                 schema_;
    COMMON_NS::Vector<ImportMapItem*>   importList_;
};

XS_NAMESPACE_END

#endif // IMPORT_MAP_H_
