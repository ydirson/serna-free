// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#include "stylelib.h"
#include "Style.h"
#include "VM.h"
#include "Interpreter.h"
#include "InterpreterMessages.h"
#include "macros.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

InheritedC::InheritedC(const Identifier *ident, unsigned index)
: ident_(ident), index_(index)
{
}

InheritedC::~InheritedC()
{
}

void InheritedC::invalidValue(const Location &loc,
			      Interpreter &interp) const
{
  interp.invalidCharacteristicValue(identifier(), loc);
}

class BoolInheritedC : public InheritedC {
public:
  BoolInheritedC(const Identifier *, unsigned index, bool);
  ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const;
protected:
  bool value_;
};

BoolInheritedC::BoolInheritedC(const Identifier *ident, unsigned index, bool value)
: InheritedC(ident, index), value_(value)
{
}

ELObj *BoolInheritedC::value(VM &vm, const VarStyleObj *, Vector<size_t> &) const
{
  if (value_)
    return vm.interp->makeTrue();
  else
    return vm.interp->makeFalse();
}

class IntegerInheritedC : public InheritedC {
public:
  IntegerInheritedC(const Identifier *, unsigned index, long);
  ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const;
protected:
  long n_;
};

IntegerInheritedC::IntegerInheritedC(const Identifier *ident, unsigned index, long n)
: InheritedC(ident, index), n_(n)
{
}

ELObj *IntegerInheritedC::value(VM &vm, const VarStyleObj *, Vector<size_t> &) const
{
  return vm.interp->makeInteger(n_);
}

class LengthInheritedC : public InheritedC {
public:
  LengthInheritedC(const Identifier *, unsigned index, FOTBuilder::Length);
  ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const;
protected:
  FOTBuilder::Length size_;
};

LengthInheritedC::LengthInheritedC(const Identifier *ident, unsigned index, FOTBuilder::Length size)
: InheritedC(ident, index), size_(size)
{
}

ELObj *LengthInheritedC::value(VM &vm, const VarStyleObj *, Vector<size_t> &) const
{
  return new (*vm.interp) LengthObj(size_);
}

class SymbolInheritedC : public InheritedC {
public:
  SymbolInheritedC(const Identifier *, unsigned index, FOTBuilder::Symbol);
  ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const;
protected:
  FOTBuilder::Symbol sym_;
};

SymbolInheritedC::SymbolInheritedC(const Identifier *ident, unsigned index, FOTBuilder::Symbol sym)
: InheritedC(ident, index), sym_(sym)
{
}

ELObj *SymbolInheritedC::value(VM &vm, const VarStyleObj *, Vector<size_t> &) const
{
  return vm.interp->cValueSymbol(sym_);
}

class PublicIdInheritedC : public InheritedC {
public:
  PublicIdInheritedC(const Identifier *, unsigned index,
		     FOTBuilder::PublicId = 0);
  ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const;
protected:
  FOTBuilder::PublicId pubid_;
};

PublicIdInheritedC::PublicIdInheritedC(const Identifier *ident, unsigned index, FOTBuilder::PublicId pubid)
: InheritedC(ident, index), pubid_(pubid)
{
}

ELObj *PublicIdInheritedC::value(VM &vm, const VarStyleObj *, Vector<size_t> &) const
{
  Interpreter &interp = *vm.interp;
  if (pubid_)
    return interp.makeSymbol(interp.makeStringC(pubid_));
  else
    return interp.makeFalse();
}

class Letter2InheritedC : public InheritedC {
public:
  Letter2InheritedC(const Identifier *, unsigned index,
		     FOTBuilder::Letter2 = 0);
  ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const;
protected:
  FOTBuilder::Letter2 code_;
};

Letter2InheritedC::Letter2InheritedC(const Identifier *ident, unsigned index,
				     FOTBuilder::Letter2 code)
: InheritedC(ident, index), code_(code)
{
}

ELObj *Letter2InheritedC::value(VM &vm, const VarStyleObj *, Vector<size_t> &) const
{
  Interpreter &interp = *vm.interp;
  if (code_) {
    StringC buf;
    buf += (code_ >> 8) & 0xff;
    buf += code_ & 0xff;
    buf += 0;
    return interp.makeSymbol(buf);
  }
  else
    return interp.makeFalse();
}

class LengthSpecInheritedC : public InheritedC {
public:
  LengthSpecInheritedC(const Identifier *, unsigned index, FOTBuilder::Length);
  ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const;
protected:
  bool setValue(ELObj *, const Location &, Interpreter &);
  FOTBuilder::LengthSpec value_;
};


LengthSpecInheritedC::LengthSpecInheritedC(const Identifier *ident,
					   unsigned index, FOTBuilder::Length n)
: InheritedC(ident, index), value_(n)
{
}

ELObj *LengthSpecInheritedC::value(VM &vm, const VarStyleObj *,
				   Vector<size_t> &) const
{
  return vm.interp->makeLengthSpec(value_);
}

bool LengthSpecInheritedC::setValue(ELObj *obj, const Location &loc,
				    Interpreter &interp)
{
  return interp.convertLengthSpecC(obj, identifier(), loc, value_);
}

class OptLengthSpecInheritedC : public InheritedC {
public:
  OptLengthSpecInheritedC(const Identifier *, unsigned index);
  ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const;
protected:
  bool setValue(ELObj *, const Location &, Interpreter &);
  FOTBuilder::OptLengthSpec value_;
};


OptLengthSpecInheritedC::OptLengthSpecInheritedC(const Identifier *ident,
						 unsigned index)
: InheritedC(ident, index)
{
}

ELObj *OptLengthSpecInheritedC::value(VM &vm, const VarStyleObj *,
				      Vector<size_t> &) const
{
  if (!value_.hasLength)
    return vm.interp->makeFalse();
  else
    return vm.interp->makeLengthSpec(value_.length);
}

bool OptLengthSpecInheritedC::setValue(ELObj *obj, const Location &loc,
				    Interpreter &interp)
{
  return interp.convertOptLengthSpecC(obj, identifier(), loc, value_);
}

class StringInheritedC : public InheritedC {
public:
  StringInheritedC(const Identifier *, unsigned index, const Char *, size_t );
  StringInheritedC(const Identifier *, unsigned index, const StringC &);
  ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const;
protected:
  StringC str_;
};

StringInheritedC::StringInheritedC(const Identifier *ident, unsigned index,
				   const Char *s, size_t n)
: InheritedC(ident, index), str_(s, n)
{
}

StringInheritedC::StringInheritedC(const Identifier *ident, unsigned index,
				   const StringC &s)
: InheritedC(ident, index), str_(s)
{
}

ELObj *StringInheritedC::value(VM &vm, const VarStyleObj *, Vector<size_t> &) const
{
  return new(*vm.interp) StringObj(str_);
}

class GenericInlineSpaceInheritedC : public InheritedC {
public:
  typedef void (FOTBuilder::*Setter)(const FOTBuilder::InlineSpace &);
  GenericInlineSpaceInheritedC(const Identifier *, unsigned index, Setter setter);
  ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const;
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
private:
  FOTBuilder::InlineSpace value_;
  Setter setter_;
};

GenericInlineSpaceInheritedC::GenericInlineSpaceInheritedC(const Identifier *ident,
							   unsigned index,
							   Setter setter)
: InheritedC(ident, index), setter_(setter)
{
}

ELObj *GenericInlineSpaceInheritedC::value(VM &vm, const VarStyleObj *,
				    Vector<size_t> &) const
{
  return new (*vm.interp) InlineSpaceObj(value_);
}

