// (c) 2008 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//
class DavIoRequestHandle : SimpleWrappedObject {
%TypeHeaderCode
#include "sapi/common/DavProtocol.h"
using namespace SernaApi;
%End
public:
    typedef DavManager::OpStatus OpStatus;
    
    DavIoRequestHandle(const DavSession&);

    virtual OpStatus close() /ReleaseGIL/;
    virtual int64 position() const;
    virtual OpStatus setPosition(int64) /ReleaseGIL/;
    virtual int64 size() const;
    virtual ~DavIoRequestHandle();

    virtual int64 writeRaw(const char* /Array/, int64 /ArraySize/) /ReleaseGIL/;
    
    virtual SIP_PYOBJECT 
        readRaw(int64 maxlen) /ReleaseGIL/ [int64 (char* data, int64 maxlen)];
%MethodCode
        if (a0 < 0) {
            PyErr_SetString(PyExc_ValueError, 
                "maximum length of data to be read cannot be negative");
            sipIsErr = 1;
        }
        (void) sipSelfWasArg;
        Py_INCREF(Py_None);
        sipRes = Py_None;
%End
%VirtualCatcherCode
        PyObject *result = sipCallMethod(&sipIsErr, sipMethod, "n", a1);
        if (result != NULL) {
            PyObject* buf = 0;
            sipParseResult(&sipIsErr, sipMethod, result, "O", &buf);
            if (buf == Py_None)
                sipRes = -1L;
            else if (!PyString_Check(buf)) {
                sipBadCatcherResult(sipMethod);
                sipIsErr = 1;
            } else {
                memcpy(a0, PyString_AS_STRING(buf), PyString_GET_SIZE(buf));
                sipRes = PyString_GET_SIZE(buf);
            }
            Py_DECREF(buf);
            Py_DECREF(result);
        }
%End
private:
    DavIoRequestHandle(const DavIoRequestHandle&);
};

class DavSession : SimpleWrappedObject {
%TypeHeaderCode
#include "sapi/common/DavProtocol.h"
using namespace SernaApi;
%End
public:
    typedef DavManager::OpStatus OpStatus;

    virtual DavIoRequestHandle* open(const Url&, int, OpStatus&) /Factory/;
    virtual OpStatus listCollection(const Url&, const PropertyNode&);
    virtual OpStatus makeCollection(const Url&);
    virtual OpStatus getResourceInfo(const Url&, const PropertyNode&);
    virtual OpStatus lock(const Url&, DavManager::LockOp, int);

    DavSession();
    virtual ~DavSession();

private:
    DavSession(const DavSession&);
};

class DavProtocol : SimpleWrappedObject {
%TypeHeaderCode
#include "sapi/common/DavProtocol.h"
using namespace SernaApi;
%End
public:
    virtual SString                 name() const = 0;
    virtual DavSession*             makeSession(const Url&) const /Factory/;
    virtual bool                    persistentSessions() const;
    static  void                    registerProtocol(DavProtocol*);
    static  void                    deregisterProtocol(DavProtocol*);
    virtual ~DavProtocol();
};
