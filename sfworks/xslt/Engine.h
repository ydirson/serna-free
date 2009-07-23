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

#ifndef XSLT_ENGINE_H
#define XSLT_ENGINE_H

#include "xslt/xslt_defs.h"
#include "xslt/Exception.h"
#include "common/String.h"
#include "common/SernaApiBase.h"
#include "grove/Decls.h"
#include "grove/StripInfo.h"

COMMON_NS_BEGIN
class Messenger;
class MessageStream;
class PropertyNode;
COMMON_NS_END

namespace CatMgr {
    class CatalogManager;
};

namespace Xslt {

class TopParamValueHolder;
class ExternalFunction;
class NumberCache;
class DocumentCache;

/*! \brief Engine does an XSL transformations of XML documents
 */
class XSLT_EXPIMP Engine : public SernaApiBase {
public:
    XSLT_OALLOC(Engine);

    class EngineError : public Xslt::Exception {
    public:
       EngineError(const COMMON_NS::String& str);
    };
    static  Engine* makeEngine(COMMON_NS::Messenger* messenger,
                               COMMON_NS::MessageStream* pstream = 0,
                               const CatMgr::CatalogManager* catMgr = 0);
    virtual void    setMessenger(COMMON_NS::Messenger*) = 0;

    //! Sets the XSL style to be applied to the document
    virtual void    setStyle(const COMMON_NS::String& uri) = 0;
    virtual COMMON_NS::String getStyle() const = 0;

    //! Returns document that is result of transformation of given document
    virtual const GroveLib::GrovePtr& transform(const GroveLib::GrovePtr& src,
                                                bool noStrip = false,
                                                Engine* sharedEngine = 0) = 0;

    /*! Updates the result of style transformation if necessary
     * (if source of style changed)
     */
    virtual void    update() = 0;

    /// Returns pointer to StripInfo
    virtual const GroveLib::StripInfo* stripInfo() const = 0;

    /// Returns reference to TopParamValueHolder of top-param \a name
    virtual TopParamValueHolder*
        getTopParamValueHolder(const Common::String& name) const = 0;
    virtual void getTopParamList(Common::PropertyNode* root) const = 0;

    virtual void registerExternalFunction(ExternalFunction* func) = 0;
    virtual void deregisterExternalFunction(ExternalFunction* func) = 0;

    virtual NumberCache*    numberCache() const = 0;
    virtual DocumentCache*  documentCache() const = 0;
    
    virtual ~Engine();
};

}

#endif // XSLT_ENGINE_H
