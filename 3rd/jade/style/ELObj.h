// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef ELObj_INCLUDED
#define ELObj_INCLUDED 1

#include "types.h"
#include "Collector.h"
#include "OutputCharStream.h"
#include "Boolean.h"
#include "Location.h"
#include "Node.h"
#include "FOTBuilder.h"
#include <string.h>

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class Interpreter;
class EvalContext;

class Unit;
class PairObj;
class FunctionObj;
class SymbolObj;
class KeywordObj;
class BoxObj;
class StringObj;
class SosofoObj;
class ColorObj;
class ColorSpaceObj;
class StyleObj;
class AddressObj;
class NodeListObj;
class NamedNodeListObj;
class Identifier;
class LengthSpec;
class DisplaySpaceObj;
class InlineSpaceObj;
class GlyphSubstTableObj;
class VectorObj;

class ELObj : public Collector::Object {
public:
  ELObj();
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(0);
  }
#ifdef SP_HAVE_PLACEMENT_OPERATOR_DELETE
private:
  void operator delete(void *p) { }
public:
  void operator delete(void *p, Collector &c) {
    c.unallocateObject(p);
  }
#endif
  virtual bool isNil() const;
  virtual bool isList() const;
  virtual bool isTrue() const;
  virtual PairObj *asPair();
  virtual SymbolObj *asSymbol();
  virtual KeywordObj *asKeyword();
  virtual FunctionObj *asFunction();
  virtual SosofoObj *asSosofo();
  virtual ColorObj *asColor();
  virtual ColorSpaceObj *asColorSpace();
  virtual StyleObj *asStyle();
  virtual AddressObj *asAddress();
  virtual DisplaySpaceObj *asDisplaySpace();
  virtual InlineSpaceObj *asInlineSpace();
  virtual GlyphSubstTableObj *asGlyphSubstTable();
  virtual NodeListObj *asNodeList();
  virtual NamedNodeListObj *asNamedNodeList();
  virtual StringObj *convertToString();	// either symbol or string
  virtual BoxObj *asBox();
  virtual VectorObj *asVector();
  virtual bool charValue(Char &);
  virtual bool stringData(const Char *&, size_t &);
  virtual void print(Interpreter &, OutputCharStream &);
  virtual void print(Interpreter &, OutputCharStream &, unsigned);
  virtual bool exactIntegerValue(long &);
  virtual bool realValue(double &);
  virtual bool inexactRealValue(double &);
  virtual bool lengthValue(long &);
  virtual const FOTBuilder::GlyphId *glyphId() const;
  virtual const LengthSpec *lengthSpec() const;
  enum QuantityType {
    noQuantity,
    longQuantity,
    doubleQuantity
    };
  virtual QuantityType quantityValue(long &, double &, int &);
  virtual ELObj *resolveQuantities(bool force, Interpreter &,
				   const Location &);
  virtual bool optSingletonNodeList(EvalContext &, Interpreter &, NodePtr &);
  static bool equal(ELObj &, ELObj &);
  // Note that in DSSSL2 unlike Scheme eqv? for lists and strings is the same
  // as equal?.  Only for vectors do they differ.
  static bool eqv(ELObj &, ELObj &);
protected:
  // This checks for equality of *distinct* objects.
  virtual bool isEqual(ELObj &);
  // This checks for equivalence of *distinct* objects.
  virtual bool isEquiv(ELObj &);
};

class ErrorObj : public ELObj {
public:
private:
  ErrorObj();
  friend class Interpreter;
};

class UnspecifiedObj : public ELObj {
public:
private:
  UnspecifiedObj();
  friend class Interpreter;
};

class NilObj : public ELObj {
public:
  bool isNil() const;
  bool isList() const;
  void print(Interpreter &, OutputCharStream &);
private:
  NilObj();
  friend class Interpreter;
};

class TrueObj : public ELObj {
public:
  void print(Interpreter &, OutputCharStream &);
private:
  TrueObj();
  friend class Interpreter;
};

class FalseObj : public ELObj {
public:
  void print(Interpreter &, OutputCharStream &);
  bool isTrue() const;
private:
  FalseObj();
  friend class Interpreter;
};

