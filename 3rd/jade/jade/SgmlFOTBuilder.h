// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef SgmlFOTBuilder_INCLUDED
#define SgmlFOTBuilder_INCLUDED 1

#include "FOTBuilder.h"
#include "OutputCharStream.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

FOTBuilder *makeSgmlFOTBuilder(OutputCharStream *);

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not SgmlFOTBuilder_INCLUDED */
