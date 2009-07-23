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

#ifndef XSLT_ENGINE_IMPL_H
#define XSLT_ENGINE_IMPL_H

#include "xslt/xslt_defs.h"
#include "xslt/Engine.h"
#include "xslt/impl/KeyProvider.h"
#include "xslt/impl/KeyGenerator.h"
#include "common/RefCntPtr.h"
#include "common/Message.h"

namespace Xslt {

/*! \brief EngineImpl
 */
class KeyGenerator;
class TopStylesheet;
class TopStylesheetInst;
class WhitespaceStripper;
class XsltResource;
class XsltResourceBuilder;
class DocumentCache;

class XSLT_EXPIMP EngineImpl : public Engine {
public:
    XSLT_OALLOC(EngineImpl);
    EngineImpl(COMMON_NS::Messenger* messenger,
               COMMON_NS::MessageStream* pstream = 0,
               const CatMgr::CatalogManager* = 0);

    virtual void setMessenger(COMMON_NS::Messenger*);

    //! Sets the XSL style to be applied to the document
    void                        setStyle(const COMMON_NS::String&);
    virtual COMMON_NS::String   getStyle() const;

    //! Returns document that is result of transformation of given document
    const GroveLib::GrovePtr&   transform(const GroveLib::GrovePtr& source,
                                          bool noStrip = false,
                                          Engine* sharedEngine = 0);

    //! Updates the result of style transformation if necessary
    //! (if source of style changed)
    void                        update();

    TopStylesheet*              topStylesheet() const;
    TopStylesheetInst*          topStylesheetInst() const;

    virtual const GroveLib::StripInfo* stripInfo() const;

    virtual TopParamValueHolder*
        getTopParamValueHolder(const Common::String& name) const ;
    virtual void getTopParamList(Common::PropertyNode* root) const;
    virtual void registerExternalFunction(ExternalFunction* func);
    virtual void deregisterExternalFunction(ExternalFunction* func);
    virtual NumberCache* numberCache() const;
    virtual DocumentCache*  documentCache() const;

private:
    KeyProvider                             keyProvider_;
    COMMON_NS::OwnerPtr<KeyGenerator>       keyGenerator_;
    COMMON_NS::RefCntPtr<TopStylesheet>     stylesheet_;
    COMMON_NS::RefCntPtr<TopStylesheetInst> instance_;
    COMMON_NS::MessageStream                mstream_;
    COMMON_NS::RefCntPtr<XsltResource>      xsltResource_;
    COMMON_NS::OwnerPtr<XsltResourceBuilder> xsltResourceBuilder_;
    COMMON_NS::OwnerPtr<FunctionRegistry>   functionRegistry_;
    COMMON_NS::String                       styleUri_;
    Common::MessageStream*                  pstream_;
};

} // namespace Xslt

#endif // XSLT_ENGINE_IMPL_H