void GenericInlineSpaceInheritedC::set(VM &, const VarStyleObj *,
				      FOTBuilder &fotb,
				      ELObj *&,
				      Vector<size_t> &) const
{
  (fotb.*setter_)(value_);
}

ConstPtr<InheritedC>
GenericInlineSpaceInheritedC::make(ELObj *obj, const Location &loc,
				   Interpreter &interp) const
{
  GenericInlineSpaceInheritedC *copy
    = new GenericInlineSpaceInheritedC(identifier(), index(), setter_);
  InlineSpaceObj *iso = obj->asInlineSpace();
  if (iso) {
    copy->value_ = iso->inlineSpace();
    return copy;
  }
  else if (interp.convertLengthSpecC(obj, identifier(), loc, copy->value_.nominal))  {
    copy->value_.min = copy->value_.nominal;
    copy->value_.max = copy->value_.nominal;
    return copy;
  }
  delete copy;
  return 0;
}


class GenericBoolInheritedC : public BoolInheritedC {
public:
  typedef void (FOTBuilder::*Setter)(bool);
  GenericBoolInheritedC(const Identifier *, unsigned index, Setter,
			bool = 0);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
private:
  Setter setter_;
};

GenericBoolInheritedC
::GenericBoolInheritedC(const Identifier *ident, unsigned index,
			Setter setter, bool b)
: BoolInheritedC(ident, index, b), setter_(setter)
{
}

void GenericBoolInheritedC::set(VM &, const VarStyleObj *,
				FOTBuilder &fotb,
				ELObj *&,
				Vector<size_t> &) const
{
  (fotb.*setter_)(value_);
}

ConstPtr<InheritedC>
GenericBoolInheritedC::make(ELObj *obj, const Location &loc,
			    Interpreter &interp) const
{
  bool b;
  if (interp.convertBooleanC(obj, identifier(), loc, b))
    return new GenericBoolInheritedC(identifier(), index(), setter_, b);
  return ConstPtr<InheritedC>();
}

class ExtensionBoolInheritedC : public BoolInheritedC {
public:
  typedef void (FOTBuilder::*Setter)(bool);
  ExtensionBoolInheritedC(const Identifier *, unsigned index, Setter,
			  bool = 0);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
private:
  Setter setter_;
};

ExtensionBoolInheritedC
::ExtensionBoolInheritedC(const Identifier *ident, unsigned index,
			  Setter setter, bool b)
: BoolInheritedC(ident, index, b), setter_(setter)
{
}

void ExtensionBoolInheritedC::set(VM &, const VarStyleObj *,
				  FOTBuilder &fotb,
				  ELObj *&,
				  Vector<size_t> &) const
{
  fotb.extensionSet(setter_, value_);
}

ConstPtr<InheritedC>
ExtensionBoolInheritedC::make(ELObj *obj, const Location &loc,
			    Interpreter &interp) const
{
  bool b;
  if (interp.convertBooleanC(obj, identifier(), loc, b))
    return new ExtensionBoolInheritedC(identifier(), index(), setter_, b);
  return ConstPtr<InheritedC>();
}

class ExtensionStringInheritedC : public StringInheritedC {
public:
  typedef void (FOTBuilder::*Setter)(const StringC &);
  ExtensionStringInheritedC(const Identifier *, unsigned index, Setter);
  ExtensionStringInheritedC(const Identifier *, unsigned index, Setter,
			    const Char *, size_t);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
private:
  Setter setter_;
};

ExtensionStringInheritedC
::ExtensionStringInheritedC(const Identifier *ident, unsigned index,
			    Setter setter)
: StringInheritedC(ident, index, StringC()), setter_(setter)
{
}

ExtensionStringInheritedC
::ExtensionStringInheritedC(const Identifier *ident, unsigned index,
			    Setter setter, const Char *s, size_t n)
: StringInheritedC(ident, index, s, n), setter_(setter)
{
}

void ExtensionStringInheritedC::set(VM &, const VarStyleObj *,
				  FOTBuilder &fotb,
				  ELObj *&,
				  Vector<size_t> &) const
{
  fotb.extensionSet(setter_, str_);
}

ConstPtr<InheritedC>
ExtensionStringInheritedC::make(ELObj *obj, const Location &loc,
			    Interpreter &interp) const
{
  const Char *s;
  size_t n;
  if (obj->stringData(s, n))
    return new ExtensionStringInheritedC(identifier(), index(), setter_, s, n);
  invalidValue(loc, interp);
  return ConstPtr<InheritedC>();
}

class ExtensionIntegerInheritedC : public IntegerInheritedC {
public:
  typedef void (FOTBuilder::*Setter)(long);
  ExtensionIntegerInheritedC(const Identifier *, unsigned index, Setter,
			  long = 0);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
private:
  Setter setter_;
};

ExtensionIntegerInheritedC
::ExtensionIntegerInheritedC(const Identifier *ident, unsigned index,
			     Setter setter, long n)
: IntegerInheritedC(ident, index, n), setter_(setter)
{
}

void ExtensionIntegerInheritedC::set(VM &, const VarStyleObj *,
				     FOTBuilder &fotb,
				     ELObj *&,
				     Vector<size_t> &) const
{
  fotb.extensionSet(setter_, n_);
}

ConstPtr<InheritedC>
ExtensionIntegerInheritedC::make(ELObj *obj, const Location &loc,
	  			 Interpreter &interp) const
{
  long n;
  if (interp.convertIntegerC(obj, identifier(), loc, n))
    return new ExtensionIntegerInheritedC(identifier(), index(), setter_, n);
  return ConstPtr<InheritedC>();
}

class ExtensionLengthInheritedC : public LengthInheritedC {
public:
  typedef void (FOTBuilder::*Setter)(FOTBuilder::Length);
  ExtensionLengthInheritedC(const Identifier *, unsigned index, Setter, FOTBuilder::Length = 0);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
private:
  Setter setter_;
};

ExtensionLengthInheritedC
::ExtensionLengthInheritedC(const Identifier *ident, unsigned index,
			    Setter setter, FOTBuilder::Length n)
: LengthInheritedC(ident, index, n), setter_(setter)
{
}

void ExtensionLengthInheritedC::set(VM &, const VarStyleObj *,
				    FOTBuilder &fotb,
				    ELObj *&,
				    Vector<size_t> &) const
{
  fotb.extensionSet(setter_, size_);
}

ConstPtr<InheritedC>
ExtensionLengthInheritedC::make(ELObj *obj, const Location &loc,
				Interpreter &interp) const
{
  FOTBuilder::Length n;
  if (interp.convertLengthC(obj, identifier(), loc, n))
    return new ExtensionLengthInheritedC(identifier(), index(), setter_, n);
  return ConstPtr<InheritedC>();
}

class GenericLengthInheritedC : public LengthInheritedC {
public:
  typedef void (FOTBuilder::*Setter)(FOTBuilder::Length);
  GenericLengthInheritedC(const Identifier *, unsigned index, Setter,
			  FOTBuilder::Length = 0);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
private:
  Setter setter_;
};

GenericLengthInheritedC
::GenericLengthInheritedC(const Identifier *ident, unsigned index,
			  Setter setter, FOTBuilder::Length n)
: LengthInheritedC(ident, index, n), setter_(setter)
{
}

void GenericLengthInheritedC::set(VM &, const VarStyleObj *,
				  FOTBuilder &fotb,
				  ELObj *&,
				  Vector<size_t> &) const
{
  (fotb.*setter_)(size_);
}

