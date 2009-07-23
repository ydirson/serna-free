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

__instantiate(`PointerTable<ElementChunk *,StringC,Hash,ElementChunk>')
__instantiate(`Vector<ElementChunk *>')
__instantiate(Vector<ConstPtr<Origin> >)

#ifdef SP_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
