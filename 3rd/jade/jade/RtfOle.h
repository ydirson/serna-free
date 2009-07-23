// Copyright (c) 1997 James Clark
// See the file copying.txt for copying permission.

#ifndef RtfOle_INCLUDED
#define RtfOle_INCLUDED 1

#include "dsssl_ns.h"
#include "OutputByteStream.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

int outputObject(const wchar_t *filename,
		 const wchar_t *clsidString,
		 OutputByteStream &os);

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not RtfOle_INCLUDED */