class StringObj;
class CharObj;

class SymbolObj : public ELObj {
public:
  void print(Interpreter &, OutputCharStream &);
  SymbolObj *asSymbol();
  StringObj *convertToString();
  StringObj *name() const;
  void traceSubObjects(Collector &) const;
  FOTBuilder::Symbol cValue() const;
  void setCValue(FOTBuilder::Symbol);
  static const StringC &key(const SymbolObj &);
private:
  SymbolObj(StringObj *);
  StringObj *name_;
  FOTBuilder::Symbol cValue_;
  friend class Interpreter;
};

class Identifier;

class KeywordObj : public ELObj {
public:
  void print(Interpreter &, OutputCharStream &);
  KeywordObj *asKeyword();
  const Identifier *identifier() const;
  bool isEqual(ELObj &);
private:
  KeywordObj(const Identifier *);
  const Identifier *ident_;
friend class Interpreter;
};

class PairObj : public ELObj {
public:
  PairObj(ELObj *car, ELObj *cdr);
  ELObj *car();
  ELObj *cdr();
  void setCar(ELObj *);
  void setCdr(ELObj *);
  PairObj *asPair();
  bool isList() const;
  void print(Interpreter &, OutputCharStream &);
  void traceSubObjects(Collector &) const;
  ELObj *resolveQuantities(bool force, Interpreter &, const Location &);
  bool isEqual(ELObj &);
  bool isEquiv(ELObj &);
private:
  ELObj *car_;
  ELObj *cdr_;
};

class VectorObj : public ELObj, public Vector<ELObj *> {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  VectorObj();
  VectorObj(Vector<ELObj *> &v);
  void traceSubObjects(Collector &) const;
  VectorObj *asVector();
  bool isEqual(ELObj &);
  bool isEquiv(ELObj &);
  void print(Interpreter &, OutputCharStream &);
  ELObj *resolveQuantities(bool force, Interpreter &, const Location &);
};

class CharObj : public ELObj {
public:
  Char ch();
  void print(Interpreter &, OutputCharStream &);
  void display(Interpreter &, OutputCharStream &) const;
  bool isEqual(ELObj &);
  bool charValue(Char &);
private:
  CharObj(Char);
  Char ch_;
  friend class Interpreter;
};

class StringObj : public ELObj, public StringC {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  StringObj();
  StringObj(const StringC &str);
  StringObj(const Char *, size_t);
  void print(Interpreter &, OutputCharStream &);
  StringObj *convertToString();
  bool stringData(const Char *&, size_t &);
  bool isEqual(ELObj &);
};

class IntegerObj : public ELObj {
public:
  void print(Interpreter &, OutputCharStream &);
  void print(Interpreter &, OutputCharStream &, unsigned);
  bool exactIntegerValue(long &);
  bool realValue(double &);
  QuantityType quantityValue(long &, double &, int &);
  bool isEqual(ELObj &);
private:
  friend class Interpreter;
  IntegerObj();
  IntegerObj(long);
  long n_;
};

class RealObj : public ELObj {
public:
  RealObj(double);
  bool realValue(double &);
  bool inexactRealValue(double &);
  QuantityType quantityValue(long &, double &, int &);
  bool isEqual(ELObj &);
  void print(Interpreter &, OutputCharStream &);
private:
  double n_;
};

class LengthObj : public ELObj {
public:
  LengthObj(long units);
  bool lengthValue(long &);
  QuantityType quantityValue(long &, double &, int &);
  void print(Interpreter &, OutputCharStream &);
  bool isEqual(ELObj &);
private:
  long n_;
};

class QuantityObj : public ELObj {
public:
  QuantityObj(double, int);
  QuantityType quantityValue(long &, double &, int &);
  void print(Interpreter &, OutputCharStream &);
  bool isEqual(ELObj &);
  bool realValue(double &);
  bool inexactRealValue(double &);
private:
  // This is in units per inch.
  double val_;
  int dim_;
};

