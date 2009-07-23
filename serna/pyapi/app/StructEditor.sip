// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class StructEditor : UiItem {

%TypeHeaderCode
#include "sapi/grove/Grove.h"
#include "sapi/grove/GroveNodes.h"
#include "sapi/grove/GrovePos.h"
#include "sapi/grove/GroveEditor.h"
#include "sapi/app/StructEditor.h"
#include "sapi/app/XsltEngine.h"
using namespace SernaApi;
%End
public:
    GroveEditor     groveEditor() const;
    bool            executeAndUpdate(const Command&);
    
    bool            setCursorBySrcPos(const GrovePos&, const GroveNode&,
                                      bool = false);
    
    GrovePos        toResultPos(const GrovePos&, const GroveNode&,
                                bool = true) const;
    void            setSelection(const GrovePos&,
                                 const GrovePos&,
                                 const GrovePos& = GrovePos());
    
    GrovePos        getCheckedPos() const;
    bool            getSelection(GrovePos&, GrovePos&, bool = true);
    void            resetSelection();
    SString        generateId(const SString& = SString()) const;
    
    Grove           sourceGrove() const;
    Grove           foGrove() const;
    GrovePos        getSrcPos() const;
    GrovePos        getFoPos() const;
    GroveStripInfo  stripInfo() const;
    XsltEngine      xsltEngine() const;
    CatalogManager  catalogManager() const;
    bool            canInsertElement(const SString&, 
                      const SString& = SString(""),
                      const GrovePos& = GrovePos());
    XmlValidatorProvider validatorProvider() const;
    
    static GroveNode getOrigin(const GroveNode&);

    void            setElementPositionWatcher(SimpleWatcher*);
    void            setSelectionWatcher(SimpleWatcher*);
    void            setDoubleClickWatcher(SimpleWatcher*);
    void            setTripleClickWatcher(SimpleWatcher*);

    StructEditor(const StructEditor&);
};
