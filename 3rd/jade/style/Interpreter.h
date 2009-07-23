// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef Interpreter_INCLUDED
#define Interpreter_INCLUDED 1

#include "ELObj.h"
#include "Expression.h"
#include "Message.h"
#include "PointerTable.h"
#include "NamedTable.h"
#include "Collector.h"
#include "InputSource.h"
#include "XcharMap.h"
#include "Owner.h"
#include "Style.h"
#include "SosofoObj.h"
#include "ProcessingMode.h"
#include "NumberCache.h"
#include "HashTable.h"
#include "FOTBuilder.h"
#include "Owner.h"
#include "Boolean.h"
#include "Node.h"
#include "GroveManager.h"
#include "Pattern.h"

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class Interpreter;

class Identifier : public Named {
public:
  enum SyntacticKey {
    notKey,
    keyQuote,
    keyLambda,
    keyIf,
    keyCond,
    keyAnd, 
    keyOr,
    keyCase,
    keyLet, 
    keyLetStar, 
    keyLetrec,
    keyQuasiquote,
    keyUnquote, 
    keyUnquoteSplicing,
    keyDefine,
    keyElse,
    keyArrow,
    keySet,
    keyBegin,
    keyMake,
    keyStyle,
    keyWithMode,
    keyDefineUnit,
    keyElement,
    keyDefault,
    keyRoot,
    keyId,
    keyMode,
    keyDeclareInitialValue,
    keyDeclareCharacteristic,
    keyDeclareFlowObjectClass,
    keyDeclareCharCharacteristicAndProperty,
    keyDeclareReferenceValueType,
    keyDeclareDefaultLanguage,
    keyDeclareCharProperty,
    keyDefinePageModel,
    keyDefineColumnSetModel,
    keyDefineLanguage,
    keyAddCharProperties,
    keyUse,
    keyLabel,
    keyContentMap,
    keyIsKeepWithPrevious,
    keyIsKeepWithNext,
    keySpaceBefore,
    keySpaceAfter,
    keyLeftHeader,
    keyCenterHeader,
    keyRightHeader,
    keyLeftFooter,
    keyCenterFooter,
    keyRightFooter,
    keyDestination,
    keyType,
    keyCoalesceId,
    keyIsDisplay,
    keyScale,
    keyMaxWidth,
    keyMaxHeight,
    keyEntitySystemId,
    keyNotationSystemId,
    keyPositionPointX,
    keyPositionPointY,
    keyEscapementDirection,
    keyBreakBeforePriority,
    keyBreakAfterPriority,
    keyOrientation,
    keyLength,
    keyChar,
    keyGlyphId,
    keyIsSpace,
    keyIsRecordEnd,
    keyIsInputTab,
    keyIsInputWhitespace,
    keyIsPunct,
    keyIsDropAfterLineBreak,
    keyIsDropUnlessBeforeLineBreak,
    keyMathClass,
    keyMathFontPosture,
    keyScript,
    keyStretchFactor,
    keyKeep,
    keyBreakBefore,
    keyBreakAfter,
    keyIsMayViolateKeepBefore,
    keyIsMayViolateKeepAfter,
    keyBeforeRowBorder,
    keyAfterRowBorder,
    keyBeforeColumnBorder,
    keyAfterColumnBorder,
    keyColumnNumber,
    keyRowNumber,
    keyNColumnsSpanned,
    keyNRowsSpanned,
    keyWidth,
    keyIsStartsRow,
    keyIsEndsRow,
    keyTableWidth,
    keyMultiModes,
    keyData,
    keyMin,
    keyMax,
    keyIsConditional,
    keyPriority,
    keyGridNRows,
    keyGridNColumns,
    keyRadical,
    keyNull,
    keyIsRcs,
    keyParent,
    keyActive,
    keyAttributes,
    keyChildren,
    keyRepeat,
    keyPosition,
    keyOnly,
    keyClass,
    keyImportance,
    keyDeclareClassAttribute,
    keyDeclareIdAttribute,
    keyDeclareFlowObjectMacro,
    keyOrElement,
    keyPositionPreference,
    keyArchitecture
  };
  enum { lastSyntacticKey = keyWithMode };
  Identifier(const StringC &name);
  // Return 0 is value can't yet be computed.
  ELObj *computeValue(bool force, Interpreter &) const;
  bool syntacticKey(SyntacticKey &) const;
  void setSyntacticKey(SyntacticKey);
  bool defined(unsigned &, Location &) const;
  void setDefinition(Owner<Expression> &, unsigned part,
                     const Location &);
  void setValue(ELObj *, unsigned defPart = unsigned(-1));
  bool evaluated() const;
  const ConstPtr<InheritedC> &inheritedC() const;
  bool inheritedCDefined(unsigned &, Location &) const;
  void setInheritedC(const ConstPtr<InheritedC> &);
  void setInheritedC(const ConstPtr<InheritedC> &, unsigned part,
		     const Location &);
  FlowObj *flowObj() const;
  bool flowObjDefined(unsigned &, Location &) const;
  void setFlowObj(FlowObj *);
  void setFlowObj(FlowObj *, unsigned part, const Location &);
private:
  unsigned defPart_;
  Owner<Expression> def_;
  InsnPtr insn_;
  // Value in top-level environment.
  ELObj *value_;		// must be permanent
  FlowObj *flowObj_;    // prototype FlowObj with this name
  unsigned flowObjPart_;
  Location flowObjLoc_;
  Location defLoc_;
  SyntacticKey syntacticKey_;
  bool beingComputed_;
  ConstPtr<InheritedC> inheritedC_;
  unsigned inheritedCPart_;
  Location inheritedCLoc_;
};

