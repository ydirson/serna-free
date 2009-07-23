// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#if defined(__GNUG__) && defined(GNUG_PRAGMA_INTERFACE)
#pragma implementation
#endif

#include "splib.h"
#include "Id.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

Id::Id(const StringC &name)
: Named(name)
{
}

void Id::define(const Location &loc)
{
  defLocation_ = loc;
  // release memory for pendingRefs_
  Vector<Location> tem;
  pendingRefs_.swap(tem);
}

#ifdef SP_NAMESPACE
}
#endif
