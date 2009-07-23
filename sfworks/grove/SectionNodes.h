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
#ifndef GROVE_SECTION_NODES_H_
#define GROVE_SECTION_NODES_H_

#include "grove/grove_defs.h"
#include "grove/GroveSection.h"
#include "grove/EntityDecl.h"
#include "common/CDList.h"

namespace GroveLib {

class MarkedSectionEnd;
class Element;
class StripInfo;

/*! This class represent a flavor of GroveSection which represents
    marked sections (and XML CDATA sections) in a grove.
 */
class GROVE_EXPIMP MarkedSectionStart : public GroveSectionStart {
public:
    //! A type of marked section.
    enum Type   { temp, include, rcdata, cdata, ignore, entityRef };

    //! Current status of this marked section.
    enum Status { Included, Rcdata, Cdata, Ignored };

    MarkedSectionStart()
        : GroveSectionStart(MARKED_SECTION_START_NODE),
          type_(cdata), status_(Cdata) {}

    virtual const String& nodeName() const;

    //! Returns marked section type.
    Type                type() const { return (Type)type_; }

    //! SGML/XML name for current marked section type.
    String              typeName() const;

    //! Sets a new marked section type.
    void                setType(const Type t) { type_ = t; }

    //! Current status of marked section.
    Status              status() const { return (Status)status_; }

    //! Set a new status for a marked section.
    void                setStatus(const Status s) { status_ = s; }

    /*! For ignored marked sections, this functions returns the data
     *  content of ignored marked section.
     */
    const String&       ignoredChars() const { return ignoredChars_; }

    /// Use this function to alter the contents of ignored marked secion.
    void                setIgnoredChars(const String& s) { ignoredChars_ = s; }

    /// Append to ignored chars
    void                appendIgnoredChars(const String& s)
    {
        ignoredChars_ += s;
    }

    /*! If marked section declaration contains parameter entity reference,
     * this is a entity declaration pointer for this entity.
     */
    EntityDecl*         entityDecl() const { return entity_.pointer(); }

    /// Set new entity decl
    void                setDecl(EntityDecl* d) { entity_ = d; }

    void                dumpInherited() const;
    virtual int         memSize() const;
    
    static GroveSectionStart* make() { return new MarkedSectionStart; }
    
    virtual void            saveAsXml(GroveSaverContext&, int) const;

    GROVE_OALLOC(MarkedSS);

private:
    virtual void copy_sect_start(const GroveSectionStart* other,
                                 Node* newParent);
    EntDeclPtr          entity_;
    String              ignoredChars_;
    char                type_;
    char                status_;
};

/*! The node which signifies end of the marked section.
 */
class GROVE_EXPIMP MarkedSectionEnd : public GroveSectionEnd {
public:
    MarkedSectionEnd()
        : GroveSectionEnd(MARKED_SECTION_END_NODE) {}

    virtual const String&   nodeName() const;
    void                    dumpInherited() const;
    static GroveSectionEnd* make() { return new MarkedSectionEnd; }
    virtual void            saveAsXml(GroveSaverContext&, int) const;
    virtual int             memSize() const;

    GROVE_OALLOC(MarkedSE);
};

/////////////////////////////////////////////////////////////////////

class EntityReferenceEnd;

/*! The kind of GroveSection which represents entity references.
 */
class GROVE_EXPIMP EntityReferenceStart: public GroveSectionStart {
public:
    EntityReferenceStart();
    virtual ~EntityReferenceStart();

    virtual const String& nodeName() const;
    
    EntityReferenceTable* ert() { return erTable_.pointer(); } 

    /// Entity declaration for referenced entity.
    EntityDecl*         entityDecl() const { return decl_.pointer(); }
    
    /// Set new entity declaration.
    void                setDecl(EntityDecl* newDecl) { decl_ = newDecl; }

    /// Checks if upward entity-reference path contains name
    bool                hasLoop(const String& name) const;

