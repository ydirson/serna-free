// Copyright (c) 1995 James Clark
// See the file COPYING for copying permission.

#if defined(__GNUG__) && defined(GNUG_PRAGMA_INTERFACE)
#pragma implementation
#endif
#include "splib.h"
#include "EntityCatalog.h"
#include "EntityDecl.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

EntityCatalog::~EntityCatalog()
{
}


Boolean EntityCatalog::sgmlDecl(const CharsetInfo &,
				Messenger &,
				StringC &) const
{
  return 0;
}

Boolean EntityCatalog::lookup(const EntityDecl &decl,
			      const Syntax &,
			      const CharsetInfo &,
			      Messenger &,
			      StringC &str) const
{
  const StringC *p = decl.systemIdPointer();
  if (!p)
    return 0;
  str = *p;
  return 1;
}

Boolean EntityCatalog::lookupPublic(const StringC &,
				    const CharsetInfo &,
				    Messenger &,
				    StringC &) const
{
  return 0;
}

Boolean EntityCatalog::lookupChar(const StringC &,
				  const CharsetInfo &,
			    	  Messenger &,
				  UnivChar &) const
{
  return 0;
}

EntityCatalog::PublicIdPredicate::PublicIdPredicate()
    : isTc(0), isOt(0), isOwner(0), isUnavailable(0), isDescription(0),
      isLang(0), isDs(0), isDv(0)
{}

void EntityCatalog::PublicIdPredicate::setTextClass(const PublicId::TextClass& tc)
{
    isTc = 1;
    tc_ = tc;
}

void EntityCatalog::PublicIdPredicate::setOwnerType(const PublicId::OwnerType& ot)
{
    isOt = 1;
    ot_ = ot;
}

void EntityCatalog::PublicIdPredicate::setOwner(const StringC& owner)
{
    isOwner = 1;
    owner_ = owner;
}

void EntityCatalog::PublicIdPredicate::setUnavailable(const Boolean& unavailable)
{
    isUnavailable = 1;
    unavailable_ = unavailable;
}

void EntityCatalog::PublicIdPredicate::setDescription(const StringC& description)
{
    isDescription = 1;
    description_ = description;
}

void EntityCatalog::PublicIdPredicate::setLanguage(const StringC& lang)
{
    isLang = 1;
    lang_ = lang;
}

void EntityCatalog::PublicIdPredicate::setDesignatingSequence(const StringC& ds)
{
    isDs = 1;
    ds_ = ds;
}

void EntityCatalog::PublicIdPredicate::setDisplayVersion(const StringC& dv)
{
    isDv = 1;
    dv_ = dv;
}

#ifdef SP_NAMESPACE
}
#endif
