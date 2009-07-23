// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#include "config.h"
#include "stylelib.h"

#ifdef SP_MANUAL_INST

#define SP_DEFINE_TEMPLATES
#include "Owner.h"
#include "CopyOwner.h"
#undef SP_DEFINE_TEMPLATES

#if _MSC_VER >= 1100

#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif

#ifdef DSSSL_NAMESPACE
using namespace DSSSL_NAMESPACE;
#endif

#ifdef GROVE_NAMESPACE
using namespace GROVE_NAMESPACE;
#endif

#else

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

#endif

#ifdef __DECCXX
#pragma define_template Owner<SimplePageSequenceFlowObj::HeaderFooter>
#else
#ifdef __xlC__
#pragma define(Owner<SimplePageSequenceFlowObj::HeaderFooter>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Owner<SimplePageSequenceFlowObj::HeaderFooter>;
#else
typedef Owner<SimplePageSequenceFlowObj::HeaderFooter> Dummy_0;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template CopyOwner<ScoreFlowObj::Type>
#else
#ifdef __xlC__
#pragma define(CopyOwner<ScoreFlowObj::Type>)
#else
#ifdef SP_ANSI_CLASS_INST
template class CopyOwner<ScoreFlowObj::Type>;
#else
typedef CopyOwner<ScoreFlowObj::Type> Dummy_1;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Owner<TableFlowObj::NIC>
#else
#ifdef __xlC__
#pragma define(Owner<TableFlowObj::NIC>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Owner<TableFlowObj::NIC>;
#else
typedef Owner<TableFlowObj::NIC> Dummy_2;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Owner<ScoreFlowObj::Type>
#else
#ifdef __xlC__
#pragma define(Owner<ScoreFlowObj::Type>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Owner<ScoreFlowObj::Type>;
#else
typedef Owner<ScoreFlowObj::Type> Dummy_3;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Owner<TableColumnFlowObj::NIC>
#else
#ifdef __xlC__
#pragma define(Owner<TableColumnFlowObj::NIC>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Owner<TableColumnFlowObj::NIC>;
#else
typedef Owner<TableColumnFlowObj::NIC> Dummy_4;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Owner<TableCellFlowObj::NIC>
#else
#ifdef __xlC__
#pragma define(Owner<TableCellFlowObj::NIC>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Owner<TableCellFlowObj::NIC>;
#else
typedef Owner<TableCellFlowObj::NIC> Dummy_5;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Owner<FOTBuilder::BoxNIC>
#else
#ifdef __xlC__
#pragma define(Owner<FOTBuilder::BoxNIC>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Owner<FOTBuilder::BoxNIC>;
#else
typedef Owner<FOTBuilder::BoxNIC> Dummy_6;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Owner<FOTBuilder::LeaderNIC>
#else
#ifdef __xlC__
#pragma define(Owner<FOTBuilder::LeaderNIC>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Owner<FOTBuilder::LeaderNIC>;
#else
typedef Owner<FOTBuilder::LeaderNIC> Dummy_7;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Owner<FOTBuilder::RuleNIC>
#else
#ifdef __xlC__
#pragma define(Owner<FOTBuilder::RuleNIC>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Owner<FOTBuilder::RuleNIC>;
#else
typedef Owner<FOTBuilder::RuleNIC> Dummy_8;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Owner<FOTBuilder::DisplayNIC>
#else
#ifdef __xlC__
#pragma define(Owner<FOTBuilder::DisplayNIC>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Owner<FOTBuilder::DisplayNIC>;
#else
typedef Owner<FOTBuilder::DisplayNIC> Dummy_9;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Owner<FOTBuilder::InlineNIC>
#else
#ifdef __xlC__
#pragma define(Owner<FOTBuilder::InlineNIC>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Owner<FOTBuilder::InlineNIC>;
#else
typedef Owner<FOTBuilder::InlineNIC> Dummy_10;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Owner<FOTBuilder::CharacterNIC>
#else
#ifdef __xlC__
#pragma define(Owner<FOTBuilder::CharacterNIC>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Owner<FOTBuilder::CharacterNIC>;
#else
typedef Owner<FOTBuilder::CharacterNIC> Dummy_11;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Owner<FOTBuilder::ExternalGraphicNIC>
#else
#ifdef __xlC__
#pragma define(Owner<FOTBuilder::ExternalGraphicNIC>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Owner<FOTBuilder::ExternalGraphicNIC>;
#else
typedef Owner<FOTBuilder::ExternalGraphicNIC> Dummy_12;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Owner<MultiModeFlowObj::NIC>
#else
#ifdef __xlC__
#pragma define(Owner<MultiModeFlowObj::NIC>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Owner<MultiModeFlowObj::NIC>;
#else
typedef Owner<MultiModeFlowObj::NIC> Dummy_13;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Owner<FOTBuilder::GridNIC>
#else
#ifdef __xlC__
#pragma define(Owner<FOTBuilder::GridNIC>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Owner<FOTBuilder::GridNIC>;
#else
typedef Owner<FOTBuilder::GridNIC> Dummy_14;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Owner<FOTBuilder::GridCellNIC>
#else
#ifdef __xlC__
#pragma define(Owner<FOTBuilder::GridCellNIC>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Owner<FOTBuilder::GridCellNIC>;
#else
typedef Owner<FOTBuilder::GridCellNIC> Dummy_15;
#endif
#endif
#endif

#ifdef SP_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
