// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#include "stylelib.h"
#include "Interpreter.h"
#include "Insn.h"
#include "InterpreterMessages.h"
#include "LocNode.h"
#include "Pattern.h"
#include "MacroFlowObj.h"
#include "ELObjMessageArg.h"
#include "VM.h"
#include "macros.h"
#include <stdlib.h>

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

const Char defaultChar = 0xfffd;

static
size_t maxObjSize()
{
  static size_t sz[] = {
    sizeof(UnresolvedQuantityObj),
    sizeof(VarStyleObj),
    sizeof(OverriddenStyleObj),
    sizeof(MergeStyleObj),
    sizeof(DeviceRGBColorObj),
    sizeof(ColorSpaceObj),
    sizeof(PairObj),
    sizeof(QuantityObj),
    sizeof(GlyphIdObj),
    sizeof(NamedNodeListPtrNodeListObj),
    sizeof(ProcessNodeSosofoObj),
    sizeof(AppendSosofoObj),
    sizeof(SetNonInheritedCsSosofoObj),
    sizeof(LabelSosofoObj),
    sizeof(MacroFlowObj),
    sizeof(FlowObj) + sizeof(StringC), // for FormattingInstructionFlowObj
  };
  size_t n = sz[0];
  for (size_t i = 1; i < SIZEOF(sz); i++)
    if (sz[i] > n)
      n = sz[i];
  return n;
}

Interpreter::Interpreter(GroveManager *groveManager,
			 Messenger *messenger,
			 int unitsPerInch,
			 bool debugMode,
			 bool dsssl2,
			 const FOTBuilder::Extension *extensionTable)
: groveManager_(groveManager),
  messenger_(messenger),
  extensionTable_(extensionTable),
  Collector(maxObjSize()),
  partIndex_(1),  // 0 is for command-line definitions
  lexCategory_(lexOther),
  unitsPerInch_(unitsPerInch),
  nInheritedC_(0),
  initialProcessingMode_(StringC()),
  currentPartFirstInitialValue_(0),
  initialStyle_(0),
  nextGlyphSubstTableUniqueId_(0),
  debugMode_(debugMode),
  dsssl2_(dsssl2)
{
  makePermanent(theNilObj_ = new (*this) NilObj);
  makePermanent(theFalseObj_ = new (*this) FalseObj);
  makePermanent(theTrueObj_ = new (*this) TrueObj);
  makePermanent(theErrorObj_ = new (*this) ErrorObj);
  makePermanent(theUnspecifiedObj_ = new (*this) UnspecifiedObj);
  makePermanent(addressNoneObj_
                = new (*this) AddressObj(FOTBuilder::Address::none));
  makePermanent(emptyNodeListObj_
		= new (*this) NodePtrNodeListObj);
  installSyntacticKeys();
  installCValueSymbols();
  installPortNames();
  installPrimitives();
  installUnits();
  installCharNames();
  installFlowObjs();
  installInheritedCs();
  installSdata();
  installNodeProperties();

  static const char *lexCategories[] = {
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
    "!$%&*/<=>?~_^:",
    "0123456789",
    "-+.",
    "();\"",
    " \t\r\n\f",
  };
      //
  for (size_t i = 0; i < SIZEOF(lexCategories); i++)
    for (const char *s = lexCategories[i]; *s; s++)
      lexCategory_.setChar(*s, i);
  initialProcessingMode_.setDefined();
}

void Interpreter::compile()
{
  // FIXME compile all definitions
  compileInitialValues();
  initialProcessingMode_.compile(*this);
  NamedTableIter<ProcessingMode> iter(processingModeTable_);
  for (;;) {
    ProcessingMode *mode = iter.next();
    if (!mode)
      break;
    mode->compile(*this);
  }
}

void Interpreter::compileInitialValues()
{
  Vector<ConstPtr<InheritedC> > ics;
  for (size_t i = 0; i < initialValueNames_.size(); i++) {
    const Identifier *ident = initialValueNames_[i];
    Owner<Expression> &expr = initialValueValues_[i];
    ConstPtr<InheritedC> ic(ident->inheritedC());
    expr->optimize(*this, Environment(), expr);
    ELObj *val = expr->constantValue();
    if (val) {
      ConstPtr<InheritedC> tem(ic->make(val, expr->location(), *this));
      if (!tem.isNull())
	ics.push_back(tem);
    }
    else
      ics.push_back(new VarInheritedC(ic,
    			              expr->compile(*this, Environment(), 0, InsnPtr()),
			              expr->location()));
  }
  if (ics.size()) {
    Vector<ConstPtr<InheritedC> > forceIcs;
    initialStyle_ = new (*this) VarStyleObj(new StyleSpec(forceIcs, ics), 0, 0, NodePtr());
    makePermanent(initialStyle_);
  }
}

void Interpreter::installInitialValue(Identifier *ident, Owner<Expression> &expr)
{
  for (size_t i = 0; i < initialValueNames_.size(); i++) {
    if (ident == initialValueNames_[i]) {
      if (i >= currentPartFirstInitialValue_) {
	setNextLocation(expr->location());
	message(InterpreterMessages::duplicateInitialValue,
	        StringMessageArg(ident->name()),
		initialValueValues_[i]->location());
      }
      return;
    }
  }
  initialValueValues_.resize(initialValueValues_.size() + 1);
  expr.swap(initialValueValues_.back());
  initialValueNames_.push_back(ident);
}

void Interpreter::defineVariable(const StringC &str)
{
  lookup(str)->setValue(makeTrue(), 0);
}

void Interpreter::installUnits()
{
  static struct {
    const char *name;
    int numer;
    int denom;
    bool dsssl2;
  } units[] = {
    { "m", 5000, 127 },
    { "cm", 50, 127 },
    { "mm", 5, 127 },
    { "in", 1, 1 },
    { "pt", 1, 72 },
    { "pica", 1, 6 },
    { "pc", 1, 6 } // a DSSSL2 addition
  };
  size_t nUnits = dsssl2() ? SIZEOF(units) : SIZEOF(units) - 1;
  for (size_t i = 0; i < nUnits; i++) {
    Unit *unit = lookupUnit(makeStringC(units[i].name));
    long n = unitsPerInch_ * units[i].numer;
    if (n % units[i].denom == 0)
      unit->setValue(long(n / units[i].denom));
    else
      unit->setValue(double(n)/units[i].denom);
  }
}

