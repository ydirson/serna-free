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
#ifndef ATTRIBUTE_PARSER_H_
#define ATTRIBUTE_PARSER_H_

#include "xs/xs_defs.h"
#include "common/String.h"
#include "common/Vector.h"
#include "common/RefCntPtr.h"
#include "grove/Decls.h"
#include "xs/Origin.h"

class Schema;

XS_NAMESPACE_BEGIN

/*! A class which extracts all Schema-related attributes from an element
    and keeps track of attribute usage.
 */
class AttributeParser {
public:
    typedef COMMON_NS::String String;
    /*! Check whether attribute with a given name is actually a
        Schema Extension attribute
     */
    bool    checkExt(const String& name) const;

    /*! Get an attribute from attribute set; mark attribute as used.
     */
    bool    get(const String& name, String* vp = 0);

    /*! Check whether some attribute present in attributeSet
     */
    bool    has(const String& name) const;

    /*! Get an attribute from attribute set, mark and tokenize it.
     */
    bool    getTokenized(const String& name, COMMON_NS::Vector<String>& tokens);

    /*! Get boolean value of attribute, mark and error report.
     */
    bool    getBoolean(const String& name, bool* bv);

    /*! Get unsigned integer from the value of attribute
     */
    bool    getUnsigned(const String& name, uint* u);

    /*! Return total number of attributes
     */
    ulong  nAttributes() const;

    /*! Return attribute by index
     */
    COMMON_NS::RefCntPtr<GROVE_NAMESPACE::Attr>& getAttr(ulong index) const;

    /*! Report error on all unused attributes
     */
    bool    check() const;

    void    dump() const;

    const GroveLib::Element* origin() { return origin_; }

    AttributeParser(const GroveLib::Element* e, Schema* schema,
                    bool  autoCheck = true);
    ~AttributeParser();

    XS_OALLOC(AttributeParser);

private:
    struct AttrItem {
        const GROVE_NAMESPACE::Attr*   attr;
        bool                    used;
        AttrItem(GROVE_NAMESPACE::Attr* a)
            : attr(a), used(false) {}
        ~AttrItem(){};
    };
    COMMON_NS::Vector<AttrItem>    attrs_;
    Schema*             schema_;
    bool                autoCheck_;
    const GroveLib::Element* origin_;

};

XS_NAMESPACE_END

#endif  //ATTRIBUTE_PARSER_H_
