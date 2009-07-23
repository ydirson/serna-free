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

__instantiate(IList<MifFOTBuilder::DisplayInfo>)
__instantiate(IList<MifFOTBuilder::Format>)
__instantiate(Vector<MifFOTBuilder::Row>)
__instantiate(Vector<MifFOTBuilder::Cell>)
__instantiate(Vector<MifFOTBuilder::Column>)
__instantiate(Vector<MifFOTBuilder::TablePart>)
__instantiate(Vector<MifFOTBuilder::NodeInfo>)
__instantiate(Vector<MifFOTBuilder::LinkInfo>)
__instantiate(Vector<MifDoc::Marker>)
__instantiate(IQueue<MifFOTBuilder::DisplaySpaceInfo>)
__instantiate(Vector<MifDoc::TagStream*>)
__instantiate(Vector<MifDoc::T_dimension>)
__instantiate(Vector<MifDoc::Tbl>)
__instantiate(Vector<MifDoc::TblColumn>)
__instantiate(Vector<MifDoc::Row>)
__instantiate(Vector<MifDoc::Cell>)
__instantiate(Vector<MifDoc::Page>)
__instantiate(Vector<MifDoc::TextRect>)
__instantiate(Vector<MifDoc::TextFlow>)
__instantiate(Vector<MifDoc::ParagraphFormat>)
__instantiate(Vector<MifDoc::TblFormat>)
__instantiate(Vector<MifDoc::TabStop>)
__instantiate(Vector<MifDoc::BookComponent>)
__instantiate(Vector<MifDoc::Frame>)
__instantiate(Vector<MifDoc::Object*>)
__instantiate(Vector<MifDoc::T_XY>)
__instantiate(Vector<MifDoc::CrossRefInfo>)
__instantiate(Vector<MifDoc::ElementSet::SgmlIdInfo*>)
__instantiate(`PointerTable<MifDoc::Ruling*, String<char>, StringHash, MifDoc::Ruling>')
__instantiate(`PointerTableIter<MifDoc::Ruling*, String<char>, StringHash, MifDoc::Ruling>')
__instantiate(Vector<MifDoc::Ruling*>)
__instantiate(`PointerTable<MifDoc::Color*, String<char>, StringHash, MifDoc::Color>')
__instantiate(`PointerTableIter<MifDoc::Color*, String<char>, StringHash, MifDoc::Color>')
__instantiate(Vector<MifDoc::Color*>)
__instantiate(`PointerTable<MifDoc::ElementSet::SgmlIdInfo*, StringC, Hash, MifDoc::ElementSet::SgmlIdInfo>')
__instantiate(`PointerTableIter<MifDoc::ElementSet::SgmlIdInfo*, StringC, Hash, MifDoc::ElementSet::SgmlIdInfo>')
__instantiate(Vector<MifDoc::ElementSet::SgmlIdInfo*>)
__instantiate(Vector<MifDoc::XRefFormat>)
__instantiate(Vector<unsigned short>)
__instantiate(Vector<Vector<unsigned short> >)

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
