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

#ifndef SCHEMA_H_
#define SCHEMA_H_

#include "xs/xs_defs.h"
#include "common/RefCounted.h"
#include "common/RefCntPtr.h"
#include "common/OwnerPtr.h"
#include "common/StringDecl.h"
#include "grove/Decls.h"
#include "grove/XNodePtr.h"
#include "grove/XmlValidator.h"

#include "common/STQueue.h"
#include "common/Message.h"
#include "common/ThreadingPolicies.h"

#include <list>

COMMON_NS_BEGIN
class Messenger;
class PropertyNode;
template <class T> class Vector;
COMMON_NS_END

namespace CatMgr {
    class CatalogManager;
}

class EnumFetcher;

XS_NAMESPACE_BEGIN
class XsData;
class XsDataImpl;
class ExtEnumCache;

XS_EXPIMP extern const char* const ATTR_TYPE;
XS_EXPIMP extern const char* const ATTR_REQUIRED;
XS_EXPIMP extern const char* const DEFAULT_ATTR_VALUE;
XS_EXPIMP extern const char* const FIXED_ATTR_VALUE;
XS_EXPIMP extern const char* const ATTR_VALUE_ENUM;
XS_EXPIMP extern const char* const ELEM_VALUE_DEFAULT;
XS_EXPIMP extern const char* const ELEM_VALUE_ENUM;
XS_EXPIMP extern const char* const ELEM_VALUE_ENUM_ITEM;
XS_EXPIMP extern const char* const ELEM_VALUE_IS_LIST;

XS_NAMESPACE_END

namespace GroveLib {
    class ElemNameList;
    class RequiredAttributesProvider;
}

/*! Top-level interface to XML Schema validator.
 */
class XS_EXPIMP Schema : public GroveLib::XmlValidator {
public:
    /// reimplemented from XmlValidator
    virtual bool validate(GroveLib::Element* elem, int flags) const;
    
    enum vFlags {
        makeChanges  = 0001,    // Allow changes to original document
        beSilent     = 0002,    // Do not generate error messages
        nodeNotes    = 0004,    // attach Invalidity structs to nodes
        fixupComplex = 0040,    // fixup complex content
        editMode     = 0100,    // editing mode: make node exts & choices
        dontMakeElems = 0200,   // dont't create elems (edit mode only)
        rmExtraAttrs = 0400,    // remove extra attributes
        dontCheckIdRefs = 01000, // do not check ID refs
        defaultMode = (makeChanges)
    };
    /*! Set validation flags
     */
    void                setValidationFlags(int f);

    /*! Get validation flags
     */
    vFlags              validationFlags() const { return validationFlags_; }

    /*! Validate document (or sub-document)
     */
    bool                validate(GROVE_NAMESPACE::Element* root);

    /*! Validate simple type by the type name and data value
     */
    bool                validate(const COMMON_NS::String& simpleType,
                                 const COMMON_NS::String& data,
                                 COMMON_NS::String* result = 0);

    /*! Partial validation - possible only in edit mode */
    bool                partialValidate(GROVE_NAMESPACE::Element* elem,
                                        bool dontCreateContent = false);

    /*! Create recursive skeleton of an element, given \a elementName.
     */
    bool                makeSkeleton(const COMMON_NS::String& elementName,
                                      GROVE_NAMESPACE::Grove* referenceGrove,
                                      GROVE_NAMESPACE::ElementPtr& ep,
                                      const GROVE_NAMESPACE::Element* pe = 0);

    void                getRootElementList(Common::PropertyNode*) const;
    void                getAttrSpecsForTopElement(
                                const Common::String& localName,
                                const Common::String& uri,
                                Common::PropertyNode* specList,
                                const GroveLib::Element* = 0) const;

    /*! Get pointer to the invalidity collection list
     */
    const COMMON_NS::STQueue<GROVE_NAMESPACE::Invalidity>& invalidities() const;

    /*! Clean an invalidity collection list
     */
    void                cleanInvalidities();

    /*! Obtain target namespace of this XML Schema */
    const COMMON_NS::String&       targetNsUri() const;

    /*! Get a pointer to the messenger instance for the current schema.
     */
    COMMON_NS::Messenger*          getMessenger() const;

    /*! Set a new messenger for the schema. Old messenger will be
        deleted.
     */
    void                setMessenger(COMMON_NS::Messenger*);

    /*! Set a new external enum fetcher
     */
    void                setEnumFetcher(EnumFetcher*);

    /*! Get a current enum fetcher
     */
    EnumFetcher*        getEnumFetcher() const;

