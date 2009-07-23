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
 *
 ***********************************************************************/

#ifndef GROVEEDITOR_GROVE_COMMANDS_H
#define GROVEEDITOR_GROVE_COMMANDS_H

#include "groveeditor/GroveCommand.h"
#include "groveeditor/SelectionCommand.h"
#include "grove/EntityReferenceTable.h"
#include "common/PropertyTree.h"
#include <set>

namespace GroveEditor {

class NodePos;

/////////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP InsertText : public GroveCommand {
public:
    InsertText(const GrovePos& pos, const COMMON_NS::String& text);

    CommonString        info(uint) const;
    GroveLib::Node*     validationContext() const;
    virtual bool        mergeWith(Common::Command*);

    const Common::String& text() const { return text_; }

protected:
    void                doExecute();
    void                doUnexecute();

private:
    GrovePos            pos_;
    COMMON_NS::String   text_;
};

/////////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP RemoveText : public GroveCommand {
public:
    RemoveText(const GrovePos& pos, uint length, GrovePos* adjustPos = 0);

    CommonString        info(uint) const;
    GroveLib::Node*     validationContext() const;

protected:
    void                doExecute();
    void                doUnexecute();

private:
    GrovePos                pos_;
    COMMON_NS::String       removed_;
    GroveLib::NodePtr       parent_;
};

class GROVEEDITOR_EXPIMP ReplaceText : public GroveCommand {
public:
    ReplaceText(const GrovePos& pos, uint length,
                const COMMON_NS::String& newText);

    CommonString        info(uint) const;
    GroveLib::Node*     validationContext() const;

protected:
    void                doExecute();
    void                doUnexecute();
    void                replace(const Common::String&, const Common::String&);

private:
    GrovePos            pos_;
    Common::String      removedText_;
    Common::String      newText_;
};

/////////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP InsertTextNode : public GroveCommand {
public:
    InsertTextNode(const GrovePos& elementPos,
                   const COMMON_NS::String& text);

    CommonString        info(uint) const;
    GroveLib::Node*     validationContext() const;
    virtual bool        mergeWith(Common::Command*);

protected:
    void                doExecute();
    void                doUnexecute();

private:
    GrovePos            pos_;
    GroveLib::TextPtr   text_;
    Common::String      str_;
};

/////////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP InsertElement : public GroveCommand {
public:
    InsertElement(const GrovePos& elementPos, const CommonString& name,
                  const COMMON_NS::PropertyNode* attrMap);

    CommonString        info(uint) const;
    GroveLib::Node*     validationContext() const;

protected:
    void                doExecute();
    void                doUnexecute();

private:
    GrovePos                pos_;
    GroveLib::ElementPtr    elem_;
};

/////////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP RenameElement : public GroveCommand {
public:
    RenameElement(const GrovePos& pos,
                  const COMMON_NS::String& name);

    CommonString        info(uint) const;
    GroveLib::Node*     validationContext() const;
    virtual CommandFlags flags() const { return CF_RMATT; }

protected:
    void                        doExecute();
    void                        doUnexecute();

private:
    GrovePos                    pos_;
    GroveLib::ElementPtr        elem_;
    GroveLib::ElementPtr        newElem_;
    COMMON_NS::String           name_;
    COMMON_NS::String           oldName_;
    GroveLib::NodePtr           parent_;
};

/////////////////////////////////////////////////////////////////////////

typedef COMMON_NS::RefCntPtr<GroveLib::DocumentFragment> FragmentPtr;

class GROVEEDITOR_EXPIMP RemoveNode : public GroveCommand {
public:
    RemoveNode(const GroveLib::NodePtr& node);

    CommonString            info(uint) const;
    const GrovePos&         pos() const;
    virtual CommandFlags    flags() const { return CF_NOSKEL; }
    GroveLib::Node*         validationContext() const;

protected:
    void                    doExecute();
    void                    doUnexecute();

private:
    GroveLib::NodePtr       node_;
    FragmentPtr             frag_;
    GroveLib::NodePtr       parent_;
    GroveLib::NodePtr       before_;
    GroveCommandPtr         joinText_;
};

