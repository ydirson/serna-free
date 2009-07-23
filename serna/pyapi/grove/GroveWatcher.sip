// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class GroveWatcherBase {

%TypeHeaderCode
#include "sapi/grove/GroveWatcher.h"
using namespace SernaApi;
%End

public:
    enum NotificationMask {
        NOTIFY_GENERIC,
        NOTIFY_NODE_DESTROYED,
        NOTIFY_CHILD_INSERTED,
        NOTIFY_CHILD_REMOVED,
        NOTIFY_ATTRIBUTE_CHANGED,
        NOTIFY_ATTRIBUTE_ADDED,
        NOTIFY_ATTRIBUTE_REMOVED,
        NOTIFY_TEXT_CHANGED,
        NOTIFY_NS_MAPPING_CHANGED,
        NOTIFY_ALL
    };
    virtual ~GroveWatcherBase();
    virtual void    nodeDestroyed(const GroveNode&);
    virtual void    childInserted(const GroveNode&);
    virtual void    childRemoved (const GroveNode&, const GroveNode&);
    virtual void    attributeChanged(const GroveAttr&);
    virtual void    attributeRemoved(const GroveElement&, const GroveAttr&);
    virtual void    attributeAdded(const GroveAttr&);
    virtual void    textChanged(const GroveText&);
    virtual void    genericNotify(const GroveNode&, void*);
    virtual void    nsMappingChanged(const GroveNodeWithNamespace&,
                                     const SString&);
};

class GroveWatcher : GroveWatcherBase, SimpleWrappedObject {
%TypeHeaderCode
#include "sapi/grove/GroveWatcher.h"
using namespace SernaApi;
%End
public:
    GroveWatcher();
    virtual ~GroveWatcher();
};

class GroveNodeWatcher : GroveWatcherBase, SimpleWrappedObject {
%TypeHeaderCode
#include "sapi/grove/GroveWatcher.h"
using namespace SernaApi;
%End
public:
    GroveNodeWatcher();
    virtual ~GroveNodeWatcher();

    bool    isRegisteredIn(const GroveNode&) const;
    void    deregisterFromAllNodes();
};