    /*! Access URI Manager instance for the current schema.
     */
    /*! Clone an instance of a schema with the current set of
        parameters.
     */
    COMMON_NS::RefCntPtr<Schema>         clone() const;

    /*! Clone an instance of a schema with all the parameters
        reset to default.
     */
    COMMON_NS::RefCntPtr<Schema>         cloneDefault() const;

    /*! Manually force resolution of all component references
     */
    void            resolveComponents();

    /*! Obtain information about imported/included schemas.
        Note that by calling this function all references
        within the schema will be resolved.
     */
    void        importedSchemas(COMMON_NS::Vector<Schema*>&);

    /*! Get (to \a splitPoints) the indexes of the elements in the
        path which may occur more than once in the content. Returns
        false of \a path does not exist in the content model.
     */
    bool        splitPoint(const COMMON_NS::Vector<COMMON_NS::String>& path,
                           COMMON_NS::Vector<int>& splitPoints) const;

    void        setCommandMaker(GroveLib::ValidationCommandsMaker* m)
    {
        commandMaker_ = m;
    }
    GroveLib::ValidationCommandsMaker* commandMaker() const 
    {
        return commandMaker_;
    }
    void    setRequiredAttrsProvider(GroveLib::RequiredAttributesProvider* ap)
    {
        reqAttrsProvider_ = ap;
    }
    GroveLib::RequiredAttributesProvider* requiredAttrsProvider() const 
    {
        return reqAttrsProvider_;
    }

    /*! Access to shared schema data
     */
    XS_NAMESPACE::XsData* xsd() const;

    ///////////////////////////////////////////////////////////
    //
    // the following functions are used internally
    //
    XS_NAMESPACE::XsDataImpl*   xsi() const;
    XS_NAMESPACE::ExtEnumCache* getEnumCache() const;
    void                        idrefDelayedCheck(const GroveLib::Attr*);

    void        setCatalogManager(const CatMgr::CatalogManager* catMgr)
    {
        catMgr_ = catMgr;
    }
    const CatMgr::CatalogManager* getCatalogManager() const { return catMgr_; }

    /*! Parse a schema from it's XML representation.
     */
    void                parse(GROVE_NAMESPACE::Element* schemaRoot);
    void                parse2(); // 2-nd step parsing

    /*! Progress stream is for handling progress messages.
        (i.e to show what validator is doing)
     */
    void setProgressStream(COMMON_NS::MessageStream* pstream);
    COMMON_NS::MessageStream* pstream();

    COMMON_NS::MessageStream& mstream();

    Schema(COMMON_NS::Messenger* messenger);
    ~Schema();

    XS_OALLOC(Schema);

protected:
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsData> xsd_; // shared XML schemas data
    COMMON_NS::RefCntPtr<XS_NAMESPACE::XsDataImpl> xsi_;

private:
    typedef Common::Vector<GroveLib::AttrPtr> IdrefCheckQueue;
    bool        check_delayed_idrefs();

    vFlags validationFlags_;
    COMMON_NS::MessageStream  mstream_;
    COMMON_NS::MessageStream* pstream_; //message stream for progress bar
    COMMON_NS::OwnerPtr<EnumFetcher> fetcher_;
    COMMON_NS::OwnerPtr<COMMON_NS::STQueue<GROVE_NAMESPACE::Invalidity> >
        invalidities_;
    COMMON_NS::OwnerPtr<XS_NAMESPACE::ExtEnumCache> enumCache_;
    GroveLib::ValidationCommandsMaker* commandMaker_;
    GroveLib::RequiredAttributesProvider* reqAttrsProvider_;
    Common::OwnerPtr<IdrefCheckQueue> idq_;
    const CatMgr::CatalogManager* catMgr_;

    Schema(const Schema&);
    Schema& operator=(const Schema&);
};

////////////////////////////////////////////////////////

inline const COMMON_NS::STQueue<GROVE_NAMESPACE::Invalidity>&
Schema::invalidities() const
{
    return *invalidities_;
}

inline COMMON_NS::Messenger* Schema::getMessenger() const
{
    return mstream_.getMessenger();
}

inline XS_NAMESPACE::XsData* Schema::xsd() const
{
    return xsd_.pointer();
}

inline XS_NAMESPACE::XsDataImpl* Schema::xsi() const
{
    return xsi_.pointer();
}

inline EnumFetcher* Schema::getEnumFetcher() const
{
    return fetcher_.pointer();
}

inline XS_NAMESPACE::ExtEnumCache* Schema::getEnumCache() const
{
    return enumCache_.pointer();
}

#endif // SCHEMA_H_
