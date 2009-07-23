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

#ifndef XML_NAME_H_
#define XML_NAME_H_

#include "grove/grove_exports.h"
#include "grove_defs.h"
#include "common/String.h"

GROVE_NAMESPACE_BEGIN

class Element;
class NodeWithNamespace;

/*! Pair of strings aka stl::pair but with comparsion operator.
 */
class StringPair {
public:
    StringPair(const COMMON_NS::String& s1)
        : first(s1), second(COMMON_NS::String::null()) {}
    StringPair(const COMMON_NS::String& s1, const COMMON_NS::String& s2)
        : first(s1), second(s2) {}

    bool    operator<(const StringPair& other) const;
    bool    operator==(const StringPair& other) const;

    COMMON_NS::String  first;
    COMMON_NS::String  second;
};

class ExpandedName;
/*! Describes XML qualified name - tuple {prefix, localName}.
 */
class GROVE_EXPIMP QualifiedName : protected StringPair {
public:
    QualifiedName()
        : StringPair(COMMON_NS::String::null()) {}
    QualifiedName(const COMMON_NS::String& localName)
        : StringPair(localName) {}
    QualifiedName(const COMMON_NS::String& localName,
                  const COMMON_NS::String& prefix)
        : StringPair(localName, prefix) {}

    const COMMON_NS::String&   prefix() const    { return second; }
    const COMMON_NS::String&   localName() const { return first;  }
    /*! Resolve ExpandedName into QualifiedName within given context of a grove.
        Does not generate prefix, if uri is not mapped - process only localName.
    */
    bool resolve(const Element* context, const ExpandedName& ename);

    // Qualified name in it's initial form
    COMMON_NS::String          asString() const;

    void            setPrefix(const COMMON_NS::String& p)    { second = p; }
    void            setLocalName(const COMMON_NS::String& l) { first  = l; }

    /*! Returns 'true' if name is really qualified (has a non-null prefix).
     *  Note that empty prefix is not a null prefix.
     */
    bool            isQualified() const { return !second.isEmpty(); }

    /*! Parse an qualified name from the colon-separated pair of names.*/
    void            parse(const COMMON_NS::String& fullname);
    void            dump() const;

    bool            operator<(const QualifiedName& other) const {
        return this->StringPair::operator<(other);
    }
    bool            operator==(const QualifiedName& other) const {
        return this->StringPair::operator==(other);
    }
};

/*! Describes XML expanded name - tuple {URI, localName}.
 */
class GROVE_EXPIMP ExpandedName : protected StringPair {
public:
    const COMMON_NS::String&   uri() const       { return second; }
    const COMMON_NS::String&   localName() const { return first;  }

    void            setUri(const COMMON_NS::String& u)       { second = u; }
    void            setLocalName(const COMMON_NS::String& l) { first  = l; }

    /// Resolve QualifiedName into ExpandedName within given context.
    bool            resolve(const NodeWithNamespace* context,
                            const QualifiedName& qname);

    /// Returns 'true' if name is really qualified (has a non-null prefix).
    /// Note that empty prefix is not a null prefix.
    bool            isQualified() const { return !second.isNull(); }

    void            dump() const;

    ExpandedName()
        : StringPair(COMMON_NS::String::null()) {}
    ExpandedName(const COMMON_NS::String& localName)
        : StringPair(localName) {}
    ExpandedName(const COMMON_NS::String& localName,
                 const COMMON_NS::String& uri)
        : StringPair(localName, uri) {}

    bool            operator<(const ExpandedName& other) const {
        return this->StringPair::operator<(other);
    }
    bool            operator==(const ExpandedName& other) const {
        return this->StringPair::operator==(other);
    }
};

////////////////////////////////////////////////////////////////

inline bool StringPair::operator<(const StringPair& other) const
{
    const int i = first.compare(other.first);
    if (i < 0)
        return true;
    if (i > 0)
        return false;
    return second < other.second;
}

inline bool StringPair::operator==(const StringPair& other) const
{
    return (second == other.second) && (first == other.first);
}

GROVE_NAMESPACE_END

#endif // XML_NAME_H_
