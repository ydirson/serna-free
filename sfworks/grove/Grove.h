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
#ifndef GROVE_GROVE_H_
#define GROVE_GROVE_H_

#include "grove/grove_exports.h"
#include "grove/Decls.h"
#include "grove/grove_defs.h"
#include "grove/GroveBuilder.h"
#include "common/common_defs.h"
#include "common/SernaApiRefCounted.h"
#include "common/OwnerPtr.h"
#include "common/RefCntPtr.h"
#include "common/String.h"
#include "common/XTreeNode.h"

namespace GroveLib {

class IdManager;
class Element;
class DocumentHeading;
class RedlineTable;

/*! Grove interface class. Grove is typically created by GroveBuilder.
 */
class GROVE_EXPIMP Grove : public Common::SernaApiRefCounted,
                           public Common::XTreeNode<Grove,
                                  Common::XTreeNodeRefCounted<Grove> > {
public:
    /*! Returns Document node of a document. This is a top-level
     *  node for a document (except prolog).
     */
    Document*           document() const;

    /*! Returns pointer to document prolog. Only subset declaration nodes
     *  are saved in prolog.
     */
    DocumentProlog*     prolog() const;
    DocumentHeading*    heading() const;

    /// Entity declaration table.
    EntityDeclSet*      entityDecls() const { return entityDecls_.pointer(); }

    IdManager*          idManager() const { return idManager_.pointer(); }
    void                setIdManager(IdManager*);

    //! Notations declaration table.
    EntityDeclSet&      notations() { return *notations_; }

    //! Parameter entity declaration table accessor
    EntityDeclSet&      parameterEntityDecls() { return *perDecls_; }

    //! Parameter entity reference table accessor
    EntityReferenceTable& parameterErt() { return *perTable_; }

    /*! Sets sysid (currently file name) for top-level fragment,
     * because we can't reliably derive it from grove builder arguments.
     */
    void                setTopSysid(const Common::String& s);

    /// Obtain top sysid of the document
    const Common::String&       topSysid() const;

    /*! Returns entity declaration associated with top-level document
     * entity. This entity is created automatically by GroveBuilder.
     */
    ExternalEntityDecl* topDecl() const;

    const ExternalEntityDecl*   doctypeEntity() const
    {
        return dtEntity_.pointer();
    }
    void                setDoctypeEntity(ExternalEntityDecl* e);

    /// DOCTYPE name string.
    const Common::String& doctypeName() const { return doctypeName_; }

    /// Sets new DOCTYPE name string for a document.
    void                setDoctypeName(const Common::String& dt)
                                      { doctypeName_ = dt; }

    /// Access to grove builder
    GroveBuilder*       groveBuilder() const { return builder_.pointer(); }

    /// Set new grove builder; old one is deleted
    void                setGroveBuilder(GroveBuilder*);

    bool                registerVisitors() const { return registerVisitors_; }
    void                setRegisterVisitors(bool v) { registerVisitors_ = v; }

    ////////////////////////////////////////////////////////////////////

    enum SaveFlags {
        GS_SAVE_PROLOG      = 0001, //! Save prolog
        GS_SAVE_CONTENT     = 0002, //! Save content
        GS_SAVE_ENTITIES    = 0004, //! Save external entities
        GS_EXPAND_ENTITIES  = 0010, //! Expand entities
        GS_SAVE_DEFATTRS    = 0020, //! Save with default attribute values
        GS_FORCE_SAVE       = 0040, //! Save even unmodified entities
        GS_RECURSIVE        = 0100, //! Save all sub-groves, if any
        GS_INDENT           = 0200, //! Indent output
        GS_DONTSAVEDEFENC   = 0400, //! don't always save default encoding
        GS_RESET_MODFLAGS   = 01000,//! reset entity modificatin flags
        GS_CRLF_LINE_BREAKS = 02000,//! use DOS/Win32-style line breaks
        GS_GENERATE_UNICODE_BOM = 04000, //! generate UTF byte-order marks
        GS_DEF_FILEFLAGS = (GS_SAVE_PROLOG|GS_SAVE_CONTENT|GS_SAVE_ENTITIES|
#if defined(_WIN32)
                            GS_CRLF_LINE_BREAKS|
#endif
                            GS_RECURSIVE),
        GS_DEF_STRFLAGS  = (GS_SAVE_PROLOG|GS_SAVE_CONTENT)
    };
    bool    saveAsXmlFile(int flags = GS_DEF_FILEFLAGS,
                          const StripInfo* si = 0,
                          const Common::String& saveAs = Common::String());
    bool    saveAsXmlString(Common::String& saveTo,
                            int flags = GS_DEF_STRFLAGS,
                            const StripInfo* si = 0);
    Grove();
    virtual ~Grove();

    GROVE_OALLOC(Grove);

private:
    DocumentPtr                            document_;
    DocumentPrologPtr                      prolog_;
    Common::RefCntPtr<DocumentHeading>     heading_;
    Common::RefCntPtr<EntityReferenceTable> perTable_;
    Common::RefCntPtr<EntityDeclSet>       perDecls_;
    Common::OwnerPtr<IdManager>            idManager_;
    Common::String                         doctypeName_;
    Common::RefCntPtr<ExternalEntityDecl>  top_decl_;
    Common::RefCntPtr<ExternalEntityDecl>  dtEntity_;
    Common::RefCntPtr<EntityDeclSet>       notations_;
    Common::RefCntPtr<EntityDeclSet>       entityDecls_;
    Common::OwnerPtr<GroveBuilder>         builder_;
    bool                                   registerVisitors_;

private:
    Grove(const Grove&);
    Grove& operator=(const Grove&);
};

} // namespace GroveLib

#endif // GROVE_GROVE_H_
