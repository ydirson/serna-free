// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#include "stylelib.h"
#include "ELObj.h"
#include "Interpreter.h"
#include "InterpreterMessages.h"
#include "macros.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class ReverseNodeListObj : public NodeListObj {
public:
  ReverseNodeListObj(NodeListObj *);
  NodePtr nodeListFirst(EvalContext &, Interpreter &);
  NodeListObj *nodeListRest(EvalContext &, Interpreter &);
  NodeListObj *nodeListReverse(EvalContext &context, Interpreter &interp);
  NodePtr nodeListRef(long, EvalContext &, Interpreter &);
  long nodeListLength(EvalContext &context, Interpreter &interp);
  void traceSubObjects(Collector &) const;
private:
  NodeListObj *reversed(EvalContext &context, Interpreter &interp);
  NodeListObj *nl_;
  NodeListObj *reversed_;
};

ELObj::ELObj()
{
  hasSubObjects_ = 0;
}

bool ELObj::isEqual(ELObj &)
{
  return false;
}
    
bool ELObj::isEquiv(ELObj &obj)
{
  return isEqual(obj);
}
    
bool ELObj::isNil() const
{
  return false;
}

bool ELObj::isList() const
{
  return false;
}

bool ELObj::isTrue() const
{
  return true;
}

PairObj *ELObj::asPair()
{
  return 0;
}

VectorObj *ELObj::asVector()
{
  return 0;
}

SymbolObj *ELObj::asSymbol()
{
  return 0;
}

KeywordObj *ELObj::asKeyword()
{
  return 0;
}

StringObj *ELObj::convertToString()
{
  return 0;
}

FunctionObj *ELObj::asFunction()
{
  return 0;
}

SosofoObj *ELObj::asSosofo()
{
  return 0;
}

ColorObj *ELObj::asColor()
{
  return 0;
}

ColorSpaceObj *ELObj::asColorSpace()
{
  return 0;
}

AddressObj *ELObj::asAddress()
{
  return 0;
}

DisplaySpaceObj *ELObj::asDisplaySpace()
{
  return 0;
}

InlineSpaceObj *ELObj::asInlineSpace()
{
  return 0;
}

GlyphSubstTableObj *ELObj::asGlyphSubstTable()
{
  return 0;
}

bool ELObj::optSingletonNodeList(EvalContext &, Interpreter &, NodePtr &)
{
  return 0;
}

NodeListObj *ELObj::asNodeList()
{
  return 0;
}

NamedNodeListObj *ELObj::asNamedNodeList()
{
  return 0;
}

StyleObj *ELObj::asStyle()
{
  return 0;
}

BoxObj *ELObj::asBox()
{
  return 0;
}

void ELObj::print(Interpreter &interp, OutputCharStream &out)
{
  out << "#<unknown object " << (unsigned long)this << ">";
}

void ELObj::print(Interpreter &interp, OutputCharStream &out, unsigned)
{
  print(interp, out);
}

bool ELObj::exactIntegerValue(long &)
{
  return false;
}

bool ELObj::realValue(double &)
{
  return false;
}

bool ELObj::inexactRealValue(double &)
{
  return false;
}

bool ELObj::lengthValue(long &n)
{
  return false;
}

ELObj::QuantityType ELObj::quantityValue(long &, double &, int &)
{
  return noQuantity;
}

const LengthSpec *ELObj::lengthSpec() const
{
  return 0;
}

bool ELObj::stringData(const Char *&, size_t &)
{
  return false;
}

bool ELObj::charValue(Char &)
{
  return false;
}

const FOTBuilder::GlyphId *ELObj::glyphId() const
{
  return 0;
}

ELObj *ELObj::resolveQuantities(bool, Interpreter &, const Location &)
{
  return this;
}

ErrorObj::ErrorObj()
{
}

UnspecifiedObj::UnspecifiedObj()
{
}

NilObj::NilObj()
{
}

bool NilObj::isNil() const
{
  return true;
}

bool NilObj::isList() const
{
  return true;
}

void NilObj::print(Interpreter &, OutputCharStream &out)
{
  out << "()";
}

TrueObj::TrueObj()
{
}

void TrueObj::print(Interpreter &, OutputCharStream &out)
{
  out << "#t";
}

FalseObj::FalseObj()
{
}

bool FalseObj::isTrue() const
{
  return false;
}

