// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class DavManager : SimpleWrappedObject {

%TypeHeaderCode
#include "sapi/common/DavManager.h"
using namespace SernaApi;
%End

public:
    static DavManager   instance();

    enum OpenFlags {
        DAV_OPEN_READ,
        DAV_OPEN_WRITE,
        DAV_OPEN_MKPATH
    };

    enum OpStatus {
        DAV_RESULT_OK,
        DAV_RESULT_BAD_URL,
        DAV_RESULT_CONNECT_FAILED,
        DAV_RESULT_AUTH_FAILED,
        DAV_RESULT_BAD_PROTOCOL,
        DAV_RESULT_IO_ERROR,
        DAV_RESULT_CANNOT_OPEN,
        DAV_RESULT_LOCKED
    };

    enum LockOp {
        DAV_LOCK,
        DAV_UNLOCK,
        DAV_CHECK_LOCK,
        DAV_CHECK_LOCKSTORE
    };

    OpStatus    listCollection(const Url&, PropertyNode&);
    OpStatus    makeCollection(const Url&);
    OpStatus    getResourceInfo(const Url&, PropertyNode&);

    OpStatus    lock(const Url&, LockOp, int);
    OpStatus    copy(const Url&, const Url&);
    OpStatus    open(const Url&, int openFlags, QIODevice*& /TransferFrom/);
    
    void        closeSessions();
    SString     lastError() const; 
};