ConstPtr<InheritedC>
GenericLengthInheritedC::make(ELObj *obj, const Location &loc,
			      Interpreter &interp) const
{
  FOTBuilder::Length n;
  if (interp.convertLengthC(obj, identifier(), loc, n))
    return new GenericLengthInheritedC(identifier(), index(), setter_, n);
  return ConstPtr<InheritedC>();
}

class GenericIntegerInheritedC : public IntegerInheritedC {
public:
  typedef void (FOTBuilder::*Setter)(long);
  GenericIntegerInheritedC(const Identifier *, unsigned index, Setter, long);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
protected:
  Setter setter_;
};

GenericIntegerInheritedC
::GenericIntegerInheritedC(const Identifier *ident, unsigned index,
			  Setter setter, long n)
: IntegerInheritedC(ident, index, n), setter_(setter)
{
}

void GenericIntegerInheritedC::set(VM &, const VarStyleObj *,
				  FOTBuilder &fotb,
				  ELObj *&,
				  Vector<size_t> &) const
{
  (fotb.*setter_)(n_);
}

ConstPtr<InheritedC>
GenericIntegerInheritedC::make(ELObj *obj, const Location &loc,
			      Interpreter &interp) const
{
  long n;
  if (interp.convertIntegerC(obj, identifier(), loc, n))
    return new GenericIntegerInheritedC(identifier(), index(), setter_, n);
  return ConstPtr<InheritedC>();
}

class GenericMaybeIntegerInheritedC : public GenericIntegerInheritedC {
public:
  GenericMaybeIntegerInheritedC(const Identifier *, unsigned index,
				Setter, long);
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
  ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const;
};

GenericMaybeIntegerInheritedC
::GenericMaybeIntegerInheritedC(const Identifier *ident, unsigned index,
				Setter setter, long n)
: GenericIntegerInheritedC(ident, index, setter, n)
{
}

ELObj *GenericMaybeIntegerInheritedC::value(VM &vm, const VarStyleObj *style,
					    Vector<size_t> &dep) const
{
  if (!n_)
    return vm.interp->makeFalse();
  else
    return IntegerInheritedC::value(vm, style, dep);
}

ConstPtr<InheritedC>
GenericMaybeIntegerInheritedC::make(ELObj *obj, const Location &loc,
				    Interpreter &interp) const
{
  long n;
  if (interp.convertOptPositiveIntegerC(obj, identifier(), loc, n))
    return new GenericMaybeIntegerInheritedC(identifier(), index(),
					     setter_, n);
  return ConstPtr<InheritedC>();
}

class GenericSymbolInheritedC : public SymbolInheritedC {
public:
  typedef void (FOTBuilder::*Setter)(FOTBuilder::Symbol);
  GenericSymbolInheritedC(const Identifier *, unsigned index, Setter,
		          FOTBuilder::Symbol = FOTBuilder::symbolFalse);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
private:
  Setter setter_;
};

GenericSymbolInheritedC
::GenericSymbolInheritedC(const Identifier *ident, unsigned index,
			  Setter setter, FOTBuilder::Symbol sym)
: SymbolInheritedC(ident, index, sym), setter_(setter)
{
}

void GenericSymbolInheritedC::set(VM &, const VarStyleObj *,
				  FOTBuilder &fotb,
				  ELObj *&,
				  Vector<size_t> &) const
{
  (fotb.*setter_)(sym_);
}

ConstPtr<InheritedC>
GenericSymbolInheritedC::make(ELObj *obj, const Location &loc,
			      Interpreter &interp) const
{
  FOTBuilder::Symbol sym;
  if (interp.convertEnumC(obj, identifier(), loc, sym))
    return new GenericSymbolInheritedC(identifier(), index(), setter_, sym);
  return ConstPtr<InheritedC>();
}

class GenericPublicIdInheritedC : public PublicIdInheritedC {
public:
  typedef void (FOTBuilder::*Setter)(FOTBuilder::PublicId);
  GenericPublicIdInheritedC(const Identifier *, unsigned index, Setter,
			    FOTBuilder::PublicId = 0);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
private:
  Setter setter_;
};

GenericPublicIdInheritedC
::GenericPublicIdInheritedC(const Identifier *ident, unsigned index,
			  Setter setter, FOTBuilder::PublicId pubid)
: PublicIdInheritedC(ident, index, pubid), setter_(setter)
{
}

void GenericPublicIdInheritedC::set(VM &, const VarStyleObj *,
				    FOTBuilder &fotb,
				    ELObj *&,
				    Vector<size_t> &) const
{
  (fotb.*setter_)(pubid_);
}

ConstPtr<InheritedC>
GenericPublicIdInheritedC::make(ELObj *obj, const Location &loc,
				Interpreter &interp) const
{
  FOTBuilder::PublicId pubid;
  if (interp.convertPublicIdC(obj, identifier(), loc, pubid))
    return new GenericPublicIdInheritedC(identifier(), index(), setter_,
					 pubid);
  return ConstPtr<InheritedC>();
}

class GenericLetter2InheritedC : public Letter2InheritedC {
public:
  typedef void (FOTBuilder::*Setter)(FOTBuilder::Letter2);
  GenericLetter2InheritedC(const Identifier *, unsigned index, Setter,
			    FOTBuilder::Letter2 = 0);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
private:
  Setter setter_;
};

GenericLetter2InheritedC
::GenericLetter2InheritedC(const Identifier *ident, unsigned index,
			   Setter setter, FOTBuilder::Letter2 code)
: Letter2InheritedC(ident, index, code), setter_(setter)
{
}

void GenericLetter2InheritedC::set(VM &, const VarStyleObj *,
				   FOTBuilder &fotb,
				   ELObj *&,
				   Vector<size_t> &) const
{
  (fotb.*setter_)(code_);
}

ConstPtr<InheritedC>
GenericLetter2InheritedC::make(ELObj *obj, const Location &loc,
				Interpreter &interp) const
{
  FOTBuilder::Letter2 code;
  if (interp.convertLetter2C(obj, identifier(), loc, code))
    return new GenericLetter2InheritedC(identifier(), index(), setter_, code);
  return ConstPtr<InheritedC>();
}

class GenericLengthSpecInheritedC : public LengthSpecInheritedC {
public:
  typedef void (FOTBuilder::*Setter)(const FOTBuilder::LengthSpec &);
  GenericLengthSpecInheritedC(const Identifier *, unsigned index, Setter,
			      long = 0);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
private:
  Setter setter_;
};

GenericLengthSpecInheritedC
::GenericLengthSpecInheritedC(const Identifier *ident, unsigned index,
			      Setter setter, long n)
: LengthSpecInheritedC(ident, index, n), setter_(setter)
{
}

void GenericLengthSpecInheritedC::set(VM &, const VarStyleObj *,
				      FOTBuilder &fotb,
				      ELObj *&,
				      Vector<size_t> &) const
{
  (fotb.*setter_)(value_);
}

ConstPtr<InheritedC>
GenericLengthSpecInheritedC::make(ELObj *obj, const Location &loc,
				  Interpreter &interp) const
{
  GenericLengthSpecInheritedC *copy
    = new GenericLengthSpecInheritedC(identifier(), index(), setter_);
  if (!copy->setValue(obj, loc, interp)) {
    delete copy;
    copy = 0;
  }
  return copy;
}

class GenericOptLengthSpecInheritedC : public OptLengthSpecInheritedC {
public:
  typedef void (FOTBuilder::*Setter)(const FOTBuilder::OptLengthSpec &);
  GenericOptLengthSpecInheritedC(const Identifier *, unsigned index, Setter);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
private:
  Setter setter_;
};