void FalseObj::print(Interpreter &, OutputCharStream &out)
{
  out << "#f";
}

SymbolObj::SymbolObj(StringObj *name)
: name_(name), cValue_(FOTBuilder::symbolFalse)
{
  hasSubObjects_ = 1;
}

void SymbolObj::traceSubObjects(Collector &c) const
{
  c.trace(name_);
}

void SymbolObj::print(Interpreter &interp, OutputCharStream &out)
{
  out.write(name()->data(), name()->size());
}

SymbolObj *SymbolObj::asSymbol()
{
  return this;
}

StringObj *SymbolObj::convertToString()
{
  return name();
}

KeywordObj::KeywordObj(const Identifier *ident)
: ident_(ident)
{
}

KeywordObj *KeywordObj::asKeyword()
{
  return this;
}

bool KeywordObj::isEqual(ELObj &obj)
{
  KeywordObj *k = obj.asKeyword();
  return k && k->ident_ == ident_;
}

void KeywordObj::print(Interpreter &interp, OutputCharStream &out)
{
  out << ident_->name() << ":";
}

PairObj::PairObj(ELObj *car, ELObj *cdr)
: car_(car), cdr_(cdr)
{
  hasSubObjects_ = 1;
}

bool PairObj::isEqual(ELObj &obj)
{
  PairObj *p = obj.asPair();
  // FIXME need non-recursive implementation
  return p && equal(*p->car(), *car()) && equal(*p->cdr(), *cdr());
}

bool PairObj::isEquiv(ELObj &obj)
{
  PairObj *p = obj.asPair();
  // FIXME need non-recursive implementation
  return p && eqv(*p->car(), *car()) && eqv(*p->cdr(), *cdr());
}

void PairObj::traceSubObjects(Collector &c) const
{
  c.trace(car_);
  c.trace(cdr_);
}

ELObj *PairObj::resolveQuantities(bool force, Interpreter &interp,
				  const Location &loc)
{
  bool fail = 0;
  PairObj *pair = this;
  for (;;) {
    ELObj *tem = pair->car_->resolveQuantities(force, interp, loc);
    if (!tem)
      fail = 1;
    else {
      if (pair->permanent())
	interp.makePermanent(tem);
      pair->car_ = tem;
    }
    PairObj *nextPair = pair->cdr_->asPair();
    if (!nextPair)
      break;
    pair = nextPair;
  }
  ELObj *tem = pair->cdr_->resolveQuantities(force, interp, loc);
  if (!tem)
    fail = 1;
  else {
    if (pair->permanent())
      interp.makePermanent(tem);
    pair->cdr_ = tem;
  }
  if (fail)
    return 0;
  else
    return this;
}


PairObj *PairObj::asPair()
{
  return this;
}

bool PairObj::isList() const
{
  ELObj *p = cdr_;
  while (!p->isNil()) {
    PairObj *pair = p->asPair();
    if (!pair)
      return false;
    p = pair->cdr();
  }
  return true;
}

void PairObj::print(Interpreter &interp, OutputCharStream &out)
{
  out << "(";
  car_->print(interp, out);
  ELObj *p = cdr_;
  for (;;) {
    if (p->isNil()) {
      out << ")";
      return;
    }
    PairObj *pair = p->asPair();
    if (!pair) {
      out << " . ";
      p->print(interp, out);
      out << ")";
      return;
    }
    out << " ";
    pair->car()->print(interp, out);
    p = pair->cdr();
  }
}

VectorObj::VectorObj()
{
  hasSubObjects_ = 1;
}

VectorObj::VectorObj(Vector<ELObj *> &v)
{
  hasSubObjects_ = 1;
  v.swap(*this);
}

void VectorObj::traceSubObjects(Collector &c) const
{
  for (size_t i = 0; i < Vector<ELObj *>::size(); i++)
    c.trace((*this)[i]);
}

VectorObj *VectorObj::asVector()
{
  return this;
}

bool VectorObj::isEquiv(ELObj &)
{
  return 0;
}

bool VectorObj::isEqual(ELObj &obj)
{
  VectorObj *v = obj.asVector();
  if (!v)
    return 0;
  if (size() != v->size())
    return 0;
  for (size_t i = 0; i < size(); i++)
    if (!equal(*(*this)[i], *(*v)[i]))
      return 0;
  return 1;
}

