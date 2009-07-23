// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef RtfFOTBuilder_INCLUDED
#define RtfFOTBuilder_INCLUDED 1

#include "OutputByteStream.h"
#include "FOTBuilder.h"
#include "Ptr.h"
#include "ExtendEntityManager.h"
#include "CharsetInfo.h"
#include "Message.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

FOTBuilder *
makeRtfFOTBuilder(OutputByteStream *,
		  const Vector<StringC> &options,
		  const Ptr<ExtendEntityManager> &,
		  const CharsetInfo &, Messenger *,
		  const FOTBuilder::Extension *&);


#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not RtfFOTBuilder_INCLUDED */
