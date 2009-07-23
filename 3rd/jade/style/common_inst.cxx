#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif


#ifdef SP_NAMESPACE
}
#endif
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

#ifdef __DECCXX
#pragma define_template Vector<const CmdLineApp::AppChar *>
#else
#ifdef __xlC__
#pragma define(Vector<const CmdLineApp::AppChar *>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<const CmdLineApp::AppChar *>;
#else
typedef Vector<const CmdLineApp::AppChar *> Dummy_0;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Ptr<CodingSystemKit>
#else
#ifdef __xlC__
#pragma define(Ptr<CodingSystemKit>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Ptr<CodingSystemKit>;
#else
typedef Ptr<CodingSystemKit> Dummy_1;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Ptr<Sd>
#else
#ifdef __xlC__
#pragma define(Ptr<Sd>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Ptr<Sd>;
#else
typedef Ptr<Sd> Dummy_2;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Ptr<InputSourceOrigin>
#else
#ifdef __xlC__
#pragma define(Ptr<InputSourceOrigin>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Ptr<InputSourceOrigin>;
#else
typedef Ptr<InputSourceOrigin> Dummy_3;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<ParsedSystemId::Map>
#else
#ifdef __xlC__
#pragma define(Vector<ParsedSystemId::Map>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<ParsedSystemId::Map>;
#else
typedef Vector<ParsedSystemId::Map> Dummy_4;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<size_t>
#else
#ifdef __xlC__
#pragma define(Vector<size_t>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<size_t>;
#else
typedef Vector<size_t> Dummy_5;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template String<Char>
#else
#ifdef __xlC__
#pragma define(String<Char>)
#else
#ifdef SP_ANSI_CLASS_INST
template class String<Char>;
#else
typedef String<Char> Dummy_6;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<String<Char> >
#else
#ifdef __xlC__
#pragma define(Vector<String<Char> >)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<String<Char> >;
#else
typedef Vector<String<Char> > Dummy_7;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template String<char>
#else
#ifdef __xlC__
#pragma define(String<char>)
#else
#ifdef SP_ANSI_CLASS_INST
template class String<char>;
#else
typedef String<char> Dummy_8;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template OwnerTable<Named,StringC,Hash,NamedTableKeyFunction>
#else
#ifdef __xlC__
#pragma define(OwnerTable<Named,StringC,Hash,NamedTableKeyFunction>)
#else
#ifdef SP_ANSI_CLASS_INST
template class OwnerTable<Named,StringC,Hash,NamedTableKeyFunction>;
#else
typedef OwnerTable<Named,StringC,Hash,NamedTableKeyFunction> Dummy_9;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template OwnerTableIter<Named,StringC,Hash,NamedTableKeyFunction>
#else
#ifdef __xlC__
#pragma define(OwnerTableIter<Named,StringC,Hash,NamedTableKeyFunction>)
#else
#ifdef SP_ANSI_CLASS_INST
template class OwnerTableIter<Named,StringC,Hash,NamedTableKeyFunction>;
#else
typedef OwnerTableIter<Named,StringC,Hash,NamedTableKeyFunction> Dummy_10;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template XcharMap<PackedBoolean>
#else
#ifdef __xlC__
#pragma define(XcharMap<PackedBoolean>)
#else
#ifdef SP_ANSI_CLASS_INST
template class XcharMap<PackedBoolean>;
#else
typedef XcharMap<PackedBoolean> Dummy_11;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template SharedXcharMap<PackedBoolean>
#else
#ifdef __xlC__
#pragma define(SharedXcharMap<PackedBoolean>)
#else
#ifdef SP_ANSI_CLASS_INST
template class SharedXcharMap<PackedBoolean>;
#else
typedef SharedXcharMap<PackedBoolean> Dummy_12;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Ptr<SharedXcharMap<PackedBoolean> >
#else
#ifdef __xlC__
#pragma define(Ptr<SharedXcharMap<PackedBoolean> >)
#else
#ifdef SP_ANSI_CLASS_INST
template class Ptr<SharedXcharMap<PackedBoolean> >;
#else
typedef Ptr<SharedXcharMap<PackedBoolean> > Dummy_13;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template ConstPtr<SharedXcharMap<PackedBoolean> >
#else
#ifdef __xlC__
#pragma define(ConstPtr<SharedXcharMap<PackedBoolean> >)
#else
#ifdef SP_ANSI_CLASS_INST
template class ConstPtr<SharedXcharMap<PackedBoolean> >;
#else
typedef ConstPtr<SharedXcharMap<PackedBoolean> > Dummy_14;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template PointerTableIter<Named*,StringC,Hash,NamedTableKeyFunction>
#else
#ifdef __xlC__
#pragma define(PointerTableIter<Named*,StringC,Hash,NamedTableKeyFunction>)
#else
#ifdef SP_ANSI_CLASS_INST
template class PointerTableIter<Named*,StringC,Hash,NamedTableKeyFunction>;
#else
typedef PointerTableIter<Named*,StringC,Hash,NamedTableKeyFunction> Dummy_15;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template PointerTable<Named*,StringC,Hash,NamedTableKeyFunction>
#else
#ifdef __xlC__
#pragma define(PointerTable<Named*,StringC,Hash,NamedTableKeyFunction>)
#else
#ifdef SP_ANSI_CLASS_INST
template class PointerTable<Named*,StringC,Hash,NamedTableKeyFunction>;
#else
typedef PointerTable<Named*,StringC,Hash,NamedTableKeyFunction> Dummy_16;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template HashTableItemBase<String<Char> >
#else
#ifdef __xlC__
#pragma define(HashTableItemBase<String<Char> >)
#else
#ifdef SP_ANSI_CLASS_INST
template class HashTableItemBase<String<Char> >;
#else
typedef HashTableItemBase<String<Char> > Dummy_17;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template OwnerTable<HashTableItemBase<String<Char> >,String<Char>,Hash,HashTableKeyFunction<String<Char> > >
#else
#ifdef __xlC__
#pragma define(OwnerTable<HashTableItemBase<String<Char> >,String<Char>,Hash,HashTableKeyFunction<String<Char> > >)
#else
#ifdef SP_ANSI_CLASS_INST
template class OwnerTable<HashTableItemBase<String<Char> >,String<Char>,Hash,HashTableKeyFunction<String<Char> > >;
#else
typedef OwnerTable<HashTableItemBase<String<Char> >,String<Char>,Hash,HashTableKeyFunction<String<Char> > > Dummy_18;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template CopyOwnerTable<HashTableItemBase<String<Char> >,String<Char>,Hash,HashTableKeyFunction<String<Char> > >
#else
#ifdef __xlC__
#pragma define(CopyOwnerTable<HashTableItemBase<String<Char> >,String<Char>,Hash,HashTableKeyFunction<String<Char> > >)
#else
#ifdef SP_ANSI_CLASS_INST
template class CopyOwnerTable<HashTableItemBase<String<Char> >,String<Char>,Hash,HashTableKeyFunction<String<Char> > >;
#else
typedef CopyOwnerTable<HashTableItemBase<String<Char> >,String<Char>,Hash,HashTableKeyFunction<String<Char> > > Dummy_19;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template OwnerTableIter<HashTableItemBase<String<Char> >, String<Char>, Hash, HashTableKeyFunction<String<Char> > >
#else
#ifdef __xlC__
#pragma define(OwnerTableIter<HashTableItemBase<String<Char> >, String<Char>, Hash, HashTableKeyFunction<String<Char> > >)
#else
#ifdef SP_ANSI_CLASS_INST
template class OwnerTableIter<HashTableItemBase<String<Char> >, String<Char>, Hash, HashTableKeyFunction<String<Char> > >;
#else
typedef OwnerTableIter<HashTableItemBase<String<Char> >, String<Char>, Hash, HashTableKeyFunction<String<Char> > > Dummy_20;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template PointerTable<HashTableItemBase<String<Char> >*,String<Char>,Hash,HashTableKeyFunction<String<Char> > >
#else
#ifdef __xlC__
#pragma define(PointerTable<HashTableItemBase<String<Char> >*,String<Char>,Hash,HashTableKeyFunction<String<Char> > >)
#else
#ifdef SP_ANSI_CLASS_INST
template class PointerTable<HashTableItemBase<String<Char> >*,String<Char>,Hash,HashTableKeyFunction<String<Char> > >;
#else
typedef PointerTable<HashTableItemBase<String<Char> >*,String<Char>,Hash,HashTableKeyFunction<String<Char> > > Dummy_21;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template PointerTableIter<HashTableItemBase<String<Char> > *, String<Char>, Hash, HashTableKeyFunction<String<Char> > >
#else
#ifdef __xlC__
#pragma define(PointerTableIter<HashTableItemBase<String<Char> > *, String<Char>, Hash, HashTableKeyFunction<String<Char> > >)
#else
#ifdef SP_ANSI_CLASS_INST
template class PointerTableIter<HashTableItemBase<String<Char> > *, String<Char>, Hash, HashTableKeyFunction<String<Char> > >;
#else
typedef PointerTableIter<HashTableItemBase<String<Char> > *, String<Char>, Hash, HashTableKeyFunction<String<Char> > > Dummy_22;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<HashTableItemBase<String<Char> >*>
#else
#ifdef __xlC__
#pragma define(Vector<HashTableItemBase<String<Char> >*>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<HashTableItemBase<String<Char> >*>;
#else
typedef Vector<HashTableItemBase<String<Char> >*> Dummy_23;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Ptr<Origin>
#else
#ifdef __xlC__
#pragma define(Ptr<Origin>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Ptr<Origin>;
#else
typedef Ptr<Origin> Dummy_24;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template HashTable<StringC,Char>
#else
#ifdef __xlC__
#pragma define(HashTable<StringC,Char>)
#else
#ifdef SP_ANSI_CLASS_INST
template class HashTable<StringC,Char>;
#else
typedef HashTable<StringC,Char> Dummy_25;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template ConstPtr<Entity>
#else
#ifdef __xlC__
#pragma define(ConstPtr<Entity>)
#else
#ifdef SP_ANSI_CLASS_INST
template class ConstPtr<Entity>;
#else
typedef ConstPtr<Entity> Dummy_26;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Ptr<Entity>
#else
#ifdef __xlC__
#pragma define(Ptr<Entity>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Ptr<Entity>;
#else
typedef Ptr<Entity> Dummy_27;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template SubstTable<Char>
#else
#ifdef __xlC__
#pragma define(SubstTable<Char>)
#else
#ifdef SP_ANSI_CLASS_INST
template class SubstTable<Char>;
#else
typedef SubstTable<Char> Dummy_28;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Ptr<Syntax>
#else
#ifdef __xlC__
#pragma define(Ptr<Syntax>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Ptr<Syntax>;
#else
typedef Ptr<Syntax> Dummy_29;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template ConstPtr<Syntax>
#else
#ifdef __xlC__
#pragma define(ConstPtr<Syntax>)
#else
#ifdef SP_ANSI_CLASS_INST
template class ConstPtr<Syntax>;
#else
typedef ConstPtr<Syntax> Dummy_30;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template ISet<Char>
#else
#ifdef __xlC__
#pragma define(ISet<Char>)
#else
#ifdef SP_ANSI_CLASS_INST
template class ISet<Char>;
#else
typedef ISet<Char> Dummy_31;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<TextItem>
#else
#ifdef __xlC__
#pragma define(Vector<TextItem>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<TextItem>;
#else
typedef Vector<TextItem> Dummy_32;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template HashTableItem<StringC, int>
#else
#ifdef __xlC__
#pragma define(HashTableItem<StringC, int>)
#else
#ifdef SP_ANSI_CLASS_INST
template class HashTableItem<StringC, int>;
#else
typedef HashTableItem<StringC, int> Dummy_33;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template HashTable<StringC,int>
#else
#ifdef __xlC__
#pragma define(HashTable<StringC,int>)
#else
#ifdef SP_ANSI_CLASS_INST
template class HashTable<StringC,int>;
#else
typedef HashTable<StringC,int> Dummy_34;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<PackedBoolean>
#else
#ifdef __xlC__
#pragma define(Vector<PackedBoolean>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<PackedBoolean>;
#else
typedef Vector<PackedBoolean> Dummy_35;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Owner<EventHandler>
#else
#ifdef __xlC__
#pragma define(Owner<EventHandler>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Owner<EventHandler>;
#else
typedef Owner<EventHandler> Dummy_36;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Ptr<EntityManager>
#else
#ifdef __xlC__
#pragma define(Ptr<EntityManager>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Ptr<EntityManager>;
#else
typedef Ptr<EntityManager> Dummy_37;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Ptr<ExtendEntityManager>
#else
#ifdef __xlC__
#pragma define(Ptr<ExtendEntityManager>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Ptr<ExtendEntityManager>;
#else
typedef Ptr<ExtendEntityManager> Dummy_38;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<StorageObjectSpec>
#else
#ifdef __xlC__
#pragma define(Vector<StorageObjectSpec>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<StorageObjectSpec>;
#else
typedef Vector<StorageObjectSpec> Dummy_39;
#endif
#endif
#endif

#ifdef SP_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