class Unit : public Named {
public:
  Unit(const StringC &);
  void setValue(long);
  void setValue(double);
  bool defined(unsigned &, Location &) const;
  // return 0 if it can't be done
  ELObj *resolveQuantity(bool force, Interpreter &, double val, int unitExp);
  ELObj *resolveQuantity(bool force, Interpreter &, long val, int valExp);
  void setDefinition(Owner<Expression> &, unsigned part, const Location &);
private:
  void tryCompute(bool force, Interpreter &);
  static bool scale(long val, int valExp, long num, long &result);

  unsigned defPart_;
  Location defLoc_;
  Owner<Expression> def_;
  InsnPtr insn_;
  enum {
    notComputed,
    beingComputed,
    computedExact,
    computedInexact,
    computedError
    } computed_;
  union {
    long exact_;
    double inexact_;
  };
  int dim_;
};

class ELObjDynamicRoot : public Collector::DynamicRoot {
public:
  ELObjDynamicRoot(Collector &c, ELObj *obj = 0)
    : Collector::DynamicRoot(c), obj_(obj) { }
  void operator=(ELObj *obj) { obj_ = obj; }
  operator ELObj *() const { return obj_; }
private:
  void trace(Collector &) const;
  ELObj *obj_;
};

class Interpreter : 
  public Collector,
  public Pattern::MatchContext,
  public NumberCache,
  public Messenger {
public:
  enum PortName {
    portNumerator,
    portDenominator,
    portPreSup,
    portPreSub,
    portPostSup,
    portPostSub,
    portMidSup,
    portMidSub,
    portOverMark,
    portUnderMark,
    portOpen,
    portClose,
    portDegree,
    portOperator,
    portLowerLimit,
    portUpperLimit,
    portHeader,
    portFooter
  };
  enum { nPortNames = portFooter + 1 };
  Interpreter(GroveManager *, Messenger *, int unitsPerInch, bool debugMode,
	      bool dsssl2, const FOTBuilder::Extension *);
  void defineVariable(const StringC &);
  void endPart();
  FalseObj *makeFalse();
  TrueObj *makeTrue();
  NilObj *makeNil();
  SymbolObj *makeSymbol(const StringC &);
  KeywordObj *makeKeyword(const StringC &);
  IntegerObj *makeInteger(long n);
  ErrorObj *makeError();
  UnspecifiedObj *makeUnspecified();
  PairObj *makePair(ELObj *, ELObj *);
  ELObj *convertGlyphId(const Char *, size_t, const Location &);
  bool isError(const ELObj *) const;
  bool isUnspecified(const ELObj *) const;
  CharObj *makeChar(Char);
  ELObj *makeLengthSpec(const FOTBuilder::LengthSpec &);
  AddressObj *makeAddressNone();
  NodeListObj *makeEmptyNodeList();
  void dispatchMessage(Message &);
  void dispatchMessage(const Message &);
  Identifier *lookup(const StringC &);
  Unit *lookupUnit(const StringC &);
  FunctionObj *lookupExternalProc(const StringC &);
  int unitsPerInch() const;
  unsigned currentPartIndex() const;
  void compile();
  static StringC makeStringC(const char *);
  SymbolObj *portName(PortName);
  ELObj *cValueSymbol(FOTBuilder::Symbol);
  // Map of LexCategory
  XcharMap<char> lexCategory_;
  static void normalizeGeneralName(const NodePtr &, StringC &);
  GroveManager *groveManager() const;
  StyleObj *initialStyle() const;
  StyleObj *borderTrueStyle() const;
  StyleObj *borderFalseStyle() const;
  bool convertBooleanC(ELObj *, const Identifier *, const Location &, bool &);
  bool convertPublicIdC(ELObj *, const Identifier *, const Location &,
			FOTBuilder::PublicId &);
  bool convertStringC(ELObj *, const Identifier *, const Location &, StringC &);
  bool convertLengthC(ELObj *, const Identifier *, const Location &, FOTBuilder::Length &);
  bool convertLengthSpecC(ELObj *, const Identifier *, const Location &, FOTBuilder::LengthSpec &);
  bool convertLetter2C(ELObj *, const Identifier *, const Location &, FOTBuilder::Letter2 &);
  bool convertOptLengthSpecC(ELObj *, const Identifier *, const Location &, FOTBuilder::OptLengthSpec &);
  bool convertCharC(ELObj *, const Identifier *, const Location &, Char &);
  bool convertColorC(ELObj *, const Identifier *, const Location &, ColorObj *&);
  bool convertOptColorC(ELObj *, const Identifier *, const Location &, ColorObj *&);
  // FIXME allow inexact value
  bool convertIntegerC(ELObj *, const Identifier *, const Location &, long &);
  bool convertOptPositiveIntegerC(ELObj *, const Identifier *, const Location &, long &);
  bool convertRealC(ELObj *, const Identifier *, const Location &, double &);
  bool convertEnumC(const FOTBuilder::Symbol *, size_t,
                    ELObj *, const Identifier *, const Location &, FOTBuilder::Symbol &);
  bool convertEnumC(ELObj *, const Identifier *, const Location &, FOTBuilder::Symbol &);
  void invalidCharacteristicValue(const Identifier *ident, const Location &loc);
  bool convertLengthSpec(ELObj *, FOTBuilder::LengthSpec &);
  bool convertToPattern(ELObj *, const Location &, Pattern &);
  const ConstPtr<InheritedC> &tableBorderC() const;
  const ConstPtr<InheritedC> &cellBeforeRowBorderC() const;
  const ConstPtr<InheritedC> &cellAfterRowBorderC() const;
  const ConstPtr<InheritedC> &cellBeforeColumnBorderC() const;
  const ConstPtr<InheritedC> &cellAfterColumnBorderC() const;
  const ConstPtr<InheritedC> &fractionBarC() const;
  const char *storePublicId(const Char *, size_t, const Location &);
  unsigned allocGlyphSubstTableUniqueId();
  bool lookupNodeProperty(const StringC &, ComponentName::Id &);
  bool debugMode() const;
  bool dsssl2() const;
  void setNodeLocation(const NodePtr &);
  void makeReadOnly(ELObj *);
  ProcessingMode *lookupProcessingMode(const StringC &);
  ProcessingMode *initialProcessingMode();
  void addClassAttributeName(const StringC &name);
  void addIdAttributeName(const StringC &name);
  void installInitialValue(Identifier *, Owner<Expression> &);
  void installExtensionInheritedC(Identifier *, const StringC &, const Location &);
  void installExtensionFlowObjectClass(Identifier *, const StringC &, const Location &);
  // Return 0 if an invalid number.
  ELObj *convertNumber(const StringC &, int radix = 10);
  bool convertCharName(const StringC &str, Char &c) const;
  enum LexCategory {
    lexLetter,			// a - z A - Z
    lexOtherNameStart,		// 
    lexDigit,			// 0-9
    lexOtherNumberStart,	// -+.
    lexDelimiter,		// ;()"
    lexWhiteSpace,
    lexOther
  };
  LexCategory lexCategory(Xchar);
private:
  Interpreter(const Interpreter &); // undefined
  void operator=(const Interpreter &); // undefined
  void installSyntacticKeys();
  void installPortNames();
  void installCValueSymbols();
  void installPrimitives();
  void installPrimitive(const char *s, PrimitiveObj *value);
  void installXPrimitive(const char *s, PrimitiveObj *value);
  void installUnits();
  void installCharNames();
  void installInheritedCs();
  void installInheritedC(const char *, InheritedC *);
  void installInheritedCProc(const Identifier *);
  void installFlowObjs();
  void installSdata();
  void installNodeProperties();
  void compileInitialValues();
  bool sdataMap(GroveString, GroveString, GroveChar &) const;
  static bool convertUnicodeCharName(const StringC &str, Char &c);
  bool convertToPattern(ELObj *obj, const Location &loc,
			bool isChild, IList<Pattern::Element> &list);
  bool patternAddAttributeQualifiers(ELObj *obj,
				     const Location &loc,
				     Pattern::Element &elem);
  enum {
    convertAllowBoolean = 01,
    convertAllowSymbol = 02,
    convertAllowNumber = 04
  };
  ELObj *convertFromString(ELObj *, unsigned hints, const Location &);
  ELObj *convertNumberFloat(const StringC &);
  bool scanSignDigits(const StringC &str, size_t &i, int &n);
  Unit *scanUnit(const StringC &str, size_t i, int &unitExp);

  NilObj *theNilObj_;
  TrueObj *theTrueObj_;
  FalseObj *theFalseObj_;
  ErrorObj *theErrorObj_;
  UnspecifiedObj *theUnspecifiedObj_;
  typedef PointerTable<SymbolObj *, StringC, Hash, SymbolObj>
    SymbolTable;
  SymbolTable symbolTable_;
  NamedTable<Identifier> identTable_;
  NamedTable<Unit> unitTable_;
  HashTable<StringC,FunctionObj *> externalProcTable_;
  Messenger *messenger_;
  const FOTBuilder::Extension *extensionTable_;
  unsigned partIndex_;
  int unitsPerInch_;
  unsigned nInheritedC_;
  GroveManager *groveManager_;
  ProcessingMode initialProcessingMode_;
  NamedTable<ProcessingMode> processingModeTable_;
  SymbolObj *portNames_[nPortNames];
  ELObj *cValueSymbols_[FOTBuilder::nSymbols];
  HashTable<StringC,Char> namedCharTable_;
  Vector<const Identifier *> initialValueNames_;
  NCVector<Owner<Expression> > initialValueValues_;
  size_t currentPartFirstInitialValue_;
  StyleObj *initialStyle_;
  StyleObj *borderTrueStyle_;
  StyleObj *borderFalseStyle_;
  ConstPtr<InheritedC> tableBorderC_;
  ConstPtr<InheritedC> cellBeforeRowBorderC_;
  ConstPtr<InheritedC> cellAfterRowBorderC_;
  ConstPtr<InheritedC> cellBeforeColumnBorderC_;
  ConstPtr<InheritedC> cellAfterColumnBorderC_;
  ConstPtr<InheritedC> fractionBarC_;
  class StringSet {
  public:
    StringSet();
    const char *store(String<char> &);
    static unsigned long hash(const String<char> &);
    static inline const String<char> &key(const String<char> &str) { return str; }
  private:
    OwnerTable<String<char>, String<char>, StringSet, StringSet> table_;
  };
  StringSet publicIds_;
  HashTable<StringC,Char> sdataEntityNameTable_;
  unsigned nextGlyphSubstTableUniqueId_;
  AddressObj *addressNoneObj_;
  NodeListObj *emptyNodeListObj_;
  HashTable<StringC,int> nodePropertyTable_;
  bool debugMode_;
  bool dsssl2_;
  friend class Identifier;
};