void VectorObj::print(Interpreter &interp, OutputCharStream &out)
{
  out << "#(";
  Vector<ELObj *> &v = *this;
  for (size_t i = 0; i < v.size(); i++) {
    if (i)
      out << " ";
    ELObj *tem = v[i];
    if (!tem)
      out << "#<cycle>";
    else {
      v[i] = 0;
      tem->print(interp, out);
      v[i] = tem;
    }
  }
  out << ")";
}

ELObj *VectorObj::resolveQuantities(bool force, Interpreter &interp,
				    const Location &loc)
{
  bool fail = 0;
  Vector<ELObj *> &v = *this;
  for (size_t i = 0; i < v.size(); i++) {
    ELObj *tem = v[i]->resolveQuantities(force, interp, loc);
    if (tem) {
      if (permanent())
	interp.makePermanent(tem);
      v[i] = tem;
    }
    else
      fail = 1;
  }
  if (fail)
    return 0;
  return this;
}

CharObj::CharObj(Char ch)
: ch_(ch)
{
}

void CharObj::display(Interpreter &interp, OutputCharStream &out) const
{
  out.put(ch_);		// FIXME
}

void CharObj::print(Interpreter &, OutputCharStream &out)
{
  out << "#\\";
  out.put(ch_);	// FIXME
}

bool CharObj::charValue(Char &c)
{
  c = ch_;
  return 1;
}

bool CharObj::isEqual(ELObj &obj)
{
  Char c;
  return obj.charValue(c) && c == ch_;
}

StringObj::StringObj()
{
}

StringObj::StringObj(const StringC &str)
: StringC(str)
{
}

StringObj::StringObj(const Char *s, size_t len)
: StringC(s, len)
{
}

bool StringObj::stringData(const Char *&s, size_t &n)
{
  s = data();
  n = size();
  return true;
}

bool StringObj::isEqual(ELObj &obj)
{
  const Char *s;
  size_t n;
  return (obj.stringData(s, n) 
	  && n == size()
	  && (n == 0 || memcmp(s, data(), n*sizeof(Char)) == 0));
}

StringObj *StringObj::convertToString()
{
  return this;
}

void StringObj::print(Interpreter &interp, OutputCharStream &out)
{
  // FIXME
  out << "\"";
  const Char *s = data();
  for (size_t i = 0; i < size(); i++)
    switch (s[i]) {
    case '\\':
    case '"':
      out << "\\";
      // fall through
    default:
      out.put(s[i]);
      break;
    }
  out << "\"";
}

IntegerObj::IntegerObj()
: n_(0)
{
}

IntegerObj::IntegerObj(long n)
: n_(n)
{
}

bool IntegerObj::isEqual(ELObj &obj)
{
  long n;
  return obj.exactIntegerValue(n) && n == n_;
}

void IntegerObj::print(Interpreter &interp, OutputCharStream &out)
{
  print(interp, out, 10);
}

void IntegerObj::print(Interpreter &, OutputCharStream &out, unsigned radix)
{
  if (radix == 10) {
    if (n_ < 0)
      out << '-' << (unsigned long)-n_;
    else
      out << (unsigned long)n_;
    return;
  }

  if (n_ == 0) {
    out << '0';
    return;
  }
  
  unsigned long n;

  if (n_ < 0) {
    out << '-';
    n = -n_;
  }
  else
    n = n_;
  
  char buf[64];
  int i = 0;

  while (n != 0) {
    buf[i++] = "0123456789abcdef"[n % radix];
    n /= radix;
  }

  while (i > 0)
    out << buf[--i];
}

bool IntegerObj::exactIntegerValue(long &n)
{
  n = n_;
  return true;
}

bool IntegerObj::realValue(double &n)
{
  n = n_;
  return true;
}

ELObj::QuantityType IntegerObj::quantityValue(long &val, double &, int &dim)
{
  val = n_;
  dim = 0;
  return longQuantity;
}

RealObj::RealObj(double n)
: n_(n)
{
}

bool RealObj::isEqual(ELObj &obj)
{
  double n;
  return obj.inexactRealValue(n) && n == n_;
}


bool RealObj::realValue(double &n)
{
  n = n_;
  return true;
}

bool RealObj::inexactRealValue(double &n)
{
  n = n_;
  return true;
}

ELObj::QuantityType RealObj::quantityValue(long &, double &val, int &dim)
{
  val = n_;
  dim = 0;
  return doubleQuantity;
}

