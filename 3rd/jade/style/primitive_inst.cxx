#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif


#ifdef SP_NAMESPACE
}
#endif
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

#ifdef __DECCXX
#pragma define_template ConstPtr<MapNodeListObj::Context>
#else
#ifdef __xlC__
#pragma define(ConstPtr<MapNodeListObj::Context>)
#else
#ifdef SP_ANSI_CLASS_INST
template class ConstPtr<MapNodeListObj::Context>;
#else
typedef ConstPtr<MapNodeListObj::Context> Dummy_0;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Ptr<MapNodeListObj::Context>
#else
#ifdef __xlC__
#pragma define(Ptr<MapNodeListObj::Context>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Ptr<MapNodeListObj::Context>;
#else
typedef Ptr<MapNodeListObj::Context> Dummy_1;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Ptr<SelectElementsNodeListObj::PatternSet>
#else
#ifdef __xlC__
#pragma define(Ptr<SelectElementsNodeListObj::PatternSet>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Ptr<SelectElementsNodeListObj::PatternSet>;
#else
typedef Ptr<SelectElementsNodeListObj::PatternSet> Dummy_2;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template ConstPtr<SelectElementsNodeListObj::PatternSet>
#else
#ifdef __xlC__
#pragma define(ConstPtr<SelectElementsNodeListObj::PatternSet>)
#else
#ifdef SP_ANSI_CLASS_INST
template class ConstPtr<SelectElementsNodeListObj::PatternSet>;
#else
typedef ConstPtr<SelectElementsNodeListObj::PatternSet> Dummy_3;
#endif
#endif
#endif

#ifdef SP_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
