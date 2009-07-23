// Copyright (c) 1997 James Clark
// See the file copying.txt for copying permission.

#ifndef TransformFOTBuilder_INCLUDED
#define TransformFOTBuilder_INCLUDED 1

#include "FOTBuilder.h"
#include "CmdLineApp.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

FOTBuilder *makeTransformFOTBuilder(CmdLineApp *app,
				    bool xml,
				    const FOTBuilder::Extension *&);

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not TransformFOTBuilder_INCLUDED */
