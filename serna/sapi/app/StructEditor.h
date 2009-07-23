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
#ifndef SAPI_STRUCT_EDITOR_H_
#define SAPI_STRUCT_EDITOR_H_

#include "sapi/sapi_defs.h"
#include "sapi/grove/GroveDecls.h"
#include "sapi/grove/GrovePos.h"
#include "sapi/app/UiItem.h"

namespace SernaApi {

class Command;
class GroveEditor;
class GroveStripInfo;
class SimpleWatcher;
class DocumentPlugin;
class StructEditorData;
class XsltEngine;
class CatalogManager;
class XmlValidatorProvider;

/// StructEditor is interface to the main edit view of the document.
/*! Gives access to the XML grove, cursor and command execution.
 */
class SAPI_EXPIMP StructEditor : public UiItem {
public:
    StructEditor(SernaApiBase*, DocumentPlugin* dp);
    virtual ~StructEditor();

    /// Returns GroveEditor instance.Needed for operations on
    /// xml nodes (insert Element, Text ...).
    GroveEditor     groveEditor() const;
    
    /// Execute Command, add it to undo/redo stack, validate, update view and 
    /// set cursor. command must be either a command returned by the 
    /// GroveEditor, or be a GroveBatchCommand.
    bool            executeAndUpdate(const Command& command);

    /// Set cursor by position in the source (document) grove. 
    /// if isTop is true, view will be scrolled so cursor appears on top
    /// of the page. FoHint is optional pointer to the closest node in
    /// the FO tree; attempt will be made to set cursor as close as possible
    /// to the foHint. This is necessary because XSLT may produce multiple
    /// results in the FO tree for the same source, and foHint helps to
    /// determine which to choose.
    bool            setCursorBySrcPos(const GrovePos& srcPos,
                                      const GroveNode& foHint,
                                      bool isTop = false);
    
    /// Convert position in the source grove to the position in the
    /// result grove. FoHint is optional pointer to the closest node in the
    /// FO tree; attempt will be made to compute position closest to the
    /// foHint (if specified). 
    GrovePos        toResultPos(const GrovePos& srcPos,
                                const GroveNode& foHint,
                                bool diffuse = true) const;
   
    /// Obtain current source position. Returns null pos if position cannot
    /// be obtained, or if changes are prohibited in this position (eg
    /// this content is a part of internal entity declaration of 
    /// external subset).
    GrovePos        getCheckedPos() const;
    
    /// Returns boundaries of current selection, if any (as a pair of
    /// document-ordered positions in the source grove). Parameter 
    /// mustBeEditable means that content in these positions must be editable
    /// (this is may be not so for read-only operations such as Copy).
    bool            getSelection(GrovePos& from,
                                 GrovePos& to,
                                 bool mustBeEditable = true);

    /// Set the selection in the source tree between the positions
    /// \a from and \a to. If the optional foHint argument is passed,
    /// then the visual selection will appear in the place which is
    /// possibly closest to the foHint.
    void            setSelection(const GrovePos& from,
                                 const GrovePos& to,
                                 const GrovePos& foHint = GrovePos());

    /// Reset active selection (this does not affect the document).
    void            resetSelection();
    
    /// Returns reference to the source grove
    Grove           sourceGrove() const;

    /// Returns reference to the FO grove
    Grove           foGrove() const;
    
    /// Returns current cursor position in the source tree (unchecked)
    GrovePos        getSrcPos() const;
    
    /// Returns current cursor position in the FO tree (unchecked)
    GrovePos        getFoPos() const;

    /// Generate unique element ID. This ID is guaranteed to be unique in
    /// the current document and all its XIncluded fragments. Optional
    /// parameter \a idFormat specifies desired ID format string (it is
    /// described in more detail in Serna Custom Content documentation)
    SString        generateId(const SString& idFormat = SString()) const;
    
    /// Obtain (source) origin of the node in the FO tree. Note that
    /// origin may be NULL e.g. for generated text (<xsl:text>).
    static GroveNode getOrigin(const GroveNode& foNode);

    /// Set this watcher if you are interested in tracking current
    /// (cursor) position changes.
    void            setElementPositionWatcher(SimpleWatcher*);

    /// Set this watcher if you want to track selection changes.
    void            setSelectionWatcher(SimpleWatcher*);

    /// Set this watcher if you want to track double clicks.
    void            setDoubleClickWatcher(SimpleWatcher*);

    /// Set this watcher if you want to track double clicks.
    void            setTripleClickWatcher(SimpleWatcher*);

    /// Information about stripped and preserved spaces, line feeds and so on.
    GroveStripInfo  stripInfo() const;

    /// Access to the XSLT engine for the current document
    XsltEngine      xsltEngine() const;

    /// Get the catalog manager
    CatalogManager  catalogManager() const;
    
    /// Returns TRUE of given element can be inserted at the position
    /// specified by grovePos. If grovePos is not specified, current
    /// cursor position is used.
    bool    canInsertElement(const SString& elemName, 
                             const SString& ns = SString(""),
                             const GrovePos& gpos = GrovePos());
    
    /// Returns XML validator provider for the current document instance
    XmlValidatorProvider validatorProvider() const;

    StructEditor(const StructEditor&);
    StructEditor& operator=(const StructEditor&);
    virtual void setRep(SernaApiBase*);

private:
    StructEditorData* impl_;
};

} // namespace SernaApi

#endif // SAPI_STRUCT_EDITOR_H_
