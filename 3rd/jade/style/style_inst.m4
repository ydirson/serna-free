// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#include "stylelib.h"

#ifdef SP_MANUAL_INST

#define SP_DEFINE_TEMPLATES
#include "Vector.h"
#include "NCVector.h"
#include "Ptr.h"
#include "Owner.h"
#include "PointerTable.h"
#include "OwnerTable.h"
#include "NamedTable.h"
#include "IList.h"
#include "IListIter.h"
#include "IQueue.h"
#include "HashTable.h"
#ifdef SP_SIZEOF_BOOL_1
#include "XcharMap.h"
#endif
#undef SP_DEFINE_TEMPLATES

#include "Insn.h"
#include "Expression.h"
#include "Interpreter.h"
#include "ELObj.h"
#include "Style.h"
#include "FOTBuilder.h"
#include "ProcessingMode.h"
#include "ProcessContext.h"
#include "SosofoObj.h"
#include "DssslSpecEventHandler.h"
#include "Boolean.h"
#include "Node.h"
#include "DssslApp.h"
#include "Pattern.h"
#include "MacroFlowObj.h"

#ifdef DSSSL_NAMESPACE
class DSSSL_NAMESPACE::ProcessingMode;
class DSSSL_NAMESPACE::SosofoObj;
#else
class ProcessingMode;
class SosofoObj;
#endif

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

__instantiate(Ptr<Insn>)
__instantiate(Owner<Expression>)
__instantiate(Vector<const Identifier *>)
__instantiate(NCVector<Owner<Expression> >)
__instantiate(Vector<BoundVar>)
__instantiate(Owner<FOTBuilder>)
__instantiate(Owner<SaveFOTBuilder>)
__instantiate(Owner<InputSource>)
__instantiate(Vector<SymbolObj *>)
__instantiate(Owner<OutputCharStream>)
__instantiate(Ptr<Environment::FrameVarList>)
__instantiate(ConstPtr<Environment::FrameVarList>)
__instantiate(`PointerTable<SymbolObj *, StringC, Hash, SymbolObj>')
__instantiate(NamedTableIter<Identifier>)
__instantiate(Ptr<StyleSpec>)
__instantiate(Ptr<InheritedC>)
__instantiate(ConstPtr<InheritedC>)
__instantiate(Vector<ConstPtr<InheritedC> >)
__instantiate(Vector<const Vector<ConstPtr<InheritedC> > *>)
__instantiate(Vector<const VarStyleObj *>)
__instantiate(Ptr<InheritedCInfo>)
__instantiate(NamedTable<ProcessingMode::ElementRules>)
__instantiate(NamedTableIter<ProcessingMode::ElementRules>)
__instantiate(NamedTableIter<ProcessingMode>)
__instantiate(NCVector<ProcessingMode::GroveRules>)
__instantiate(Vector<ProcessingMode::Rule>)
__instantiate(Ptr<ProcessingMode::Action>)
__instantiate(IList<ProcessingMode::ElementRule>)
__instantiate(IListIter<ProcessingMode::ElementRule>)
__instantiate(Vector<const ProcessingMode::ElementRule *>)
__instantiate(IList<SaveFOTBuilder>)
__instantiate(Vector<const ProcessingMode *>)
__instantiate(Owner<FOTBuilder::DisplayGroupNIC>)
__instantiate(Vector<SosofoObj *>)
__instantiate(Vector<FOTBuilder *>)
__instantiate(IQueue<SaveFOTBuilder>)
__instantiate(NCVector<IQueue<SaveFOTBuilder> >)
__instantiate(IList<ProcessContext::Connection>)
__instantiate(IListIter<ProcessContext::Connection>)
__instantiate(IList<ProcessContext::Connectable>)
__instantiate(IListIter<ProcessContext::Connectable>)
__instantiate(Ptr<PopList>)
__instantiate(Vector<Ptr<InheritedCInfo> >)
__instantiate(NCVector<ProcessContext::Port>)
__instantiate(NCVector<CaseExpression::Case>)
__instantiate(NamedTable<NumberCache::Entry>)
__instantiate(NCVector<NamedTable<NumberCache::Entry> >)
__instantiate(NamedTable<NumberCache::ElementEntry>)
__instantiate(NCVector<NamedTable<NumberCache::ElementEntry> >)
__instantiate(Vector<ELObj *>)
__instantiate(Vector<ConstPtr<Insn> >)
__instantiate(Vector<StyleObj *>)
__instantiate(Vector<Vector<StyleObj *> >)
__instantiate(IList<ProcessContext::Table>)
__instantiate(IListIter<ProcessContext::Table>)
__instantiate(Vector<String<char> >)
__instantiate(Owner<LengthSpec>)
__instantiate(Vector<FOTBuilder::MultiMode>)
__instantiate(Vector<Vector<FOTBuilder::MultiMode> >)
__instantiate(`HashTable<StringC,FunctionObj *>')
__instantiate(`HashTableItem<StringC,FunctionObj *>')
__instantiate(Vector<DssslSpecEventHandler::Part *>)
__instantiate(Vector<DssslSpecEventHandler::PartHeader *>)
__instantiate(IListIter<DssslSpecEventHandler::BodyElement>)
__instantiate(IList<DssslSpecEventHandler::BodyElement>)
__instantiate(IListIter<DssslSpecEventHandler::PartHeader>)
__instantiate(IList<DssslSpecEventHandler::PartHeader>)
__instantiate(IListIter<DssslSpecEventHandler::Doc>)
__instantiate(IList<DssslSpecEventHandler::Doc>)
__instantiate(Owner<DssslSpecEventHandler::SpecPart>)
__instantiate(Owner<Location>)
__instantiate(Owner<FOTBuilder::DisplaySpace>)
__instantiate(Owner<FOTBuilder::InlineSpace>)
__instantiate(Owner<FOTBuilder::Address>)
__instantiate(Vector<String<char> *>)
__instantiate(`PointerTable<String<char> *, String<char>, Interpreter::StringSet, Interpreter::StringSet>')
__instantiate(`OwnerTable<String<char>, String<char>, Interpreter::StringSet, Interpreter::StringSet>')
__instantiate(Vector<FOTBuilder::GlyphId>)
__instantiate(Ptr<FOTBuilder::GlyphSubstTable>)
__instantiate(ConstPtr<FOTBuilder::GlyphSubstTable>)
__instantiate(Vector<ConstPtr<FOTBuilder::GlyphSubstTable> >)
__instantiate(Owner<FOTBuilder::ExtensionFlowObj>)
__instantiate(Owner<FOTBuilder::CompoundExtensionFlowObj>)
__instantiate(`HashTable<StringC,NodePtr>')
__instantiate(`HashTableItem<StringC,NodePtr>')
__instantiate(Vector<ProcessContext::NodeStackEntry>)
__instantiate(Vector<const Pattern::Element *>)
__instantiate(NCVector<Pattern>)
__instantiate(IList<Pattern::Element>)
__instantiate(IListIter<Pattern::Element>)
__instantiate(IList<Pattern::Qualifier>)
__instantiate(IListIter<Pattern::Qualifier>)
__instantiate(Ptr<MacroFlowObj::Definition>)
#ifdef SP_SIZEOF_BOOL_1
__instantiate(XcharMap<char>)
__instantiate(SharedXcharMap<char>)
__instantiate(Ptr<SharedXcharMap<char> >)
__instantiate(ConstPtr<SharedXcharMap<char> >)
#endif

#ifdef SP_NAMESPACE
}
#endif

#endif /* SP_MANUAL_INST */
