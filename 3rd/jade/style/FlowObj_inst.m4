// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

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

__instantiate(Owner<SimplePageSequenceFlowObj::HeaderFooter>)
__instantiate(CopyOwner<ScoreFlowObj::Type>)
__instantiate(Owner<TableFlowObj::NIC>)
__instantiate(Owner<ScoreFlowObj::Type>)
__instantiate(Owner<TableColumnFlowObj::NIC>)
__instantiate(Owner<TableCellFlowObj::NIC>)
__instantiate(Owner<FOTBuilder::BoxNIC>)
__instantiate(Owner<FOTBuilder::LeaderNIC>)
__instantiate(Owner<FOTBuilder::RuleNIC>)
__instantiate(Owner<FOTBuilder::DisplayNIC>)
__instantiate(Owner<FOTBuilder::InlineNIC>)
__instantiate(Owner<FOTBuilder::CharacterNIC>)
__instantiate(Owner<FOTBuilder::ExternalGraphicNIC>)
__instantiate(Owner<MultiModeFlowObj::NIC>)
__instantiate(Owner<FOTBuilder::GridNIC>)
__instantiate(Owner<FOTBuilder::GridCellNIC>)

#ifdef SP_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