void Interpreter::installSyntacticKeys()
{
  static struct {
    const char *name;
    Identifier::SyntacticKey key;
  } keys[] = {
    { "quote", Identifier::keyQuote },
    { "lambda", Identifier::keyLambda },
    { "if", Identifier::keyIf },
    { "cond", Identifier::keyCond },
    { "and", Identifier::keyAnd },
    { "or", Identifier::keyOr },
    { "case", Identifier::keyCase },
    { "let", Identifier::keyLet },
    { "let*", Identifier::keyLetStar },
    { "letrec", Identifier::keyLetrec },
    { "quasiquote", Identifier::keyQuasiquote },
    { "unquote", Identifier::keyUnquote },
    { "unquote-splicing", Identifier::keyUnquoteSplicing },
    { "define", Identifier::keyDefine },
    { "else", Identifier::keyElse },
    { "=>", Identifier::keyArrow },
    { "make", Identifier::keyMake },
    { "style", Identifier::keyStyle },
    { "with-mode", Identifier::keyWithMode },
    { "define-unit", Identifier::keyDefineUnit },
    { "element", Identifier::keyElement },
    { "default", Identifier::keyDefault },
    { "root", Identifier::keyRoot },
    { "id", Identifier::keyId },
    { "mode", Identifier::keyMode },
    { "declare-initial-value", Identifier::keyDeclareInitialValue },
    { "declare-characteristic", Identifier::keyDeclareCharacteristic },
    { "declare-flow-object-class", Identifier::keyDeclareFlowObjectClass },
    { "declare-char-characteristic+property", Identifier::keyDeclareCharCharacteristicAndProperty },
    { "declare-reference-value-type", Identifier::keyDeclareReferenceValueType },
    { "declare-default-language", Identifier::keyDeclareDefaultLanguage },
    { "declare-char-property", Identifier::keyDeclareCharProperty },
    { "define-page-model", Identifier::keyDefinePageModel },
    { "define-column-set-model", Identifier::keyDefineColumnSetModel },
    { "define-language", Identifier::keyDefineLanguage },
    { "add-char-properties", Identifier::keyAddCharProperties },
    { "use", Identifier::keyUse },
    { "label", Identifier::keyLabel },
    { "content-map", Identifier::keyContentMap },
    { "keep-with-previous?", Identifier::keyIsKeepWithPrevious },
    { "keep-with-next?", Identifier::keyIsKeepWithNext },
    { "space-before", Identifier::keySpaceBefore },
    { "space-after", Identifier::keySpaceAfter },
    { "left-header", Identifier::keyLeftHeader },
    { "center-header", Identifier::keyCenterHeader },
    { "right-header", Identifier::keyRightHeader },
    { "left-footer", Identifier::keyLeftFooter },
    { "center-footer", Identifier::keyCenterFooter },
    { "right-footer", Identifier::keyRightFooter },
    { "destination", Identifier::keyDestination },
    { "type", Identifier::keyType },
    { "coalesce-id", Identifier::keyCoalesceId },
    { "display?", Identifier::keyIsDisplay },
    { "scale", Identifier::keyScale },
    { "max-width", Identifier::keyMaxWidth },
    { "max-height", Identifier::keyMaxHeight },
    { "entity-system-id", Identifier::keyEntitySystemId },
    { "notation-system-id", Identifier::keyNotationSystemId },
    { "position-point-x", Identifier::keyPositionPointX },
    { "position-point-y", Identifier::keyPositionPointY },
    { "escapement-direction", Identifier::keyEscapementDirection },
    { "break-before-priority", Identifier::keyBreakBeforePriority },
    { "break-after-priority", Identifier::keyBreakAfterPriority },
    { "orientation", Identifier::keyOrientation },
    { "length", Identifier::keyLength },
    { "char", Identifier::keyChar },
    { "glyph-id", Identifier::keyGlyphId },
    { "space?", Identifier::keyIsSpace },
    { "record-end?", Identifier::keyIsRecordEnd },
    { "input-tab?", Identifier::keyIsInputTab },
    { "input-whitespace?", Identifier::keyIsInputWhitespace },
    { "punct?", Identifier::keyIsPunct },
    { "drop-after-line-break?", Identifier::keyIsDropAfterLineBreak },
    { "drop-unless-before-line-break?", Identifier::keyIsDropUnlessBeforeLineBreak },
    { "math-class", Identifier::keyMathClass },
    { "math-font-posture", Identifier::keyMathFontPosture },
    { "script", Identifier::keyScript },
    { "stretch-factor", Identifier::keyStretchFactor },
    { "keep", Identifier::keyKeep },
    { "break-before", Identifier::keyBreakBefore },
    { "break-after", Identifier::keyBreakAfter },
    { "may-violate-keep-before?", Identifier::keyIsMayViolateKeepBefore },
    { "may-violate-keep-after?", Identifier::keyIsMayViolateKeepAfter },
    { "before-row-border", Identifier::keyBeforeRowBorder },
    { "after-row-border", Identifier::keyAfterRowBorder },
    { "before-column-border", Identifier::keyBeforeColumnBorder },
    { "after-column-border", Identifier::keyAfterColumnBorder },
    { "column-number", Identifier::keyColumnNumber },
    { "row-number", Identifier::keyRowNumber },
    { "n-columns-spanned", Identifier::keyNColumnsSpanned },
    { "n-rows-spanned", Identifier::keyNRowsSpanned },
    { "width", Identifier::keyWidth },
    { "starts-row?", Identifier::keyIsStartsRow },
    { "ends-row?", Identifier::keyIsEndsRow },
    { "table-width", Identifier::keyTableWidth },
    { "multi-modes", Identifier::keyMultiModes },
    { "data", Identifier::keyData },
    { "min", Identifier::keyMin },
    { "max", Identifier::keyMax },
    { "conditional?", Identifier::keyIsConditional },
    { "priority", Identifier::keyPriority },
    { "grid-n-rows", Identifier::keyGridNRows },
    { "grid-n-columns", Identifier::keyGridNColumns },
    { "radical", Identifier::keyRadical },
    { "null", Identifier::keyNull },
    { "rcs?", Identifier::keyIsRcs },
    { "parent", Identifier::keyParent },
    { "active", Identifier::keyActive },
    { "attributes", Identifier::keyAttributes },
    { "children", Identifier::keyChildren },
    { "repeat", Identifier::keyRepeat },
    { "position", Identifier::keyPosition },
    { "only", Identifier::keyOnly },
    { "class", Identifier::keyClass },
    { "importance", Identifier::keyImportance },
    { "position-preference", Identifier::keyPositionPreference },
    { "architecture", Identifier::keyArchitecture },
  }, keys2[] = {
    { "declare-class-attribute", Identifier::keyDeclareClassAttribute },
    { "declare-id-attribute", Identifier::keyDeclareIdAttribute },
    { "declare-flow-object-macro", Identifier::keyDeclareFlowObjectMacro },
    { "or-element", Identifier::keyOrElement },
    { "set!", Identifier::keySet },
    { "begin", Identifier::keyBegin },
  };
  for (size_t i = 0; i < SIZEOF(keys); i++) {
    StringC tem(makeStringC(keys[i].name));
    lookup(tem)->setSyntacticKey(keys[i].key);
    if (dsssl2() && tem[tem.size() - 1] == '?') {
      tem.resize(tem.size() - 1);
      lookup(tem)->setSyntacticKey(keys[i].key);
    }
  }
  if (dsssl2()) {
    for (size_t i = 0; i < SIZEOF(keys2); i++)
      lookup(makeStringC(keys2[i].name))->setSyntacticKey(keys2[i].key);
  }
}