/////////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP Untag : public GroveCommand {
public:
    Untag(const GrovePos& pos);

    CommonString            info(uint) const;
    virtual CommandFlags    flags() const { return CF_NOSKEL; }
    GroveLib::Node*         validationContext() const;

protected:
    void                    doExecute();
    void                    doUnexecute();

private:
    GrovePos            untagTo_;
    GrovePos            origPos_;
    GroveLib::NodePtr   elem_;
    GroveLib::NodePtr   from_;
    GroveLib::NodePtr   to_;
    GroveCommandPtr     joinLeft_;
    GroveCommandPtr     joinRight_;
};

/////////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP RemoveGroup : public GroveCommand {
public:
    RemoveGroup(const GroveLib::NodePtr& from, const GroveLib::NodePtr& to, 
                GrovePos* adjustPos = 0);

    CommonString            info(uint) const;
    const GrovePos&         pos() const;
    virtual CommandFlags    flags() const { return CF_NOSKEL; }
    GroveLib::Node*         validationContext() const;

protected:
    void                    doExecute();
    void                    doUnexecute();
private:
    GroveLib::NodePtr       from_;
    GroveLib::NodePtr       to_;
    FragmentPtr             frag_;
    GroveLib::NodePtr       parent_;
    GroveLib::NodePtr       before_;
    GroveCommandPtr         joinText_;
};

////////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP RemoveSelection : public SelectionCommand {
public:
    RemoveSelection(const GrovePos& first, const GrovePos& last, 
                    GrovePos* adjustPos = 0);

    CommonString            info(uint) const;
    virtual CommandFlags    flags() const { return CF_NOSKEL; }
    GroveLib::Node*         validationContext() const;

    // reimplemented from MacroCommand
    virtual void            buildSubCommands();
private:
    GrovePos*               adjustPos_;
};

class GROVEEDITOR_EXPIMP TagRegion : public SplitSelectionCommand {
public:
    TagRegion(const GrovePos& first,
              const GrovePos& last,
              const COMMON_NS::String& elemName,
              const COMMON_NS::PropertyNode* attrMap);

    CommonString            info(uint) const;
    virtual CommandFlags    flags() const { return CF_NOSKEL; }
    const GrovePos&         pos() const;
    GroveLib::Node*         validationContext() const;

    virtual void makeSubcommand(GroveLib::Node* first,
                                GroveLib::Node* last);
private:
    COMMON_NS::String           elemName_;
    GroveCommand*               posCmd_;
    const Common::PropertyNode* attrMap_;
};

class GROVEEDITOR_EXPIMP ConvertToTag : public GroveCommand {
public:
    ConvertToTag(const GroveLib::NodePtr& first,
                 const GroveLib::NodePtr& last,
                 const COMMON_NS::String& elemName,
                 const GrovePos& origPos,
                 const Common::PropertyNode* attrMap);

    CommonString            info(uint) const;
    virtual CommandFlags    flags() const { return CF_NOSKEL; }

protected:
    void                    doExecute();
    void                    doUnexecute();

private:
    GroveLib::NodePtr    first_;
    GroveLib::NodePtr    last_;
    GroveLib::ElementPtr tag_;
    GrovePos             origPos_;
};

////////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP SetAttribute : public GroveCommand {
public:
    SetAttribute(GroveLib::Attr* attr, const Common::String& newValue);

    CommonString            info(uint) const;

    virtual CommandFlags    flags() const 
        { return (CommandFlags)(CF_NOVISUAL|CF_OLDPOS); }
    GroveLib::Node*         validationContext() const;

protected:
    void                    doExecute();
    void                    doUnexecute();

private:
    GroveLib::AttrPtr           attr_;
    COMMON_NS::String           newValue_;
    COMMON_NS::String           oldValue_;
};

