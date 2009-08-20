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
// Copyright (c) 2006 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#ifndef UTILS_ELEMENT_HELP_H_
#define UTILS_ELEMENT_HELP_H_

#include "utils/utils_defs.h"
#include "common/PropertyTree.h"

namespace GroveLib {
    class Node;
}

/// properties structure the help information (element/attribute):
/// <element-localname>ns-uri</element-localname>
///   <short-help>ritchtext-string...</short-help>
///   <qta-help>
///     <href>http://...</href>
///     <adp-file>myfile.adp</adp-file>

class UTILS_EXPIMP HelpHandle : public Common::RefCounted<> {
public:
    // properties returned by elemHelp, attrHelp
    static const char ELEMENT_HELP[];
    static const char ATTR_HELP[];
    static const char ATTR_NAME[];
    static const char ATTR_GROUP[];
    static const char ATTR_GROUP_REF[];
    static const char NSURI[];
    static const char MATCH[];
    static const char SHORT_HELP[];
    static const char QTA_HELP[];
    static const char QTA_ADP_FILE[];
    static const char QTA_HREF[];
    static const char QTA_BASEURL[];

    /// Returns help for the \a element.
    virtual Common::PropertyNodePtr
                    elemHelp(const Common::String& elemName,
                             const GroveLib::Node* nsContext = 0) const = 0;

    /// Returns help info about the attribute in the context of an element
    virtual Common::PropertyNodePtr
                    attrHelp(const Common::String& elemName,
                             const Common::String& attrName,
                             const GroveLib::Node* nsContext = 0) const = 0;
    virtual ~HelpHandle() {}
};

typedef Common::RefCntPtr<HelpHandle> HelpHandlePtr;

UTILS_EXPIMP HelpHandlePtr get_help_handle(const Common::String& filename);

#endif // UTILS_ELEMENT_HELP_H_