GenericOptLengthSpecInheritedC
::GenericOptLengthSpecInheritedC(const Identifier *ident, unsigned index,
				 Setter setter)
: OptLengthSpecInheritedC(ident, index), setter_(setter)
{
}

void GenericOptLengthSpecInheritedC::set(VM &, const VarStyleObj *,
				      FOTBuilder &fotb,
				      ELObj *&,
				      Vector<size_t> &) const
{
  (fotb.*setter_)(value_);
}

ConstPtr<InheritedC>
GenericOptLengthSpecInheritedC::make(ELObj *obj, const Location &loc,
				     Interpreter &interp) const
{
  GenericOptLengthSpecInheritedC *copy
    = new GenericOptLengthSpecInheritedC(identifier(), index(), setter_);
  if (!copy->setValue(obj, loc, interp)) {
    delete copy;
    copy = 0;
  }
  return copy;
}

class FontSizeC : public LengthInheritedC {
public:
  FontSizeC(const Identifier *, unsigned index, long);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
};

FontSizeC::FontSizeC(const Identifier *ident, unsigned index, long size)
: LengthInheritedC(ident, index, size)
{
}

void FontSizeC::set(VM &vm, const VarStyleObj *, FOTBuilder &fotb, ELObj *&,
		    Vector<size_t> &) const
{
  fotb.setFontSize(size_);
}

ConstPtr<InheritedC> FontSizeC::make(ELObj *obj, const Location &loc,
				     Interpreter &interp) const
{
  long n;
  if (interp.convertLengthC(obj, identifier(), loc, n))
    return new FontSizeC(identifier(), index(), n);
  return ConstPtr<InheritedC>();
}

class FontFamilyNameC : public StringInheritedC {
public:
  FontFamilyNameC(const Identifier *, unsigned index, const Char *, size_t);
  FontFamilyNameC(const Identifier *, unsigned index, const StringC &);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
};

FontFamilyNameC::FontFamilyNameC(const Identifier *ident, unsigned index, const Char *s, size_t n)
: StringInheritedC(ident, index, s, n)
{
}

FontFamilyNameC::FontFamilyNameC(const Identifier *ident, unsigned index, const StringC &s)
: StringInheritedC(ident, index, s)
{
}

void FontFamilyNameC::set(VM &vm, const VarStyleObj *, FOTBuilder &fotb, ELObj *&,
		      Vector<size_t> &) const
{
  fotb.setFontFamilyName(str_);
}

ConstPtr<InheritedC> FontFamilyNameC::make(ELObj *obj, const Location &loc,
				       Interpreter &interp) const
{
  const Char *s;
  size_t n;
  if (obj->stringData(s, n))
    return new FontFamilyNameC(identifier(), index(), s, n);
  invalidValue(loc, interp);
  return ConstPtr<InheritedC>();
}

class ColorC : public InheritedC, private Collector::DynamicRoot {
public:
  ColorC(const Identifier *, unsigned index, ColorObj *, Interpreter &);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
  ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const;
  void trace(Collector &) const;
private:
  ColorObj *color_;
};

ColorC::ColorC(const Identifier *ident, unsigned index, ColorObj *color,
	       Interpreter &interp)
: InheritedC(ident, index), Collector::DynamicRoot(interp), color_(color)
{
}

void ColorC::set(VM &, const VarStyleObj *, FOTBuilder &fotb,
		 ELObj *&, Vector<size_t> &) const
{
  color_->set(fotb);
}

ConstPtr<InheritedC> ColorC::make(ELObj *obj, const Location &loc,
				  Interpreter &interp) const
{
  ColorObj *color;
  if (interp.convertColorC(obj, identifier(), loc, color))
    return new ColorC(identifier(), index(), color, interp);
  return ConstPtr<InheritedC>();
}

ELObj *ColorC::value(VM &vm, const VarStyleObj *, Vector<size_t> &) const
{
  if (!color_)
    return vm.interp->makeFalse();
  else
    return color_;
}

void ColorC::trace(Collector &c) const
{
  c.trace(color_);
}

class BackgroundColorC : public InheritedC, private Collector::DynamicRoot {
public:
  BackgroundColorC(const Identifier *, unsigned index, ColorObj *,
		   Interpreter &);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
  ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const;
  void trace(Collector &) const;
private:
  ColorObj *color_;
};

BackgroundColorC::BackgroundColorC(const Identifier *ident, unsigned index,
				   ColorObj *color, Interpreter &interp)
: InheritedC(ident, index), Collector::DynamicRoot(interp), color_(color)
{
}

void BackgroundColorC::set(VM &, const VarStyleObj *, FOTBuilder &fotb,
			   ELObj *&, Vector<size_t> &) const
{
  if (color_)
    color_->setBackground(fotb);
  else
    fotb.setBackgroundColor();
}

ConstPtr<InheritedC> BackgroundColorC::make(ELObj *obj, const Location &loc,
					    Interpreter &interp) const
{
  ColorObj *color;
  if (interp.convertOptColorC(obj, identifier(), loc, color))
    return new BackgroundColorC(identifier(), index(), color, interp);
  return ConstPtr<InheritedC>();
}

ELObj *
BackgroundColorC::value(VM &vm, const VarStyleObj *, Vector<size_t> &) const
{
  if (color_)
    return color_;
  else
    return vm.interp->makeFalse();
}

void BackgroundColorC::trace(Collector &c) const
{
  c.trace(color_);
}

class GlyphSubstTableC : public InheritedC {
public:
  GlyphSubstTableC(const Identifier *, unsigned index, bool isList);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&value, Vector<size_t> &dependencies) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
  ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const;
  void append(const ConstPtr<FOTBuilder::GlyphSubstTable> &table) { tables_.push_back(table); }
private:
  // Distinguish between #f and () for inherited-glyph-subst-table
  bool isList_;
  Vector<ConstPtr<FOTBuilder::GlyphSubstTable> > tables_;
};

GlyphSubstTableC::GlyphSubstTableC(const Identifier *ident, unsigned index, bool isList)
: InheritedC(ident, index), isList_(isList)
{
}

void GlyphSubstTableC::set(VM &, const VarStyleObj *, FOTBuilder &fotb,
			   ELObj *&, Vector<size_t> &) const
{
  fotb.setGlyphSubstTable(tables_);
}

ConstPtr<InheritedC> GlyphSubstTableC::make(ELObj *obj, const Location &loc,
					    Interpreter &interp) const
{
  if (obj == interp.makeFalse())
    return new GlyphSubstTableC(identifier(), index(), 0);
  GlyphSubstTableObj *table = obj->asGlyphSubstTable();
  if (table) {
    Ptr<InheritedC> tem = new GlyphSubstTableC(identifier(), index(), 0);
    ((GlyphSubstTableC *)tem.pointer())->append(table->glyphSubstTable());
    return tem;
  }
  Ptr<InheritedC> tem = new GlyphSubstTableC(identifier(), index(), 1);
  for (;;) {
    if (obj->isNil())
      return tem;
    PairObj *pair = obj->asPair();
    if (!pair)
      break;
    obj = pair->cdr();
    table = pair->car()->asGlyphSubstTable();
    if (!table)
      break;
    ((GlyphSubstTableC *)tem.pointer())->append(table->glyphSubstTable());
  }
  invalidValue(loc, interp);
  return ConstPtr<InheritedC>();
}

