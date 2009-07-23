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

#ifndef XSLT_STYLESHEET_H
#define XSLT_STYLESHEET_H

#ifdef _MSC_VER
# pragma warning( disable : 4786 )
#endif

#include "xslt/xslt_defs.h"
#include "common/String.h"
#include "common/OwnerPtr.h"
#include "grove/Decls.h"
#include "xslt/impl/Instruction.h"

namespace Xslt {

class TemplateProvider;
class TopStylesheet;

/*! \brief Stylesheet is abstraction of stylesheet instruction.
 *  Each Stylesheet represents also Style Context; note that only
 *  imported (not included!) stylesheets introduce new style context.
 */
class Stylesheet : public Instruction {
public:
    XSLT_OALLOC(Stylesheet);

    Stylesheet(const GroveLib::Element* element,
               const COMMON_NS::String& origin,
               Instruction* p,
               const Stylesheet* parentStylesheet = 0,
               const TopStylesheet* topStylesheet = 0);

    virtual ~Stylesheet();
    //! Reimplemented from InstructionBase
    Type                    type() const { return STYLESHEET; }
    //! Origin URI
    const COMMON_NS::String& origin() const { return origin_; }

    //! Link to the top stylesheet
    const TopStylesheet*    topStylesheet() const { return topStylesheet_; }

    //! Link to the parent stylesheet
    const Stylesheet*       parentStylesheet() const { return parentSheet_; }

    //! Link to the next imported stylesheet in the precedence order
    const Stylesheet*       nextImported() const { return nextImported_; }

    //! returns TRUE if node is stylesheet extension node
    bool                    isExtNode(const GroveLib::Node*) const;

    //! Returns associated template provider
    const TemplateProvider* templateProvider() const
    {
        return tprovider_.pointer();
    }
    //
    bool            isForwardCompatible() const { return isFc_; }
    //!
    void            dump() const;
    void            setNextImport(const Stylesheet* ni) { nextImported_ = ni; }

protected:
    bool            allowsChild(Type) const;

private:
    class AttributeSetMap;

    double                  version_;
    Common::String          origin_;
    Common::String          extensionPrefixes_;
    const TopStylesheet*    topStylesheet_;
    const Stylesheet*       parentSheet_;
    const Stylesheet*       nextImported_;
    Common::OwnerPtr<TemplateProvider> tprovider_;
    bool                    isFc_;
};

} // namespace Xslt

#endif // XSLT_STYLESHEET_H