void Interpreter::installCValueSymbols()
{
  cValueSymbols_[0] = makeFalse();
  cValueSymbols_[1] = makeTrue();
  for (size_t i = 2; i < FOTBuilder::nSymbols; i++) {
    SymbolObj *sym = makeSymbol(makeStringC(FOTBuilder::symbolName(FOTBuilder::Symbol(i))));
    sym->setCValue(FOTBuilder::Symbol(i));
    cValueSymbols_[i] = sym;
  }
}

void Interpreter::installPortNames()
{
  // These must match the order in SymbolObj.
  static const char *names[] = {
    "numerator",
    "denominator",
    "pre-sup",
    "pre-sub",
    "post-sup",
    "post-sub",
    "mid-sup",
    "mid-sub",
    "over-mark",
    "under-mark",
    "open",
    "close",
    "degree",
    "operator",
    "lower-limit",
    "upper-limit",
    "header",
    "footer"
  };
  ASSERT(SIZEOF(names) == nPortNames);
  for (size_t i = 0; i < SIZEOF(names); i++)
    portNames_[i] = makeSymbol(makeStringC(names[i]));
}

void Interpreter::installCharNames()
{
  static struct {
    Char c;
    const char *name;
  } chars[] = {
#include "charNames.h"
  };
  for (size_t i = 0; i < SIZEOF(chars); i++)
    namedCharTable_.insert(makeStringC(chars[i].name), chars[i].c);
}

void Interpreter::installSdata()
{
  // This comes from uni2sgml.txt on ftp://unicode.org.
  // It is marked there as obsolete, so it probably ought to be checked.
  // The definitions of apos and quot have been fixed for consistency with XML.
  static struct {
    Char c;
    const char *name;
  } entities[] = {
#include "sdata.h"
  };
  for (size_t i = 0; i < SIZEOF(entities); i++)
    sdataEntityNameTable_.insert(makeStringC(entities[i].name), entities[i].c);
}

void Interpreter::installNodeProperties()
{
  for (int i = 0; i < ComponentName::nIds; i++) {
    ComponentName::Id id = ComponentName::Id(i);
    nodePropertyTable_.insert(makeStringC(ComponentName::rcsName(id)), i);
    nodePropertyTable_.insert(makeStringC(ComponentName::sdqlName(id)), i);
  }
}

bool Interpreter::sdataMap(GroveString name, GroveString, GroveChar &c) const
{
  StringC tem(name.data(), name.size());
  const Char *cp = sdataEntityNameTable_.lookup(tem);
  if (cp) {
    c = *cp;
    return 1;
  }
  if (convertUnicodeCharName(tem, c))
    return 1;
  // I think this is the most thing to do.
  // At least it makes preserve-sdata work with unknown SDATA entities.
  c = defaultChar;
  return 1;
}

ELObj *Interpreter::convertGlyphId(const Char *str, size_t len, const Location &loc)
{
  unsigned long n = 0;
  const char *publicId = 0;
  for (size_t i = len; i > 1; --i) {
    if (str[i - 1] == ':' && str[i - 2] == ':' && i < len && str[i] != '0') {
      for (size_t j = i; j < len; j++)
	n = n*10 + (str[j] - '0');
      publicId = storePublicId(str, i - 2, loc);
      break;
    }
    if (str[i - 1] < '0' || str[i - 1] > '9')
      break;
  }
  if (!publicId)
    publicId = storePublicId(str, len, loc);
  return new (*this) GlyphIdObj(FOTBuilder::GlyphId(publicId, n));
}

bool Interpreter::convertCharName(const StringC &str, Char &c) const
{
  const Char *cp = namedCharTable_.lookup(str);
  if (cp) {
    c = *cp;
    return 1;
  }
  return convertUnicodeCharName(str, c);
}

bool Interpreter::convertUnicodeCharName(const StringC &str, Char &c)
{
  if (str.size() != 6 || str[0] != 'U' || str[1] != '-')
    return 0;
  Char value = 0;
  for (int i = 2; i < 6; i++) {
    value <<= 4;
    switch (str[i]) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      value |= str[i] - '0';
      break;
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
      value |= 10 + (str[i] - 'A');
      break;
    default:
      return 0;
    }
  }
  c = value;
  return 1;
}

SymbolObj *Interpreter::makeSymbol(const StringC &str)
{
  SymbolObj *sym = symbolTable_.lookup(str);
  if (!sym) {
    StringObj *strObj = new (*this) StringObj(str);
    makePermanent(strObj);
    sym = new (*this) SymbolObj(strObj);
    makePermanent(sym);
    symbolTable_.insert(sym);
  }
  return sym;
}

Identifier *Interpreter::lookup(const StringC &str)
{
  Identifier *ident = identTable_.lookup(str);
  if (!ident) {
    ident = new Identifier(str);
    identTable_.insert(ident);
  }
  return ident;
}

bool Interpreter::lookupNodeProperty(const StringC &str, ComponentName::Id &id)
{
  const int *val = nodePropertyTable_.lookup(str);
  if (!val) {
    StringC tem(str);
    for (size_t i = 0; i < tem.size(); i++) {
      if (tem[i] >= 'A' && tem[i] <= 'Z')
        tem[i] = 'a' + (tem[i] - 'A');
    }
    val = nodePropertyTable_.lookup(tem);
    if (!val)
      return 0;
  }
  id = ComponentName::Id(*val);
  return 1;
}

Unit *Interpreter::lookupUnit(const StringC &str)
{
  Unit *unit = unitTable_.lookup(str);
  if (!unit) {
    unit = new Unit(str);
    unitTable_.insert(unit);
  }
  return unit;
}

ProcessingMode *Interpreter::lookupProcessingMode(const StringC &str)
{
  ProcessingMode *mode = processingModeTable_.lookup(str);
  if (!mode) {
    mode = new ProcessingMode(str, &initialProcessingMode_);
    processingModeTable_.insert(mode);
  }
  return mode;
}

