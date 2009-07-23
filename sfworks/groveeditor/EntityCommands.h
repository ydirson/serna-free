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
#ifndef ENTITY_COMMANDS_H
#define ENTITY_COMMANDS_H

#include "groveeditor/groveeditor_defs.h"
#include "groveeditor/GroveCommands.h"

#include "common/STQueue.h"
#include "common/Vector.h"
#include "common/PropertyTree.h"

#include "grove/ExternalId.h"
#include "grove/Decls.h"
#include "grove/StripInfo.h"
#include "grove/EntityDecl.h"
#include "grove/EntityReferenceTable.h"

namespace GroveEditor {

typedef COMMON_NS::RefCntPtr<GroveLib::EntityDecl> EntityDeclPtr;

/////////////////////////////////////////////////////////////////////

class InsertEntity : public GroveCommand {
public:
    InsertEntity(const GrovePos& elementPos,
                 const CommonString& name,
                 const CommonString& path,
                 const GroveLib::StripInfo* si);

    CommonString        info(uint) const;
    GroveLib::Node*     validationContext() const;

protected:
    void                doExecute();
    void                doUnexecute();
    GroveLib::Node*     parseExternalEntity() const;
    GroveLib::Node*     parseInternalEntity() const;

private:
    GrovePos                        pos_;
    GroveLib::EntDeclPtr            decl_;
    GrovePos                        ers_;
    GroveLib::DocumentFragmentPtr   removedRef_;
    const GroveLib::StripInfo*      stripInfo_;
    bool                            declCreated_;
};

/////////////////////////////////////////////////////////////////////

class DeclareEntity : public GroveCommand {
public:
    DeclareEntity(GroveLib::Grove* grove,
                  const GroveLib::EntityDecl* decl);
    DeclareEntity(GroveLib::Grove* grove, const Common::String& name);
    virtual ~DeclareEntity() {}

    CommonString            info(uint) const;
    virtual CommandFlags    flags() const
        { return (CommandFlags)(CF_NOVISUAL|CF_OLDPOS); }

protected:
    void                doExecute();
    void                doUnexecute();

private:
    GroveLib::Grove*     grove_;
    GroveLib::EntDeclPtr decl_;
};

/////////////////////////////////////////////////////////////////////

class ChangeEntityDecl : public GroveCommand {
public:
    ChangeEntityDecl(const GrovePos& pos,
                     const COMMON_NS::String& name,
                     const COMMON_NS::String& newName,
                     const COMMON_NS::String& newpath);

    struct AffectedDecl : public COMMON_NS::STQueueItem<AffectedDecl> {
        COMMON_NS::String   name;
        bool                modified;
        AffectedDecl(const COMMON_NS::String& n, bool m)
            : name(n), modified(m) {}
    };
    typedef COMMON_NS::OwnerSTQueue<AffectedDecl> AffectedDeclList;

    COMMON_NS::String           info(uint) const;

    virtual CommandFlags        flags() const { return CF_NOVISUAL; }

protected:
    void                        doExecute();
    void                        doUnexecute();

private:
    GrovePos                    pos_;
    EntityDeclPtr               decl_;
    COMMON_NS::String           oldName_;
    COMMON_NS::String           oldPath_;
    COMMON_NS::String           newName_;
    COMMON_NS::String           newPath_;
    AffectedDeclList            affectedDecls_;
    bool                        was_modified_;
    bool                        removed_;
};

/////////////////////////////////////////////////////////////////////

struct ConvertToEntityData {
    COMMON_NS::String       name;
    GroveLib::ExternalId    externalId;
    bool                    isExternalEntity;
};

class ConvertToEntitySelection : public SplitSelectionCommand {
public:
    ConvertToEntitySelection(const GrovePos& from, const GrovePos& to,
                             const ConvertToEntityData&, bool replicated);

    COMMON_NS::String           info(uint) const;
    const GrovePos&             pos() const;

    virtual void                makeSubcommand(GroveLib::Node* first,
                                               GroveLib::Node* last);
private:
    ConvertToEntityData         data_;
    GroveCommand*               posCmd_;
    bool                        replicated_;
};

/////////////////////////////////////////////////////////////////////

class ConvertToEntity : public GroveCommand {
public:

    ConvertToEntity(GroveLib::Node* from, GroveLib::Node* to,
                    const ConvertToEntityData&,
                    const GrovePos& origPos,
                    bool  replicated);

    CommonString        info(uint) const;

protected:
    void                doExecute();
    void                doUnexecute();

private:
    GroveLib::NodePtr   firstPos_;
    GroveLib::NodePtr   lastPos_;
    ConvertToEntityData data_;
    EntityDeclPtr       decl_;      // new decl
    EntityDeclPtr       oed_;       // old decl
    GrovePos            origPos_;
    GroveLib::EntityReferenceStartPtr savedErs_;
    GroveLib::EntityReferenceEndPtr   savedEre_;
    bool                replicated_;
};

/////////////////////////////////////////////////////////////////////

class ConvertFromEntity : public GroveCommand {
public:
    ConvertFromEntity(const GrovePos& pos);

    CommonString        info(uint) const;

protected:
    void                doExecute();
    void                doUnexecute();

private:
    GrovePos            pos_;
    GroveCommandPtr     convFromSect_;
    EntityDeclPtr       entityDecl_;
};

/////////////////////////////////////////////////////////////////////

class DeclareXinclude : public GroveCommand {
public:
    DeclareXinclude(GroveLib::Node* node, GroveLib::Grove* mainGrove, 
                    const CommonString& url, const CommonString& xpointer, 
                    const CommonString& encoding, bool isConverted);
    virtual ~DeclareXinclude() {}

    CommonString        info(uint) const;
    GroveLib::XincludeDecl* decl() const { return decl_.pointer(); }
    
protected:
    void                doExecute();
    void                doUnexecute();

private:
    GroveLib::NodePtr   node_;
    GroveLib::GrovePtr  grove_;
    GroveLib::Grove*    mainGrove_;
    bool                isConverted_;
    bool                groveInserted_;

    Common::RefCntPtr<GroveLib::XincludeDecl>   decl_;
    GroveLib::EntityReferenceStartPtr           ers_;
    GroveLib::EntityReferenceEndPtr             ere_;
};
    
/////////////////////////////////////////////////////////////////////

class InsertXinclude : public GroveCommand {
public:
    InsertXinclude(const GrovePos& elementPos, GroveLib::XincludeDecl* decl,
                   const CommonString& url, const CommonString& xpointer, 
                   const CommonString& encoding,
                   const Common::PropertyNode* attrs);
    virtual ~InsertXinclude() {}

    CommonString        info(uint) const;
    GroveLib::Node*     validationContext() const 
        { return suggestedPos_.node(); }

protected:
    void                doExecute();
    void                doUnexecute();

private:
    GrovePos                            pos_;
    Common::RefCntPtr<GroveLib::XincludeDecl> decl_;
    GroveLib::DocumentFragmentPtr       oldSect_;
    GroveLib::EntityReferenceStartPtr   sourceErs_;
    GroveLib::EntityReferenceEndPtr     sourceEre_;
    GroveLib::ElementPtr                xinclude_elem_;
};

/////////////////////////////////////////////////////////////////////

GroveLib::EntityDeclSet* entity_decls(const GrovePos&);

} // namespace GroveEditor

#endif // GROVE_COMMANDS_H
