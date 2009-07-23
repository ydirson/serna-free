#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif


#ifdef SP_NAMESPACE
}
#endif
// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#include "config.h"

#ifdef SP_MANUAL_INST

#define SP_DEFINE_TEMPLATES
#include "Vector.h"
#include "HashTable.h"
#include "IList.h"
#undef SP_DEFINE_TEMPLATES

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

#ifdef __DECCXX
#pragma define_template Vector<RtfFOTBuilder::ParaFormat>
#else
#ifdef __xlC__
#pragma define(Vector<RtfFOTBuilder::ParaFormat>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<RtfFOTBuilder::ParaFormat>;
#else
typedef Vector<RtfFOTBuilder::ParaFormat> Dummy_0;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<RtfFOTBuilder::PageFormat>
#else
#ifdef __xlC__
#pragma define(Vector<RtfFOTBuilder::PageFormat>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<RtfFOTBuilder::PageFormat>;
#else
typedef Vector<RtfFOTBuilder::PageFormat> Dummy_1;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<RtfFOTBuilder::DisplayInfo>
#else
#ifdef __xlC__
#pragma define(Vector<RtfFOTBuilder::DisplayInfo>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<RtfFOTBuilder::DisplayInfo>;
#else
typedef Vector<RtfFOTBuilder::DisplayInfo> Dummy_2;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<RtfFOTBuilder::Format>
#else
#ifdef __xlC__
#pragma define(Vector<RtfFOTBuilder::Format>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<RtfFOTBuilder::Format>;
#else
typedef Vector<RtfFOTBuilder::Format> Dummy_3;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<RtfFOTBuilder::Cell>
#else
#ifdef __xlC__
#pragma define(Vector<RtfFOTBuilder::Cell>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<RtfFOTBuilder::Cell>;
#else
typedef Vector<RtfFOTBuilder::Cell> Dummy_4;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<Vector<RtfFOTBuilder::Cell> >
#else
#ifdef __xlC__
#pragma define(Vector<Vector<RtfFOTBuilder::Cell> >)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<Vector<RtfFOTBuilder::Cell> >;
#else
typedef Vector<Vector<RtfFOTBuilder::Cell> > Dummy_5;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<RtfFOTBuilder::Column>
#else
#ifdef __xlC__
#pragma define(Vector<RtfFOTBuilder::Column>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<RtfFOTBuilder::Column>;
#else
typedef Vector<RtfFOTBuilder::Column> Dummy_6;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<NodePtr>
#else
#ifdef __xlC__
#pragma define(Vector<NodePtr>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<NodePtr>;
#else
typedef Vector<NodePtr> Dummy_7;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<long>
#else
#ifdef __xlC__
#pragma define(Vector<long>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<long>;
#else
typedef Vector<long> Dummy_8;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<RtfFOTBuilder::FontFamilyCharsets>
#else
#ifdef __xlC__
#pragma define(Vector<RtfFOTBuilder::FontFamilyCharsets>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<RtfFOTBuilder::FontFamilyCharsets>;
#else
typedef Vector<RtfFOTBuilder::FontFamilyCharsets> Dummy_9;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template HashTableIter<StringC,int>
#else
#ifdef __xlC__
#pragma define(HashTableIter<StringC,int>)
#else
#ifdef SP_ANSI_CLASS_INST
template class HashTableIter<StringC,int>;
#else
typedef HashTableIter<StringC,int> Dummy_10;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template IList<RtfFOTBuilder::ReorderFlowObject>
#else
#ifdef __xlC__
#pragma define(IList<RtfFOTBuilder::ReorderFlowObject>)
#else
#ifdef SP_ANSI_CLASS_INST
template class IList<RtfFOTBuilder::ReorderFlowObject>;
#else
typedef IList<RtfFOTBuilder::ReorderFlowObject> Dummy_11;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template IList<RtfFOTBuilder::Grid>
#else
#ifdef __xlC__
#pragma define(IList<RtfFOTBuilder::Grid>)
#else
#ifdef SP_ANSI_CLASS_INST
template class IList<RtfFOTBuilder::Grid>;
#else
typedef IList<RtfFOTBuilder::Grid> Dummy_12;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<Vector<String<char> > >
#else
#ifdef __xlC__
#pragma define(Vector<Vector<String<char> > >)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<Vector<String<char> > >;
#else
typedef Vector<Vector<String<char> > > Dummy_13;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<Vector<char> >
#else
#ifdef __xlC__
#pragma define(Vector<Vector<char> >)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<Vector<char> >;
#else
typedef Vector<Vector<char> > Dummy_14;
#endif
#endif
#endif

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