StringC Interpreter::makeStringC(const char *s)
{
  StringC tem;
  if (s)
    while (*s)
      tem += (unsigned char)*s++;
  return tem;
}

void Interpreter::endPart()
{
  currentPartFirstInitialValue_ = initialValueNames_.size();
  partIndex_++;
}

void Interpreter::normalizeGeneralName(const NodePtr &nd, StringC &str)
{
  NamedNodeListPtr nnl;
  NodePtr root;
  if (nd->getGroveRoot(root) == accessOK
      && root->getElements(nnl) == accessOK)
    str.resize(nnl->normalize(str.begin(), str.size()));
}

ELObj *Interpreter::makeLengthSpec(const FOTBuilder::LengthSpec &ls)
{
  if (ls.displaySizeFactor != 0.0) {
    LengthSpec result(LengthSpec::displaySize, ls.displaySizeFactor);
    result += double(ls.length);
    return new (*this) LengthSpecObj(result);
  }
  else
    return new (*this) LengthObj(ls.length);
}

bool Interpreter::convertBooleanC(ELObj *obj, const Identifier *ident, const Location &loc,
				  bool &result)
{
  obj = convertFromString(obj, convertAllowBoolean, loc);
  if (obj == makeFalse()) {
    result = 0;
    return 1;
  }
  if (obj == makeTrue()) {
    result = 1;
    return 1;
  }
  invalidCharacteristicValue(ident, loc);
  return 0;
}

bool Interpreter::convertPublicIdC(ELObj *obj, const Identifier *ident,
				   const Location &loc,
				   FOTBuilder::PublicId &pubid)
{
  if (obj == makeFalse()) {
    pubid = 0;
    return 1;
  }
  const Char *s;
  size_t n;
  if (obj->stringData(s, n)) {
    if (n == 0)
      pubid = 0;
    else
      pubid = storePublicId(s, n, loc);
    return 1;
  }
  invalidCharacteristicValue(ident, loc);
  return 0;
}

const char *Interpreter::storePublicId(const Char *s, size_t n, const Location &loc)
{
  String<char> buf;
  for (; n > 0; s++, n--) {
    if (*s >= 128) {
      setNextLocation(loc);
      message(InterpreterMessages::invalidPublicIdChar,
	      StringMessageArg(StringC(s, 1)));
    }
    else
      buf += char(*s);
  }
  buf += '\0';
  return publicIds_.store(buf);
}

bool Interpreter::convertStringC(ELObj *obj, const Identifier *ident, const Location &loc,
				 StringC &result)
{
  const Char *s;
  size_t n;
  if (obj->stringData(s, n)) {
    result.assign(s, n);
    return 1;
  }
  invalidCharacteristicValue(ident, loc);
  return 0;
}

bool Interpreter::convertLengthSpec(ELObj *obj,
				    FOTBuilder::LengthSpec &result)
{
  int dim;
  double d;
  switch (obj->quantityValue(result.length, d, dim)) {
  case ELObj::longQuantity:
    if (dim == 1)
      return 1;
    break;
  case ELObj::doubleQuantity:
    if (dim == 1) {
      // FIXME catch overflow
      result.length = d < 0.0 ? long(d - .5) : long(d + .5);
      return 1;
    }
    break;
  default:
    {
      const LengthSpec *ls = obj->lengthSpec();
      if (ls)
	return ls->convert(result);
    }
    break;
  }
  return 0;
}

bool Interpreter::convertLengthC(ELObj *obj, const Identifier *ident,
				 const Location &loc,
				 FOTBuilder::Length &n)
{
  obj = convertFromString(obj, convertAllowNumber, loc);
  int dim;
  double d;
  switch (obj->quantityValue(n, d, dim)) {
  case ELObj::longQuantity:
    if (dim == 1)
      return 1;
    break;
  case ELObj::doubleQuantity:
    if (dim == 1) {
      // FIXME catch overflow
      n = long(d);
      return 1;
    }
    break;
  default:
    break;
  }
  invalidCharacteristicValue(ident, loc);
  return 0;
}

bool Interpreter::convertLengthSpecC(ELObj *obj, const Identifier *ident,
				     const Location &loc,
				     FOTBuilder::LengthSpec &result)
{
  obj = convertFromString(obj, convertAllowNumber, loc);
  if (convertLengthSpec(obj, result))
    return 1;
  invalidCharacteristicValue(ident, loc);
  return 0;
}

bool Interpreter::convertOptLengthSpecC(ELObj *obj, const Identifier *ident,
					const Location &loc,
					FOTBuilder::OptLengthSpec &result)
{
  obj = convertFromString(obj, convertAllowBoolean|convertAllowNumber, loc);
  if (obj == makeFalse()) {
    result.hasLength = 0;
    return 1;
  }
  if (convertLengthSpecC(obj, ident, loc, result.length)) {
    result.hasLength = 1;
    return 1;
  }
  return 0;
}

bool Interpreter::convertOptPositiveIntegerC(ELObj *obj, const Identifier *ident, const Location &loc,
					     long &result)
{
  obj = convertFromString(obj, convertAllowNumber|convertAllowBoolean, loc);
  if (obj == makeFalse()) {
    result = 0;
    return 1;
  }
  if (obj->exactIntegerValue(result) && result > 0)
    return 1;
  // FIXME allow inexact value
  invalidCharacteristicValue(ident, loc);
  return 0;
}

bool Interpreter::convertIntegerC(ELObj *obj, const Identifier *ident, const Location &loc,
				  long &result)
{
  obj = convertFromString(obj, convertAllowNumber, loc);
  if (obj->exactIntegerValue(result))
    return 1;
  // FIXME allow inexact value
  invalidCharacteristicValue(ident, loc);
  return 0;
}


bool Interpreter::convertLetter2C(ELObj *obj, const Identifier *ident, const Location &loc,
				  FOTBuilder::Letter2 &code)
{
  StringObj *strObj = obj->convertToString();
  if (strObj) {
    const StringC &str = *strObj;
    if (str.size() == 2
	&& str[0] >= 'A' && str[0] <= 'Z'
	&& str[1] >= 'A' && str[1] <= 'Z') {
      code = SP_LETTER2(str[0], str[1]);
      return 1;
    }
    if (str.size() == 0) {
      code = 0;
      return 1;
    }
  }
  else if (obj == makeFalse()) {
    code = 0;
    return 1;
  }
  invalidCharacteristicValue(ident, loc);
  return 0;
}

bool Interpreter::convertCharC(ELObj *obj, const Identifier *ident, const Location &loc,
			       Char &result)
{
  if (obj->charValue(result))
    return 1;
  const Char *s;
  size_t n;
  if (obj->stringData(s, n) && n == 1) {
    result = s[0];
    return 1;
  }
  invalidCharacteristicValue(ident, loc);
  return 0;
}

