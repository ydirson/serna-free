// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class GroveCommand : Command {
%TypeHeaderCode
#include "sapi/grove/GroveCommand.h"
using namespace SernaApi;
%End
public:
    GroveCommand(const GroveCommand&);

    enum CommandFlags {
        CF_NONE,
        CF_NOVISUAL,
        CF_NOSKEL,
        CF_RMATT,
        CF_BATCH
    };
    virtual CommandFlags    flags() const;
    virtual GroveNode       validationContext() const;
    virtual GrovePos pos() const;
    
    virtual ~GroveCommand();
};

class GroveBatchCommand : GroveCommand {
%TypeHeaderCode
#include "sapi/grove/GroveCommand.h"
using namespace SernaApi;
%End
public:
    GroveBatchCommand();
    virtual ~GroveBatchCommand();

    bool            executeAndAdd(const Command&, bool = true);
    virtual CommandFlags flags() const;
    void            setFlags(CommandFlags);
    virtual GroveNode validationContext() const;
    void            setValidationContext(const GroveNode&);
    virtual GrovePos pos() const;
    void            setSuggestedPos(const GrovePos&);
    void            setInfo(const SString&);
};
