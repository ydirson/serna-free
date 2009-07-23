// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

// Copyright (c) 1996 James Clark
// See the file COPYING for copying permission.

#include "stylelib.h"

#ifdef SP_MANUAL_INST

#define SP_DEFINE_TEMPLATES
#include "StringOf.h"
#include "Vector.h"
#include "XcharMap.h"
#include "Ptr.h"
#include "PointerTable.h"
#include "OwnerTable.h"
#include "HashTable.h"
#include "HashTableItemBase.h"
#include "SubstTable.h"
#include "ISet.h"
#include "Owner.h"
#undef SP_DEFINE_TEMPLATES

#include "types.h"
#include "Entity.h"
#include "Syntax.h"
#include "ExtendEntityManager.h"
#include "Event.h"
#include "CmdLineApp.h"

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

__instantiate(Vector<const CmdLineApp::AppChar *>)
__instantiate(Ptr<CodingSystemKit>)
__instantiate(Ptr<Sd>)
__instantiate(Ptr<InputSourceOrigin>)
__instantiate(Vector<ParsedSystemId::Map>)
__instantiate(Vector<size_t>)
__instantiate(String<Char>)
__instantiate(Vector<String<Char> >)
__instantiate(String<char>)
__instantiate(`OwnerTable<Named,StringC,Hash,NamedTableKeyFunction>')
__instantiate(`OwnerTableIter<Named,StringC,Hash,NamedTableKeyFunction>')
__instantiate(XcharMap<PackedBoolean>)
__instantiate(SharedXcharMap<PackedBoolean>)
__instantiate(Ptr<SharedXcharMap<PackedBoolean> >)
__instantiate(ConstPtr<SharedXcharMap<PackedBoolean> >)
__instantiate(`PointerTableIter<Named*,StringC,Hash,NamedTableKeyFunction>')
__instantiate(`PointerTable<Named*,StringC,Hash,NamedTableKeyFunction>')
__instantiate(HashTableItemBase<String<Char> >)
__instantiate(`OwnerTable<HashTableItemBase<String<Char> >,String<Char>,Hash,HashTableKeyFunction<String<Char> > >')
__instantiate(`CopyOwnerTable<HashTableItemBase<String<Char> >,String<Char>,Hash,HashTableKeyFunction<String<Char> > >')
__instantiate(`OwnerTableIter<HashTableItemBase<String<Char> >, String<Char>, Hash, HashTableKeyFunction<String<Char> > >')
__instantiate(`PointerTable<HashTableItemBase<String<Char> >*,String<Char>,Hash,HashTableKeyFunction<String<Char> > >')
__instantiate(`PointerTableIter<HashTableItemBase<String<Char> > *, String<Char>, Hash, HashTableKeyFunction<String<Char> > >')
__instantiate(Vector<HashTableItemBase<String<Char> >*>)
__instantiate(Ptr<Origin>)
__instantiate(`HashTable<StringC,Char>')
__instantiate(ConstPtr<Entity>)
__instantiate(Ptr<Entity>)
__instantiate(SubstTable<Char>)
__instantiate(Ptr<Syntax>)
__instantiate(ConstPtr<Syntax>)
__instantiate(ISet<Char>)
__instantiate(Vector<TextItem>)
__instantiate(`HashTableItem<StringC, int>')
__instantiate(`HashTable<StringC,int>')
__instantiate(Vector<PackedBoolean>)
__instantiate(Owner<EventHandler>)
__instantiate(Ptr<EntityManager>)
__instantiate(Ptr<ExtendEntityManager>)
__instantiate(Vector<StorageObjectSpec>)

#ifdef SP_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
