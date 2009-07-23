// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef Style_INCLUDED
#define Style_INCLUDED 1

#include "types.h"
#include "StringC.h"
#include "Named.h"
#include <stddef.h>
#include "ELObj.h"
#include "Vector.h"
#include "Ptr.h"
#include "Resource.h"
#include "Owner.h"
#include "Insn.h"
#include "FOTBuilder.h"
#include "Boolean.h"
#include "Node.h"
#include "Location.h"
#include "ProcessingMode.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class StyleStack;
class Interpreter;
class VarStyleObj;

// InheritedC represents the specification of a value
// of an inherited characteristic.
// It is also used as a prototype for other specifications of
// values for that characteristic:
// the identifier for an inherited characteristic contains the
// InheritedC that specifies its initial value.

class InheritedC : public Resource {
public:
  InheritedC(const Identifier *ident, unsigned index);
  virtual ~InheritedC();
  virtual void set(VM &vm, const VarStyleObj *, FOTBuilder &,
                   ELObj *&, Vector<size_t> &dependencies) const = 0;
  virtual ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const = 0;
  virtual ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const = 0;
  unsigned index() const;
  const Identifier *identifier() const;
  void setIdentifier(const Identifier *);
protected:
  void invalidValue(const Location &, Interpreter &) const;
private:
  const Identifier *ident_;
  // The index of the inherited characteristic (not the specification).
  unsigned index_;
};

class VarInheritedC : public InheritedC {
public:
  VarInheritedC(const ConstPtr<InheritedC> &, const InsnPtr &code, const Location &);
  void set(VM &, const VarStyleObj *, FOTBuilder &, ELObj *&value,
           Vector<size_t> &dependencies) const;
  ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const;
  virtual ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
private:
  ConstPtr<InheritedC> inheritedC_;
  InsnPtr code_;
  Location loc_;
};

class StyleObjIter {
public:
  StyleObjIter();
  void append(const Vector<ConstPtr<InheritedC> > *, const VarStyleObj *);
  ConstPtr<InheritedC> next(const VarStyleObj *&);
private:
  size_t i_;
  size_t vi_;
  Vector<const VarStyleObj *> styleVec_;
  Vector<const Vector<ConstPtr<InheritedC> > *> vecs_;
};

class StyleObj : public ELObj {
public:
  StyleObj *asStyle();
  virtual void appendIter(StyleObjIter &) const = 0;
};

struct StyleSpec : public Resource {
  StyleSpec(Vector<ConstPtr<InheritedC> > &, Vector<ConstPtr<InheritedC> > &);
  Vector<ConstPtr<InheritedC> > forceSpecs;
  Vector<ConstPtr<InheritedC> > specs;
};

class BasicStyleObj : public StyleObj {
public:
  virtual void appendIterForce(StyleObjIter &) const = 0;
  virtual void appendIterNormal(StyleObjIter &) const = 0;
};

class VarStyleObj : public BasicStyleObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  VarStyleObj(const ConstPtr<StyleSpec> &, StyleObj *use, ELObj **display,
              const NodePtr &node);
  ~VarStyleObj();
  void appendIter(StyleObjIter &) const;
  void appendIterForce(StyleObjIter &) const;
  void appendIterNormal(StyleObjIter &) const;
  const NodePtr &node() const;
  ELObj **display() const;
  void traceSubObjects(Collector &) const;
private:
  ConstPtr<StyleSpec> styleSpec_;
  StyleObj *use_;
  ELObj **display_;
  NodePtr node_;
};

class OverriddenStyleObj : public StyleObj {
public:
  OverriddenStyleObj(BasicStyleObj *basic, StyleObj *override);
  void appendIter(StyleObjIter &) const;
  void traceSubObjects(Collector &) const;
private:
  BasicStyleObj *basic_;
  StyleObj *override_;
};

class MergeStyleObj : public StyleObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  MergeStyleObj();
  void append(StyleObj *);
  void appendIter(StyleObjIter &) const;
  void traceSubObjects(Collector &) const;
private:
  Vector<StyleObj *> styles_;
};