inline
ErrorObj *Interpreter::makeError()
{
  return theErrorObj_;
}

inline
bool Interpreter::isError(const ELObj *obj) const
{
  return obj == theErrorObj_;
}

inline
bool Interpreter::isUnspecified(const ELObj *obj) const
{
  return obj == theUnspecifiedObj_;
}

inline
FalseObj *Interpreter::makeFalse()
{
  return theFalseObj_;
}

inline
TrueObj *Interpreter::makeTrue()
{
  return theTrueObj_;
}

inline
NilObj *Interpreter::makeNil()
{
  return theNilObj_;
}

inline
UnspecifiedObj *Interpreter::makeUnspecified()
{
  return theUnspecifiedObj_;
}

inline
IntegerObj *Interpreter::makeInteger(long n)
{
  return new (*this) IntegerObj(n);
}

inline
PairObj *Interpreter::makePair(ELObj *car, ELObj *cdr)
{
  return new (*this) PairObj(car, cdr);
}

inline
CharObj *Interpreter::makeChar(Char c)
{
  return new (*this) CharObj(c);
}

inline
AddressObj *Interpreter::makeAddressNone()
{
  return addressNoneObj_;
}

inline
NodeListObj *Interpreter::makeEmptyNodeList()
{
  return emptyNodeListObj_;
}

