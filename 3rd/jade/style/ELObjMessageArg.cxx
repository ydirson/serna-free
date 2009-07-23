// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#include "stylelib.h"
#include "ELObjMessageArg.h"
#include "Interpreter.h"
#include "OutputCharStream.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

ELObjMessageArg::ELObjMessageArg(ELObj *obj, Interpreter &interp)
: StringMessageArg(convert(obj, interp))
{
}

StringC ELObjMessageArg::convert(ELObj *obj, Interpreter &interp)
{
  StrOutputCharStream os;
  obj->print(interp, os);
  StringC result;
  os.extractString(result);
  return result;
}

#ifdef DSSSL_NAMESPACE
}
#endif
