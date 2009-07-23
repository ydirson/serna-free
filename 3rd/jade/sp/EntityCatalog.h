#ifndef EntityCatalog_INCLUDED
#define EntityCatalog_INCLUDED 1

#if defined(__GNUG__) && defined(GNUG_PRAGMA_INTERFACE)
#pragma interface
#endif

#include "types.h"
#include "StringC.h"
#include "Resource.h"
#include "SubstTable.h"


// Syntext extentions require:
#include "ExternalId.h"
#include "List.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

class Messenger;
class CharsetInfo;
class EntityDecl;

class SP_API EntityCatalog : public Resource {
public:
  class SP_API Syntax {
  public:
    virtual Boolean namecaseGeneral() const = 0;
    virtual Boolean namecaseEntity() const = 0;
    virtual const SubstTable<Char> &upperSubstTable() const = 0;
    virtual const StringC &peroDelim() const = 0;
    virtual ~Syntax() {}
  };
  virtual ~EntityCatalog();
  virtual Boolean sgmlDecl(const CharsetInfo &,
			   Messenger &,
			   StringC &) const;
  virtual Boolean lookup(const EntityDecl &,
			 const Syntax &,
			 const CharsetInfo &,
			 Messenger &,
			 StringC &) const;
  virtual Boolean lookupPublic(const StringC &,
			       const CharsetInfo &,
			       Messenger &,
			       StringC &) const;
  // This is for a character described by a minimum literal
  // in the SGML declaration.
  virtual Boolean lookupChar(const StringC &,
                             const CharsetInfo &,
			     Messenger &,
			     UnivChar &) const;

    /*! Syntext extentions to SP. Helps filtering the catalog entries

       Used together with function makeEntryList(). If some field is set 
       (via set....() function) that means the entry must have indentical 
       field to present in the resulting list. If the field is not set then
       the field is not tested.

       Leaving data members public is rather inaccurate though. Justified with
       avoiding kludging the interface with the extra functions...
     */

    struct SP_API PublicIdPredicate {
        PublicIdPredicate();

        void setTextClass(const PublicId::TextClass& tc);
        void setOwnerType(const PublicId::OwnerType& ot);
        void setOwner(const StringC& owner);
        void setUnavailable(const Boolean& unavailable);
        void setDescription(const StringC& description );
        void setLanguage(const StringC& lang);
        void setDesignatingSequence(const StringC& ds);
        void setDisplayVersion(const StringC& dv);

        PublicId::TextClass tc_;
        Boolean isTc;

        PublicId::OwnerType ot_;
        Boolean isOt;
        
        StringC owner_;
        Boolean isOwner;
        
        Boolean unavailable_;
        Boolean isUnavailable;
        
        StringC description_;
        Boolean isDescription;
        
        StringC lang_;
        Boolean isLang;
        
        StringC ds_;
        Boolean isDs;
        
        StringC dv_;
        Boolean isDv;
    };

    virtual List<StringC>* 
    makeEntryList(const CharsetInfo &charset,
                  const Char& space,
                  const PublicIdPredicate& predicate) const = 0;

};

#ifdef SP_NAMESPACE
}
#endif

#endif /* not EntityCatalog_INCLUDED */
