// Copyright (c) 1997 James Clark
// See the file COPYING for copying permission.

#include "config.h"

#ifdef SP_MANUAL_INST

#define SP_DEFINE_TEMPLATES
#include "IList.h"
#undef SP_DEFINE_TEMPLATES

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

__instantiate(IList<TransformFOTBuilder::OpenFile>)

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
