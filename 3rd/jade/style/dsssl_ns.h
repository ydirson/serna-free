// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef dsssl_ns_INCLUDED
#define dsssl_ns_INCLUDED 1

#ifdef DSSSL_NAMESPACE

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE { }
#endif

#ifdef GROVE_NAMESPACE
namespace GROVE_NAMESPACE { }
#endif

namespace DSSSL_NAMESPACE {

#ifdef SP_NAMESPACE
  using namespace SP_NAMESPACE;
#endif

#ifdef GROVE_NAMESPACE
  using namespace GROVE_NAMESPACE;
#endif

#if _MSC_VER >= 1100
  /* This works around a bug with using directives in Visual C++ 6.0.
     I don't know if it also works in 5.0 */
  const int work_around_bug_in_visual_c_6 = 0;
}

namespace DSSSL_NAMESPACE {

#ifdef SP_NAMESPACE
  using namespace SP_NAMESPACE;
#endif

#ifdef GROVE_NAMESPACE
  using namespace GROVE_NAMESPACE;
#endif

#endif /* _MSC_VER >= 1100 */
}

#endif /* DSSSL_NAMESPACE */

#endif /* not dsssl_ns_INCLUDED */
