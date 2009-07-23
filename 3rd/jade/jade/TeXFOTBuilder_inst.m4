#include "config.h"

#ifdef SP_MANUAL_INST

#define SP_DEFINE_TEMPLATES
#include "Vector.h"
#undef SP_DEFINE_TEMPLATES

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

__instantiate(Vector<TeXFOTBuilder::Format>)
__instantiate(Vector<TeXFOTBuilder::Row>)
__instantiate(Vector<TeXFOTBuilder::Cell>)
__instantiate(Vector<TeXFOTBuilder::Column>)
__instantiate(Vector<TeXFOTBuilder::TablePart>)

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