ELObj *GlyphSubstTableC::value(VM &vm, const VarStyleObj *, Vector<size_t> &) const
{
  if (!isList_) {
    if (tables_.size())
      return new (*vm.interp) GlyphSubstTableObj(tables_[0]);
    else
      return vm.interp->makeFalse();
  }
  else {
    ELObj *list = vm.interp->makeNil();
    ELObjDynamicRoot protect(*vm.interp, list);
    for (size_t i = tables_.size(); i > 0; i--) {
      ELObj *tableObj = new (*vm.interp) GlyphSubstTableObj(tables_[i - 1]);
      ELObjDynamicRoot protect2(*vm.interp, tableObj);
      list = vm.interp->makePair(tableObj, list);
      protect = list;
    }
    return list;
  }
}

class IgnoredC : public InheritedC, private Collector::DynamicRoot {
public:
  IgnoredC(const Identifier *, unsigned index, ELObj *, Interpreter &);
  void set(VM &, const VarStyleObj *, FOTBuilder &,
           ELObj *&, Vector<size_t> &) const;
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
  ELObj *value(VM &, const VarStyleObj *, Vector<size_t> &) const;
  void trace(Collector &) const;
private:
  ELObj *value_;
};


IgnoredC::IgnoredC(const Identifier *ident, unsigned index, ELObj *value, Interpreter &interp)
: InheritedC(ident, index), Collector::DynamicRoot(interp), value_(value)
{
}

void IgnoredC::set(VM &, const VarStyleObj *, FOTBuilder &fotb,
		   ELObj *&, Vector<size_t> &) const
{
}

ConstPtr<InheritedC> IgnoredC::make(ELObj *obj, const Location &, Interpreter &interp) const
{
  return new IgnoredC(identifier(), index(), obj, interp);
}

ELObj *IgnoredC::value(VM &, const VarStyleObj *, Vector<size_t> &) const
{
  return value_;
}

void IgnoredC::trace(Collector &c) const
{
  c.trace(value_);
}

class BorderC : public IgnoredC {
public:
  BorderC(const Identifier *, unsigned index, ELObj *, Interpreter &);
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
};

BorderC::BorderC(const Identifier *ident, unsigned index, ELObj *value, Interpreter &interp)
: IgnoredC(ident, index, value, interp)
{
}

ConstPtr<InheritedC> BorderC::make(ELObj *obj, const Location &loc, Interpreter &interp) const
{
  StyleObj *tem;
  SosofoObj *sosofo = obj->asSosofo();
  if (sosofo && sosofo->tableBorderStyle(tem))
    return new BorderC(identifier(), index(), obj, interp);
  bool b;
  if (interp.convertBooleanC(obj, identifier(), loc, b)) {
    if (b)
      obj = interp.makeTrue();
    else
      obj = interp.makeFalse();
    return new BorderC(identifier(), index(), obj, interp);
  }
  return ConstPtr<InheritedC>();
}

class RuleC : public IgnoredC {
public:
  RuleC(const Identifier *, unsigned index, ELObj *, Interpreter &);
  ConstPtr<InheritedC> make(ELObj *, const Location &, Interpreter &) const;
};

RuleC::RuleC(const Identifier *ident, unsigned index, ELObj *value, Interpreter &interp)
: IgnoredC(ident, index, value, interp)
{
}

ConstPtr<InheritedC> RuleC::make(ELObj *obj, const Location &loc, Interpreter &interp) const
{
  SosofoObj *sosofo = obj->asSosofo();
  if (sosofo && sosofo->isRule())
    return new RuleC(identifier(), index(), obj, interp);
  invalidValue(loc, interp);
  return ConstPtr<InheritedC>();
}

class InheritedCPrimitiveObj : public PrimitiveObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  static const Signature signature_;
  InheritedCPrimitiveObj(const ConstPtr<InheritedC> &ic)
    : PrimitiveObj(&signature_), inheritedC_(ic) { }
  ELObj *primitiveCall(int, ELObj **, EvalContext &, Interpreter &, const Location &);
private:
  ConstPtr<InheritedC> inheritedC_;
};

const Signature InheritedCPrimitiveObj::signature_ = { 0, 0, 0 };

ELObj *InheritedCPrimitiveObj::primitiveCall(int, ELObj **, EvalContext &ec,
					     Interpreter &interp,
					     const Location &loc)
{
  if (!ec.styleStack) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::notInCharacteristicValue);
    return interp.makeError();
  }
  ELObj *obj = ec.styleStack->inherited(inheritedC_, ec.specLevel, interp,
				        *ec.actualDependencies);
  interp.makeReadOnly(obj);
  return obj;
}

class ActualCPrimitiveObj : public PrimitiveObj {
public:
  void *operator new(size_t, Collector &c) {
    return c.allocateObject(1);
  }
  static const Signature signature_;
  ActualCPrimitiveObj(const ConstPtr<InheritedC> &ic)
    : PrimitiveObj(&signature_), inheritedC_(ic) { }
  ELObj *primitiveCall(int, ELObj **, EvalContext &, Interpreter &, const Location &);
private:
  ConstPtr<InheritedC> inheritedC_;
};

const Signature ActualCPrimitiveObj::signature_ = { 0, 0, 0 };

ELObj *ActualCPrimitiveObj::primitiveCall(int, ELObj **, EvalContext &ec, Interpreter &interp,
					  const Location &loc)
{
  if (!ec.styleStack) {
    interp.setNextLocation(loc);
    interp.message(InterpreterMessages::notInCharacteristicValue);
    return interp.makeError();
  }
  ELObj *obj = ec.styleStack->actual(inheritedC_, loc, interp, *ec.actualDependencies);
  interp.makeReadOnly(obj);
  return obj;
}

#define INHERITED_C(name, C, init) \
    installInheritedC(name, new C(0, nInheritedC_++, init))
#define INHERITED_C2(name, C, init1, init2) \
    installInheritedC(name, new C(0, nInheritedC_++, init1, init2))
#define STORE_INHERITED_C2(var, name, C, init1, init2) \
   { InheritedC *ic = new C(0, nInheritedC_++, init1, init2); \
     installInheritedC(name, ic); \
     var = ic; }

#define IGNORED_C(name, init) INHERITED_C2(name, IgnoredC, init, *this)

static StyleObj *makeBorderStyle(bool b, unsigned index, Interpreter &interp)
{
  Vector<ConstPtr<InheritedC> > forceSpecs;
  Vector<ConstPtr<InheritedC> > specs;
  specs.push_back(
    new GenericBoolInheritedC(interp.lookup(interp.makeStringC("border-present?")),
			      index, &FOTBuilder::setBorderPresent, b));

  StyleObj *style
    = new (interp) VarStyleObj(new StyleSpec(forceSpecs, specs), 0, 0, NodePtr());
  interp.makePermanent(style);
  return style;
}