class ColorObj : public ELObj {
public:
  ColorObj *asColor();
  virtual void set(FOTBuilder &) const = 0;
  virtual void setBackground(FOTBuilder &) const = 0;
};

class DeviceRGBColorObj : public ColorObj {
public:
  DeviceRGBColorObj(unsigned char, unsigned char, unsigned char);
  void set(FOTBuilder &) const;
  void setBackground(FOTBuilder &) const;
private:
  FOTBuilder::DeviceRGBColor color_;
};

class ColorSpaceObj : public ELObj {
public:
  ColorSpaceObj *asColorSpace();
  virtual ELObj *makeColor(int argc, ELObj **argv, Interpreter &, const Location &) = 0;
};

class DeviceRGBColorSpaceObj : public ColorSpaceObj {
public:
  ELObj *makeColor(int argc, ELObj **argv, Interpreter &interp, const Location &);
};

struct InheritedCInfo : public Resource {
  InheritedCInfo(const ConstPtr<InheritedC> &, const VarStyleObj *,
		 unsigned valLevel, unsigned specLevel, const ProcessingMode::Rule *,
		 const Ptr<InheritedCInfo> &);
  ConstPtr<InheritedC> spec;
  Ptr<InheritedCInfo> prev;
  unsigned valLevel;
  unsigned specLevel;
  const ProcessingMode::Rule *rule;
  // If there are dependencies, then the cached value can only
  // be used only when the "value" flow object is at this level.
  ELObj *cachedValue;
  const VarStyleObj *style;
  // Includes both direct and indirect dependencies.
  Vector<size_t> dependencies;
};

struct PopList : public Resource {
  PopList(const Ptr<PopList> &);
  Vector<size_t> list;
  // List of the indices of those ICs that have dependencies
  // (ie that use actual-*) directly or indirectly.
  Vector<size_t> dependingList;
  Ptr<PopList> prev;
};

class StyleStack {
public:
  StyleStack();
  // These append on to dependencies.
  ELObj *actual(const ConstPtr<InheritedC> &, const Location &, Interpreter &,
		Vector<size_t> &dependencies);
  ELObj *actual(const ConstPtr<InheritedC> &, Interpreter &,
		Vector<size_t> &dependencies);
  ELObj *inherited(const ConstPtr<InheritedC> &, unsigned specLevel, Interpreter &,
		   Vector<size_t> &dependencies);
  void push(StyleObj *, VM &, FOTBuilder &);
  void pushStart();
  void pushContinue(StyleObj *, const ProcessingMode::Rule *, const NodePtr &,
		    Messenger *);
  void pushEnd(VM &, FOTBuilder &);
  void pop();
  void pushEmpty() { level_++; }
  void popEmpty() { level_--; }
  unsigned level() const { return level_; }
  void trace(Collector &) const;
private:
  Vector<Ptr<InheritedCInfo> > inheritedCInfo_;
  unsigned level_;
  Ptr<PopList> popList_;
};

inline
PopList::PopList(const Ptr<PopList> &p)
: prev(p)
{
}

inline
const Identifier *InheritedC::identifier() const
{
  return ident_;
}

inline
void InheritedC::setIdentifier(const Identifier *ident)
{
  ident_ = ident;
}

inline
unsigned InheritedC::index() const
{
  return index_;
}

inline
ELObj **VarStyleObj::display() const
{
  return display_;
}

inline
const NodePtr &VarStyleObj::node() const
{
  return node_;
}

inline
void StyleStack::pushStart()
{
  level_++;
  popList_ = new PopList(popList_);
}

inline
void StyleStack::push(StyleObj *style, VM &vm, FOTBuilder &fotb)
{
  pushStart();
  pushContinue(style, 0, NodePtr(), 0);
  pushEnd(vm, fotb);
}

inline
ELObj *StyleStack::actual(const ConstPtr<InheritedC> &ic,
			  Interpreter &interp,
			  Vector<size_t> &dep)
{
  return actual(ic, Location(), interp, dep);
}

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not Style_INCLUDED */