bool Interpreter::convertColorC(ELObj *obj, const Identifier *ident, const Location &loc, ColorObj *&color)
{
  color = obj->asColor();
  if (color)
    return 1;
  invalidCharacteristicValue(ident, loc);
  return 0;
}

bool Interpreter::convertOptColorC(ELObj *obj, const Identifier *ident, const Location &loc, ColorObj *&color)
{
  color = obj->asColor();
  if (color)
    return 1;
  if (obj == makeFalse())
    return 1;
  invalidCharacteristicValue(ident, loc);
  return 0;
}

bool Interpreter::convertRealC(ELObj *obj, const Identifier *ident, const Location &loc,
			       double &result)
{
  obj = convertFromString(obj, convertAllowNumber, loc);
  if (obj->realValue(result))
    return 1;
  invalidCharacteristicValue(ident, loc);
  return 0;
}

bool Interpreter::convertEnumC(ELObj *obj, const Identifier *ident, const Location &loc,
			       FOTBuilder::Symbol &sym)
{
  obj = convertFromString(obj, convertAllowSymbol|convertAllowBoolean, loc);
  if (obj == makeFalse()) {
    sym = FOTBuilder::symbolFalse;
    return 1;
  }
  SymbolObj *symObj = obj->asSymbol();
  if (symObj) {
    sym = symObj->cValue();
    if (sym != FOTBuilder::symbolFalse)
      return 1;
  }
  if (obj == makeTrue()) {
    sym = FOTBuilder::symbolTrue;
    return 1;
  }
  invalidCharacteristicValue(ident, loc);
  return 0;
}

bool Interpreter::convertEnumC(const FOTBuilder::Symbol *syms,  size_t nSyms,
			       ELObj *obj, const Identifier *ident, const Location &loc,
			       FOTBuilder::Symbol &result)
{
  obj = convertFromString(obj, convertAllowSymbol|convertAllowBoolean, loc);
  SymbolObj *symObj = obj->asSymbol();
  FOTBuilder::Symbol val;
  if (symObj) {
    val = symObj->cValue();
    if (val == FOTBuilder::symbolFalse) {
      invalidCharacteristicValue(ident, loc);
      return 0;
    }
  }
  else if (obj == makeFalse())
    val = FOTBuilder::symbolFalse;
  else if (obj == makeTrue())
    val = FOTBuilder::symbolTrue;
  else {
    invalidCharacteristicValue(ident, loc);
    return 0;
  }
  for (size_t i = 0; i < nSyms; i++)
    if (val == syms[i]) {
      result = val;
      return 1;
    }
  invalidCharacteristicValue(ident, loc);
  return 0;
}

void Interpreter::invalidCharacteristicValue(const Identifier *ident, const Location &loc)
{
  setNextLocation(loc);
  message(InterpreterMessages::invalidCharacteristicValue,
	  StringMessageArg(ident->name()));
}

static
bool equal(const Char *s1, const char *s2, size_t n)
{
  while (n > 0) {
    if (*s1++ != (unsigned char)*s2++)
      return 0;
    --n;
  }
  return 1;
}

ELObj *Interpreter::convertFromString(ELObj *obj, unsigned hints, const Location &loc)
{
  // FIXME fold to lower case
  const Char *s;
  size_t n;
  if (!dsssl2() || !obj->stringData(s, n))
    return obj;
  if (hints & convertAllowNumber) {
    ELObj *tem = convertNumber(StringC(s, n));
    if (tem)
      return tem->resolveQuantities(1, *this, loc);
  }
  if (hints & convertAllowSymbol) {
    StringC tem(s, n);
    SymbolObj *sym = symbolTable_.lookup(tem);
    if (sym && sym->cValue() != FOTBuilder::symbolFalse)
      return sym;
  }
  if (hints & convertAllowBoolean) {
    switch (n) {
    case 2:
      if (equal(s, "no", n))
	return makeFalse();
      break;
    case 3:
      if (equal(s, "yes", n))
	return makeTrue();
      break;
    case 4:
      if (equal(s, "true", n))
	return makeTrue();
      break;
    case 5:
      if (equal(s, "false", n))
	return makeFalse();
      break;
    }
  }
  return obj;
}

ELObj *Interpreter::convertNumber(const StringC &str, int radix)
{
  {
    if (str.size() == 0)
      return 0;
    size_t i = 0;
    if (str[0] == '#') {
      if (str.size() < 2)
	return 0;
      switch (str[1]) {
      case 'd':
	radix = 10;
	break;
      case 'x':
	radix = 16;
	break;
      case 'o':
	radix = 8;
	break;
      case 'b':
	radix = 2;
	break;
      default:
	return 0;
      }
      i += 2;
    }
    if (i >= str.size())
      return 0;
    bool negative;
    if (str[i] == '-') {
      negative = 1;
      i++;
    }
    else {
      negative = 0;
      if (str[i] == '+')
      i++;
    }
    bool hadDecimalPoint = 0;
    bool hadDigit = 0;
    long n = 0;
    int exp = 0;
    for (; i < str.size(); i++) {
      Char c = str[i];
      int weight;
      switch (c) {
      case '0':
	weight = 0;
	break;
      case '1':
	weight = 1;
	break;
      case '2':
	weight = 2;
	break;
      case '3':
	weight = 3;
	break;
      case '4':
	weight = 4;
	break;
      case '5':
	weight = 5;
	break;
      case '6':
	weight = 6;
	break;
      case '7':
	weight = 7;
	break;
      case '8':
	weight = 8;
	break;
      case '9':
	weight = 9;
	break;
      case 'a':
	weight = 10;
	break;
      case 'b':
	weight = 11;
	break;
      case 'c':
	weight = 12;
	break;
      case 'd':
	weight = 13;
	break;
      case 'e':
	weight = 14;
	break;
      case 'f':
	weight = 15;
	break;
      default:
	weight = -1;
	break;
      }
      if (weight >= 0 && weight < radix) {
	hadDigit = 1;
	if (negative) {
	  if (-(unsigned long)n > (-(unsigned long)LONG_MIN - weight)/radix) {
	    if (radix != 10)
	      return 0;
	    return convertNumberFloat(str);
	  }
	  else
	    n = n*radix - weight;
	}
	else {
	  if (n > (LONG_MAX - weight)/radix) {
	    if (radix != 10)
	      return 0;
	    return convertNumberFloat(str);
	  }
	  else
	    n = n*radix + weight;
	}
	if (hadDecimalPoint)
	  exp--;
      }
      else if (c == '.' && radix == 10) {
	if (hadDecimalPoint)
	  return 0;
	hadDecimalPoint = 1;
      }
      else
	break;
    }
    if (!hadDigit || (radix != 10 && i < str.size()))
      return 0;
    if (i + 1 < str.size() && str[i] == 'e'
        && lexCategory(str[i + 1]) != lexLetter) {
      hadDecimalPoint = 1;
      i++;
      int e;
      if (!scanSignDigits(str, i, e))
	return 0;
      exp += e;
    }
    if (i < str.size()) {
      int unitExp;
      Unit *unit = scanUnit(str, i, unitExp);
      if (!unit)
	return 0;
      if (unitExp == 1)
	return new (*this) UnresolvedLengthObj(n, exp, unit);
      else
	return convertNumberFloat(str);
    }
    if (hadDecimalPoint)
      return convertNumberFloat(str);
    return makeInteger(n);
  }
}