class LengthSpec {
public:
  enum Unknown { displaySize = 1, tableUnit };
  LengthSpec();
  LengthSpec(double);
  LengthSpec(Unknown, double);
  void operator+=(const LengthSpec &);
  void operator+=(double d) { val_[0] += d; }
  void operator-=(const LengthSpec &);
  void operator-=(double d) { val_[0] -= d; }
  void operator*=(double);
  void operator/=(double);
  bool convert(FOTBuilder::LengthSpec &) const;
  bool convert(FOTBuilder::TableLengthSpec &) const;
private:
  enum { nVals = 3 };
  double val_[nVals];
};

class LengthSpecObj : public ELObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  LengthSpecObj(const LengthSpec &);
  const LengthSpec *lengthSpec() const;
private:
  Owner<LengthSpec> lengthSpec_;
};

class DisplaySpaceObj : public ELObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  DisplaySpaceObj(const FOTBuilder::DisplaySpace &);
  const FOTBuilder::DisplaySpace &displaySpace() const;
  DisplaySpaceObj *asDisplaySpace();
private:
  Owner<FOTBuilder::DisplaySpace> displaySpace_;
};

class InlineSpaceObj : public ELObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  InlineSpaceObj(const FOTBuilder::InlineSpace &);
  const FOTBuilder::InlineSpace &inlineSpace() const;
  InlineSpaceObj *asInlineSpace();
private:
  Owner<FOTBuilder::InlineSpace> inlineSpace_;
};

class UnresolvedQuantityObj : public ELObj {
public:
  UnresolvedQuantityObj(double, Unit *, int);
  ELObj *resolveQuantities(bool force, Interpreter &, const Location &);
private:
  double val_;
  Unit *unit_;
  int unitExp_;
};

class UnresolvedLengthObj : public ELObj {
public:
  UnresolvedLengthObj(long val, int valExp, Unit *);
  ELObj *resolveQuantities(bool force, Interpreter &, const Location &);
private:
  long val_;
  Unit *unit_;
  int valExp_;
};

class GlyphIdObj : public ELObj {
public:
  GlyphIdObj(const FOTBuilder::GlyphId &);
  const FOTBuilder::GlyphId *glyphId() const;
  bool isEqual(ELObj &);
private:
  FOTBuilder::GlyphId glyphId_;
};

class GlyphSubstTableObj : public ELObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  GlyphSubstTableObj(const ConstPtr<FOTBuilder::GlyphSubstTable> &);
  GlyphSubstTableObj *asGlyphSubstTable();
  const ConstPtr<FOTBuilder::GlyphSubstTable> &glyphSubstTable() const;
private:
  ConstPtr<FOTBuilder::GlyphSubstTable> table_;
};

class AddressObj : public ELObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  AddressObj(FOTBuilder::Address::Type,
             const NodePtr & = NodePtr(),
	     const StringC & = StringC(),
	     const StringC & = StringC(),
	     const StringC & = StringC());
  AddressObj *asAddress();
  const FOTBuilder::Address &address() const;
private:
  Owner<FOTBuilder::Address> address_;
};

class NodeListObj : public ELObj {
public:
  NodeListObj *asNodeList();
  bool optSingletonNodeList(EvalContext &, Interpreter &, NodePtr &);
  virtual NodePtr nodeListFirst(EvalContext &, Interpreter &) = 0;
  virtual NodeListObj *nodeListRest(EvalContext &, Interpreter &) = 0;
  virtual NodeListObj *nodeListNoOrder(Collector &);
  // If this node contains all the node in the chunk of the first node, then
  // this *may* return the node list containing the nodes following the chunk;
  // If so chunk, will be set to 1.  Otherwise return nodeListRest() and set
  // chunk to 0.  If charChunk() returns a string of length n, then
  // the chunk of the node is the node together with the n - 1 following siblings;
  // otherwise the chunk of the node is the node itself.
  virtual NodeListObj *nodeListChunkRest(EvalContext &, Interpreter &, bool &chunk);
  virtual NodePtr nodeListRef(long, EvalContext &, Interpreter &);
  virtual NodeListObj *nodeListReverse(EvalContext &, Interpreter &);
  virtual long nodeListLength(EvalContext &, Interpreter &);
  virtual bool suppressError();
};

