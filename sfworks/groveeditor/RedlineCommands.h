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
#ifndef GROVEEDITOR_REDLINE_COMMANDS_H_
#define GROVEEDITOR_REDLINE_COMMANDS_H_

#include "groveeditor/SelectionCommand.h"
#include "grove/RedlineUtils.h"
#include "grove/SectionNodes.h"

namespace GroveEditor {

class ConvertFromSection;

class NewRedlineSelection : public SplitSelectionCommand {
public:
    NewRedlineSelection(const GrovePos& first,
                        const GrovePos& last,
                        GroveLib::RedlineData* rd);
public:
    CommonString            info(uint) const;
    virtual CommandFlags    flags() const { return CF_NOSKEL; }
    GroveLib::Node*         validationContext() const { return cp_; }
    const GrovePos&         pos() const;
    
    virtual void makeSubcommand(GroveLib::Node* first,
                                GroveLib::Node* last);
private:
    GroveCommand*               posCmd_;
    Common::RefCntPtr<GroveLib::RedlineSectionStart> redlineSect_;
};

///////////////////////////////////////////////////////////////////

class NewRedline : public GroveCommand {
public:
    NewRedline(GroveLib::Node* first, 
               GroveLib::Node* last, 
               const GrovePos& origPos,
               GroveLib::RedlineSectionStart*);

    CommonString            info(uint) const;
    virtual CommandFlags    flags() const { return CF_NOSKEL; }
    
private:
    void                    doExecute();
    void                    doUnexecute();
    
    GroveLib::NodePtr               first_;
    GroveLib::NodePtr               last_;
    GrovePos                        origPos_;
    Common::RefCntPtr<GroveLib::RedlineSectionStart> redlineSect_;
    Common::RefCntPtr<GroveLib::GroveSectionEnd>   savedGse_;
};

//////////////////////////////////////////////////////////////////

class ConvertFromRedline : public GroveCommand {
public:
    ConvertFromRedline(const GrovePos& pos);
    ~ConvertFromRedline();
    
    CommonString            info(uint) const;
    virtual CommandFlags    flags() const { return CF_NOSKEL; }

private:
    void                    doExecute();
    void                    doUnexecute();

    GrovePos                pos_;
    Common::RefCntPtr<ConvertFromSection> convFromSect_;
};

///////////////////////////////////////////////////////////////////

class ChangeRedline : public GroveCommand {
public:
    ChangeRedline(const GrovePos& pos, uint redlineMask,
                  const Common::String& annotation);
    ~ChangeRedline();

    CommonString            info(uint) const;
    virtual CommandFlags    flags() const
        { return (CommandFlags)(CF_NOSKEL|CF_NOVISUAL); }
    
private:
    void                    doExecute();
    void                    doUnexecute();
    void                    change_rinfo(bool);

    uint                    redlineMask_;
    Common::String          annotation_;
};

///////////////////////////////////////////////////////////////////

class InsertRedline : public GroveCommand {
public:
    InsertRedline(const GrovePos& pos, GroveLib::RedlineData* rd);
    ~InsertRedline();

    CommonString            info(uint) const;
    virtual CommandFlags    flags() const { return CF_NOSKEL; }
    
private:
    void                    doExecute();
    void                    doUnexecute();
    GrovePos                pos_;
    Common::RefCntPtr<GroveLib::RedlineSectionStart> savedGss_;
    Common::RefCntPtr<GroveLib::RedlineSectionEnd>   savedGse_;
};

} // namespace

#endif // GROVEEDITOR_REDLINE_COMMANDS_H_