bool Interpreter::scanSignDigits(const StringC &str, size_t &i, int &n)
{
  bool negative = 0;
  if (i < str.size()) {
    if (str[i] == '-') {
      i++;
      negative = 1;
    } else if (str[i] == '+')
      i++;
  }
  size_t j = i;
  n = 0;
  while (i < str.size()
	 && ('0' <= str[i] && str[i] <= '9')) {
    if (negative)
      n = n*10 - (str[i] - '0');
    else
      n = n*10 + (str[i] - '0');
    i++;
  }
  if (i == j)
    return 0;
  return 1;
}

ELObj *Interpreter::convertNumberFloat(const StringC &str)
{
  String<char> buf;
  // omit an optional radix prefix
  size_t i0 = 0;
  if (str.size() > 1 && str[0] == '#' && str[1] == 'd')
    i0 = 2;
  for (size_t i = i0; i < str.size(); i++) {
    if (str[i] > CHAR_MAX || str[i] == '\0')
      return 0;
    // 'E' is a valid exponent marker for C but not us
    if (str[i] == 'E')
      break;
    buf += char(str[i]);
  }
  buf += '\0';
  const char *endPtr;
  double val = strtod((char *)buf.data(), (char **)&endPtr);
  if (endPtr - buf.data() == str.size() - i0)
    return new (*this) RealObj(val);
  if (endPtr == buf.data())
    return 0;
  int unitExp;
  Unit *unit = scanUnit(str, endPtr - buf.data(), unitExp);
  if (!unit)
    return 0;
  return new (*this) UnresolvedQuantityObj(val, unit, unitExp);
}

// Return 0 for error.

Unit *Interpreter::scanUnit(const StringC &str, size_t i, int &unitExp)
{
  StringC unitName;
  while (i < str.size()) {
    if (str[i] == '-' || str[i] == '+' || ('0' <= str[i] && str[i] <= '9'))
      break;
    unitName += str[i++];
  }
  if (i >= str.size())
    unitExp = 1;
  else {
    unitExp = 0;
    bool neg = 0;
    if (str[i] == '-' || str[i] == '+') {
      if (str[i] == '-')
      neg = 1;
      i++;
      if (i >= str.size())
	return 0;
    }
    while (i < str.size()) {
      if (str[i] < '0' || str[i] > '9')
	return 0;
      unitExp *= 10;
      if (neg)
	unitExp -= (str[i] - '0');
      else
	unitExp += (str[i] - '0');
      i++;
    }
  }
  return lookupUnit(unitName);
}

void Interpreter::setNodeLocation(const NodePtr &nd)
{
  const LocNode *lnp;
  Location nodeLoc;
  if ((lnp = LocNode::convert(nd)) != 0
      && lnp->getLocation(nodeLoc) == accessOK)
    setNextLocation(nodeLoc);
}

bool Interpreter::convertToPattern(ELObj *obj, const Location &loc, Pattern &pattern)
{
  IList<Pattern::Element> list;
  if (!convertToPattern(obj, loc, 0, list))
    return 0;
  Pattern tem(list);
  tem.swap(pattern);
  return 1;
}

