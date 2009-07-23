// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.


#define BUILD_LIBSP
#include "config.h"
#undef BUILD_LIBSP
#define BUILD_LIBSTYLE

#ifdef SP_USE_DLL
#undef SP_API
#define SP_API SP_DLLIMPORT
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4355)
#endif

#include "dsssl_ns.h"
#ifdef SP_PCH
#include "style_pch.h"
#endif
