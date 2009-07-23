// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#if defined(__GNUG__) && defined(GNUG_PRAGMA_INTERFACE)
#pragma implementation
#endif
#include "splib.h"
#include "Hash.h"
#include "StringC.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

unsigned long Hash::hash(const StringC &str)
{
  const Char *p = str.data();
  unsigned long h = 0;
  for (size_t n = str.size(); n > 0; n--)
    h = (h << 5) + h + *p++;	// from Chris Torek
  return h;
}

#ifdef SP_NAMESPACE
}
#endif
