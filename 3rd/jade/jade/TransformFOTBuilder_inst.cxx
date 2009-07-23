#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif


#ifdef SP_NAMESPACE
}
#endif
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

#ifdef __DECCXX
#pragma define_template IList<TransformFOTBuilder::OpenFile>
#else
#ifdef __xlC__
#pragma define(IList<TransformFOTBuilder::OpenFile>)
#else
#ifdef SP_ANSI_CLASS_INST
template class IList<TransformFOTBuilder::OpenFile>;
#else
typedef IList<TransformFOTBuilder::OpenFile> Dummy_0;
#endif
#endif
#endif

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
