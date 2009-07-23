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

#ifndef XSLT_TEMPLATE_PROVIDER_H
#define XSLT_TEMPLATE_PROVIDER_H

#ifdef _MSC_VER
# pragma warning( disable : 4786 )
#endif

#include "xslt/xslt_defs.h"
#include "common/String.h"
#include <list>
#include <map>

namespace Xslt {

class Template;
class Stylesheet;

/*! \brief TemplateProvider
 *  This class provides templates by name or in the order of priority
 *  and import precedence.
 */
class TemplateProvider {
public:
    XSLT_OALLOC(TemplateProvider);

    typedef std::list<const Template*> TemplateList;
    typedef TemplateList::const_iterator TemplateListCIter;
    typedef std::map<COMMON_NS::String, TemplateList> ModeMap;
    typedef std::map<COMMON_NS::String, const Template*> NamedTemplateMap;

    class TemplateIterator {
    public:
        /// Fetch next template in list (by priority)
        const Template* nextTemplate();
        TemplateIterator(const TemplateProvider* tp,
                         const TemplateList& list,
                         const COMMON_NS::String& mode,
                         bool  importsOnly);
    private:
        const TemplateProvider* tp_;
        const Stylesheet* startStylesheet_;
        TemplateListCIter current_;
        TemplateListCIter end_;
        COMMON_NS::String mode_;
        bool  importsOnly_;
    };
    TemplateProvider(const Stylesheet* s);
    ~TemplateProvider();

    /// Get template by name
    const Template* getTemplate(const COMMON_NS::String& name) const;

    /// Get first template in template list (by mode).
    /// If importsOnly is true, only imported templates which belong
    /// to the subtree of current stylesheet is considered.
    TemplateIterator getFirstTemplate(const COMMON_NS::String& mode,
                                      bool  importsOnly = false) const;

    /// Get next-iterator for the given template
    TemplateIterator iteratorForTemplate(const Template* t,
                                         bool importsOnly) const;

    /// Return associated style context (stylesheet)
    const Stylesheet*   styleContext() const { return stylesheet_; }

    /// Register new template with TemplateProvider
    bool                registerTemplate(Template*);

    void                dump() const;

private:
    friend class TemplateIterator;
    const TemplateList& get_first(const COMMON_NS::String& mode) const;

    NamedTemplateMap namedMap_;
    ModeMap          modeMap_;
    const Stylesheet* stylesheet_;
};

} // namespace Xslt

#endif // XSLT_TEMPLATE_PROVIDER_H