bool Interpreter::convertToPattern(ELObj *obj, const Location &loc,
				   bool isChild,
				   IList<Pattern::Element> &list)
{
  StringObj *str = obj->convertToString();
  if (str) {
    const Char *s;
    size_t n;
    str->stringData(s, n);
    if (!n) {
      setNextLocation(loc);
      message(InterpreterMessages::patternEmptyGi);
      return 0;
    }
    list.insert(new Pattern::Element(StringC(s, n)));
    return 1;
  }
  if (obj == makeTrue()) {
    list.insert(new Pattern::Element(StringC()));
    return 1;
  }
  Pattern::Element *curElement = 0;
  while (!obj->isNil()) {
    PairObj *pair = obj->asPair();
    if (!pair) {
      setNextLocation(loc);
      message(InterpreterMessages::patternNotList);
      return 0;
    }
    ELObj *head = pair->car();
    obj = pair->cdr();
    if (head == makeTrue() && dsssl2()) {
      list.insert(curElement = new Pattern::Element(StringC()));
      continue;
    }
    str = head->convertToString();
    if (str) {
      const Char *s;
      size_t n;
      str->stringData(s, n);
      if (!n) {
	setNextLocation(loc);
	message(InterpreterMessages::patternEmptyGi);
	return 0;
      }
      list.insert(curElement = new Pattern::Element(StringC(s, n)));
      continue;
    }
    if (!curElement) {
      setNextLocation(loc);
      message(InterpreterMessages::patternBadGi,
	      ELObjMessageArg(head, *this));
      return 0;
    }
    if (head->isNil())
      continue; // empty attribute list
    if (head->asPair()) {
      if (!patternAddAttributeQualifiers(head, loc, *curElement)) {
	setNextLocation(loc);
	message(InterpreterMessages::patternBadAttributeQualifier);
	return 0;
      }
      continue;
    }
    KeywordObj *key = dsssl2() ? head->asKeyword() : 0;
    if (!key) {
      setNextLocation(loc);
      message(InterpreterMessages::patternBadMember,
	      ELObjMessageArg(head, *this));
      return 0;
    }
    pair = obj->asPair();
    if (!pair) {
      setNextLocation(loc);
      message(obj->isNil()
	      ? InterpreterMessages::patternMissingQualifierValue
	      : InterpreterMessages::patternNotList);
      return 0;
    }
    ELObj *value = pair->car();
    obj = pair->cdr();
    Identifier::SyntacticKey k;
    if (!key->identifier()->syntacticKey(k)) {
      setNextLocation(loc);
      message(InterpreterMessages::patternUnknownQualifier,
	      StringMessageArg(key->identifier()->name()));
      return 0;
    }
    switch (k) {
    case Identifier::keyAttributes:
      if (!patternAddAttributeQualifiers(value, loc, *curElement)) {
	setNextLocation(loc);
	message(InterpreterMessages::patternBadAttributeQualifier);
	return 0;
      }
      break;
    case Identifier::keyChildren:
      {
	IList<Pattern::Element> children;
	if (!convertToPattern(value, loc, 1, children))
	  return 0;
	if (!children.empty())
	  curElement->addQualifier(new Pattern::ChildrenQualifier(children));
      }
      break;
    case Identifier::keyRepeat:
      {
	if (isChild) {
	  setNextLocation(loc);
	  message(InterpreterMessages::patternChildRepeat);
	  return 0;
	}
        SymbolObj *sym = value->asSymbol();
	if (sym) {
	  const StringC &str = *sym->name();
	  if (str.size() == 1) {
	    switch (str[0]) {
	    case '*':
	      curElement->setRepeat(0, Pattern::Repeat(-1));
	      value = 0;
	      break;
	    case '?':
	      curElement->setRepeat(0, 1);
	      value = 0;
	      break;
	    case '+':
	      curElement->setRepeat(1, Pattern::Repeat(-1));
	      value = 0;
	      break;
	    default:
	      break;
	    }
	  }
	}
	if (value) {
	  setNextLocation(loc);
	  message(InterpreterMessages::patternBadQualifierValue,
	          ELObjMessageArg(value, *this),
		  StringMessageArg(key->identifier()->name()));
	  return 0;
	}
      }
      break;
    case Identifier::keyPosition:
      {
        SymbolObj *sym = value->asSymbol();
	if (sym) {
	  Pattern::Qualifier *qual = 0;
	  const StringC &str = *sym->name();
	  if (str == "first-of-type")
	    qual = new Pattern::FirstOfTypeQualifier;
	  else if (str == "last-of-type")
	    qual = new Pattern::LastOfTypeQualifier;
	  else if (str == "first-of-any")
	    qual = new Pattern::FirstOfAnyQualifier;
	  else if (str == "last-of-any")
	    qual = new Pattern::LastOfAnyQualifier;
	  if (qual) {
	    curElement->addQualifier(qual);
	    break;
	  }
	}
        setNextLocation(loc);
	message(InterpreterMessages::patternBadQualifierValue,
	        ELObjMessageArg(value, *this),
		StringMessageArg(key->identifier()->name()));
	return 0;
      }
    case Identifier::keyOnly:
      {
        SymbolObj *sym = value->asSymbol();
	if (sym) {
	  Pattern::Qualifier *qual = 0;
	  const StringC &str = *sym->name();
	  if (str == "of-type")
	    qual = new Pattern::OnlyOfTypeQualifier;
	  else if (str == "of-any")
	    qual = new Pattern::OnlyOfAnyQualifier;
	  if (qual) {
	    curElement->addQualifier(qual);
	    break;
	  }
	}
	setNextLocation(loc);
    	message(InterpreterMessages::patternBadQualifierValue,
	        ELObjMessageArg(value, *this),
		StringMessageArg(key->identifier()->name()));
	return 0;
      }
      break;
    case Identifier::keyId:
      {
	StringObj *str = value->convertToString();
	if (!str) {
	  setNextLocation(loc);
	  message(InterpreterMessages::patternBadQualifierValue,
	          ELObjMessageArg(value, *this),
		  StringMessageArg(key->identifier()->name()));
	  return 0;
	}
	const Char *s;
	size_t n;
	str->stringData(s, n);
	curElement->addQualifier(new Pattern::IdQualifier(StringC(s, n)));
      }
      break;
    case Identifier::keyClass:
      {
	StringObj *str = value->convertToString();
	if (!str) {
	  setNextLocation(loc);
	  message(InterpreterMessages::patternBadQualifierValue,
	          ELObjMessageArg(value, *this),
		  StringMessageArg(key->identifier()->name()));
	  return 0;
	}
	const Char *s;
	size_t n;
	str->stringData(s, n);
	curElement->addQualifier(new Pattern::ClassQualifier(StringC(s, n)));
      }
      break;
    case Identifier::keyImportance:
      {
	long n;
	if (!value->exactIntegerValue(n)) {
	  setNextLocation(loc);
	  message(InterpreterMessages::patternBadQualifierValue,
	          ELObjMessageArg(value, *this),
		  StringMessageArg(key->identifier()->name()));
	  return 0;
	}
	curElement->addQualifier(new Pattern::ImportanceQualifier(n));
      }
      break;
    case Identifier::keyPriority:
      {
	long n;
	if (!value->exactIntegerValue(n)) {
	  setNextLocation(loc);
	  message(InterpreterMessages::patternBadQualifierValue,
	          ELObjMessageArg(value, *this),
		  StringMessageArg(key->identifier()->name()));
	  return 0;
	}
	curElement->addQualifier(new Pattern::PriorityQualifier(n));
      }
      break;
    default:
      setNextLocation(loc);
      message(InterpreterMessages::patternUnknownQualifier,
	      StringMessageArg(key->identifier()->name()));
      return 0;
    }
  }
  return 1;
}

bool Interpreter::patternAddAttributeQualifiers(ELObj *obj,
						const Location &loc,
						Pattern::Element &elem)
{
  while (!obj->isNil()) {
    PairObj *pair = obj->asPair();
    if (!pair)
      return 0;
    StringObj *tem = pair->car()->convertToString();
    if (!tem)
      return 0;
    const Char *s;
    size_t n;
    tem->stringData(s, n);
    if (n == 0)
      return 0;
    StringC name(s, n);
    obj = pair->cdr();
    pair = obj->asPair();
    if (!pair)
      return 0;
    obj = pair->cdr();
    if (pair->car() == makeFalse() && dsssl2())
      elem.addQualifier(new Pattern::AttributeMissingValueQualifier(name));
    else if (pair->car() == makeTrue() && dsssl2())
      elem.addQualifier(new Pattern::AttributeHasValueQualifier(name));
    else {
      tem = pair->car()->convertToString();
      if (!tem)
	return 0;
      tem->stringData(s, n);
      elem.addQualifier(new Pattern::AttributeQualifier(name, StringC(s, n)));
    }
  }
  return 1;
}

void Interpreter::dispatchMessage(Message &msg)
{
  messenger_->dispatchMessage(msg);
}

void Interpreter::dispatchMessage(const Message &msg)
{
  messenger_->dispatchMessage(msg);
}

Interpreter::StringSet::StringSet()
{
}

