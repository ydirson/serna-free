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
#ifndef GROVE_ENTITYDECL_H_
#define GROVE_ENTITYDECL_H_

#include "grove/grove_exports.h"
#include "grove/grove_defs.h"
#include "grove/Named.h"
#include "grove/ExternalId.h"
#include "common/Encodings.h"
#include "common/RefCntPtr.h"
#include "common/SernaApiRefCounted.h"
#include "common/prtti.h"
#include "common/Vector.h"

namespace Common {
    class Message;
}

namespace GroveLib {

class Notation;
class InternalEntityDecl;
class ExternalEntityDecl;
class EntityDeclNode;
class Grove;
class DocumentFragment;
class EntityReferenceStart;
class XincludeDecl;

/*! Internal representation of an Entity Declaration.
 */
class GROVE_EXPIMP EntityDecl : public Common::SernaApiRefCounted,
                                public NamedBase {
public:
    typedef Common::String String;

    /*! Declaration type. Those enums are self-evident. Note that
     * notation is represented as a kind of entity, and DOCTYPE
     * is a special kind of entity too. Special value "defaulted"
     * is used when entity was implicitly defined via defaulting
     * (referencing of undefined entity).
     */
    enum DeclType {
        invalidDeclType,
        internalGeneralEntity,   externalGeneralEntity,
        internalParameterEntity, externalParameterEntity,
        notation, doctype, pi, subdoc, defaulted, document,
        xinclude, undefined
    };

    /// Entity data type (CDATA, SDATA, NDATA, parsed text (SGML)).
    enum DataType {
        invalidDataType,
        cdata, sdata, ndata, sgml
    };

    /*! Origin of an entity declaration. The bitmask-style representation
     * is for ease of if's. "prolog" means that entity was declared
     * in prolog, "dtd" means that entity was defined in external subset,
     * "schema" means that entity was declared via some schema, "sd"
     * means that entity was declared in SGML declaration, and "special"
     * is used for top-level document entity (which are created implicitly).
     */
    enum DeclOrigin {
        invalidDeclOrigin = 0000,
        prolog = 0001, dtd = 0002, schema = 0004, sd = 0010, special = 0020
    };
    /// Declaration type
    DeclType            declType() const { return (DeclType)declType_; }

    /// Entity content data type
    DataType            dataType() const { return (DataType)dataType_; }

    /// Declaration origin flags
    DeclOrigin          declOrigin() const { return (DeclOrigin)declOrigin_; }

    /// Pointer to origin (parent) entity where this entity was defined.
    const EntityDecl*   originEntityDecl() const;

    /// Sets entity name
    void                setEntityName(const String&);

    /// Sets declaration type
    void                setDeclType(DeclType t) { declType_ = t; }

    /// Sets entity content data type
    void                setDataType(DataType t) { dataType_ = t; }

    /// Sets entity declaration origin
    void                setDeclOrigin(DeclOrigin orig) { declOrigin_ = orig; }

    /// Sets parent entity pointer
    void                setOriginEntityDecl(EntityDecl* decl);

    /*! Sets "modified" flag. This flag should be set after any modification
     *  of entity data, and has an effect that this entity declaration
     *  will be saved in the beginning of the prolog.
     *  NOTE: user applications MUST set this flag because GroveSaver
     *  requires it.
     */
    void                setDeclModified(bool v = true) { declModified_ = v; }

    /// True, if entity declaration data was modified.
    bool                isDeclModified() const { return declModified_; }

    /*! Set this flag only of entity _content_ was modified. For internal
     *  entities, you need also call setDeclModified(), because modification
     *  of internal entity implies declaration of it's declaration.
     *  NOTE: user applications MUST set this flag because GroveSaver
     *  requires it for correct saving.
     */
    void                setContentModified(bool v = 1) { contentModified_ = v; }

    /// True, if entity content was modified
    bool                isContentModified() const { return contentModified_; }

    /// True if current entity is actually a parameter entity.
    bool                isParameterEntity() const;

    /*! The string of original entity declaration. Used for fast saving
     *  of unmodified entities without reconstructing them.
     */
    const String&       originalDecl() const;

    /*! Sets original declaration string (not for general use, although
     *  together with setModified(false) may be used as a brute-force method
     *  of redefining prolog entity when saving a document).
     */
    void                setOriginalDecl(const String&);

    /*! If original entity declaration had any comments, those are
     *  preserved here. Used for reconstructing modified entity,
     *  preserving user comments.
     */
    const String&       comment() const;

    /// Sets comment string.
    void                setComment(const String&);

    /// Returns reconstructed declaration string.*/
    virtual String      asDeclString() const;

    virtual String      asAnnotationString() const;

    /// Returns pointer to the declaration node in prolog, if any
    EntityDeclNode*     entityDeclNode() const { return declNode_; }

    /// Set new declaration node
    void                setEntityDeclNode(EntityDeclNode* n) { declNode_ = n; }

    /// Parse entity content; return readonly value to \a isReadonly
    virtual DocumentFragment*
            parse(const EntityReferenceStart*,
                  bool* /*isReadonly*/ = 0) const { return 0; }
    
    bool                isReadOnly() const { return readOnly_; }
    void                setReadOnly(bool v) { readOnly_ = v; }
    
    bool                isSingleCharEntity() const;

    /// Deep-copy entity declaration.
    virtual EntityDecl* copy() const;

    /// Dumps out debugging information.
    void                dump() const;
            
    PRTTI_DECL(Notation);
    PRTTI_DECL(InternalEntityDecl);
    PRTTI_DECL(ExternalEntityDecl);
    PRTTI_DECL(XincludeDecl);

    EntityDecl();
    virtual ~EntityDecl();

    GROVE_OALLOC(EntityDecl);

protected:
    EntityDecl&         operator=(const EntityDecl&);
    bool                entity_prolog(String& s) const;
    
    COMMON_NS::RefCntPtr<EntityDecl> origin_;
    COMMON_NS::String   originalDecl_;
    COMMON_NS::String   comment_;
    
    EntityDeclNode*     declNode_;

    uint                declType_        : 6;
    uint                dataType_        : 4;
    uint                declOrigin_      : 8;
    uint                declModified_    : 1;
    uint                contentModified_ : 1;
    uint                readOnly_        : 1;

private:
    EntityDecl(const EntityDecl&);
};

/*! The common class for external-entity-like entities (brr), namely
    notations and external entity themselves.
 */
class GROVE_EXPIMP EntityDeclExt : public EntityDecl {
public:
    virtual ~EntityDeclExt();

