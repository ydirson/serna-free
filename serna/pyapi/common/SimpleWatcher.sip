// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//
class SimpleWatcher {

%TypeHeaderCode
#include "sapi/common/SimpleWatcher.h"
using namespace SernaApi;
%End

public:
    virtual bool notifyChanged() = 0;
    virtual ~SimpleWatcher();
};