void RealObj::print(Interpreter &, OutputCharStream &out)
{
  char buf[1024];
  sprintf(buf, "%g", n_);
  out << buf;
}

LengthObj::LengthObj(long n)
: n_(n)
{
}

bool LengthObj::lengthValue(long &n)
{
  n = n_;
  return true;
}

ELObj::QuantityType LengthObj::quantityValue(long &val, double &, int &dim)
{
  val = n_;
  dim = 1;
  return longQuantity;
}

bool LengthObj::isEqual(ELObj &obj)
{
  long n;
  double d;
  int dim;
  switch (obj.quantityValue(n, d, dim)) {
  case noQuantity:
    break;
  case doubleQuantity:
    return dim == 1 && d == n_;
  case longQuantity:
    return dim == 1 && n == n_;
  }
  return 0;
}

void LengthObj::print(Interpreter &interp, OutputCharStream &out)
{
  char buf[1024];
  sprintf(buf, "%gpt", n_ * 72.0/interp.unitsPerInch());
  out << buf;
}

QuantityObj::QuantityObj(double val, int dim)
: val_(val), dim_(dim)
{
}

bool QuantityObj::isEqual(ELObj &obj)
{
  long n;
  double d;
  int dim;
  switch (obj.quantityValue(n, d, dim)) {
  case noQuantity:
    break;
  case doubleQuantity:
    return dim == dim_ && d == val_;
  case longQuantity:
    return dim == dim_ && n == val_;
  }
  return 0;
}

ELObj::QuantityType QuantityObj::quantityValue(long &, double &val, int &dim)
{
  val = val_;
  dim = dim_;
  return doubleQuantity;
}

void QuantityObj::print(Interpreter &interp, OutputCharStream &out)
{
  char buf[1024];
  sprintf(buf, "%gpt%d", val_ * pow(72.0/interp.unitsPerInch(), double(dim_)),
	  dim_);
  out << buf;
}

bool QuantityObj::realValue(double &d)
{
  if (dim_ != 0)
    return 0;
  d = val_;
  return 1;
}

bool QuantityObj::inexactRealValue(double &d)
{
  if (dim_ != 0)
    return 0;
  d = val_;
  return 1;
}

LengthSpecObj::LengthSpecObj(const LengthSpec &spec)
: lengthSpec_(new LengthSpec(spec))
{
}

const LengthSpec *LengthSpecObj::lengthSpec() const
{
  return lengthSpec_.pointer();
}

LengthSpec::LengthSpec()
{
  for (int i = 0; i < nVals; i++)
    val_[i] = 0.0;
}

LengthSpec::LengthSpec(double d)
{
  val_[0] = d;
  for (int i = 1; i < nVals; i++)
    val_[i] = 0.0;
}
 
LengthSpec::LengthSpec(Unknown unknown, double d)
{
  int i;
  for (i = 0; i < unknown; i++)
    val_[i] = 0.0;
  val_[unknown] = d;
  for (i = unknown + 1; i < nVals; i++)
    val_[i] = 0.0;
}

void LengthSpec::operator+=(const LengthSpec &ls)
{
  for (int i = 0; i < nVals; i++)
    val_[i] += ls.val_[i];
}

void LengthSpec::operator-=(const LengthSpec &ls)
{
  for (int i = 0; i < nVals; i++)
    val_[i] -= ls.val_[i];
}

void LengthSpec::operator*=(double d)
{
  for (int i = 0; i < nVals; i++)
    val_[i] *= d;
}

void LengthSpec::operator/=(double d)
{
  for (int i = 0; i < nVals; i++)
    val_[i] /= d;
}

bool LengthSpec::convert(FOTBuilder::LengthSpec &result) const
{
  // FIXME do some checking
  result.length = long(val_[0] < 0.0 ? val_[0] - 0.5 : val_[0] + .5);
  result.displaySizeFactor = val_[1];
  return 1;
}

bool LengthSpec::convert(FOTBuilder::TableLengthSpec &result) const
{
  // FIXME do some checking
  result.length = long(val_[0] < 0.0 ? val_[0] - 0.5 : val_[0] + .5);
  result.displaySizeFactor = val_[1];
  result.tableUnitFactor = val_[2];
  return 1;
}

DisplaySpaceObj::DisplaySpaceObj(const FOTBuilder::DisplaySpace &displaySpace)
: displaySpace_(new FOTBuilder::DisplaySpace(displaySpace))
{
}
 
