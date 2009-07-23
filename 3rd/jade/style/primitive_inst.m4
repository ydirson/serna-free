// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#include "stylelib.h"

#ifdef SP_MANUAL_INST

#define SP_DEFINE_TEMPLATES
#include "Ptr.h"
#undef SP_DEFINE_TEMPLATES

#if _MSC_VER >= 1100

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

#ifdef DSSSL_NAMESPACE
using namespace DSSSL_NAMESPACE;
#endif

#ifdef GROVE_NAMESPACE
using namespace GROVE_NAMESPACE;
#endif

#else

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

#endif

__instantiate(ConstPtr<MapNodeListObj::Context>)
__instantiate(Ptr<MapNodeListObj::Context>)
__instantiate(Ptr<SelectElementsNodeListObj::PatternSet>)
__instantiate(ConstPtr<SelectElementsNodeListObj::PatternSet>)

#ifdef SP_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