class NamedNodeListObj : public NodeListObj {
public:
  NamedNodeListObj *asNamedNodeList();
  virtual NodePtr namedNode(const Char *, size_t) = 0;
  virtual bool nodeName(const NodePtr &, GroveString &) = 0;
  virtual size_t normalize(Char *, size_t) = 0;
};

class NodePtrNodeListObj : public NodeListObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  NodePtrNodeListObj();
  NodePtrNodeListObj(const NodePtr &);
  NodePtr nodeListFirst(EvalContext &, Interpreter &);
  NodeListObj *nodeListRest(EvalContext &, Interpreter &);
  bool optSingletonNodeList(EvalContext &, Interpreter &, NodePtr &);
  bool chunkComplete();
private:
  // Null for no node.
  NodePtr node_;
};

class NodeListPtrNodeListObj : public NodeListObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  NodeListPtrNodeListObj(const NodeListPtr &);
  NodePtr nodeListFirst(EvalContext &, Interpreter &);
  NodeListObj *nodeListRest(EvalContext &, Interpreter &);
  NodeListObj *nodeListChunkRest(EvalContext &, Interpreter &, bool &);
  NodePtr nodeListRef(long, EvalContext &, Interpreter &);
private:
  // never null
  NodeListPtr nodeList_;
};

class NamedNodeListPtrNodeListObj : public NamedNodeListObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  NamedNodeListPtrNodeListObj(const NamedNodeListPtr &);
  NodePtr nodeListFirst(EvalContext &, Interpreter &);
  NodeListObj *nodeListRest(EvalContext &, Interpreter &);
  NodePtr namedNode(const Char *, size_t);
  bool nodeName(const NodePtr &, GroveString &);
  size_t normalize(Char *, size_t);
  NodeListObj *nodeListNoOrder(Collector &);
private:
  NamedNodeListPtr namedNodeList_;
  // cached node list, null if not yet computed
  NodeListPtr nodeList_;
};

class PairNodeListObj : public NodeListObj {
public:
  PairNodeListObj(NodeListObj *, NodeListObj *);
  NodePtr nodeListFirst(EvalContext &, Interpreter &);
  NodeListObj *nodeListRest(EvalContext &, Interpreter &);
  NodeListObj *nodeListChunkRest(EvalContext &, Interpreter &, bool &);
  void traceSubObjects(Collector &) const;
private:
  NodeListObj *head_; // may be null
  NodeListObj *tail_;
};

inline
bool ELObj::equal(ELObj &obj1, ELObj &obj2)
{
  return &obj1 == &obj2 || obj1.isEqual(obj2);
}
    
inline
bool ELObj::eqv(ELObj &obj1, ELObj &obj2)
{
  return &obj1 == &obj2 || obj1.isEquiv(obj2);
}

inline
ELObj *PairObj::car()
{
  return car_;
}

inline
ELObj *PairObj::cdr()
{
  return cdr_;
}

inline
void PairObj::setCar(ELObj *car)
{
  car_ = car;
}

inline
void PairObj::setCdr(ELObj *cdr)
{
  cdr_ = cdr;
}

inline
StringObj *SymbolObj::name() const
{
  return name_;
}

inline
FOTBuilder::Symbol SymbolObj::cValue() const
{
  return cValue_;
}

inline
void SymbolObj::setCValue(FOTBuilder::Symbol sym)
{
  cValue_ = sym;
}

inline
Char CharObj::ch()
{
  return ch_;
}

inline
const StringC &SymbolObj::key(const SymbolObj &sym)
{
  return *sym.name();
}

inline
const Identifier *KeywordObj::identifier() const
{
  return ident_;
}

inline
const FOTBuilder::Address &AddressObj::address() const
{
  return *address_;
}

inline
const ConstPtr<FOTBuilder::GlyphSubstTable> &
GlyphSubstTableObj::glyphSubstTable() const
{
  return table_;
}

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not ELObj_INCLUDED */