const char *Interpreter::StringSet::store(String<char> &str)
{
  str += '\0';
  const String<char> *p = table_.lookup(str);
  if (!p) {
    String<char> *tem = new String<char>;
    str.swap(*tem);
    table_.insert(tem);
    p = tem;
  }
  return p->data();
}

unsigned long Interpreter::StringSet::hash(const String<char> &str)
{
  const char *p = str.data();
  unsigned long h = 0;
  for (size_t n = str.size(); n > 0; n--)
    h = (h << 5) + h + (unsigned char)*p++;	// from Chris Torek
  return h;
}

Identifier::Identifier(const StringC &name)
: Named(name), value_(0), syntacticKey_(notKey), beingComputed_(0), flowObj_(0)
{
}

void Identifier::setDefinition(Owner<Expression> &expr,
			       unsigned part,
			       const Location &loc)
{
  def_.swap(expr);
  defPart_ = part;
  defLoc_ = loc;
  value_ = 0;
}

void Identifier::setValue(ELObj *value, unsigned partIndex)
{
  value_ = value;
  // Built in functions have lowest priority.
  defPart_ = partIndex;
}

bool Identifier::defined(unsigned &part, Location &loc) const
{
  if (!def_ && !value_)
    return 0;
  part = defPart_;
  loc = defLoc_;
  return 1;
}

ELObj *Identifier::computeValue(bool force, Interpreter &interp) const
{
  if (value_)
    return value_;
  ASSERT(def_);
  if (beingComputed_) {
    if (force) {
      interp.setNextLocation(defLoc_);
      interp.message(InterpreterMessages::identifierLoop,
	             StringMessageArg(name()));
      ((Identifier *)this)->value_ = interp.makeError();
    }
  }
  else {
    ((Identifier *)this)->beingComputed_ = 1;
    if (insn_.isNull())
      ((Identifier *)this)->insn_ 
        = Expression::optimizeCompile(((Identifier *)this)->def_, interp,
	                              Environment(), 0, InsnPtr());
    if (force || def_->canEval(0)) {
      VM vm(interp);
      ELObj *v = vm.eval(insn_.pointer());
      interp.makePermanent(v);
      ((Identifier *)this)->value_ = v;
    }
    ((Identifier *)this)->beingComputed_ = 0;
  }
  return value_;
}

Unit::Unit(const StringC &name)
: Named(name), computed_(notComputed)
{
}

bool Unit::defined(unsigned &part, Location &loc) const
{
  if (!def_ && computed_ == notComputed)
    return 0;
  part = defPart_;
  loc = defLoc_;
  return 1;
}

void Unit::setDefinition(Owner<Expression> &expr,
			 unsigned part,
			 const Location &loc)
{
  def_.swap(expr);
  defPart_ = part;
  defLoc_ = loc;
  computed_ = notComputed;
}

void Unit::setValue(long n)
{
  computed_ = computedExact;
  exact_ = n;
  dim_ = 1;
  defPart_ = unsigned(-1);
}

void Unit::setValue(double n)
{
  computed_ = computedInexact;
  inexact_ = n;
  dim_ = 1;
  defPart_ = unsigned(-1);
}

void Unit::tryCompute(bool force, Interpreter &interp)
{
  if (computed_ == notComputed) {
    computed_ = beingComputed;
    if (insn_.isNull())
      insn_ = Expression::optimizeCompile(def_, interp, Environment(), 0, InsnPtr());
    if (force || def_->canEval(0)) {
      VM vm(interp);
      ELObj *v = vm.eval(insn_.pointer());
      switch (v->quantityValue(exact_, inexact_, dim_)) {
      case ELObj::noQuantity:
	if (!interp.isError(v)) {
	  interp.setNextLocation(defLoc_);
	  interp.message(InterpreterMessages::badUnitDefinition,
			 StringMessageArg(name()));
	}
	computed_ = computedError;
	break;
      case ELObj::longQuantity:
	computed_ = computedExact;
	break;
      case ELObj::doubleQuantity:
	computed_ = computedInexact;
	break;
      default:
	CANNOT_HAPPEN();
      }
    }
    if (computed_ == beingComputed)
      computed_ = notComputed;
  }
  else if (computed_ == beingComputed) {
    interp.setNextLocation(defLoc_);
    interp.message(InterpreterMessages::unitLoop,
		   StringMessageArg(name()));
    computed_ = computedError;
  }
}

// multiply by 10^valExp
// quantity has exponent of 1

ELObj *Unit::resolveQuantity(bool force, Interpreter &interp,
			     long val, int valExp)
{
  tryCompute(force, interp);
  long result;
  if (computed_ == computedExact && scale(val, valExp, exact_, result))
    return new (interp) LengthObj(result);
  double x = val;
  while (valExp > 0) {
    x *= 10.0;
    valExp--;
  }
  while (valExp < 0) {
    x /= 10.0;
    valExp++;
  }
  return resolveQuantity(force, interp, x, 1);
}

// val * 10^valExp * factor
// return 0 if it can't be done without overflow

bool Unit::scale(long val, int valExp, long factor, long &result)
{
  if (factor <= 0)
    return 0; // feeble
  while (valExp > 0) {
    if (factor > LONG_MAX/10)
      return 0;
    valExp--;
    factor *= 10;
  }
  if (val >= 0) {
    if (val > LONG_MAX/factor)
      return 0;
  }
  else {
    if (-(unsigned long)val > -(unsigned long)LONG_MIN/factor)
      return 0;
  }
  result = val*factor;
  while (valExp < 0) {
    result /= 10;
    valExp++;
  }
  return 1;
}

ELObj *Unit::resolveQuantity(bool force, Interpreter &interp,
			     double val, int unitExp)
{
  tryCompute(force, interp);
  double factor;
  switch (computed_) {
  case computedExact:
    factor = exact_;
    break;
  case computedInexact:
    factor = inexact_;
    break;
  case computedError:
    return interp.makeError();
  default:
    return 0;
  }
  int resultDim = 0;
  double resultVal = val;
  while (unitExp > 0) {
    resultDim += dim_;
    resultVal *= factor;
    unitExp--;
  }
  while (unitExp < 0) {
    resultDim -= dim_;
    resultVal /= factor;
    unitExp++;
  }
  if (resultDim == 0)
    return new (interp) RealObj(resultVal);
  return new (interp) QuantityObj(resultVal, resultDim);
}

void ELObjDynamicRoot::trace(Collector &c) const
{
  c.trace(obj_);
}

bool operator==(const StringC &s, const char *p)
{
  for (size_t i = 0; i < s.size(); i++)
    if (p[i] == '\0' || (unsigned char)p[i] != s[i])
      return 0;
  return p[s.size()] == '\0';
}

#ifdef DSSSL_NAMESPACE
}
#endif