const FOTBuilder::DisplaySpace &DisplaySpaceObj::displaySpace() const
{
  return *displaySpace_;
}

DisplaySpaceObj *DisplaySpaceObj::asDisplaySpace()
{
  return this;
}

InlineSpaceObj::InlineSpaceObj(const FOTBuilder::InlineSpace &inlineSpace)
: inlineSpace_(new FOTBuilder::InlineSpace(inlineSpace))
{
}
 
const FOTBuilder::InlineSpace &InlineSpaceObj::inlineSpace() const
{
  return *inlineSpace_;
}

InlineSpaceObj *InlineSpaceObj::asInlineSpace()
{
  return this;
}

UnresolvedLengthObj::UnresolvedLengthObj(long val, int valExp, Unit *unit)
: val_(val), valExp_(valExp), unit_(unit)
{
}

ELObj *UnresolvedLengthObj::resolveQuantities(bool force,
					      Interpreter &interp,
					      const Location &loc)
{
  unsigned part;
  Location defLoc;
  if (!unit_->defined(part, defLoc)) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::undefinedQuantity,
		   StringMessageArg(unit_->name()));
    return interp.makeError();
  }
  return unit_->resolveQuantity(force, interp, val_, valExp_);
}

UnresolvedQuantityObj
::UnresolvedQuantityObj(double val, Unit *unit, int unitExp)
: val_(val), unit_(unit), unitExp_(unitExp)
{
}

ELObj *UnresolvedQuantityObj::resolveQuantities(bool force,
						Interpreter &interp,
						const Location &loc)
{
  unsigned part;
  Location defLoc;
  if (!unit_->defined(part, defLoc)) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::undefinedQuantity,
		   StringMessageArg(unit_->name()));
    return interp.makeError();
  }
  return unit_->resolveQuantity(force, interp, val_, unitExp_);
}

GlyphIdObj::GlyphIdObj(const FOTBuilder::GlyphId &glyphId)
: glyphId_(glyphId)
{
}

const FOTBuilder::GlyphId *GlyphIdObj::glyphId() const
{
  return &glyphId_;
}

bool GlyphIdObj::isEqual(ELObj &obj)
{
  const FOTBuilder::GlyphId *p = obj.glyphId();
  return (p
          && p->publicId == glyphId_.publicId
	  && p->suffix == glyphId_.suffix);
}

GlyphSubstTableObj::GlyphSubstTableObj(const ConstPtr<FOTBuilder::GlyphSubstTable> &table)
: table_(table)
{
}

GlyphSubstTableObj *GlyphSubstTableObj::asGlyphSubstTable()
{
  return this;
}

AddressObj::AddressObj(FOTBuilder::Address::Type type,
		       const NodePtr &node,
		       const StringC &str1,
		       const StringC &str2,
		       const StringC &str3)
{
  address_ = new FOTBuilder::Address;
  address_->type = type;
  address_->node = node;
  address_->params[0] = str1;
  address_->params[1] = str2;
  address_->params[2] = str3;
}

AddressObj *AddressObj::asAddress()
{
  return this;
}

NodeListObj *NodeListObj::asNodeList()
{
  return this;
}

NodeListObj *NodeListObj::nodeListNoOrder(Collector &)
{
  return this;
}

NodeListObj *NodeListObj::nodeListChunkRest(EvalContext &context, Interpreter &interp, bool &chunk)
{
  chunk = 0;
  return nodeListRest(context, interp);
}

bool NodeListObj::optSingletonNodeList(EvalContext &context, Interpreter &interp, NodePtr &node)
{
  NodeListObj *rest = nodeListRest(context, interp);
  ELObjDynamicRoot protect(interp, rest);
  if (rest->nodeListFirst(context, interp))
    return 0;
  node = nodeListFirst(context, interp);
  return 1;
}

NodePtr NodeListObj::nodeListRef(long n, EvalContext &context, Interpreter &interp)
{
  if (n < 0)
    return NodePtr();
  NodeListObj *nl = this;
  ELObjDynamicRoot protect(interp, nl);
  while (n > 0) {
    NodePtr nd(nl->nodeListFirst(context, interp));
    if (!nd)
      return NodePtr();
    GroveString str;
    if (nd->charChunk(interp, str) == accessOK && str.size() <= n) {
      bool chunk;
      nl = nl->nodeListChunkRest(context, interp, chunk);
      if (chunk)
	n -= str.size();
      else
	n--;
    }
    else {
      nl = nl->nodeListRest(context, interp);
      n--;
    }
    protect = nl;
  }
  return nl->nodeListFirst(context, interp);
}

