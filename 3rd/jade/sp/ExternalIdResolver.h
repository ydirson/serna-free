// Copyright (c) 1994 James Clark
// See the file COPYING for copying permission.

#ifndef ExternalIdResolver_INCLUDED
#define ExternalIdResolver_INCLUDED 1

#include "StringC.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class ExternalIdResolver {
public:
    virtual ~ExternalIdResolver() {}
    virtual bool resolveExternalId(const StringC& pubid, const StringC& sysid,
                                   StringC& result) const = 0;
protected:
    ExternalIdResolver() {}
private:
    ExternalIdResolver(const ExternalIdResolver&);
    ExternalIdResolver& operator=(const ExternalIdResolver&);
};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not ExternalIdResolver_INCLUDED */