inline
ELObj *Interpreter::cValueSymbol(FOTBuilder::Symbol sym)
{
  return cValueSymbols_[sym];
}

inline
SymbolObj *Interpreter::portName(PortName i)
{
  return portNames_[i];
}

inline
ProcessingMode *Interpreter::initialProcessingMode()
{
  return &initialProcessingMode_;
}

inline
int Interpreter::unitsPerInch() const
{
  return unitsPerInch_;
}

inline
unsigned Interpreter::currentPartIndex() const
{
  return partIndex_;
}

inline
KeywordObj *Interpreter::makeKeyword(const StringC &str)
{
  return new (*this) KeywordObj(lookup(str));
}

inline
StyleObj *Interpreter::initialStyle() const
{
  return initialStyle_;
}

inline
StyleObj *Interpreter::borderTrueStyle() const
{
  return borderTrueStyle_;
}

inline
StyleObj *Interpreter::borderFalseStyle() const
{
  return borderFalseStyle_;
}

inline
GroveManager *Interpreter::groveManager() const
{
  return groveManager_;
}

inline
const ConstPtr<InheritedC> &Interpreter::tableBorderC() const
{
  return tableBorderC_;
}

inline
const ConstPtr<InheritedC> &Interpreter::cellBeforeRowBorderC() const
{
  return cellBeforeRowBorderC_;
}