////////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP AddAttribute : public GroveCommand {
public:
    AddAttribute(GroveLib::Element* element,
                 const Common::PropertyNode* attrspec);

    CommonString            info(uint) const;

    virtual CommandFlags    flags() const 
        { return (CommandFlags)(CF_NOVISUAL|CF_OLDPOS); }
    GroveLib::Node*         validationContext() const;

protected:
    void                    doExecute();
    void                    doUnexecute();

private:
    GroveLib::ElementPtr        element_;
    GroveLib::AttrPtr           attr_;
};

///////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP RemoveAttribute : public GroveCommand {
public:
    RemoveAttribute(GroveLib::Attr* attr);

    CommonString            info(uint) const;
    virtual CommandFlags    flags() const 
        { return (CommandFlags)(CF_NOVISUAL|CF_OLDPOS); }
    GroveLib::Node*         validationContext() const;

protected:
    void                    doExecute();
    void                    doUnexecute();
private:
    GroveLib::ElementPtr    element_;
    GroveLib::AttrPtr       attr_;
};

///////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP RenameAttribute : public GroveCommand {
public:
    RenameAttribute(GroveLib::Attr* attr, const Common::PropertyNode* spec);

    CommonString            info(uint) const;
    virtual CommandFlags    flags() const 
        { return (CommandFlags)(CF_NOVISUAL|CF_OLDPOS); }
    GroveLib::Node*         validationContext() const;

protected:
    void                    doExecute();
    void                    doUnexecute();

private:
    GroveCommandPtr         removeAttr_;
    GroveCommandPtr         addAttr_;
};

///////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP MapXmlNs : public GroveCommand {
public:
    MapXmlNs(GroveLib::Element* element,
             const Common::String& prefix,
             const Common::String& uri);

    CommonString            info(uint) const;
    virtual CommandFlags    flags() const 
        { return (CommandFlags)(CF_NOVISUAL|CF_OLDPOS); }
    GroveLib::Node*         validationContext() const;

protected:
    void                    doExecute();
    void                    doUnexecute();

private:
    GroveLib::ElementPtr    element_;
    Common::String          prefix_;
    Common::String          oldUri_;
    Common::String          newUri_;
};

///////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP SplitText : public GroveCommand {
public:
    SplitText(const GrovePos& pos);

    CommonString                info(uint) const;
    GrovePos                    elementPos() const;
    GroveLib::Node*             validationContext() const;

protected:
    void                        doExecute();
    void                        doUnexecute();

private:
    GrovePos                    pos_;
    GroveLib::TextPtr           split_;
};

class GROVEEDITOR_EXPIMP SplitElement : public GroveCommand {
public:
    SplitElement(const GrovePos& pos);

    CommonString                info(uint) const;
    GroveLib::Node*             validationContext() const;

protected:
    void                        doExecute();
    void                        doUnexecute();

private:
    GrovePos                    pos_;
    GroveLib::NodePtr           split_;
};

/////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP Join : public GroveCommand {
public:
    Join(const GroveLib::NodePtr& first, const GroveLib::NodePtr& second, 
         bool placeCursorAtEnd = false);

    CommonString                info(uint) const;
    virtual CommandFlags        flags() const { return CF_NOSKEL; }
    GroveLib::Node*             validationContext() const;

protected:
    void                        doExecute();
    void                        doUnexecute();
private:
    GroveLib::NodePtr           first_;
    GroveLib::NodePtr           second_;
    ulong                       pos_;
    bool                        placeCursorAtEnd_;
};

/////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP Copy : public GroveCommand {
public:
    Copy(const GrovePos& first, const GrovePos& last,
         const GroveLib::NodePtr& clipboard);

    CommonString        info(uint) const;

protected:
    void                doExecute();
    void                doUnexecute() {}

