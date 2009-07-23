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
#include "Ptr.h"
#include "IList.h"
#include "IListIter.h"
#include "Vector.h"
#include "OwnerTable.h"
#include "PointerTable.h"
#undef SP_DEFINE_TEMPLATES

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

#ifdef __DECCXX
#pragma define_template IList<HtmlFOTBuilder::Item>
#else
#ifdef __xlC__
#pragma define(IList<HtmlFOTBuilder::Item>)
#else
#ifdef SP_ANSI_CLASS_INST
template class IList<HtmlFOTBuilder::Item>;
#else
typedef IList<HtmlFOTBuilder::Item> Dummy_0;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template IListIter<HtmlFOTBuilder::Item>
#else
#ifdef __xlC__
#pragma define(IListIter<HtmlFOTBuilder::Item>)
#else
#ifdef SP_ANSI_CLASS_INST
template class IListIter<HtmlFOTBuilder::Item>;
#else
typedef IListIter<HtmlFOTBuilder::Item> Dummy_1;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template IList<HtmlFOTBuilder::DestInfo>
#else
#ifdef __xlC__
#pragma define(IList<HtmlFOTBuilder::DestInfo>)
#else
#ifdef SP_ANSI_CLASS_INST
template class IList<HtmlFOTBuilder::DestInfo>;
#else
typedef IList<HtmlFOTBuilder::DestInfo> Dummy_2;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<HtmlFOTBuilder::FlowObjectInfo>
#else
#ifdef __xlC__
#pragma define(Vector<HtmlFOTBuilder::FlowObjectInfo>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<HtmlFOTBuilder::FlowObjectInfo>;
#else
typedef Vector<HtmlFOTBuilder::FlowObjectInfo> Dummy_3;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<HtmlFOTBuilder::Addressable *>
#else
#ifdef __xlC__
#pragma define(Vector<HtmlFOTBuilder::Addressable *>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<HtmlFOTBuilder::Addressable *>;
#else
typedef Vector<HtmlFOTBuilder::Addressable *> Dummy_4;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<Vector<HtmlFOTBuilder::Addressable *> >
#else
#ifdef __xlC__
#pragma define(Vector<Vector<HtmlFOTBuilder::Addressable *> >)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<Vector<HtmlFOTBuilder::Addressable *> >;
#else
typedef Vector<Vector<HtmlFOTBuilder::Addressable *> > Dummy_5;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<Vector<size_t> >
#else
#ifdef __xlC__
#pragma define(Vector<Vector<size_t> >)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<Vector<size_t> >;
#else
typedef Vector<Vector<size_t> > Dummy_6;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template OwnerTable<HtmlFOTBuilder::CharStyle, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle>
#else
#ifdef __xlC__
#pragma define(OwnerTable<HtmlFOTBuilder::CharStyle, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle>)
#else
#ifdef SP_ANSI_CLASS_INST
template class OwnerTable<HtmlFOTBuilder::CharStyle, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle>;
#else
typedef OwnerTable<HtmlFOTBuilder::CharStyle, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle> Dummy_7;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template OwnerTable<HtmlFOTBuilder::ParaStyle, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle>
#else
#ifdef __xlC__
#pragma define(OwnerTable<HtmlFOTBuilder::ParaStyle, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle>)
#else
#ifdef SP_ANSI_CLASS_INST
template class OwnerTable<HtmlFOTBuilder::ParaStyle, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle>;
#else
typedef OwnerTable<HtmlFOTBuilder::ParaStyle, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle> Dummy_8;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template OwnerTable<HtmlFOTBuilder::ClassPrefix, StringC, Hash, HtmlFOTBuilder::ClassPrefix>
#else
#ifdef __xlC__
#pragma define(OwnerTable<HtmlFOTBuilder::ClassPrefix, StringC, Hash, HtmlFOTBuilder::ClassPrefix>)
#else
#ifdef SP_ANSI_CLASS_INST
template class OwnerTable<HtmlFOTBuilder::ClassPrefix, StringC, Hash, HtmlFOTBuilder::ClassPrefix>;
#else
typedef OwnerTable<HtmlFOTBuilder::ClassPrefix, StringC, Hash, HtmlFOTBuilder::ClassPrefix> Dummy_9;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template PointerTable<HtmlFOTBuilder::CharStyle *, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle>
#else
#ifdef __xlC__
#pragma define(PointerTable<HtmlFOTBuilder::CharStyle *, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle>)
#else
#ifdef SP_ANSI_CLASS_INST
template class PointerTable<HtmlFOTBuilder::CharStyle *, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle>;
#else
typedef PointerTable<HtmlFOTBuilder::CharStyle *, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle> Dummy_10;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template PointerTable<HtmlFOTBuilder::ParaStyle *, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle>
#else
#ifdef __xlC__
#pragma define(PointerTable<HtmlFOTBuilder::ParaStyle *, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle>)
#else
#ifdef SP_ANSI_CLASS_INST
template class PointerTable<HtmlFOTBuilder::ParaStyle *, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle>;
#else
typedef PointerTable<HtmlFOTBuilder::ParaStyle *, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle> Dummy_11;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template PointerTable<HtmlFOTBuilder::ClassPrefix *, StringC, Hash, HtmlFOTBuilder::ClassPrefix>
#else
#ifdef __xlC__
#pragma define(PointerTable<HtmlFOTBuilder::ClassPrefix *, StringC, Hash, HtmlFOTBuilder::ClassPrefix>)
#else
#ifdef SP_ANSI_CLASS_INST
template class PointerTable<HtmlFOTBuilder::ClassPrefix *, StringC, Hash, HtmlFOTBuilder::ClassPrefix>;
#else
typedef PointerTable<HtmlFOTBuilder::ClassPrefix *, StringC, Hash, HtmlFOTBuilder::ClassPrefix> Dummy_12;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template OwnerTableIter<HtmlFOTBuilder::CharStyle, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle>
#else
#ifdef __xlC__
#pragma define(OwnerTableIter<HtmlFOTBuilder::CharStyle, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle>)
#else
#ifdef SP_ANSI_CLASS_INST
template class OwnerTableIter<HtmlFOTBuilder::CharStyle, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle>;
#else
typedef OwnerTableIter<HtmlFOTBuilder::CharStyle, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle> Dummy_13;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template OwnerTableIter<HtmlFOTBuilder::ParaStyle, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle>
#else
#ifdef __xlC__
#pragma define(OwnerTableIter<HtmlFOTBuilder::ParaStyle, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle>)
#else
#ifdef SP_ANSI_CLASS_INST
template class OwnerTableIter<HtmlFOTBuilder::ParaStyle, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle>;
#else
typedef OwnerTableIter<HtmlFOTBuilder::ParaStyle, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle> Dummy_14;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template PointerTableIter<HtmlFOTBuilder::CharStyle *, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle>
#else
#ifdef __xlC__
#pragma define(PointerTableIter<HtmlFOTBuilder::CharStyle *, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle>)
#else
#ifdef SP_ANSI_CLASS_INST
template class PointerTableIter<HtmlFOTBuilder::CharStyle *, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle>;
#else
typedef PointerTableIter<HtmlFOTBuilder::CharStyle *, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle> Dummy_15;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template PointerTableIter<HtmlFOTBuilder::ParaStyle *, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle>
#else
#ifdef __xlC__
#pragma define(PointerTableIter<HtmlFOTBuilder::ParaStyle *, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle>)
#else
#ifdef SP_ANSI_CLASS_INST
template class PointerTableIter<HtmlFOTBuilder::ParaStyle *, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle>;
#else
typedef PointerTableIter<HtmlFOTBuilder::ParaStyle *, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle> Dummy_16;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<HtmlFOTBuilder::CharStyle *>
#else
#ifdef __xlC__
#pragma define(Vector<HtmlFOTBuilder::CharStyle *>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<HtmlFOTBuilder::CharStyle *>;
#else
typedef Vector<HtmlFOTBuilder::CharStyle *> Dummy_17;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<HtmlFOTBuilder::ParaStyle *>
#else
#ifdef __xlC__
#pragma define(Vector<HtmlFOTBuilder::ParaStyle *>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<HtmlFOTBuilder::ParaStyle *>;
#else
typedef Vector<HtmlFOTBuilder::ParaStyle *> Dummy_18;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<HtmlFOTBuilder::ClassPrefix *>
#else
#ifdef __xlC__
#pragma define(Vector<HtmlFOTBuilder::ClassPrefix *>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<HtmlFOTBuilder::ClassPrefix *>;
#else
typedef Vector<HtmlFOTBuilder::ClassPrefix *> Dummy_19;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template IList<HtmlFOTBuilder::CharStyleClass>
#else
#ifdef __xlC__
#pragma define(IList<HtmlFOTBuilder::CharStyleClass>)
#else
#ifdef SP_ANSI_CLASS_INST
template class IList<HtmlFOTBuilder::CharStyleClass>;
#else
typedef IList<HtmlFOTBuilder::CharStyleClass> Dummy_20;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template IList<HtmlFOTBuilder::ParaStyleClass>
#else
#ifdef __xlC__
#pragma define(IList<HtmlFOTBuilder::ParaStyleClass>)
#else
#ifdef SP_ANSI_CLASS_INST
template class IList<HtmlFOTBuilder::ParaStyleClass>;
#else
typedef IList<HtmlFOTBuilder::ParaStyleClass> Dummy_21;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template IListIter<HtmlFOTBuilder::CharStyleClass>
#else
#ifdef __xlC__
#pragma define(IListIter<HtmlFOTBuilder::CharStyleClass>)
#else
#ifdef SP_ANSI_CLASS_INST
template class IListIter<HtmlFOTBuilder::CharStyleClass>;
#else
typedef IListIter<HtmlFOTBuilder::CharStyleClass> Dummy_22;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template IListIter<HtmlFOTBuilder::ParaStyleClass>
#else
#ifdef __xlC__
#pragma define(IListIter<HtmlFOTBuilder::ParaStyleClass>)
#else
#ifdef SP_ANSI_CLASS_INST
template class IListIter<HtmlFOTBuilder::ParaStyleClass>;
#else
typedef IListIter<HtmlFOTBuilder::ParaStyleClass> Dummy_23;
#endif
#endif
#endif

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