void Interpreter::installInheritedCs()
{
  INHERITED_C("font-size", FontSizeC, (unitsPerInch()*10)/72);
  INHERITED_C("font-family-name", FontFamilyNameC, makeStringC("iso-serif"));
  INHERITED_C2("font-weight", GenericSymbolInheritedC, &FOTBuilder::setFontWeight,
               FOTBuilder::symbolMedium);
  INHERITED_C2("font-posture", GenericSymbolInheritedC, &FOTBuilder::setFontPosture,
               FOTBuilder::symbolUpright);
  INHERITED_C2("quadding", GenericSymbolInheritedC, &FOTBuilder::setQuadding,
               FOTBuilder::symbolStart);
  INHERITED_C2("display-alignment", GenericSymbolInheritedC, &FOTBuilder::setDisplayAlignment,
               FOTBuilder::symbolStart);
  INHERITED_C2("field-align", GenericSymbolInheritedC, &FOTBuilder::setFieldAlign,
               FOTBuilder::symbolStart);
  INHERITED_C2("lines", GenericSymbolInheritedC, &FOTBuilder::setLines, FOTBuilder::symbolWrap);
  INHERITED_C("start-indent", GenericLengthSpecInheritedC,
	      &FOTBuilder::setStartIndent);
  INHERITED_C("first-line-start-indent", GenericLengthSpecInheritedC,
	      &FOTBuilder::setFirstLineStartIndent);
  INHERITED_C("end-indent", GenericLengthSpecInheritedC,
	      &FOTBuilder::setEndIndent);
  INHERITED_C("last-line-end-indent", GenericLengthSpecInheritedC,
	      &FOTBuilder::setLastLineEndIndent);
  INHERITED_C2("line-spacing", GenericLengthSpecInheritedC,
	       &FOTBuilder::setLineSpacing, unitsPerInch()*12/72);
  INHERITED_C("field-width", GenericLengthSpecInheritedC,
	      &FOTBuilder::setFieldWidth);
  INHERITED_C("left-margin", GenericLengthInheritedC,
	      &FOTBuilder::setLeftMargin);
  INHERITED_C("right-margin", GenericLengthInheritedC,
	      &FOTBuilder::setRightMargin);
  INHERITED_C("top-margin", GenericLengthInheritedC,
	      &FOTBuilder::setTopMargin);
  INHERITED_C("bottom-margin", GenericLengthInheritedC,
	      &FOTBuilder::setBottomMargin);
  INHERITED_C("header-margin", GenericLengthInheritedC,
	      &FOTBuilder::setHeaderMargin);
  INHERITED_C("footer-margin", GenericLengthInheritedC,
	      &FOTBuilder::setFooterMargin);
  INHERITED_C2("page-width", GenericLengthInheritedC,
	       &FOTBuilder::setPageWidth,
	       unitsPerInch()*8 + unitsPerInch()/2);
  INHERITED_C2("page-height", GenericLengthInheritedC,
	       &FOTBuilder::setPageHeight,
	       unitsPerInch()*11);
  INHERITED_C2("color", ColorC, new (*this) DeviceRGBColorObj(0, 0, 0), *this);
  INHERITED_C2("background-color", BackgroundColorC, 0, *this);
  INHERITED_C2("border-present?", GenericBoolInheritedC,
	       &FOTBuilder::setBorderPresent, 1);
  borderTrueStyle_ = makeBorderStyle(1, nInheritedC_ - 1, *this);
  borderFalseStyle_ = makeBorderStyle(0, nInheritedC_ - 1, *this);
  STORE_INHERITED_C2(tableBorderC_, "table-border", BorderC, makeFalse(), *this);
  STORE_INHERITED_C2(cellBeforeRowBorderC_, "cell-before-row-border",
		     BorderC, makeFalse(), *this);
  STORE_INHERITED_C2(cellAfterRowBorderC_, "cell-after-row-border",
		     BorderC, makeFalse(), *this);
  STORE_INHERITED_C2(cellBeforeColumnBorderC_, "cell-before-column-border",
		     BorderC, makeFalse(), *this);
  STORE_INHERITED_C2(cellAfterColumnBorderC_, "cell-after-column-border",
		     BorderC, makeFalse(), *this);
  STORE_INHERITED_C2(fractionBarC_, "fraction-bar",
		     RuleC, lookup(makeStringC("rule"))->flowObj(), *this);
  INHERITED_C2("line-thickness", GenericLengthInheritedC, &FOTBuilder::setLineThickness,
               unitsPerInch()/72);
  INHERITED_C2("cell-before-row-margin", GenericLengthInheritedC,
               &FOTBuilder::setCellBeforeRowMargin, 0);
  INHERITED_C2("cell-after-row-margin", GenericLengthInheritedC,
               &FOTBuilder::setCellAfterRowMargin, 0);
  INHERITED_C2("cell-before-column-margin", GenericLengthInheritedC,
	       &FOTBuilder::setCellBeforeColumnMargin, 0);
  INHERITED_C2("cell-after-column-margin", GenericLengthInheritedC,
	       &FOTBuilder::setCellAfterColumnMargin, 0);
  INHERITED_C2("line-sep", GenericLengthInheritedC,
	       &FOTBuilder::setLineSep, unitsPerInch()/72);
  INHERITED_C2("box-size-before", GenericLengthInheritedC, &FOTBuilder::setBoxSizeBefore,
               8*unitsPerInch()/72);
  INHERITED_C2("box-size-after", GenericLengthInheritedC, &FOTBuilder::setBoxSizeAfter,
               4*unitsPerInch()/72);
  INHERITED_C2("position-point-shift", GenericLengthSpecInheritedC,
               &FOTBuilder::setPositionPointShift, 0);
  INHERITED_C2("start-margin", GenericLengthSpecInheritedC, &FOTBuilder::setStartMargin, 0);
  INHERITED_C2("end-margin", GenericLengthSpecInheritedC, &FOTBuilder::setEndMargin, 0);
  INHERITED_C2("sideline-sep", GenericLengthSpecInheritedC, &FOTBuilder::setSidelineSep,
               4*unitsPerInch()/72);
  INHERITED_C2("asis-wrap-indent", GenericLengthSpecInheritedC, &FOTBuilder::setAsisWrapIndent, 0);
  INHERITED_C2("line-number-sep", GenericLengthSpecInheritedC, &FOTBuilder::setLineNumberSep, 0);
  INHERITED_C2("last-line-justify-limit", GenericLengthSpecInheritedC,
               &FOTBuilder::setLastLineJustifyLimit, 0);
  INHERITED_C2("justify-glyph-space-max-add", GenericLengthSpecInheritedC,
               &FOTBuilder::setJustifyGlyphSpaceMaxAdd, 0);
  INHERITED_C2("justify-glyph-space-max-remove", GenericLengthSpecInheritedC,
               &FOTBuilder::setJustifyGlyphSpaceMaxRemove, 0);
  INHERITED_C2("table-corner-radius", GenericLengthSpecInheritedC,
               &FOTBuilder::setTableCornerRadius, 3*unitsPerInch()/72);
  INHERITED_C2("box-corner-radius", GenericLengthSpecInheritedC,
               &FOTBuilder::setBoxCornerRadius, 3*unitsPerInch()/72);
  INHERITED_C2("marginalia-sep", GenericLengthSpecInheritedC,
               &FOTBuilder::setMarginaliaSep, 0);
  INHERITED_C2("inhibit-line-breaks?", GenericBoolInheritedC, &FOTBuilder::setInhibitLineBreaks, 0);
  INHERITED_C2("hyphenate?", GenericBoolInheritedC, &FOTBuilder::setHyphenate, 0);
  INHERITED_C2("kern?", GenericBoolInheritedC, &FOTBuilder::setKern, 0);
  INHERITED_C2("ligature?", GenericBoolInheritedC, &FOTBuilder::setLigature, 0);
  INHERITED_C2("score-spaces?", GenericBoolInheritedC, &FOTBuilder::setScoreSpaces, 0);
  INHERITED_C2("float-out-sidelines?", GenericBoolInheritedC, &FOTBuilder::setFloatOutSidelines, 0);
  INHERITED_C2("float-out-marginalia?", GenericBoolInheritedC, &FOTBuilder::setFloatOutMarginalia, 0);
  INHERITED_C2("float-out-line-numbers?", GenericBoolInheritedC, &FOTBuilder::setFloatOutLineNumbers, 0);
  INHERITED_C2("cell-background?", GenericBoolInheritedC, &FOTBuilder::setCellBackground, 0);
  INHERITED_C2("span-weak?", GenericBoolInheritedC, &FOTBuilder::setSpanWeak, 0);
  INHERITED_C2("ignore-record-end?", GenericBoolInheritedC, &FOTBuilder::setIgnoreRecordEnd, 0);
  INHERITED_C2("numbered-lines?", GenericBoolInheritedC, &FOTBuilder::setNumberedLines, 1);
  INHERITED_C2("hanging-punct?", GenericBoolInheritedC, &FOTBuilder::setHangingPunct, 0);
  INHERITED_C2("box-open-end?", GenericBoolInheritedC, &FOTBuilder::setBoxOpenEnd, 0);
  INHERITED_C2("truncate-leader?", GenericBoolInheritedC, &FOTBuilder::setTruncateLeader, 0);
  INHERITED_C2("align-leader?", GenericBoolInheritedC, &FOTBuilder::setAlignLeader, 1);
  INHERITED_C2("table-part-omit-middle-header?", GenericBoolInheritedC,
	       &FOTBuilder::setTablePartOmitMiddleHeader, 0);
  INHERITED_C2("table-part-omit-middle-footer?", GenericBoolInheritedC,
	       &FOTBuilder::setTablePartOmitMiddleFooter, 0);
  INHERITED_C2("border-omit-at-break?", GenericBoolInheritedC, &FOTBuilder::setBorderOmitAtBreak, 0);
  INHERITED_C2("principal-mode-simultaneous?", GenericBoolInheritedC,
	       &FOTBuilder::setPrincipalModeSimultaneous, 0);
  INHERITED_C2("marginalia-keep-with-previous?", GenericBoolInheritedC,
	       &FOTBuilder::setMarginaliaKeepWithPrevious, 0);
  INHERITED_C2("grid-equidistant-rows?", GenericBoolInheritedC, &FOTBuilder::setGridEquidistantRows, 0);
  INHERITED_C2("grid-equidistant-columns?", GenericBoolInheritedC, &FOTBuilder::setGridEquidistantColumns, 0);
  INHERITED_C2("line-join", GenericSymbolInheritedC, &FOTBuilder::setLineJoin,
	       FOTBuilder::symbolMiter);
  INHERITED_C2("line-cap", GenericSymbolInheritedC, &FOTBuilder::setLineCap,
	       FOTBuilder::symbolButt);
  INHERITED_C2("line-number-side", GenericSymbolInheritedC, &FOTBuilder::setLineNumberSide,
	       FOTBuilder::symbolStart);
  INHERITED_C2("kern-mode", GenericSymbolInheritedC, &FOTBuilder::setKernMode,
	       FOTBuilder::symbolNormal);
  INHERITED_C2("input-whitespace-treatment", GenericSymbolInheritedC,
	       &FOTBuilder::setInputWhitespaceTreatment, FOTBuilder::symbolPreserve);
  INHERITED_C2("filling-direction", GenericSymbolInheritedC, &FOTBuilder::setFillingDirection,
	       FOTBuilder::symbolTopToBottom );
  INHERITED_C2("writing-mode", GenericSymbolInheritedC, &FOTBuilder::setWritingMode,
	       FOTBuilder::symbolLeftToRight );
  INHERITED_C2("last-line-quadding", GenericSymbolInheritedC, &FOTBuilder::setLastLineQuadding,
	       FOTBuilder::symbolRelative);
  INHERITED_C2("math-display-mode", GenericSymbolInheritedC, &FOTBuilder::setMathDisplayMode,
	       FOTBuilder::symbolDisplay);
  INHERITED_C2("script-pre-align", GenericSymbolInheritedC, &FOTBuilder::setScriptPreAlign,
               FOTBuilder::symbolIndependent);
  INHERITED_C2("script-post-align", GenericSymbolInheritedC, &FOTBuilder::setScriptPostAlign,
               FOTBuilder::symbolIndependent);
  INHERITED_C2("script-mid-sup-align", GenericSymbolInheritedC, &FOTBuilder::setScriptMidSupAlign,
               FOTBuilder::symbolCenter);
  INHERITED_C2("script-mid-sub-align", GenericSymbolInheritedC, &FOTBuilder::setScriptMidSubAlign,
               FOTBuilder::symbolCenter);
  INHERITED_C2("numerator-align", GenericSymbolInheritedC, &FOTBuilder::setNumeratorAlign,
               FOTBuilder::symbolCenter);
  INHERITED_C2("denominator-align", GenericSymbolInheritedC, &FOTBuilder::setDenominatorAlign,
               FOTBuilder::symbolCenter);
  INHERITED_C2("grid-position-cell-type", GenericSymbolInheritedC, &FOTBuilder::setGridPositionCellType,
               FOTBuilder::symbolRowMajor);
  INHERITED_C2("grid-column-alignment", GenericSymbolInheritedC, &FOTBuilder::setGridColumnAlignment,
               FOTBuilder::symbolCenter);
  INHERITED_C2("grid-row-alignment", GenericSymbolInheritedC, &FOTBuilder::setGridRowAlignment,
               FOTBuilder::symbolCenter);
  INHERITED_C2("box-type", GenericSymbolInheritedC, &FOTBuilder::setBoxType,
	       FOTBuilder::symbolBorder);
  INHERITED_C2("glyph-alignment-mode", GenericSymbolInheritedC, &FOTBuilder::setGlyphAlignmentMode,
	       FOTBuilder::symbolFont);
  INHERITED_C2("box-border-alignment", GenericSymbolInheritedC, &FOTBuilder::setBoxBorderAlignment,
	       FOTBuilder::symbolOutside);
  INHERITED_C2("cell-row-alignment", GenericSymbolInheritedC, &FOTBuilder::setCellRowAlignment,
	       FOTBuilder::symbolStart);
  INHERITED_C2("border-alignment", GenericSymbolInheritedC, &FOTBuilder::setBorderAlignment,
	       FOTBuilder::symbolCenter);
  INHERITED_C2("sideline-side", GenericSymbolInheritedC, &FOTBuilder::setSidelineSide,
	       FOTBuilder::symbolStart);
  INHERITED_C2("hyphenation-keep", GenericSymbolInheritedC, &FOTBuilder::setHyphenationKeep,
	       FOTBuilder::symbolFalse);
  INHERITED_C2("font-structure", GenericSymbolInheritedC, &FOTBuilder::setFontStructure,
	       FOTBuilder::symbolSolid);
  INHERITED_C2("font-proportionate-width", GenericSymbolInheritedC,
	       &FOTBuilder::setFontProportionateWidth, FOTBuilder::symbolMedium);
  INHERITED_C2("cell-crossed", GenericSymbolInheritedC, &FOTBuilder::setCellCrossed,
	       FOTBuilder::symbolFalse);
  INHERITED_C2("marginalia-side", GenericSymbolInheritedC, &FOTBuilder::setMarginaliaSide,
	       FOTBuilder::symbolStart);
  INHERITED_C2("layer", GenericIntegerInheritedC, &FOTBuilder::setLayer, 0);
  INHERITED_C2("background-layer", GenericIntegerInheritedC, &FOTBuilder::setBackgroundLayer, -1);
  INHERITED_C2("border-priority", GenericIntegerInheritedC, &FOTBuilder::setBorderPriority, 0);
  INHERITED_C2("line-repeat", GenericIntegerInheritedC, &FOTBuilder::setLineRepeat, 1);
  INHERITED_C2("span", GenericIntegerInheritedC, &FOTBuilder::setSpan, 1);
  INHERITED_C2("min-leader-repeat", GenericIntegerInheritedC, &FOTBuilder::setMinLeaderRepeat, 1);
  INHERITED_C2("hyphenation-remain-char-count", GenericIntegerInheritedC,
	       &FOTBuilder::setHyphenationRemainCharCount, 2);
  INHERITED_C2("hyphenation-push-char-count", GenericIntegerInheritedC,
	       &FOTBuilder::setHyphenationPushCharCount, 2);
  INHERITED_C2("widow-count", GenericIntegerInheritedC, &FOTBuilder::setWidowCount, 2);
  INHERITED_C2("orphan-count", GenericIntegerInheritedC, &FOTBuilder::setOrphanCount, 2);
  // #f or strictly positive integer
  INHERITED_C2("expand-tabs?", GenericMaybeIntegerInheritedC,
	       &FOTBuilder::setExpandTabs, 8);
  INHERITED_C2("hyphenation-ladder-count", GenericMaybeIntegerInheritedC,
	       &FOTBuilder::setHyphenationLadderCount, 0);
  INHERITED_C("background-tile", GenericPublicIdInheritedC, &FOTBuilder::setBackgroundTile);
  INHERITED_C("line-breaking-method", GenericPublicIdInheritedC, &FOTBuilder::setLineBreakingMethod);
  INHERITED_C("line-composition-method", GenericPublicIdInheritedC, &FOTBuilder::setLineCompositionMethod);
  INHERITED_C("implicit-bidi-method", GenericPublicIdInheritedC, &FOTBuilder::setImplicitBidiMethod);
  INHERITED_C("glyph-subst-method", GenericPublicIdInheritedC, &FOTBuilder::setGlyphSubstMethod);
  INHERITED_C("glyph-reorder-method", GenericPublicIdInheritedC, &FOTBuilder::setGlyphReorderMethod);
  INHERITED_C("hyphenation-method", GenericPublicIdInheritedC, &FOTBuilder::setHyphenationMethod);
  INHERITED_C("table-auto-width-method", GenericPublicIdInheritedC, &FOTBuilder::setTableAutoWidthMethod);
  INHERITED_C("font-name", GenericPublicIdInheritedC, &FOTBuilder::setFontName);
  // 2 letter symbol or #f
  INHERITED_C("language", GenericLetter2InheritedC, &FOTBuilder::setLanguage);
  INHERITED_C("country", GenericLetter2InheritedC, &FOTBuilder::setCountry);
  // Ignored characteristics
  ELObjDynamicRoot length0(*this, new (*this) LengthObj(0));
  // #f or length-spec
  INHERITED_C("min-pre-line-spacing", GenericOptLengthSpecInheritedC, &FOTBuilder::setMinPreLineSpacing);
  INHERITED_C("min-post-line-spacing", GenericOptLengthSpecInheritedC, &FOTBuilder::setMinPostLineSpacing);
  INHERITED_C("min-leading", GenericOptLengthSpecInheritedC, &FOTBuilder::setMinLeading);
  // inline spaces
  INHERITED_C("escapement-space-before", GenericInlineSpaceInheritedC, &FOTBuilder::setEscapementSpaceBefore);
  INHERITED_C("escapement-space-after", GenericInlineSpaceInheritedC, &FOTBuilder::setEscapementSpaceAfter);
  // #f or glyph-subst-table
  INHERITED_C("glyph-subst-table", GlyphSubstTableC, 0);

  // #f or inline-space
  IGNORED_C("inline-space-space", makeFalse());
  // integers
  // float
  IGNORED_C("line-miter-limit", makeInteger(10));
  // #f or percentage
  IGNORED_C("alignment-point-offset", makeInteger(50));
  // char
  IGNORED_C("hyphenation-char", new (*this) CharObj('-'));
  // char or #f
  IGNORED_C("asis-truncate-char", makeFalse());
  IGNORED_C("asis-wrap-char", makeFalse());
  // #f, #t or char
  IGNORED_C("first-line-align", makeFalse());
  // list of strings
  IGNORED_C("hyphenation-exceptions", makeNil());
  // set of corners
  IGNORED_C("box-corner-rounded", makeFalse());
  IGNORED_C("table-corner-rounded", makeFalse());
  // list of lengths
  IGNORED_C("line-dash", new (*this) PairObj(length0, makeNil()));
  // list of glyphs/chars
  IGNORED_C("allowed-ligatures", makeNil());
  // #f or sosofo
  IGNORED_C("line-number", makeFalse());
  // integer or 'force
  IGNORED_C("line-spacing-priority", makeInteger(0));
  // procedure or #f
  IGNORED_C("char-map", makeFalse());
}

