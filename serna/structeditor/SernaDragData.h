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
#ifndef STRUCTEDITOR_SERNA_DRAG_DATA_H_
#define STRUCTEDITOR_SERNA_DRAG_DATA_H_

#include "structeditor/se_defs.h"
#include "groveeditor/GrovePos.h"
#include "common/SernaApiBase.h"
#include "common/RefCntPtr.h"
#include "ui/MimeHandler.h"

#include <Qt>

class QDrag;
class QDragEnterEvent;
class SernaDoc;
class StructEditor;

namespace GroveLib {
    class DocumentFragment;
}

class STRUCTEDITOR_EXPIMP SernaDocFragment : public SernaApiBase {
public:
    SernaDocFragment();
    virtual ~SernaDocFragment();

    void                        setFragment(GroveLib::DocumentFragment*);
    GroveLib::DocumentFragment* clipboard() const;

    // sets the context when about-to-drop or validating
    void                        setDropData(const GroveEditor::GrovePos&,
                                            StructEditor*);

    // validate for the specific dropPos and allowed actions
    virtual bool                validate(QDropEvent* event);
    virtual bool                dropContentCopy();
    virtual bool                dropContentMove();
    
    virtual GroveEditor::GrovePos fromPos() const;
    virtual GroveEditor::GrovePos toPos() const;

    static Common::String       fragMimeType(const Common::PropertyNode*);
    static SernaDocFragment*    fragment(const Sui::MimeHandler&,
                                    const Common::String& mimeType,
                                    bool useDefault = false);
    static SernaDocFragment*    fragment(const SernaDoc*);
    static SernaDocFragment*    newFragment(Sui::MimeHandler& mh, 
                                            const Common::String& type);
    // calls relevant callbacks for drag enter in editview/contentmap
    static bool                 dragEnter(const StructEditor*, 
                                          QDragEnterEvent*);
    virtual StructEditor*       structEditor() const { return 0; }

private:
    SernaDocFragment(const SernaDocFragment&);
    SernaDocFragment& operator=(const SernaDocFragment&);

    Common::RefCntPtr<GroveLib::DocumentFragment> clipboard_;
    const GroveLib::Element*      firstElem_;
    bool                          hasText_;

protected:
    virtual bool    is_within_range() const;
    bool            valid_at(QDropEvent* event) const;

    GroveEditor::GrovePos         dropPos_;
    StructEditor*                 dropStructEditor_;
};

class STRUCTEDITOR_EXPIMP SernaDragData : public SernaDocFragment {
public:
    SernaDragData(StructEditor*);
    virtual ~SernaDragData();

    virtual StructEditor*       structEditor() const
        { return structEditor_; }
    Qt::DropActions             allowedActions() const 
        { return allowedActions_; }
    void                        startDrag(QDrag*, Sui::MimeHandler&);

    virtual bool                dropContentMove();
    virtual bool                validate(QDropEvent* event);

    virtual GroveEditor::GrovePos fromPos() const;
    virtual GroveEditor::GrovePos toPos() const;
    void                          setSelection(const GroveEditor::GrovePos&, 
                                               const GroveEditor::GrovePos&);
private:
    SernaDragData(const SernaDragData&);
    SernaDragData& operator=(const SernaDragData&);
    
    virtual bool is_within_range() const;

    StructEditor*                 structEditor_;
    GroveEditor::GrovePos         from_;
    GroveEditor::GrovePos         to_;
    Qt::DropActions               allowedActions_;
    bool                          completed_;  
};

////////////////////////////////////////////////////////////////////////

STRUCTEDITOR_EXPIMP void register_struct_dnd_callbacks(SernaDoc*);

#endif // STRUCTEDITOR_SERNA_DRAG_DATA_H_