NodeListObj *NodeListObj::nodeListReverse(EvalContext &, Interpreter &interp)
{
  return new (interp) ReverseNodeListObj(this);
}

long NodeListObj::nodeListLength(EvalContext &context, Interpreter &interp)
{
  NodeListObj *nl = this;
  long n = 0;
  ELObjDynamicRoot protect(interp, nl);
  for (;;) {
    NodePtr nd(nl->nodeListFirst(context, interp));
    if (!nd)
      break;
    bool chunk;
    nl = nl->nodeListChunkRest(context, interp, chunk);
    protect = nl;
    GroveString str;
    if (chunk && nd->charChunk(interp, str) == accessOK)
      n += str.size();
    else
      n += 1;
  }
  return n;
}

bool NodeListObj::suppressError()
{
  return 0;
}

NamedNodeListObj *NamedNodeListObj::asNamedNodeList()
{
  return this;
}

NodePtrNodeListObj::NodePtrNodeListObj()
{
}

NodePtrNodeListObj::NodePtrNodeListObj(const NodePtr &node)
: node_(node)
{
}

bool NodePtrNodeListObj::optSingletonNodeList(EvalContext &, Interpreter &, NodePtr &node)
{
  node = node_;
  return 1;
}

NodePtr NodePtrNodeListObj::nodeListFirst(EvalContext &, Interpreter &)
{
  return node_;
}

NodeListObj *NodePtrNodeListObj::nodeListRest(EvalContext &context, Interpreter &interp)
{
  if (node_)
    return new (interp) NodePtrNodeListObj;
  else
    return this;
}

NodeListPtrNodeListObj::NodeListPtrNodeListObj(const NodeListPtr &nodeList)
: nodeList_(nodeList)
{
}

NodePtr NodeListPtrNodeListObj::nodeListFirst(EvalContext &, Interpreter &)
{
  NodePtr nd;
  if (nodeList_->first(nd) == accessOK)
    return nd;
  else
    return NodePtr();
}

NodeListObj *NodeListPtrNodeListObj::nodeListRest(EvalContext &context, Interpreter &interp)
{
  NodeListPtr nl;
  if (nodeList_->rest(nl) == accessOK)
    return new (interp) NodeListPtrNodeListObj(nl);
  else
    return interp.makeEmptyNodeList();
}

NodeListObj *NodeListPtrNodeListObj::nodeListChunkRest(EvalContext &context, Interpreter &interp, bool &chunk)
{
  NodeListPtr nl;
  if (nodeList_->chunkRest(nl) == accessOK) {
    chunk = 1;
    return new (interp) NodeListPtrNodeListObj(nl);
  }
  else {
    chunk = 0;
    return interp.makeEmptyNodeList();
  }
}

NodePtr NodeListPtrNodeListObj::nodeListRef(long i, EvalContext &, Interpreter &interp)
{
  NodePtr nd;
  if (i < 0 || nodeList_->ref(i, nd) != accessOK)
    return NodePtr();
  return nd;
}

NamedNodeListPtrNodeListObj
::NamedNodeListPtrNodeListObj(const NamedNodeListPtr &nnl)
: namedNodeList_(nnl)
{
}

NodePtr NamedNodeListPtrNodeListObj::namedNode(const Char *s, size_t n)
{
  NodePtr node;
  if (namedNodeList_->namedNode(GroveString(s, n), node) != accessOK)
    node.clear();
  return node;
}

bool NamedNodeListPtrNodeListObj::nodeName(const NodePtr &nd, GroveString &str)
{
  return namedNodeList_->nodeName(nd, str) == accessOK;
}

size_t NamedNodeListPtrNodeListObj::normalize(Char *s, size_t n)
{
  return namedNodeList_->normalize(s, n);
}

NodePtr NamedNodeListPtrNodeListObj::nodeListFirst(EvalContext &, Interpreter &)
{
  if (!nodeList_)
    nodeList_ = namedNodeList_->nodeList();
  NodePtr nd;
  if (nodeList_->first(nd) == accessOK)
    return nd;
  else
    return NodePtr();
}
 
