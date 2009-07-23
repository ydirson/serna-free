#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif


#ifdef SP_NAMESPACE
}
#endif
#ifdef SP_MANUAL_INST

#define SP_DEFINE_TEMPLATES
#include "IList.h"
#include "IQueue.h"
#include "Vector.h"
#include "Hash.h"
#include "PointerTable.h"
#undef SP_DEFINE_TEMPLATES

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

#ifdef __DECCXX
#pragma define_template IList<MifFOTBuilder::DisplayInfo>
#else
#ifdef __xlC__
#pragma define(IList<MifFOTBuilder::DisplayInfo>)
#else
#ifdef SP_ANSI_CLASS_INST
template class IList<MifFOTBuilder::DisplayInfo>;
#else
typedef IList<MifFOTBuilder::DisplayInfo> Dummy_0;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template IList<MifFOTBuilder::Format>
#else
#ifdef __xlC__
#pragma define(IList<MifFOTBuilder::Format>)
#else
#ifdef SP_ANSI_CLASS_INST
template class IList<MifFOTBuilder::Format>;
#else
typedef IList<MifFOTBuilder::Format> Dummy_1;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifFOTBuilder::Row>
#else
#ifdef __xlC__
#pragma define(Vector<MifFOTBuilder::Row>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifFOTBuilder::Row>;
#else
typedef Vector<MifFOTBuilder::Row> Dummy_2;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifFOTBuilder::Cell>
#else
#ifdef __xlC__
#pragma define(Vector<MifFOTBuilder::Cell>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifFOTBuilder::Cell>;
#else
typedef Vector<MifFOTBuilder::Cell> Dummy_3;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifFOTBuilder::Column>
#else
#ifdef __xlC__
#pragma define(Vector<MifFOTBuilder::Column>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifFOTBuilder::Column>;
#else
typedef Vector<MifFOTBuilder::Column> Dummy_4;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifFOTBuilder::TablePart>
#else
#ifdef __xlC__
#pragma define(Vector<MifFOTBuilder::TablePart>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifFOTBuilder::TablePart>;
#else
typedef Vector<MifFOTBuilder::TablePart> Dummy_5;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifFOTBuilder::NodeInfo>
#else
#ifdef __xlC__
#pragma define(Vector<MifFOTBuilder::NodeInfo>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifFOTBuilder::NodeInfo>;
#else
typedef Vector<MifFOTBuilder::NodeInfo> Dummy_6;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifFOTBuilder::LinkInfo>
#else
#ifdef __xlC__
#pragma define(Vector<MifFOTBuilder::LinkInfo>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifFOTBuilder::LinkInfo>;
#else
typedef Vector<MifFOTBuilder::LinkInfo> Dummy_7;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::Marker>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::Marker>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::Marker>;
#else
typedef Vector<MifDoc::Marker> Dummy_8;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template IQueue<MifFOTBuilder::DisplaySpaceInfo>
#else
#ifdef __xlC__
#pragma define(IQueue<MifFOTBuilder::DisplaySpaceInfo>)
#else
#ifdef SP_ANSI_CLASS_INST
template class IQueue<MifFOTBuilder::DisplaySpaceInfo>;
#else
typedef IQueue<MifFOTBuilder::DisplaySpaceInfo> Dummy_9;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::TagStream*>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::TagStream*>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::TagStream*>;
#else
typedef Vector<MifDoc::TagStream*> Dummy_10;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::T_dimension>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::T_dimension>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::T_dimension>;
#else
typedef Vector<MifDoc::T_dimension> Dummy_11;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::Tbl>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::Tbl>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::Tbl>;
#else
typedef Vector<MifDoc::Tbl> Dummy_12;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::TblColumn>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::TblColumn>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::TblColumn>;
#else
typedef Vector<MifDoc::TblColumn> Dummy_13;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::Row>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::Row>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::Row>;
#else
typedef Vector<MifDoc::Row> Dummy_14;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::Cell>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::Cell>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::Cell>;
#else
typedef Vector<MifDoc::Cell> Dummy_15;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::Page>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::Page>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::Page>;
#else
typedef Vector<MifDoc::Page> Dummy_16;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::TextRect>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::TextRect>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::TextRect>;
#else
typedef Vector<MifDoc::TextRect> Dummy_17;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::TextFlow>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::TextFlow>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::TextFlow>;
#else
typedef Vector<MifDoc::TextFlow> Dummy_18;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::ParagraphFormat>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::ParagraphFormat>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::ParagraphFormat>;
#else
typedef Vector<MifDoc::ParagraphFormat> Dummy_19;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::TblFormat>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::TblFormat>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::TblFormat>;
#else
typedef Vector<MifDoc::TblFormat> Dummy_20;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::TabStop>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::TabStop>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::TabStop>;
#else
typedef Vector<MifDoc::TabStop> Dummy_21;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::BookComponent>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::BookComponent>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::BookComponent>;
#else
typedef Vector<MifDoc::BookComponent> Dummy_22;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::Frame>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::Frame>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::Frame>;
#else
typedef Vector<MifDoc::Frame> Dummy_23;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::Object*>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::Object*>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::Object*>;
#else
typedef Vector<MifDoc::Object*> Dummy_24;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::T_XY>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::T_XY>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::T_XY>;
#else
typedef Vector<MifDoc::T_XY> Dummy_25;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::CrossRefInfo>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::CrossRefInfo>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::CrossRefInfo>;
#else
typedef Vector<MifDoc::CrossRefInfo> Dummy_26;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::ElementSet::SgmlIdInfo*>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::ElementSet::SgmlIdInfo*>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::ElementSet::SgmlIdInfo*>;
#else
typedef Vector<MifDoc::ElementSet::SgmlIdInfo*> Dummy_27;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template PointerTable<MifDoc::Ruling*, String<char>, StringHash, MifDoc::Ruling>
#else
#ifdef __xlC__
#pragma define(PointerTable<MifDoc::Ruling*, String<char>, StringHash, MifDoc::Ruling>)
#else
#ifdef SP_ANSI_CLASS_INST
template class PointerTable<MifDoc::Ruling*, String<char>, StringHash, MifDoc::Ruling>;
#else
typedef PointerTable<MifDoc::Ruling*, String<char>, StringHash, MifDoc::Ruling> Dummy_28;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template PointerTableIter<MifDoc::Ruling*, String<char>, StringHash, MifDoc::Ruling>
#else
#ifdef __xlC__
#pragma define(PointerTableIter<MifDoc::Ruling*, String<char>, StringHash, MifDoc::Ruling>)
#else
#ifdef SP_ANSI_CLASS_INST
template class PointerTableIter<MifDoc::Ruling*, String<char>, StringHash, MifDoc::Ruling>;
#else
typedef PointerTableIter<MifDoc::Ruling*, String<char>, StringHash, MifDoc::Ruling> Dummy_29;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::Ruling*>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::Ruling*>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::Ruling*>;
#else
typedef Vector<MifDoc::Ruling*> Dummy_30;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template PointerTable<MifDoc::Color*, String<char>, StringHash, MifDoc::Color>
#else
#ifdef __xlC__
#pragma define(PointerTable<MifDoc::Color*, String<char>, StringHash, MifDoc::Color>)
#else
#ifdef SP_ANSI_CLASS_INST
template class PointerTable<MifDoc::Color*, String<char>, StringHash, MifDoc::Color>;
#else
typedef PointerTable<MifDoc::Color*, String<char>, StringHash, MifDoc::Color> Dummy_31;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template PointerTableIter<MifDoc::Color*, String<char>, StringHash, MifDoc::Color>
#else
#ifdef __xlC__
#pragma define(PointerTableIter<MifDoc::Color*, String<char>, StringHash, MifDoc::Color>)
#else
#ifdef SP_ANSI_CLASS_INST
template class PointerTableIter<MifDoc::Color*, String<char>, StringHash, MifDoc::Color>;
#else
typedef PointerTableIter<MifDoc::Color*, String<char>, StringHash, MifDoc::Color> Dummy_32;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::Color*>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::Color*>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::Color*>;
#else
typedef Vector<MifDoc::Color*> Dummy_33;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template PointerTable<MifDoc::ElementSet::SgmlIdInfo*, StringC, Hash, MifDoc::ElementSet::SgmlIdInfo>
#else
#ifdef __xlC__
#pragma define(PointerTable<MifDoc::ElementSet::SgmlIdInfo*, StringC, Hash, MifDoc::ElementSet::SgmlIdInfo>)
#else
#ifdef SP_ANSI_CLASS_INST
template class PointerTable<MifDoc::ElementSet::SgmlIdInfo*, StringC, Hash, MifDoc::ElementSet::SgmlIdInfo>;
#else
typedef PointerTable<MifDoc::ElementSet::SgmlIdInfo*, StringC, Hash, MifDoc::ElementSet::SgmlIdInfo> Dummy_34;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template PointerTableIter<MifDoc::ElementSet::SgmlIdInfo*, StringC, Hash, MifDoc::ElementSet::SgmlIdInfo>
#else
#ifdef __xlC__
#pragma define(PointerTableIter<MifDoc::ElementSet::SgmlIdInfo*, StringC, Hash, MifDoc::ElementSet::SgmlIdInfo>)
#else
#ifdef SP_ANSI_CLASS_INST
template class PointerTableIter<MifDoc::ElementSet::SgmlIdInfo*, StringC, Hash, MifDoc::ElementSet::SgmlIdInfo>;
#else
typedef PointerTableIter<MifDoc::ElementSet::SgmlIdInfo*, StringC, Hash, MifDoc::ElementSet::SgmlIdInfo> Dummy_35;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::ElementSet::SgmlIdInfo*>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::ElementSet::SgmlIdInfo*>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::ElementSet::SgmlIdInfo*>;
#else
typedef Vector<MifDoc::ElementSet::SgmlIdInfo*> Dummy_36;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<MifDoc::XRefFormat>
#else
#ifdef __xlC__
#pragma define(Vector<MifDoc::XRefFormat>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<MifDoc::XRefFormat>;
#else
typedef Vector<MifDoc::XRefFormat> Dummy_37;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<unsigned short>
#else
#ifdef __xlC__
#pragma define(Vector<unsigned short>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<unsigned short>;
#else
typedef Vector<unsigned short> Dummy_38;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<Vector<unsigned short> >
#else
#ifdef __xlC__
#pragma define(Vector<Vector<unsigned short> >)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<Vector<unsigned short> >;
#else
typedef Vector<Vector<unsigned short> > Dummy_39;
#endif
#endif
#endif

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