    /// Returns External ID of an entity.
    ExternalId&     externalId();

    /// Returns public ID of an entit (shortcut to externalId)
    const PublicId& pubid() const;

    /// Returns system ID of an entity (shortcut to externalId)
    const String&   sysid() const;

    /// sets new external ID on a entity
    void            setExternalId(const ExternalId&);

    /// sets new public ID on a entity
    void            setPubid(const PublicId&);

    /// sets new system ID on a entity
    void            setSysid(const String&);

protected:
    void            add_specific_decl(String&) const;
    void            assignExt(const EntityDeclExt&);

protected:
    ExternalId      extid_;
};

/*! Notation. Has same interface as EntityDeclExt.
 */
class GROVE_EXPIMP Notation : public EntityDeclExt {
public:
    virtual String asDeclString() const;
    virtual EntityDecl* copy() const;

    PRTTI_DECL(Notation);

    virtual ~Notation();
};

/*! Internal entity declaration.
 */
class GROVE_EXPIMP InternalEntityDecl : public EntityDecl {
public:
    /// CDATA-content of an internal entity.
    const COMMON_NS::String&   content() const { return content_; }

    /// set CDATA content of an internal entity
    void            setContent(const String& content) { content_ = content; }

    /// Return entity declaration as string in DTD
    virtual String  asDeclString() const;

    /// Rebuild entity declaration according to content
    void            rebuildContent(Grove*);
    
    virtual DocumentFragment* parse(const EntityReferenceStart*,
                                    bool* = 0) const;

    PRTTI_DECL(InternalEntityDecl);

