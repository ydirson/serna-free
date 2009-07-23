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
#ifndef XSLT_IMPL_DOCUMENT_CACHE_H_
#define XSLT_IMPL_DOCUMENT_CACHE_H_

#include "xslt/xslt_defs.h"
#include "common/XList.h"
#include "common/RefCntPtr.h"
#include "common/RefCounted.h"
#include "grove/Decls.h"

namespace Common {
    class Messenger;
}

namespace Xslt {

class TopStylesheetInst;

class XSLT_EXPIMP CachedDocument : public Common::RefCounted<>,
                                   public Common::XListItem<CachedDocument> {
public:
    CachedDocument(GroveLib::Grove* g,
                   const Common::String& uri);
    ~CachedDocument();

    GroveLib::Grove*    grove() const { return grove_.pointer(); }
    const Common::String& uri() const { return uri_; }

private:
    GroveLib::GrovePtr  grove_;
    Common::String   uri_;
};

typedef Common::RefCntPtr<CachedDocument> CachedDocumentPtr;

/*! Cache of documents for use with xsl:document() function. This cache
    is mainained to avoid multiple parsing of the same document for each
    re-evaluation of xsl:document().
 */
class XSLT_EXPIMP DocumentCache {
public:
    typedef Common::XList<CachedDocument> CachedDocumentList;
    
    DocumentCache(TopStylesheetInst* tsi);
    ~DocumentCache();

    CachedDocumentPtr   getDoc(const Common::String& uri,
                               const GroveLib::Element* instrNode,
                               const GroveLib::Node* contextNode,
                               bool doLoad);

    const CachedDocumentList& docList() const { return docs_; }
    
private:
    DocumentCache(const DocumentCache&);
    DocumentCache& operator=(const DocumentCache&);


    TopStylesheetInst* topInst_;
    CachedDocumentList docs_;
    Common::RefCntPtr<Common::Messenger> silentMessenger_;
};

} // namespace Xslt

#endif  // XSLT_IMPL_DOCUMENT_CACHE_H_

