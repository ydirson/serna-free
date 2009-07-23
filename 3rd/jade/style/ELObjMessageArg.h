// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef ELObjMessageArg_INCLUDED
#define ELObjMessageArg_INCLUDED 1

#include "ELObj.h"
#include "MessageArg.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class ELObjMessageArg : public StringMessageArg {
public:
  ELObjMessageArg(ELObj *, Interpreter &);
private:
  static StringC convert(ELObj *obj, Interpreter &interp);
};

#ifdef DSSSL_NAMESPACE
}
#endif


#endif /* not ELObjMessageArg_INCLUDED */
