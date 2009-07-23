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
#ifndef UTILS_DOC_TEMPLATE_H_
#define UTILS_DOC_TEMPLATE_H_

#include "utils/utils_defs.h"
#include "utils/GroveProperty.h"
#include "common/PropertyTree.h"
#include "common/String.h"
#include "common/STQueue.h"
#include "grove/Decls.h"

namespace DocTemplate {

typedef ::GroveProperty GroveProperty;

class UTILS_EXPIMP PiReader : public COMMON_NS::PropertyTree {
public:

    /// Construct PI manager and read/parse PI information from \a filename
    PiReader(const COMMON_NS::String& filename);
    ~PiReader();

    /// Update PI information on a grove, taking information from
    /// specified property set
    static void updatePi(GroveLib::Grove*,
                         const COMMON_NS::PropertyNode* fromSet);

private:
    class PrologParser;
    friend class PrologParser;
};

class UTILS_EXPIMP XmlReader : public COMMON_NS::PropertyTree {
public:
    /// Construct template manager and read PI information \a filename
    XmlReader(const COMMON_NS::String& templatePath, bool isDefault = false);
    ~XmlReader();

    bool isValid() const;
};

////////////////////////////////////////////////////////////////////

class UTILS_EXPIMP DocTemplateHolder : public SernaApiBase,
                                       public Common::PropertyTree {
public:
    DocTemplateHolder();

    /// Finds matched template, according to template priority
    bool    findMatched(Common::PropertyNode* putTo,
                        const Common::PropertyNode* tprops,
                        const Common::String& filename) const;

    static DocTemplateHolder& instance();

    static bool getDocProperties(const Common::String& path,
                                 Common::PropertyNode* root);
protected:
    void        updateTemplates();    
};

} // namespace DocTemplate

#endif // UTILS_DOC_TEMPLATE_H_

