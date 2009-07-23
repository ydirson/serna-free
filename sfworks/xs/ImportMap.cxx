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
#include "xs/xs_defs.h"
#include "xs/ImportMap.h"
#include "xs/Schema.h"
#include "xs/SchemaResource.h"
#include "xs/XsMessages.h"

USING_COMMON_NS

XS_NAMESPACE_BEGIN

ImportMap::~ImportMap()
{
    for (uint i = 0; i < importList_.size(); ++i)
        delete importList_[i];
    importList_.clear();
}

ImportMap::ImportMapItem* ImportMap::find(const String& uri) const
{
    for (uint i = 0; i < importList_.size(); ++i) {
        if (uri == importList_[i]->uri)
            return importList_[i];
    }
    return 0;
}

bool ImportMap::isMapped(const String& uri) const
{
    ImportMapItem* item = find(uri);
    if (0 == item)
        return false;
    return true;
}

void ImportMap::loadAll()
{
    for (uint i = 0; i < importList_.size(); ++i) {
        ImportMapItem* item = importList_[i];
        if (item->status == NOT_LOADED)
            ischema(item->uri);
    }
}

Schema* ImportMap::ischema(const String& uri)
{
    ImportMapItem* item = find(uri);
    if (0 == item) {
        ImportMapItem* newit = new ImportMapItem;
        newit->uri    = uri;
        newit->status = LOAD_FAILURE;
        importList_.push_back(newit);
        return 0;
    }
    if (item->status == LOADED_OK)
        return item->schema;

    if (item->status == LOAD_FAILURE)
        return 0;

    // Load schema
    SchemaResourceBuilder builder(schema_->getMessenger(),
        schema_->pstream(), schema_->getCatalogManager());
    SchemaResource* sres = dynamic_cast<SchemaResource*>(Uri::uriManager().
        getTopResource("xml-schema", builder, item->location));
    if (0 == sres) {
        schema_->mstream() << XsMessages::unableToLoadSchema
            << Message::L_ERROR << uri << item->location;
        item->status = LOAD_FAILURE;
        return 0;
    }
    item->schema    = sres->schema();
    item->schemaRef = item->schema;
    item->status = LOADED_OK;
    return item->schema;
}

void ImportMap::addImportItem(const String& uri, const String& loc)
{
    ImportMapItem* it = new ImportMapItem;
    it->uri      = uri;
    it->location = loc;
    importList_.push_back(it);
}

void ImportMap::appendMySchema(const String& myTarget, Schema* mySchema)
{
    ImportMapItem* it = new ImportMapItem;
    it->uri    = myTarget;
    it->schema = mySchema;
    it->status = LOADED_OK;
    importList_.push_back(it);
}

void ImportMap::importedSchemas(Vector<Schema*>& sv) const
{
    sv.clear();
    for (uint i = 0; i < importList_.size(); ++i)
        if (importList_[i]->status == LOADED_OK)
            sv.push_back(importList_[i]->schema);
}

XS_NAMESPACE_END