inline
const ConstPtr<InheritedC> &Interpreter::cellAfterRowBorderC() const
{
  return cellAfterRowBorderC_;
}

inline
const ConstPtr<InheritedC> &Interpreter::cellBeforeColumnBorderC() const
{
  return cellBeforeColumnBorderC_;
}

inline
const ConstPtr<InheritedC> &Interpreter::cellAfterColumnBorderC() const
{
  return cellAfterColumnBorderC_;
}

inline
const ConstPtr<InheritedC> &Interpreter::fractionBarC() const
{
  return fractionBarC_;
}

inline
FunctionObj *Interpreter::lookupExternalProc(const StringC &pubid)
{
  FunctionObj *const *func = externalProcTable_.lookup(pubid);
  return func ? *func : 0;
}

inline
unsigned Interpreter::allocGlyphSubstTableUniqueId()
{
  return nextGlyphSubstTableUniqueId_++;
}

inline
bool Interpreter::debugMode() const
{
  return debugMode_;
}

inline
bool Interpreter::dsssl2() const
{
  return dsssl2_;
}

inline
void Interpreter::makeReadOnly(ELObj *obj)
{
  if (dsssl2())
    Collector::makeReadOnly(obj);
}

inline
void Interpreter::addClassAttributeName(const StringC &name)
{
  classAttributeNames_.push_back(name);
}

inline
void Interpreter::addIdAttributeName(const StringC &name)
{
  idAttributeNames_.push_back(name);
}

inline
Interpreter::LexCategory Interpreter::lexCategory(Xchar c)
{
  return LexCategory(lexCategory_[c]);
}

inline
bool Identifier::syntacticKey(SyntacticKey &key) const
{
  if (syntacticKey_ == notKey)
    return 0;
  key = syntacticKey_;
  return 1;
}

inline
void Identifier::setSyntacticKey(SyntacticKey key)
{
  syntacticKey_ = key;
}

inline
bool Identifier::evaluated() const
{
  return value_ != 0;
}

inline
const ConstPtr<InheritedC> &Identifier::inheritedC() const
{
  return inheritedC_;
}

inline
bool Identifier::inheritedCDefined(unsigned &part, Location &loc) const
{
  if (inheritedC_.isNull())
    return 0;
  part = inheritedCPart_;
  loc = inheritedCLoc_;
  return 1;
}

inline
void Identifier::setInheritedC(const ConstPtr<InheritedC> &ic)
{
  inheritedC_ = ic;
  inheritedCPart_ = unsigned(-1);
}

inline
void Identifier::setInheritedC(const ConstPtr<InheritedC> &ic, unsigned part,
			       const Location &loc)
{
  inheritedC_ = ic;
  inheritedCPart_ = part;
  inheritedCLoc_ = loc;
}

inline
FlowObj *Identifier::flowObj() const
{
  return flowObj_;
}

inline
bool Identifier::flowObjDefined(unsigned &part, Location &loc) const
{
  if (!flowObj_)
    return 0;
  part = flowObjPart_;
  loc = flowObjLoc_;
  return 1;
}

inline
void Identifier::setFlowObj(FlowObj *fo)
{
  flowObj_ = fo;
  flowObjPart_ = unsigned(-1);
}

inline
void Identifier::setFlowObj(FlowObj *fo, unsigned part, const Location &loc)
{
  flowObj_ = fo;
  flowObjPart_ = part;
  flowObjLoc_ = loc;
}

#ifdef DSSSL_NAMESPACE
}
#endif

#endif /* not Interpreter_INCLUDED */