NodeListObj *NamedNodeListPtrNodeListObj::nodeListRest(EvalContext &context, Interpreter &interp)
{
  if (!nodeList_)
    nodeList_ = namedNodeList_->nodeList();
  NodeListPtr nl;
  if (nodeList_->rest(nl) == accessOK)
    return new (interp) NodeListPtrNodeListObj(nl);
  else
    return new (interp) NodePtrNodeListObj;
}

NodeListObj *NamedNodeListPtrNodeListObj::nodeListNoOrder(Collector &c)
{
  return new (c) NodeListPtrNodeListObj(namedNodeList_->nodeListNoOrder());
}

PairNodeListObj::PairNodeListObj(NodeListObj *head, NodeListObj *tail)
: head_(head), tail_(tail)
{
  hasSubObjects_ = 1;
}

NodePtr PairNodeListObj::nodeListFirst(EvalContext &context, Interpreter &interp)
{
  if (head_) {
    NodePtr nd(head_->nodeListFirst(context, interp));
    if (nd)
      return nd;
    head_ = 0;
  }
  return tail_->nodeListFirst(context, interp);
}

NodeListObj *PairNodeListObj::nodeListRest(EvalContext &context, Interpreter &interp)
{
  if (!head_ || !head_->nodeListFirst(context, interp))
    return tail_->nodeListRest(context, interp);
  NodeListObj *tem = head_->nodeListRest(context, interp);
  ELObjDynamicRoot protect(interp, tem);
  return new (interp) PairNodeListObj(tem, tail_);
}

NodeListObj *PairNodeListObj::nodeListChunkRest(EvalContext &context, Interpreter &interp, bool &chunk)
{
  if (!head_ || !head_->nodeListFirst(context, interp))
    return tail_->nodeListChunkRest(context, interp, chunk);
  NodeListObj *tem = head_->nodeListChunkRest(context, interp, chunk);
  ELObjDynamicRoot protect(interp, tem);
  return new (interp) PairNodeListObj(tem, tail_);
}

void PairNodeListObj::traceSubObjects(Collector &c) const
{
  c.trace(head_);
  c.trace(tail_);
}

ReverseNodeListObj::ReverseNodeListObj(NodeListObj *nl)
: nl_(nl), reversed_(0)
{
  hasSubObjects_ = 1;
}

NodePtr ReverseNodeListObj::nodeListFirst(EvalContext &context, Interpreter &interp)
{
  return reversed(context, interp)->nodeListFirst(context, interp);
}

NodeListObj *ReverseNodeListObj::nodeListRest(EvalContext &context, Interpreter &interp)
{
  return reversed(context, interp)->nodeListRest(context, interp);
}

NodePtr ReverseNodeListObj::nodeListRef(long n, EvalContext &context, Interpreter &interp)
{
  if (reversed_)
    return reversed_->nodeListRef(n, context, interp);
  if (n < 0)
    return NodePtr();
  size_t len = nl_->nodeListLength(context, interp);
  if (n >= len)
    return 0;
  return nl_->nodeListRef(len - n - 1, context, interp);
}

NodeListObj *ReverseNodeListObj::reversed(EvalContext &context, Interpreter &interp)
{
  if (!reversed_) {
    NodePtr nd(nl_->nodeListFirst(context, interp));
    if (nd) {
      reversed_ = new (interp) NodePtrNodeListObj(nd);
      NodeListObj *tem = nl_;
      ELObjDynamicRoot protect(interp, tem);
      ELObjDynamicRoot protect2(interp, reversed_);
      for (;;) {
	tem = tem->nodeListRest(context, interp);
	protect = tem;
	nd = tem->nodeListFirst(context, interp);
	if (!nd)
	  break;
	NodeListObj *single = new (interp) NodePtrNodeListObj(nd);
	protect2 = single;
	reversed_ = new (interp) PairNodeListObj(single, reversed_);
      }
    }
    else
      reversed_ = nl_;
  }
  return reversed_;
}

NodeListObj *ReverseNodeListObj::nodeListReverse(EvalContext &, Interpreter &)
{
  return nl_;
}

long ReverseNodeListObj::nodeListLength(EvalContext &context, Interpreter &interp)
{
  return nl_->nodeListLength(context, interp);
}

void ReverseNodeListObj::traceSubObjects(Collector &c) const
{
  c.trace(nl_);
  c.trace(reversed_);
}

#ifdef DSSSL_NAMESPACE
}
#endif