    virtual ~InternalEntityDecl();

private:
    virtual EntityDecl* copy() const;
    String          content_;
};

/*! External entity declaration.
 */
class GROVE_EXPIMP ExternalEntityDecl : public EntityDeclExt {
public:
    PRTTI_DECL(ExternalEntityDecl);

    ExternalEntityDecl();
    virtual ~ExternalEntityDecl();

    /// Name of notation for this entity.
    const COMMON_NS::String& notationName() const { return notationName_; }

    /// Sets notation name for this entity.
    void            setNotationName(const String& nn) { notationName_ = nn; }

    /// Return entity declaration as string in DTD
    virtual String  asDeclString() const;

    Common::Encodings::Encoding encoding() const { return encoding_; }
    void    setEncoding(Common::Encodings::Encoding e) { encoding_ = e; }

    /// Returns entity path (not just sysid)
    COMMON_NS::String entityPath(const Grove*) const;
    
    virtual DocumentFragment* parse(const EntityReferenceStart*,
                                    bool* = 0) const;

private:
    virtual EntityDecl* copy() const;
    COMMON_NS::String           notationName_;
    Common::Encodings::Encoding encoding_;
};

/*! XInclude declaration. 
 *  Some EntityDecl fields have special meaning:
 *      declType() is always xinclude
 *      originalDecl() is used for expression
 *      dataType() can be cdata or sgml (equiv. to text/xml in xinclude)
 */
class GROVE_EXPIMP XincludeDecl : public EntityDecl {
public:
    XincludeDecl();
    ~XincludeDecl();

    void            setUrl(const String& s) { url_ = s; }
    const String&   url() const { return url_; }
    const String&   expr() const { return originalDecl_; }

    void            setEncodingString(const String& s) { encoding_ = s; }
    const String&   encodingString() const { return encoding_; }

    EntityReferenceStart* referenceErs() const { return refErs_; }
    void            setReferenceErs(EntityReferenceStart* r) { refErs_ = r; }

    Grove*          parentGrove() const { return parentGrove_; }
    void            setParentGrove(Grove* g) { parentGrove_ = g; }

    void            setFallback(bool v) { isFallback_ = v; }
    bool            isFallback() const { return isFallback_; }

    typedef Common::Vector<Common::RefCntPtr<Common::Message> > MessageList;

    MessageList&    messageList() { return messageList_; }
    
    virtual String      asAnnotationString() const;

    PRTTI_DECL(XincludeDecl);

private:
    XincludeDecl(const XincludeDecl&);
    XincludeDecl& operator=(const XincludeDecl&);
    
    MessageList             messageList_;
    String                  url_;
    String                  encoding_;
    EntityReferenceStart*   refErs_;
    Grove*                  parentGrove_;
    bool                    isFallback_;
};

////////////////////////////////////////////////////////////////////
//
// Inlines
//
////////////////////////////////////////////////////////////////////

inline bool EntityDecl::isParameterEntity() const
{
    return (declType_ == internalParameterEntity ||
        declType_ == externalParameterEntity);
}

inline void EntityDecl::setEntityName(const Common::String& entityName)
{
    NamedBase::setName(entityName);
}

inline const EntityDecl* EntityDecl::originEntityDecl() const
{
    return origin_.pointer();
}

inline void EntityDecl::setOriginEntityDecl(EntityDecl* decl)
{
    origin_ = decl;
}

//////////////////////////////////////////////////////////////////////

inline const PublicId& EntityDeclExt::pubid() const
{
    return const_cast<ExternalId&>(extid_).pubid();
}

inline const Common::String& EntityDeclExt::sysid() const
{
    return extid_.sysid();
}

inline ExternalId& EntityDeclExt::externalId()
{
    return extid_;
}

inline void EntityDeclExt::setPubid(const PublicId& pubid)
{
    extid_.setPubid(pubid);
    extid_.pubid().parse();
}

inline void EntityDeclExt::setSysid(const Common::String& sysid)
{
    extid_.setSysid(sysid);
}

inline void EntityDeclExt::setExternalId(const ExternalId& extid)
{
    extid_ = extid;
}

} // namespace GroveLib

#endif // ENTITYDECL_H_
