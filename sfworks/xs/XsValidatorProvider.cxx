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
#include "xs/XsValidatorProvider.h"
#include "xs/Schema.h"
#include "xs/SchemaNamespaces.h"
#include "xs/SchemaResource.h"
#include "xs/debug.h"
#include "grove/Nodes.h"
#include "grove/Grove.h"
#include "urimgr/Resource.h"
#include "dav/DavManager.h"
#include "common/StringTokenizer.h"
#include "common/Url.h"
#include "common/PropertyTree.h"

using namespace Common;

namespace Xs {

XsValidatorProvider::XsValidatorProvider(SchemaResourceBuilder& xrb)
    : xrb_(xrb)
{
}

static String get_xsi_attr(const GroveLib::Element* element,
                           const String& attrName)
{
    const GroveLib::Attr* attr = element->attrs().firstChild();
    for (; attr; attr = attr->nextSibling()) 
        if (attr->xmlNsUri() == XSI_NAMESPACE && attr->localName() == attrName)
            return attr->value();
    return String::null();
}

GroveLib::XmlValidatorPtr 
XsValidatorProvider::getValidator(const GroveLib::Grove* grove,
                                  bool *found) const
{
    if (found)
        *found = false;
    const GroveLib::Element* docElem = grove->document()->documentElement();
    if (0 == docElem) 
        return defaultSchema_.pointer();
    String docNs = docElem->xmlNsUri();
    String schema_url;
    if (!docNs.isEmpty()) {
        String smap = get_xsi_attr(docElem, "schemaLocation");         
        for (StringTokenizer st(smap); st; ) {
            String ns = st.next();
            if (!st)
                break;
            String url = st.next();
            if (url.isEmpty() || ns.isEmpty())
                break;
            if (ns == docNs)
                schema_url = url;
        }
    } else
        schema_url = get_xsi_attr(docElem, "noNamespaceSchemaLocation");
    Url parsed_url(schema_url);
    if (!schema_url.isEmpty() && parsed_url.isRelative())
        schema_url = Url(grove->topSysid()).combinePath2Path(parsed_url);
    DDBG << "Found SURL=" << schema_url << std::endl;
    if (schema_url.isEmpty()) {
        DDBG << "getValidator: empty S-URL, returning default validator: "
            << defaultSchema_.pointer() << std::endl;
        return defaultSchema_.pointer();
    }
    RefCntPtr<Xs::SchemaResource> xsres = 
        dynamic_cast<Xs::SchemaResource*>(Uri::uriManager().
            getTopResource("xml-schema", xrb_, schema_url));
    if (xsres.isNull()) {
        DDBG << "getValidator: xsres=0, returning default validator\n";
        return defaultSchema_.pointer();
    }
    if (defaultSchema_ && xsres->schema() == defaultSchema_) {
        DDBG << "getValidator: same schema, using default\n";
        if (found)
            *found = true;
        return defaultSchema_.pointer();
    }
    xsres->schema()->parse2();
    DDBG << "returning parsed schema\n";
    if (found)
        *found = true;
    return xsres->schema()->clone().pointer();
}

void XsValidatorProvider::setDefaultSchema(Schema* s)
{
    defaultSchema_ = s;
}

XsValidatorProvider::~XsValidatorProvider()
{
}

}
