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
#include "sapi/app/SernaDoc.h"
#include "sapi/app/PluginLoader.h"
#include "sapi/common/PropertyNode.h"

#include "common/PropertyTree.h"
#include "docview/SernaDoc.h"
#include "docview/PluginLoader.h"

typedef ::SernaDoc Doc;

typedef ::PluginLoader Loader;
#define SELF (static_cast<Loader*>(getRep()))

namespace SernaApi {


const SString PluginLoader::PLUGIN_NAME = ::PluginLoader::PLUGIN_NAME;
    
const SString PluginLoader::PLUGIN_DESC = ::PluginLoader::PLUGIN_DESC;
const SString PluginLoader::LOAD_FOR    = ::PluginLoader::LOAD_FOR;
const SString PluginLoader::PLUGIN_DLL  = ::PluginLoader::PLUGIN_DLL;
const SString PluginLoader::RESOLVED_DLL = ::PluginLoader::RESOLVED_DLL;
const SString PluginLoader::PLUGIN_VENDOR = ::PluginLoader::PLUGIN_VENDOR;
const SString PluginLoader::PLUGIN_DATA = ::PluginLoader::PLUGIN_DATA;
const SString PluginLoader::IS_DISABLED = ::PluginLoader::IS_DISABLED;
const SString PluginLoader::PRELOAD_DLL = ::PluginLoader::PRELOAD_DLL;
const SString PluginLoader::RESOLVED_PATH = ::PluginLoader::RESOLVED_PATH;
const SString PluginLoader::SPD_FILE_NAME = ::PluginLoader::SPD_FILE_NAME;

PluginLoader::PluginLoader(SernaApiBase* rep)
    :SimpleWrappedObject(rep)
{
}
    
PluginLoader::~PluginLoader()
{
}
    
PluginLoader::PluginLoader(const PluginLoader& loader)
    : SimpleWrappedObject(loader.getRep())
{
}

PluginLoader& PluginLoader::operator=(const PluginLoader& loader)
{
    SimpleWrappedObject::setRep(loader.getRep());
    return *this;
}

///////////////////////////////////////////////////////////////////////

PluginLoader PluginLoader::instance()
{
    return PluginLoader(&::pluginLoader());
}

PropertyNode PluginLoader::getPluginProps() const
{
    return PropertyNode(SELF->getPluginProps());
}

#define PN_IMPL(node) (static_cast<Common::PropertyNode*>(node.getRep()))

bool PluginLoader::isEnabled(const PropertyNode& pluginProps) const
{
    return SELF->isEnabled(PN_IMPL(pluginProps));
}
    
bool PluginLoader::isLoaded(const PropertyNode& pluginProps,
                            const SernaDoc& doc) const
{
    return SELF->isLoaded(PN_IMPL(pluginProps), 
                          static_cast<const Doc*>(doc.getRep()));
}
    
} // namespace SernaApi
