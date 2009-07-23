#ifdef SP_NAMESPACE
namespace SP_NAMESPACE {
#endif


#ifdef SP_NAMESPACE
}
#endif
#include "config.h"

#ifdef SP_MANUAL_INST

#define SP_DEFINE_TEMPLATES
#include "Vector.h"
#undef SP_DEFINE_TEMPLATES

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

#ifdef __DECCXX
#pragma define_template Vector<TeXFOTBuilder::Format>
#else
#ifdef __xlC__
#pragma define(Vector<TeXFOTBuilder::Format>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<TeXFOTBuilder::Format>;
#else
typedef Vector<TeXFOTBuilder::Format> Dummy_0;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<TeXFOTBuilder::Row>
#else
#ifdef __xlC__
#pragma define(Vector<TeXFOTBuilder::Row>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<TeXFOTBuilder::Row>;
#else
typedef Vector<TeXFOTBuilder::Row> Dummy_1;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<TeXFOTBuilder::Cell>
#else
#ifdef __xlC__
#pragma define(Vector<TeXFOTBuilder::Cell>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<TeXFOTBuilder::Cell>;
#else
typedef Vector<TeXFOTBuilder::Cell> Dummy_2;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<TeXFOTBuilder::Column>
#else
#ifdef __xlC__
#pragma define(Vector<TeXFOTBuilder::Column>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<TeXFOTBuilder::Column>;
#else
typedef Vector<TeXFOTBuilder::Column> Dummy_3;
#endif
#endif
#endif
#ifdef __DECCXX
#pragma define_template Vector<TeXFOTBuilder::TablePart>
#else
#ifdef __xlC__
#pragma define(Vector<TeXFOTBuilder::TablePart>)
#else
#ifdef SP_ANSI_CLASS_INST
template class Vector<TeXFOTBuilder::TablePart>;
#else
typedef Vector<TeXFOTBuilder::TablePart> Dummy_4;
#endif
#endif
#endif

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
