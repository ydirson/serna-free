// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class Command : SimpleWrappedObject {
%TypeHeaderCode
#include "sapi/common/Command.h"
using namespace SernaApi;
%End
public:
    void            execute();
    void            unexecute();
    bool            isExecuted() const;

    SString         info(unsigned int) const;

    unsigned short  getUdata() const;
    void            setUdata(unsigned short);

    virtual void    doExecute();
    virtual void    doUnexecute();

    GroveCommand    asGroveCommand() const;

// copy-paste section: XTreeNode portion
    
    Command         firstChild() const;
    Command         lastChild() const;
    Command         getChild(unsigned long) const;
    Command         root() const;
    
    unsigned int    countChildren() const;
    
    void            appendChild(const Command&);
    void            removeAllChildren();
    
    Command         nextSibling() const;
    Command         prevSibling() const;
    unsigned int    siblingIndex() const;
    Command         parent() const;
    void            insertBefore(const Command&);
    void            insertAfter(const Command&);
    void            remove();
    void            removeGroup(const Command&);
    SString         getTreelocString(const Command& = Command()) const; 
    Command         getByTreelocString(const SString&) const;
};

class CommandExecutor {
public:
    Command         executeAndAdd(const Command&);
    Command         undo();
    Command         redo();
    bool            isModified() const;
    Command         lastDone() const;

// XLIST copy-paste portion

    Command         firstChild() const;
    Command         lastChild() const;
    Command         getChild(unsigned long) const;
    
    unsigned int    countChildren() const;
    
    void            appendChild(const Command&);
    void            removeAllChildren();
};

