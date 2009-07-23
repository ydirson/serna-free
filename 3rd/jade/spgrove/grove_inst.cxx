#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif


#ifdef SP_NAMESPACE
}
#endif
// Copyright (c) 1996, 1997 James Clark
// See the file copying.txt for copying permission.

#include "config.h"

#ifdef SP_MANUAL_INST

#define SP_DEFINE_TEMPLATES
#include "PointerTable.h"
#include "Vector.h"
#undef SP_DEFINE_TEMPLATES

#include "Hash.h"
#include "StringC.h"
#include "Node.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class ElementChunk;

#ifdef __DECCXX
#pragma define_template PointerTable<ElementChunk *,StringC,Hash,ElementChunk>
#else
#ifdef __xlC__
#pragma define(PointerTable<ElementChunk *,StringC,Hash,ElementChunk>)
#else
#ifdef SP_ANSI_CLASS_INST
template class PointerTable<ElementChunk *,StringC,Hash,ElementChunk>;
#else
typedef PointerTable<ElementChunk *,StringC,Hash,ElementChunk> Dummy_0;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<ElementChunk *>
#else
#ifdef __xlC__
#pragma define(Vector<ElementChunk *>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<ElementChunk *>;
#else
typedef Vector<ElementChunk *> Dummy_1;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<ConstPtr<Origin> >
#else
#ifdef __xlC__
#pragma define(Vector<ConstPtr<Origin> >)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<ConstPtr<Origin> >;
#else
typedef Vector<ConstPtr<Origin> > Dummy_2;
#endif
#endif
#endif

#ifdef SP_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