private:
    //!
    void                copyChildren(const GroveLib::NodePtr& node,
                                     ulong from, ulong to,
                                     const GroveLib::NodePtr& copyParent);
    //!
    GroveLib::NodePtr   copyBefore(const GroveLib::NodePtr& node, ulong to,
                                   const GroveLib::NodePtr& commonParent,
                                   const GroveLib::NodePtr& commonParentCopy);
    //!
    GroveLib::NodePtr   copyAfter(const GroveLib::NodePtr& node, ulong from,
                                  const GroveLib::NodePtr& commonParent,
                                  ulong commonTo);
private:
    GrovePos            first_;
    GrovePos            last_;
    GroveLib::GrovePtr  grove_;
    GroveLib::NodePtr   clipboard_;
};

/////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP Paste : public GroveMacroCommand {
public:
    Paste(const GroveLib::NodePtr& clipboard, const GrovePos& elementPos);

    CommonString        info(uint) const;
    GroveLib::Node*     validationContext() const;
    virtual const GrovePos& pos() const;

protected:
    virtual void        buildSubCommands();

private:
    GrovePos            pos_;
    GroveLib::NodePtr   clipboard_;
};

/////////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP InsertPi : public GroveCommand {
public:
    InsertPi(const GrovePos& pos,
             const COMMON_NS::String& target,
             const COMMON_NS::String& text);

    CommonString        info(uint) const;
    GroveLib::Node*     validationContext() const { return 0; }

protected:
    void                doExecute();
    void                doUnexecute();

private:
    GrovePos            pos_;
    GroveLib::ProcessingInstructionPtr pi_;
};

/////////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP InsertComment : public GroveCommand {
public:
    InsertComment(const GrovePos& pos, const COMMON_NS::String& text);

    CommonString        info(uint) const;
    GroveLib::Node*     validationContext() const { return 0; }

protected:
    void                doExecute();
    void                doUnexecute();

private:
    GrovePos            pos_;
    GroveLib::CommentPtr comment_;
};

/////////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP ChangePi : public GroveCommand {
public:
    ChangePi(const GrovePos& pos,
             const COMMON_NS::String& target,
             const COMMON_NS::String& text);

    CommonString        info(uint) const;
    GroveLib::Node*     validationContext() const { return 0; }
    virtual CommandFlags flags() const { return CF_NOVISUAL; }

protected:
    void                doExecute();
    void                doUnexecute();

private:
    GrovePos            pos_;
    COMMON_NS::String   oldtext_;
    COMMON_NS::String   newtext_;
    COMMON_NS::String   oldtarget_;
    COMMON_NS::String   newtarget_;
};

/////////////////////////////////////////////////////////////////////////

class GROVEEDITOR_EXPIMP ChangeComment : public GroveCommand {
public:
    ChangeComment(const GrovePos& pos, const COMMON_NS::String& text);

    CommonString        info(uint) const;
    GroveLib::Node*     validationContext() const { return 0; }
    virtual CommandFlags flags() const { return CF_NOVISUAL; }

protected:
    void                doExecute();
    void                doUnexecute();

private:
    GrovePos            pos_;
    COMMON_NS::String   oldtext_;
    COMMON_NS::String   newtext_;
};

/////////////////////////////////////////////////////////////////////////

class ConvertFromSection : public GroveCommand {
public:
    ConvertFromSection(GroveLib::GroveSectionStart* gss);

    CommonString        info(uint) const;

    GroveLib::Node*     first() const;
    GroveLib::Node*     last() const;

protected:
    void                doExecute();
    void                doUnexecute();

private:
    GroveCommandPtr     leftJoin_;
    GroveCommandPtr     rightJoin_;
    GroveLib::NodePtr   parent_;
    GroveLib::NodePtr   from_;
    GroveLib::NodePtr   to_;
    GroveLib::NodePtr   after_;
    Common::RefCntPtr<GroveLib::GroveSectionStart> gss_;
    Common::RefCntPtr<GroveLib::GroveSectionEnd> gse_;
};

} // namespace GroveEditor

#endif // GROVEEDITOR_GROVE_COMMANDS_H