    /// Set of entity declarations
    EntityDeclSet*      entityDecls() const;

    /// Current origin grove (taking XInclude into account)
    Grove*              currentGrove() const;

    /// Attach/detach ERS from entity reference table
    virtual void        attachSection();
    virtual void        detachSection();

    /// Returns current XML base
    Common::String      xmlBase(const Common::String& = String()) const;
    
    bool    saveAsXmlFile(int flags = Grove::GS_DEF_FILEFLAGS, 
                          const StripInfo* si = 0,
                          const Common::String& saveAs = Common::String());
    
    bool    saveAsXmlString(Common::String& saveTo,
                            int flags = Grove::GS_DEF_STRFLAGS,
                            const StripInfo* si = 0);
                                                                                
    virtual void        saveAsXml(GroveSaverContext&, int) const;

    GROVE_OALLOC(EntityRS);
    
    static GroveSectionStart* make() { return new EntityReferenceStart; }
    
    void                dumpInherited() const;
    virtual int         memSize() const;
    
    virtual void    processTakeAsFragment(GroveSectionStart*);
    virtual void    processInsertFragment(GroveSectionStart*);
    virtual void    initTables();

private:
    EntityReferenceStart(const EntityReferenceStart&);
    EntityReferenceStart& operator=(const EntityReferenceStart&);   
    virtual void copy_sect_start(const GroveSectionStart* other,
                                 Node* newParent);
    EntDeclPtr      decl_;
    Common::RefCntPtr<EntityReferenceTable> erTable_;
};

/*! This node signifies end of entity reference.
 */
class GROVE_EXPIMP EntityReferenceEnd: public GroveSectionEnd {
public:
    EntityReferenceEnd()
        : GroveSectionEnd(ENTITY_REF_END_NODE) {}

    virtual const String&   nodeName() const;
    void                    dumpInherited() const;
    static GroveSectionEnd* make() { return new EntityReferenceEnd; }
    virtual void            saveAsXml(GroveSaverContext&, int) const {}
    virtual int             memSize() const;

    GROVE_OALLOC(EntityRE);
};

/////////////////////////////////////////////////////////////////////

class RedlineData;

class GROVE_EXPIMP RedlineSectionStart : public GroveSectionStart,
    public Common::CDListItem<RedlineSectionStart> {
public:
    virtual const Common::String& nodeName() const;

    uint            redlineId() const { return redlineId_; }
    RedlineData*    redlineData() const;
    void            setRedlineData(RedlineData*);
    
    Common::String  asString() const;

    static GroveSectionStart* make() { return new RedlineSectionStart; }
    void            dumpInherited() const;
    
    static RedlineSectionStart* find(uint redlineId);
    
    virtual void    saveAsXml(GroveSaverContext&, int) const;
    virtual int     memSize() const;

    void            assignRedlineId();
    
    GROVE_OALLOC(RedlineStart);
    RedlineSectionStart(RedlineData* rd = 0);
    ~RedlineSectionStart();
    
private:
    RedlineSectionStart(const RedlineSectionStart&);
    RedlineSectionStart& operator=(const RedlineSectionStart&);
    
    virtual void copy_sect_start(const GroveSectionStart* other,
                                 Node* newParent);
    
    Common::RefCntPtr<RedlineData>  redlineData_;
    uint                            redlineId_;
};

class GROVE_EXPIMP RedlineSectionEnd : public GroveSectionEnd {
public:
    RedlineSectionEnd()
        : GroveSectionEnd(REDLINE_END_NODE) {}
    
    virtual const Common::String& nodeName() const;
    static GroveSectionEnd* make() { return new RedlineSectionEnd; }
    Common::String          asString() const;
    void                    dumpInherited() const;
    virtual void            saveAsXml(GroveSaverContext&, int) const;
    virtual int             memSize() const;

    GROVE_OALLOC(RedlineEnd);
};

} // namespace GroveLib

#endif // GROVE_SECTION_NODES_H_
