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

__instantiate(IList<HtmlFOTBuilder::Item>)
__instantiate(IListIter<HtmlFOTBuilder::Item>)
__instantiate(IList<HtmlFOTBuilder::DestInfo>)
__instantiate(Vector<HtmlFOTBuilder::FlowObjectInfo>)
__instantiate(Vector<HtmlFOTBuilder::Addressable *>)
__instantiate(Vector<Vector<HtmlFOTBuilder::Addressable *> >)
__instantiate(Vector<Vector<size_t> >)
__instantiate(`OwnerTable<HtmlFOTBuilder::CharStyle, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle>')
__instantiate(`OwnerTable<HtmlFOTBuilder::ParaStyle, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle>')
__instantiate(`OwnerTable<HtmlFOTBuilder::ClassPrefix, StringC, Hash, HtmlFOTBuilder::ClassPrefix>')
__instantiate(`PointerTable<HtmlFOTBuilder::CharStyle *, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle>')
__instantiate(`PointerTable<HtmlFOTBuilder::ParaStyle *, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle>')
__instantiate(`PointerTable<HtmlFOTBuilder::ClassPrefix *, StringC, Hash, HtmlFOTBuilder::ClassPrefix>')
__instantiate(`OwnerTableIter<HtmlFOTBuilder::CharStyle, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle>')
__instantiate(`OwnerTableIter<HtmlFOTBuilder::ParaStyle, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle>')
__instantiate(`PointerTableIter<HtmlFOTBuilder::CharStyle *, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharProps, HtmlFOTBuilder::CharStyle>')
__instantiate(`PointerTableIter<HtmlFOTBuilder::ParaStyle *, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaProps, HtmlFOTBuilder::ParaStyle>')
__instantiate(Vector<HtmlFOTBuilder::CharStyle *>)
__instantiate(Vector<HtmlFOTBuilder::ParaStyle *>)
__instantiate(Vector<HtmlFOTBuilder::ClassPrefix *>)
__instantiate(IList<HtmlFOTBuilder::CharStyleClass>)
__instantiate(IList<HtmlFOTBuilder::ParaStyleClass>)
__instantiate(IListIter<HtmlFOTBuilder::CharStyleClass>)
__instantiate(IListIter<HtmlFOTBuilder::ParaStyleClass>)

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