void Interpreter::installInheritedC(const char *s, InheritedC *ic)
{
  StringC name(makeStringC(s));
  Identifier *ident = lookup(name);
  ic->setIdentifier(ident);
  ident->setInheritedC(ic);
  installInheritedCProc(ident);
  if (dsssl2() && name.size() && name[name.size() - 1] == '?') {
    name.resize(name.size() - 1);
    Identifier *ident2 = lookup(name);
    ASSERT(ident2->inheritedC().isNull());
    ident2->setInheritedC(ic);
    installInheritedCProc(ident2);
  }
}

void Interpreter::installExtensionInheritedC(Identifier *ident,
					     const StringC &pubid,
					     const Location &loc)
{
  ConstPtr<InheritedC> ic;
  if (pubid.size() != 0 && extensionTable_) {
    for (const FOTBuilder::Extension *ep = extensionTable_; ep->pubid; ep++) {
      if (pubid == ep->pubid) {
	if (ep->boolSetter)
	  ic = new ExtensionBoolInheritedC(ident, nInheritedC_++, ep->boolSetter);
	else if (ep->stringSetter)
	  ic = new ExtensionStringInheritedC(ident, nInheritedC_++, ep->stringSetter);
	else if (ep->integerSetter)
	  ic = new ExtensionIntegerInheritedC(ident, nInheritedC_++, ep->integerSetter);
	else if (ep->lengthSetter)
	  ic = new ExtensionLengthInheritedC(ident, nInheritedC_++, ep->lengthSetter);
	break;
      }
    }
  }
  if (ic.isNull())
    // FIXME should call FOTBuilder with PublicId argument
    ic = new IgnoredC(ident, nInheritedC_++, makeFalse(), *this);
  ident->setInheritedC(ic, currentPartIndex(), loc);
  installInheritedCProc(ident);
}

void Interpreter::installInheritedCProc(const Identifier *ident)
{
  const ConstPtr<InheritedC> &ic = ident->inheritedC();
  StringC tem(makeStringC("inherited-"));
  tem += ident->name();
  Identifier *inhIdent = lookup(tem);
  PrimitiveObj *prim = new (*this) InheritedCPrimitiveObj(ic);
  makePermanent(prim);
  prim->setIdentifier(inhIdent);
  inhIdent->setValue(prim);
  tem = makeStringC("actual-");
  tem += ident->name();
  Identifier *actIdent = lookup(tem);
  prim = new (*this) ActualCPrimitiveObj(ic);
  makePermanent(prim);
  prim->setIdentifier(actIdent);
  actIdent->setValue(prim);
}

#ifdef DSSSL_NAMESPACE
}
#endif
