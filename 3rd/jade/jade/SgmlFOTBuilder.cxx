// Copyright (c) 1996, 1997 James Clark
// See the file copying.txt for copying permission.

// Possible improvements:
// Don't serialize ports
// Extension flow objects
// Merge text chunks
// Handle input-whitespace-treatment/tabs
// Table processing
// When element has an ID, output both ID anchor and numeric anchor

#include "config.h"
#include "SgmlFOTBuilder.h"
#include "macros.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

class SgmlFOTBuilder : public SerialFOTBuilder {
public:
  SgmlFOTBuilder(OutputCharStream *);
  ~SgmlFOTBuilder();
  void setFontSize(Length);
  void setFontFamilyName(const StringC &);
  void setFontWeight(Symbol);
  void setFontPosture(Symbol);
  void setStartIndent(const LengthSpec &);
  void setEndIndent(const LengthSpec &);
  void setFirstLineStartIndent(const LengthSpec &);
  void setLastLineEndIndent(const LengthSpec &);
  void setLineSpacing(const LengthSpec &);
  void setFieldWidth(const LengthSpec &);
  void setPositionPointShift(const LengthSpec &);
  void setStartMargin(const LengthSpec &);
  void setEndMargin(const LengthSpec &);
  void setSidelineSep(const LengthSpec &);
  void setAsisWrapIndent(const LengthSpec &);
  void setLineNumberSep(const LengthSpec &);
  void setLastLineJustifyLimit(const LengthSpec &);
  void setJustifyGlyphSpaceMaxAdd(const LengthSpec &);
  void setJustifyGlyphSpaceMaxRemove(const LengthSpec &);
  void setTableCornerRadius(const LengthSpec &);
  void setBoxCornerRadius(const LengthSpec &);
  void setMarginaliaSep(const LengthSpec &);
  void setMinPreLineSpacing(const OptLengthSpec &);
  void setMinPostLineSpacing(const OptLengthSpec &);
  void setMinLeading(const OptLengthSpec &);
  void setLines(Symbol);
  void setQuadding(Symbol);
  void setDisplayAlignment(Symbol sym);
  void setFieldAlign(Symbol);
  void setLineJoin(Symbol);
  void setLineCap(Symbol);
  void setLineNumberSide(Symbol);
  void setKernMode(Symbol);
  void setInputWhitespaceTreatment(Symbol);
  void setFillingDirection(Symbol);
  void setWritingMode(Symbol);
  void setLastLineQuadding(Symbol);
  void setMathDisplayMode(Symbol);
  void setScriptPreAlign(Symbol);
  void setScriptPostAlign(Symbol);
  void setScriptMidSupAlign(Symbol);
  void setScriptMidSubAlign(Symbol);
  void setNumeratorAlign(Symbol);
  void setDenominatorAlign(Symbol);
  void setGridPositionCellType(Symbol);
  void setGridColumnAlignment(Symbol);
  void setGridRowAlignment(Symbol);
  void setBoxType(Symbol);
  void setGlyphAlignmentMode(Symbol);
  void setBoxBorderAlignment(Symbol);
  void setCellRowAlignment(Symbol);
  void setBorderAlignment(Symbol);
  void setSidelineSide(Symbol);
  void setHyphenationKeep(Symbol);
  void setFontStructure(Symbol);
  void setFontProportionateWidth(Symbol);
  void setCellCrossed(Symbol);
  void setMarginaliaSide(Symbol);
  void setColor(const DeviceRGBColor &);
  void setBackgroundColor(const DeviceRGBColor &);
  void setBackgroundColor();
  void setPageWidth(Length);
  void setPageHeight(Length);
  void setLeftMargin(Length);
  void setRightMargin(Length);
  void setTopMargin(Length);
  void setBottomMargin(Length);
  void setHeaderMargin(Length);
  void setFooterMargin(Length);
  void setLineThickness(Length);
  void setCellBeforeRowMargin(Length);
  void setCellAfterRowMargin(Length);
  void setCellBeforeColumnMargin(Length);
  void setCellAfterColumnMargin(Length);
  void setLineSep(Length);
  void setBoxSizeBefore(Length);
  void setBoxSizeAfter(Length);
  void setLayer(long);
  void setBackgroundLayer(long);
  void setBorderPriority(long);
  void setLineRepeat(long);
  void setSpan(long);
  void setMinLeaderRepeat(long);
  void setHyphenationRemainCharCount(long);
  void setHyphenationPushCharCount(long);
  void setWidowCount(long);
  void setOrphanCount(long);
  void setExpandTabs(long);
  void setHyphenationLadderCount(long);
  void setLanguage(Letter2);
  void setCountry(Letter2);
  void setBackgroundTile(PublicId);
  void setLineBreakingMethod(PublicId);
  void setLineCompositionMethod(PublicId);
  void setImplicitBidiMethod(PublicId);
  void setGlyphSubstMethod(PublicId);
  void setGlyphReorderMethod(PublicId);
  void setHyphenationMethod(PublicId);
  void setTableAutoWidthMethod(PublicId);
  void setFontName(PublicId);
  void setBorderPresent(bool);
  void setInhibitLineBreaks(bool);
  void setHyphenate(bool);
  void setKern(bool);
  void setLigature(bool);
  void setScoreSpaces(bool);
  void setFloatOutMarginalia(bool);
  void setFloatOutSidelines(bool);
  void setFloatOutLineNumbers(bool);
  void setCellBackground(bool);
  void setSpanWeak(bool);
  void setIgnoreRecordEnd(bool);
  void setNumberedLines(bool);
  void setHangingPunct(bool);
  void setBoxOpenEnd(bool);
  void setTruncateLeader(bool);
  void setAlignLeader(bool);
  void setTablePartOmitMiddleHeader(bool);
  void setTablePartOmitMiddleFooter(bool);
  void setBorderOmitAtBreak(bool);
  void setPrincipalModeSimultaneous(bool);
  void setMarginaliaKeepWithPrevious(bool);
  void setGridEquidistantRows(bool);
  void setGridEquidistantColumns(bool);
  void setEscapementSpaceBefore(const InlineSpace &);
  void setEscapementSpaceAfter(const InlineSpace &);
  void setGlyphSubstTable(const Vector<ConstPtr<GlyphSubstTable> > &tables);

  void paragraphBreak(const ParagraphNIC &);
  void characters(const Char *, size_t);
  void charactersFromNode(const NodePtr &nd, const Char *s, size_t n);
  void character(const CharacterNIC &);
  void externalGraphic(const ExternalGraphicNIC &);
  void rule(const RuleNIC &);
  void alignmentPoint();

  void startSimplePageSequence();
  void endSimplePageSequence();
  void pageNumber();
  void startSimplePageSequenceHeaderFooter(unsigned);
  void endSimplePageSequenceHeaderFooter(unsigned);
  void endAllSimplePageSequenceHeaderFooter();
  void startScroll();
  void endScroll();
  void startLink(const Address &);
  void endLink();
  void startMarginalia();
  void endMarginalia();
  void startMultiModeSerial(const MultiMode *);
  void endMultiModeSerial();
  void startMultiModeMode(const MultiMode &);
  void endMultiModeMode();
  void startScore(Char);
  void startScore(const LengthSpec &);
  void startScore(Symbol);
  void endScore();
  void startLeader(const LeaderNIC &);
  void endLeader();
  void startSideline();
  void endSideline();
  void startBox(const BoxNIC &);
  void endBox();
  void startParagraph(const ParagraphNIC &);
  void endParagraph();
  void startSequence();
  void endSequence();
  void startLineField(const LineFieldNIC &);
  void endLineField();
  void startDisplayGroup(const DisplayGroupNIC &);
  void endDisplayGroup();

  void startTable(const TableNIC &);
  void endTable();
  void tableBeforeRowBorder();
  void tableAfterRowBorder();
  void tableBeforeColumnBorder();
  void tableAfterColumnBorder();
  void tableColumn(const TableColumnNIC &);
  void startTablePartSerial(const TablePartNIC &);
  void endTablePartSerial();
  void startTablePartHeader();
  void endTablePartHeader();
  void startTablePartFooter();
  void endTablePartFooter();
  void startTableRow();
  void endTableRow();
  void startTableCell(const TableCellNIC &);
  void endTableCell();
  void tableCellBeforeRowBorder();
  void tableCellAfterRowBorder();
  void tableCellBeforeColumnBorder();
  void tableCellAfterColumnBorder();
  void startMathSequence();
  void endMathSequence();
  void startFractionSerial();
  void endFractionSerial();
  void startFractionNumerator();
  void endFractionNumerator();
  void startFractionDenominator();
  void endFractionDenominator();
  void fractionBar();
  void startUnmath();
  void endUnmath();
  void startSuperscript();
  void endSuperscript();
  void startSubscript();
  void endSubscript();
  void startScriptSerial();
  void endScriptSerial();
  void startScriptPreSup();
  void endScriptPreSup();
  void startScriptPreSub();
  void endScriptPreSub();
  void startScriptPostSup();
  void endScriptPostSup();
  void startScriptPostSub();
  void endScriptPostSub();
  void startScriptMidSup();
  void endScriptMidSup();
  void startScriptMidSub();
  void endScriptMidSub();
  void startMarkSerial();
  void endMarkSerial();
  void startMarkOver();
  void endMarkOver();
  void startMarkUnder();
  void endMarkUnder();
  void startFenceSerial();
  void endFenceSerial();
  void startFenceOpen();
  void endFenceOpen();
  void startFenceClose();
  void endFenceClose();
  void startRadicalSerial();
  void endRadicalSerial();
  void startRadicalDegree();
  void endRadicalDegree();
  void radicalRadical(const CharacterNIC &);
  void radicalRadicalDefaulted();
  void startMathOperatorSerial();
  void endMathOperatorSerial();
  void startMathOperatorOperator();
  void endMathOperatorOperator();
  void startMathOperatorLowerLimit();
  void endMathOperatorLowerLimit();
  void startMathOperatorUpperLimit();
  void endMathOperatorUpperLimit();
  void startGrid(const GridNIC &);
  void endGrid();
  void startGridCell(const GridCellNIC &);
  void endGridCell();
  void startNode(const NodePtr &, const StringC &);
  void endNode();
  void currentNodePageNumber(const NodePtr &);
  struct Data {
    Data(const StringC &str) : ptr(str.data()), size(str.size()) { }
    Data(const Char *p, size_t n) : ptr(p), size(n) { }
    const Char *ptr;
    size_t size;
  };
  struct Units {
    Units(long i) : n(i) { }
    long n;
  };
private:
  OutputCharStream &os() { return *curOs_; }
  void outputIcs();
  void startSimpleFlowObj(const char *name);
  void simpleFlowObj(const char *name);
  void startPortFlow(const char *name);
  void endFlow(const char *name);
  void characterNIC(const CharacterNIC &nic);
  void displayNIC(const DisplayNIC &nic);
  void inlineNIC(const InlineNIC &nic);
  void boolC(const char *, bool);
  void lengthC(const char *, Length);
  void lengthSpecC(const char *, const LengthSpec &);
  void optLengthSpecC(const char *, const OptLengthSpec &);
  void symbolC(const char *name, Symbol sym);
  void integerC(const char *, long);
  void publicIdC(const char *, PublicId);
  void inlineSpaceC(const char *name, const InlineSpace &);
  void displaySpaceNIC(const char *name, const DisplaySpace &);
  void flushPendingElements();
  void outputElementName(unsigned long groveIndex, const Char *, size_t);
  void outputElementName(const NodePtr &node);
  static bool nodeIsElement(const NodePtr &node) {
    GroveString gi;
    return node->getGi(gi) == accessOK;
  }

  Owner<OutputCharStream> os_;
  StrOutputCharStream ics_;
  StrOutputCharStream hfs_;
  OutputCharStream *curOs_;
  Vector<PackedBoolean> multiModeHasPrincipalMode_;
  Vector<PackedBoolean> glyphSubstTableDefined_;

  Vector<NodePtr> pendingElements_;
  Vector<unsigned> pendingElementLevels_;
  unsigned nPendingElementsNonEmpty_;
  unsigned nodeLevel_;
  bool suppressAnchors_;
  StringC hf_[nHF];
};

const char RE = '\r';
const char quot = '"';
const char *const trueString = "true";
const char *const falseString = "false";
// May change this to "null"
const char *const nullString = "false";

typedef SgmlFOTBuilder::Units Units;

inline
OutputCharStream &operator<<(OutputCharStream &os, 
#ifdef GROVE_NAMESPACE
			     GROVE_NAMESPACE::
#endif
			     GroveString &str)
{
  return os.write(str.data(), str.size());
}

inline
OutputCharStream &operator<<(OutputCharStream &os, long n)
{
  if (n < 0)
    return os << '-' << (unsigned long)-n;
  else
    return os << (unsigned long)n;
}

inline
const char *boolString(bool b)
{
  return b ? trueString : falseString;
}

inline
OutputCharStream &operator<<(OutputCharStream &os, FOTBuilder::Symbol sym)
{
  switch (sym) {
  case FOTBuilder::symbolFalse:
    os << falseString;
    break;
  case FOTBuilder::symbolTrue:
    os << trueString;
    break;
  default:
    os << FOTBuilder::symbolName(sym);
    break;
  }
  return os;
}

inline
OutputCharStream &operator<<(OutputCharStream &os, double d)
{
  char buf[128];
  sprintf(buf, "%g", d);
  return os << buf;
}

OutputCharStream &operator<<(OutputCharStream &os, Units u)
{
  char buf[32];
  sprintf(buf, "%03d", u.n);
  int len = strlen(buf);
  int i = 0;
  for (; i < 3 && buf[len - i - 1] == '0'; i++)
    ;
  if (i == 3) {
    i = len - 3;
    if (i == 0)
      buf[i++] = '0';
  }
  else {
    int j = len + 1 - i;
    for (; i < 3; i++)
      buf[len - i] = buf[len - i - 1];
    buf[len - 3] = '.';
    i = j;
  }
  strcpy(buf + i, "pt");
  return os << buf;
}

inline
OutputCharStream &operator<<(OutputCharStream &os,
			     const FOTBuilder::LengthSpec &ls)
{
  if (ls.displaySizeFactor != 0.0) {
    // 100+20%
    if (ls.length) {
      os << Units(ls.length);
      if (ls.displaySizeFactor >= 0.0)
	os << '+';
    }
    char buf[128];
    sprintf(buf, "%.2f%%", ls.displaySizeFactor * 100.0);
  }
  else
    os << Units(ls.length);
  return os;
}

OutputCharStream &operator<<(OutputCharStream &os,
			     const FOTBuilder::TableLengthSpec &ls)
{
  if (ls.displaySizeFactor != 0.0 || ls.tableUnitFactor != 0.0) {
    // 100+20%
    bool needSign = 0;
    if (ls.length) {
      os << Units(ls.length);
      needSign = 1;
    }
    char buf[128];
    if (ls.displaySizeFactor != 0.0) {
      if (needSign && ls.displaySizeFactor >= 0.0)
	os << '+';
      sprintf(buf, "%.2f%%", ls.displaySizeFactor * 100.0);
      needSign = 1;
      os << buf;
    }
    if (ls.tableUnitFactor != 0.0) {
      if (needSign && ls.tableUnitFactor >= 0.0)
	os << '+';
      sprintf(buf, "%.2f*", ls.tableUnitFactor);
      os << buf;
    }
  }
  else
    os << Units(ls.length);
  return os;
}

OutputCharStream &operator<<(OutputCharStream &os,
			     SgmlFOTBuilder::Data data)
{
  const Char *s = data.ptr;
  size_t n = data.size;
  for (; n > 0; n--, s++) {
    switch (*s) {
    case '&':
      os << "&amp;";
      break;
    case '<':
      os << "&lt;";
      break;
    case '>':
      os << "&gt;";
      break;
    case '"':
      os << "&quot;";
      break;
    default:
      if (*s < 0x80)
	os.put(*s);
      else
	os << "&#" << (unsigned long)*s << ';';
      break;
    }
  }
  return os;
}

inline
OutputCharStream &operator<<(OutputCharStream &os,
			     const FOTBuilder::GlyphId &gid)
{
  if (gid.publicId) {
    os << gid.publicId;
    if (gid.suffix)
      os << "::" << gid.suffix;
  }
  else
    os << falseString;
  return os;
}

FOTBuilder *makeSgmlFOTBuilder(OutputCharStream *os)
{
  return new SgmlFOTBuilder(os);
}

static
void outputNumericCharRef(OutputCharStream &os, Char c)
{
  os << "&#" << (unsigned long)c << ';';
}

SgmlFOTBuilder::SgmlFOTBuilder(OutputCharStream *os)
: os_(os),
  curOs_(os),
  nodeLevel_(0),
  nPendingElementsNonEmpty_(0),
  suppressAnchors_(0)
{
  os->setEscaper(outputNumericCharRef);
  *os_ << "<?xml version=\"1.0\"?>" << RE;
#if 0
  *os_ << "<!DOCTYPE fot PUBLIC \"-//James Clark//DTD DSSSL Flow Object Tree//EN\">"
       << RE;
#endif
  *os_ << "<fot>" << RE;
}

SgmlFOTBuilder::~SgmlFOTBuilder()
{
  os() << "</fot>" << RE;
}

void SgmlFOTBuilder::characters(const Char *s, size_t n)
{
  if (n == 0)
    return;
  flushPendingElements();
  os() << "<text>" << Data(s, n) << "</text>" << RE;
}

void SgmlFOTBuilder::charactersFromNode(const NodePtr &nd, const Char *s, size_t n)
{
  GroveString name;
  GroveString text;
  NodePtr entity;
  if (n == 1
      && *s == 0xFFFD
      && nd->getEntityName(name) == accessOK
      && nd->getEntity(entity) == accessOK
      && entity->getText(text) == accessOK)
    os() << "<sdata name=" << quot << Data(name.data(), name.size()) << quot
         << " text=" << quot << Data(text.data(), text.size()) << quot
	 << "/>" << RE;
  else
    SgmlFOTBuilder::characters(s, n);
}

void SgmlFOTBuilder::characterNIC(const CharacterNIC &nic)
{
  if (nic.specifiedC) {
    if (nic.specifiedC & (1 << CharacterNIC::cChar))
      os() << " char=" << quot << "&#" << (unsigned long)nic.ch << ";" << quot;
    if (nic.specifiedC & (1 << CharacterNIC::cGlyphId))
      os() << " glyph-id=" << quot << nic.glyphId << quot;
    if (nic.specifiedC & (1 << CharacterNIC::cIsDropAfterLineBreak))
      os() << " drop-after-line-break=" << quot << boolString(nic.isDropAfterLineBreak) << quot;
    if (nic.specifiedC & (1 << CharacterNIC::cIsDropUnlessBeforeLineBreak))
      os() << " drop-unless-before-line-break=" << quot << boolString(nic.isDropUnlessBeforeLineBreak) << quot;
    if (nic.specifiedC & (1 << CharacterNIC::cIsPunct))
      os() << " punct=" << quot << boolString(nic.isPunct) << quot;
    if (nic.specifiedC & (1 << CharacterNIC::cIsInputWhitespace))
      os() << " input-whitespace=" << quot << boolString(nic.isInputWhitespace) << quot;
    if (nic.specifiedC & (1 << CharacterNIC::cIsInputTab))
      os() << " input-tab=" << quot << boolString(nic.isInputTab) << quot;
    if (nic.specifiedC & (1 << CharacterNIC::cIsRecordEnd))
      os() << " record-end=" << quot << boolString(nic.isRecordEnd) << quot;
    if (nic.specifiedC & (1 << CharacterNIC::cIsSpace))
      os() << " space=" << quot << boolString(nic.isSpace) << quot;
    if (nic.specifiedC & (1 << CharacterNIC::cScript)) {
      if (nic.script)
	os() << " script=" << quot << nic.script << quot;
      else
	os() << " script=" << quot << falseString << quot;
    }
    if (nic.specifiedC & (1 << CharacterNIC::cMathClass))
      os() << " math-class=" << quot << nic.mathClass << quot;
    if (nic.specifiedC & (1 << CharacterNIC::cMathFontPosture))
      os() << " math-font-posture=" << quot << nic.mathFontPosture << quot;
    if (nic.specifiedC & (1 << CharacterNIC::cBreakBeforePriority)) 
      os() << " break-before-priority=" << quot << nic.breakBeforePriority << quot;
    if (nic.specifiedC & (1 << CharacterNIC::cBreakAfterPriority))
      os() << " break-after-priority=" << quot << nic.breakAfterPriority << quot;
  }
  if (nic.stretchFactor != 1.0)
    os() << " stretch-factor=" << quot << nic.stretchFactor << quot;
}

void SgmlFOTBuilder::character(const CharacterNIC &nic)
{
  flushPendingElements();
  os() << "<character";
  characterNIC(nic);
  outputIcs();
  os() << RE << "/>";
}

void SgmlFOTBuilder::outputIcs()
{
  StringC str;
  ics_.extractString(str);
  os() << str;
}

void SgmlFOTBuilder::setFontSize(Length n)
{
  lengthC("font-size", n);
}

void SgmlFOTBuilder::setFontFamilyName(const StringC &str)
{
  ics_ << " font-family-name=" << quot << str << quot;
}

void SgmlFOTBuilder::setFontWeight(Symbol weight)
{
  symbolC("font-weight", weight);
}

void SgmlFOTBuilder::setFontPosture(Symbol posture)
{
  symbolC("font-posture", posture);
}

void SgmlFOTBuilder::setStartIndent(const LengthSpec &ls)
{
  lengthSpecC("start-indent", ls);
}

void SgmlFOTBuilder::setEndIndent(const LengthSpec &ls)
{
  lengthSpecC("end-indent", ls);
}

void SgmlFOTBuilder::setFirstLineStartIndent(const LengthSpec &ls)
{
  lengthSpecC("first-line-start-indent", ls);
}

void SgmlFOTBuilder::setLastLineEndIndent(const LengthSpec &ls)
{
  lengthSpecC("last-line-end-indent", ls);
}

void SgmlFOTBuilder::setLineSpacing(const LengthSpec &ls)
{
  lengthSpecC("line-spacing", ls);
}

void SgmlFOTBuilder::setFieldWidth(const LengthSpec &ls)
{
  lengthSpecC("field-width", ls);
}

void SgmlFOTBuilder::setPositionPointShift(const LengthSpec &ls)
{
  lengthSpecC("position-point-shift", ls);
}

void SgmlFOTBuilder::setStartMargin(const LengthSpec &ls)
{
  lengthSpecC("start-margin", ls);
}

void SgmlFOTBuilder::setEndMargin(const LengthSpec &ls)
{
  lengthSpecC("end-margin", ls);
}

void SgmlFOTBuilder::setSidelineSep(const LengthSpec &ls)
{
  lengthSpecC("sideline-sep", ls);
}

void SgmlFOTBuilder::setAsisWrapIndent(const LengthSpec &ls)
{
  lengthSpecC("asis-wrap-indent", ls);
}

void SgmlFOTBuilder::setLineNumberSep(const LengthSpec &ls)
{
  lengthSpecC("line-number-sep", ls);
}

void SgmlFOTBuilder::setLastLineJustifyLimit(const LengthSpec &ls)
{
  lengthSpecC("last-line-justify-limit", ls);
}

void SgmlFOTBuilder::setJustifyGlyphSpaceMaxAdd(const LengthSpec &ls)
{
  lengthSpecC("justify-glyph-space-max-add", ls);
}

void SgmlFOTBuilder::setJustifyGlyphSpaceMaxRemove(const LengthSpec &ls)
{
  lengthSpecC("justify-glyph-space-max-remove", ls);
}

void SgmlFOTBuilder::setTableCornerRadius(const LengthSpec &ls)
{
  lengthSpecC("table-corner-radius", ls);
}

void SgmlFOTBuilder::setBoxCornerRadius(const LengthSpec &ls)
{
  lengthSpecC("box-corner-radius", ls);
}

void SgmlFOTBuilder::setMarginaliaSep(const LengthSpec &ls)
{
  lengthSpecC("marginalia-sep", ls);
}

void SgmlFOTBuilder::lengthSpecC(const char *s, const LengthSpec &ls)
{
  ics_ << ' ' << s << '=' << quot << ls << quot;
}

void SgmlFOTBuilder::setMinPreLineSpacing(const OptLengthSpec &ols)
{
  optLengthSpecC("min-pre-line-spacing", ols);
}

void SgmlFOTBuilder::setMinPostLineSpacing(const OptLengthSpec &ols)
{
  optLengthSpecC("min-post-line-spacing", ols);
}

void SgmlFOTBuilder::setMinLeading(const OptLengthSpec &ols)
{
  optLengthSpecC("min-leading", ols);
}

void SgmlFOTBuilder::optLengthSpecC(const char *s, const OptLengthSpec &ols)
{
  if (ols.hasLength)
    lengthSpecC(s, ols.length);
  else
    ics_ << ' ' << s << quot << falseString << quot;
}

void SgmlFOTBuilder::setLines(Symbol sym)
{
  symbolC("lines", sym);
}

void SgmlFOTBuilder::setQuadding(Symbol sym)
{
  symbolC("quadding", sym);
}

void SgmlFOTBuilder::setDisplayAlignment(Symbol sym)
{
  symbolC("display-alignment", sym);
}

void SgmlFOTBuilder::setFieldAlign(Symbol sym)
{
  symbolC("field-align", sym);
}

void SgmlFOTBuilder::setLineJoin(Symbol sym)
{
  symbolC("line-join", sym);
}

void SgmlFOTBuilder::setLineCap(Symbol sym)
{
  symbolC("line-cap", sym);
}

void SgmlFOTBuilder::setLineNumberSide(Symbol sym)
{
  symbolC("line-number-side", sym);
}

void SgmlFOTBuilder::setKernMode(Symbol sym)
{
  symbolC("kern-mode", sym);
}

void SgmlFOTBuilder::setInputWhitespaceTreatment(Symbol sym)
{
  symbolC("input-whitespace-treatment", sym);
}

void SgmlFOTBuilder::setFillingDirection(Symbol sym)
{
  symbolC("filling-direction", sym);
}

void SgmlFOTBuilder::setWritingMode(Symbol sym)
{
  symbolC("writing-mode", sym);
}

void SgmlFOTBuilder::setLastLineQuadding(Symbol sym)
{
  symbolC("last-line-quadding", sym);
}

void SgmlFOTBuilder::setMathDisplayMode(Symbol sym)
{
  symbolC("math-display-mode", sym);
}

void SgmlFOTBuilder::setScriptPreAlign(Symbol sym)
{
  symbolC("script-pre-align", sym);
}

void SgmlFOTBuilder::setScriptPostAlign(Symbol sym)
{
  symbolC("script-post-align", sym);
}

void SgmlFOTBuilder::setScriptMidSupAlign(Symbol sym)
{
  symbolC("script-mid-sup-align", sym);
}

void SgmlFOTBuilder::setScriptMidSubAlign(Symbol sym)
{
  symbolC("script-mid-sub-align", sym);
}

void SgmlFOTBuilder::setNumeratorAlign(Symbol sym)
{
  symbolC("numerator-align", sym);
}

void SgmlFOTBuilder::setDenominatorAlign(Symbol sym)
{
  symbolC("denominator-align", sym);
}

void SgmlFOTBuilder::setGridPositionCellType(Symbol sym)
{
  symbolC("grid-position-cell-type", sym);
}

void SgmlFOTBuilder::setGridColumnAlignment(Symbol sym)
{
  symbolC("grid-column-alignment", sym);
}

void SgmlFOTBuilder::setGridRowAlignment(Symbol sym)
{
  symbolC("grid-row-alignment", sym);
}

void SgmlFOTBuilder::setBoxType(Symbol sym)
{
  symbolC("box-type", sym);
}

void SgmlFOTBuilder::setGlyphAlignmentMode(Symbol sym)
{
  symbolC("glyph-alignment-mode", sym);
}

void SgmlFOTBuilder::setBoxBorderAlignment(Symbol sym)
{
  symbolC("box-border-alignment", sym);
}

void SgmlFOTBuilder::setCellRowAlignment(Symbol sym)
{
  symbolC("cell-row-alignment", sym);
}

void SgmlFOTBuilder::setBorderAlignment(Symbol sym)
{
  symbolC("border-alignment", sym);
}

void SgmlFOTBuilder::setSidelineSide(Symbol sym)
{
  symbolC("sideline-side", sym);
}

void SgmlFOTBuilder::setHyphenationKeep(Symbol sym)
{
  symbolC("hyphenation-keep", sym);
}

void SgmlFOTBuilder::setFontStructure(Symbol sym)
{
  symbolC("font-structure", sym);
}

void SgmlFOTBuilder::setFontProportionateWidth(Symbol sym)
{
  symbolC("font-proportionate-width", sym);
}

void SgmlFOTBuilder::setCellCrossed(Symbol sym)
{
  symbolC("cell-crossed", sym);
}

void SgmlFOTBuilder::setMarginaliaSide(Symbol sym)
{
  symbolC("marginalia-side", sym);
}

void SgmlFOTBuilder::symbolC(const char *name, Symbol sym)
{
  ics_ << ' ' << name << '=' << quot << sym << quot;
}

inline
void hex2(OutputCharStream &os, unsigned char c)
{
  static const char hexDigits[] = "0123456789ABCDEF";
  os << hexDigits[c >> 4] << hexDigits[c & 0xF];
}

inline
OutputCharStream &operator<<(OutputCharStream &os, const FOTBuilder::DeviceRGBColor &color)
{
  os << '#';
  hex2(os, color.red);
  hex2(os, color.green);
  hex2(os, color.blue);
  return os;
}

void SgmlFOTBuilder::setColor(const DeviceRGBColor &color)
{
  ics_ << " color=" << quot << color << quot;
}

void SgmlFOTBuilder::setBackgroundColor()
{
  ics_ << " background-color=" << quot << falseString << quot;
}

void SgmlFOTBuilder::setBackgroundColor(const DeviceRGBColor &color)
{
  ics_ << " background-color=" << quot << color << quot;
}

void SgmlFOTBuilder::setPageWidth(Length units)
{
  lengthC("page-width", units);
}

void SgmlFOTBuilder::setPageHeight(Length units)
{
  lengthC("page-height", units);
}

void SgmlFOTBuilder::setLeftMargin(Length units)
{
  lengthC("left-margin", units);
}

void SgmlFOTBuilder::setRightMargin(Length units)
{
  lengthC("right-margin", units);
}

void SgmlFOTBuilder::setTopMargin(Length units)
{
  lengthC("top-margin", units);
}

void SgmlFOTBuilder::setBottomMargin(Length units)
{
  lengthC("bottom-margin", units);
}

void SgmlFOTBuilder::setHeaderMargin(Length units)
{
  lengthC("header-margin", units);
}

void SgmlFOTBuilder::setFooterMargin(Length units)
{
  lengthC("footer-margin", units);
}

void SgmlFOTBuilder::setLineThickness(Length units)
{
  lengthC("line-thickness", units);
}

void SgmlFOTBuilder::setCellBeforeRowMargin(Length units)
{
  lengthC("cell-before-row-margin", units);
}

void SgmlFOTBuilder::setCellAfterRowMargin(Length units)
{
  lengthC("cell-after-row-margin", units);
}

void SgmlFOTBuilder::setCellBeforeColumnMargin(Length units)
{
  lengthC("cell-before-column-margin", units);
}

void SgmlFOTBuilder::setCellAfterColumnMargin(Length units)
{
  lengthC("cell-after-column-margin", units);
}

void SgmlFOTBuilder::setLineSep(Length units)
{
  lengthC("line-sep", units);
}

void SgmlFOTBuilder::setBoxSizeBefore(Length units)
{
  lengthC("box-size-before", units);
}

void SgmlFOTBuilder::setBoxSizeAfter(Length units)
{
  lengthC("box-size-after", units);
}

void SgmlFOTBuilder::lengthC(const char *s, Length units)
{
  ics_ << ' ' << s << '=' << quot << Units(units) << quot;
}

void SgmlFOTBuilder::setLayer(long n)
{
  integerC("layer", n);
}

void SgmlFOTBuilder::setBackgroundLayer(long n)
{
  integerC("background-layer", n);
}

void SgmlFOTBuilder::setBorderPriority(long n)
{
  integerC("border-priority", n);
}

void SgmlFOTBuilder::setLineRepeat(long n)
{
  integerC("line-repeat", n);
}

void SgmlFOTBuilder::setSpan(long n)
{
  integerC("span", n);
}

void SgmlFOTBuilder::setMinLeaderRepeat(long n)
{
  integerC("min-leader-repeat", n);
}

void SgmlFOTBuilder::setHyphenationRemainCharCount(long n)
{
  integerC("hyphenation-remain-char-count", n);
}

void SgmlFOTBuilder::setHyphenationPushCharCount(long n)
{
  integerC("hyphenation-push-char-count", n);
}

void SgmlFOTBuilder::setWidowCount(long n)
{
  integerC("widow-count", n);
}

void SgmlFOTBuilder::setOrphanCount(long n)
{
  integerC("orphan-count", n);
}

void SgmlFOTBuilder::integerC(const char *s, long n)
{
  ics_ << ' ' << s << '=' << quot << n << quot;
}

void SgmlFOTBuilder::setExpandTabs(long n)
{
  integerC("expand-tabs", n);
}

void SgmlFOTBuilder::setHyphenationLadderCount(long n)
{
  integerC("hyphenation-ladder-count", n);
}

void SgmlFOTBuilder::setCountry(Letter2 code)
{
  ics_ << " country=" << quot;
  if (code)
    ics_ << char((code >> 8) & 0xff) << char(code & 0xff);
  else
    ics_ << falseString;
  ics_ << quot;
}

void SgmlFOTBuilder::setLanguage(Letter2 code)
{
  ics_ << " language=" << quot;
  if (code)
    ics_ << char((code >> 8) & 0xff) << char(code & 0xff);
  else
    ics_ << falseString;
  ics_ << quot;
}

void SgmlFOTBuilder::setBackgroundTile(PublicId pubid)
{
  publicIdC("background-tile", pubid);
}

void SgmlFOTBuilder::setLineBreakingMethod(PublicId pubid)
{
  publicIdC("line-breaking-method", pubid);
}

void SgmlFOTBuilder::setLineCompositionMethod(PublicId pubid)
{
  publicIdC("line-composition-method", pubid);
}

void SgmlFOTBuilder::setImplicitBidiMethod(PublicId pubid)
{
  publicIdC("implicit-bidi-method", pubid);
}

void SgmlFOTBuilder::setGlyphSubstMethod(PublicId pubid)
{
  publicIdC("glyph-subst-method", pubid);
}

void SgmlFOTBuilder::setGlyphReorderMethod(PublicId pubid)
{
  publicIdC("glyph-reorder-method", pubid);
}

void SgmlFOTBuilder::setHyphenationMethod(PublicId pubid)
{
  publicIdC("hyphenation-method", pubid);
}

void SgmlFOTBuilder::setTableAutoWidthMethod(PublicId pubid)
{
  publicIdC("table-auto-width-method", pubid);
}

void SgmlFOTBuilder::setFontName(PublicId pubid)
{
  publicIdC("font-name", pubid);
}

void SgmlFOTBuilder::publicIdC(const char *s, PublicId pubid)
{
  ics_ << ' ' << s << '=' << quot;
  if (pubid)
    ics_ << pubid;
  else
    ics_ << falseString;
  ics_ << quot;
}

void SgmlFOTBuilder::setBorderPresent(bool b)
{
  boolC("border-present", b);
}

void SgmlFOTBuilder::setInhibitLineBreaks(bool b)
{
  boolC("border-present", b);
}

void SgmlFOTBuilder::setHyphenate(bool b)
{
  boolC("hyphenate", b);
}

void SgmlFOTBuilder::setKern(bool b)
{
  boolC("kern", b);
}

void SgmlFOTBuilder::setLigature(bool b)
{
  boolC("ligature", b);
}

void SgmlFOTBuilder::setScoreSpaces(bool b)
{
  boolC("score-spaces", b);
}

void SgmlFOTBuilder::setFloatOutMarginalia(bool b)
{
  boolC("float-out-marginalia", b);
}

void SgmlFOTBuilder::setFloatOutSidelines(bool b)
{
  boolC("float-out-sidelines", b);
}

void SgmlFOTBuilder::setFloatOutLineNumbers(bool b)
{
  boolC("float-out-line-numbers", b);
}

void SgmlFOTBuilder::setCellBackground(bool b)
{
  boolC("cell-background", b);
}

void SgmlFOTBuilder::setSpanWeak(bool b)
{
  boolC("span-weak", b);
}

void SgmlFOTBuilder::setIgnoreRecordEnd(bool b)
{
  boolC("ignore-record-end", b);
}

void SgmlFOTBuilder::setNumberedLines(bool b)
{
  boolC("numbered-lines", b);
}

void SgmlFOTBuilder::setHangingPunct(bool b)
{
  boolC("hanging-punct", b);
}

void SgmlFOTBuilder::setBoxOpenEnd(bool b)
{
  boolC("box-open-end", b);
}

void SgmlFOTBuilder::setTruncateLeader(bool b)
{
  boolC("truncate-leader", b);
}

void SgmlFOTBuilder::setAlignLeader(bool b)
{
  boolC("align-leader", b);
}

void SgmlFOTBuilder::setTablePartOmitMiddleHeader(bool b)
{
  boolC("table-part-omit-middle-header", b);
}

void SgmlFOTBuilder::setTablePartOmitMiddleFooter(bool b)
{
  boolC("table-part-omit-middle-footer", b);
}

void SgmlFOTBuilder::setBorderOmitAtBreak(bool b)
{
  boolC("border-omit-at-break", b);
}

void SgmlFOTBuilder::setPrincipalModeSimultaneous(bool b)
{
  boolC("principal-mode-simultaneous", b);
}

void SgmlFOTBuilder::setMarginaliaKeepWithPrevious(bool b)
{
  boolC("marginalia-keep-with-previous", b);
}

void SgmlFOTBuilder::setGridEquidistantRows(bool b)
{
  boolC("grid-equidistant-rows", b);
}

void SgmlFOTBuilder::setGridEquidistantColumns(bool b)
{
  boolC("grid-equidistant-columns", b);
}

void SgmlFOTBuilder::boolC(const char *s, bool b)
{
  ics_ << ' ' << s << '=' << quot << boolString(b) << quot;
}

void SgmlFOTBuilder::setEscapementSpaceBefore(const InlineSpace &is)
{
  inlineSpaceC("escapement-space-before", is);
}

void SgmlFOTBuilder::setEscapementSpaceAfter(const InlineSpace &is)
{
  inlineSpaceC("escapement-space-after", is);
}

void SgmlFOTBuilder::setGlyphSubstTable(const Vector<ConstPtr<GlyphSubstTable> > &tables)
{
  if (tables.size() == 0) {
    ics_ << " glyph-subst-table=" << quot << quot;
    return;
  }
  static const char idPrefix[] = "gst";
  ics_ << " glyph-subst-table=" << quot;
  for (size_t i = 0; i < tables.size(); i++) {
    unsigned long n = tables[i]->uniqueId;
    if (n >= glyphSubstTableDefined_.size()) {
      for (size_t j = glyphSubstTableDefined_.size(); j < n + 1; j++)
	glyphSubstTableDefined_.push_back(0);
    }
    if (!glyphSubstTableDefined_[n]) {
      glyphSubstTableDefined_[n] = 1;
      os() << "<define-glyph-subst-table id=" << quot << idPrefix << n << quot << '>' << RE;
      const Vector<GlyphId> &pairs = tables[i]->pairs;
      for (size_t i = 0; i < pairs.size(); i += 2)
	os() << "<glyph-subst old=" << quot << pairs[i] << quot
	     << " new=" << quot << pairs[i + 1] << quot << "/>" << RE;
      os() << "</define-glyph-subst-table>";
    }
    if (i > 0)
      ics_ << ' ';
    ics_ << idPrefix << n;
  }
  ics_ << quot;
}

void SgmlFOTBuilder::inlineSpaceC(const char *s, const InlineSpace &is)
{
  if (is.nominal || is.min || is.max) {
    ics_ << ' ' << s << '=' << quot << is.nominal;
    if (is.min.length != is.nominal.length
        || is.min.displaySizeFactor != is.nominal.displaySizeFactor
	|| is.max.length != is.nominal.length
        || is.max.displaySizeFactor != is.nominal.displaySizeFactor)
      ics_ << ',' << is.min << ',' << is.max;
  }
}

void SgmlFOTBuilder::displayNIC(const DisplayNIC &nic)
{
  if (nic.keepWithPrevious)
    os() << " keep-with-previous=" << quot << trueString << quot;
  if (nic.keepWithNext)
    os() << " keep-with-next=" << quot << trueString << quot;
  if (nic.mayViolateKeepBefore)
    os() << " may-violate-keep-before=" << quot << trueString << quot;
  if (nic.mayViolateKeepAfter)
    os() << " may-violate-keep-after=" << quot << trueString << quot;
  if (nic.positionPreference != symbolFalse)
    os() << " position-preference=" << quot << nic.positionPreference << quot;
  if (nic.keep != symbolFalse)
    os() << " keep=" << quot << nic.keep << quot;
  if (nic.breakBefore != symbolFalse)
    os() << " break-before=" << quot << nic.breakBefore << quot;
  if (nic.breakAfter != symbolFalse)
    os() << " break-after=" << quot << nic.breakAfter << quot;
  displaySpaceNIC("space-before", nic.spaceBefore);
  displaySpaceNIC("space-after", nic.spaceAfter);
}

void SgmlFOTBuilder::displaySpaceNIC(const char *s, const DisplaySpace &ds)
{
  if (ds.nominal || ds.min || ds.max) {
    os() << ' ' << s << '=' << quot << ds.nominal;
    if (ds.min.length != ds.nominal.length
        || ds.min.displaySizeFactor != ds.nominal.displaySizeFactor
        || ds.max.length != ds.nominal.length
        || ds.max.displaySizeFactor != ds.nominal.displaySizeFactor)
      os() << ',' << ds.min << ',' << ds.max;
    os() << quot;
  }
  if (ds.force)
    os() << ' ' << s << "-priority=" << quot << "force" << quot;
  else if (ds.priority)
    os() << ' ' << s << "-priority=" << quot << ds.priority << quot;
  if (!ds.conditional)
    os() << ' ' << s << "-conditional=" << quot << falseString << quot;
}

void SgmlFOTBuilder::inlineNIC(const InlineNIC &nic)
{
  if (nic.breakBeforePriority)
    os() << " break-before-priority=" << quot << nic.breakBeforePriority << quot;
  if (nic.breakAfterPriority)
    os() << " break-after-priority=" << quot << nic.breakAfterPriority << quot;
}

void SgmlFOTBuilder::startParagraph(const ParagraphNIC &nic)
{
  os() << "<paragraph";
  displayNIC(nic);
  outputIcs();
  os() << ">" << RE;
}

void SgmlFOTBuilder::endParagraph()
{
  endFlow("paragraph");
}

void SgmlFOTBuilder::paragraphBreak(const ParagraphNIC &nic)
{
  os() << "<paragraph-break";
  displayNIC(nic);
  outputIcs();
  os() << "/>" << RE;
}

void SgmlFOTBuilder::startDisplayGroup(const DisplayGroupNIC &nic)
{
  os() << "<display-group";
  if (nic.hasCoalesceId)
    os() << " coalesce-id=" << quot << Data(nic.coalesceId) << quot;
  displayNIC(nic);
  outputIcs();
  os() << ">" << RE;
}

void SgmlFOTBuilder::endDisplayGroup()
{
  endFlow("display-group");
}

void SgmlFOTBuilder::simpleFlowObj(const char *name)
{
  os() << '<' << name;
  outputIcs();
  os() << "/>" << RE;
}

void SgmlFOTBuilder::startSimpleFlowObj(const char *name)
{
  os() << '<' << name;
  outputIcs();
  os() << ">" << RE;
}

void SgmlFOTBuilder::startPortFlow(const char *name)
{
  os() << "<" << name << ">" << RE;
}

void SgmlFOTBuilder::endFlow(const char *name)
{
  os() << "</" << name << ">" << RE;
}

void SgmlFOTBuilder::startSequence()
{
  startSimpleFlowObj("sequence");
}
 
void SgmlFOTBuilder::endSequence()
{
  endFlow("sequence");
}

void SgmlFOTBuilder::startLineField(const LineFieldNIC &)
{
  flushPendingElements();
  startSimpleFlowObj("line-field");
}
 
void SgmlFOTBuilder::endLineField()
{
  endFlow("line-field");
}

void SgmlFOTBuilder::startScroll()
{
  startSimpleFlowObj("scroll");
}

void SgmlFOTBuilder::endScroll()
{
  endFlow("scroll");
}

void SgmlFOTBuilder::startLink(const Address &addr)
{
  os() << "<link";
  outputIcs();
  switch (addr.type) {
  case Address::sgmlDocument:
    // FIXME
    break;
  case Address::resolvedNode:
    if (!nodeIsElement(addr.node))
      break;
    os() << " destination=" << quot;
    outputElementName(addr.node);
    os() << quot;
    break;
  case Address::idref:
    os() << " destination=" << quot;
    outputElementName(addr.node->groveIndex(), addr.params[0].data(), addr.params[0].size());
    os() << quot;
    break;
  case Address::entity:
    // FIXME
    break;
  default:
    break;
  }
  os() << '>' << RE;
}

void SgmlFOTBuilder::endLink()
{
  endFlow("link");
}

void SgmlFOTBuilder::startMarginalia()
{
  startSimpleFlowObj("marginalia");
}

void SgmlFOTBuilder::endMarginalia()
{
  endFlow("marginalia");
}

void SgmlFOTBuilder::startMultiModeSerial(const MultiMode *principalMode)
{
  startSimpleFlowObj("multi-mode");
  if (principalMode) {
    os() << "<multi-mode.mode";
    if (principalMode->hasDesc)
      os() << " desc=" << quot << Data(principalMode->desc) << quot;
    os() << '>' << RE;
  }
  multiModeHasPrincipalMode_.push_back(principalMode != 0);
}

void SgmlFOTBuilder::endMultiModeSerial()
{
  if (multiModeHasPrincipalMode_.back())
    endFlow("multi-mode.mode");
  multiModeHasPrincipalMode_.resize(multiModeHasPrincipalMode_.size() - 1);
  endFlow("multi-mode");
}

void SgmlFOTBuilder::startMultiModeMode(const MultiMode &mode)
{
  if (multiModeHasPrincipalMode_.back()) {
    endFlow("multi-mode.mode");
    multiModeHasPrincipalMode_.back() = 0;
  }
  os() << "<multi-mode.mode name=" << quot << Data(mode.name) << quot;
  if (mode.hasDesc)
    os() << " desc=" << quot << Data(mode.desc) << quot;
  os () << '>' << RE;
}

void SgmlFOTBuilder::endMultiModeMode()
{
  endFlow("multi-mode.mode");
}

void SgmlFOTBuilder::startScore(Symbol type)
{
  const char *s = symbolName(type);
  if (s) {
    os() << "<score type=" << quot << s << quot;
    outputIcs();
    os() << '>' << RE;
  }
}

void SgmlFOTBuilder::startScore(const LengthSpec &ls)
{
  os() << "<score type=" << quot << ls << quot;
  outputIcs();
  os() << '>' << RE;
}

void SgmlFOTBuilder::startScore(Char c)
{
  os() << "<score type=\"char\" char=" << quot;
  os().put(c);
  os() << quot;
  outputIcs();
  os() << '>' << RE;
}

void SgmlFOTBuilder::endScore()
{
  endFlow("score");
}

void SgmlFOTBuilder::startSimplePageSequence()
{
  startSimpleFlowObj("simple-page-sequence");
  suppressAnchors_ = 1;
  curOs_ = &hfs_;
}
 
void SgmlFOTBuilder::endSimplePageSequence()
{
  endFlow("simple-page-sequence");
}

void SgmlFOTBuilder::startSimplePageSequenceHeaderFooter(unsigned flags)
{
}

void SgmlFOTBuilder::endSimplePageSequenceHeaderFooter(unsigned flags)
{
  hfs_.extractString(hf_[flags]);
}

void SgmlFOTBuilder::endAllSimplePageSequenceHeaderFooter()
{
  curOs_ = os_.pointer();
  suppressAnchors_ = 0;
  for (int i = 0; i < nHF; i += nHF/6) {
    int front;
    if (hf_[i + (firstHF|frontHF)] != hf_[i + (firstHF|backHF)]
        || hf_[i + (otherHF|frontHF)] != hf_[i + (otherHF|backHF)])
      front = frontHF;
    else
      front = 0;
    int first;
    if (hf_[i + (firstHF|frontHF)] != hf_[i + (otherHF|frontHF)]
        || hf_[i + (firstHF|backHF)] != hf_[i + (otherHF|backHF)])
      first = firstHF;
    else
      first = 0;
    for (int j = 0; j <= front; j += frontHF) {
      for (int k = 0; k <= first; k += firstHF) {
	const StringC &str = hf_[i + j + k];
	if (str.size() != 0) {
	  const char *side;
	  if (i & centerHF)
	    side = "center";
	  else if (i & rightHF)
	    side = "right";
	  else
	    side = "left";
	  const char *hf = (i & headerHF) ? "header" : "footer";
	  os() << "<simple-page-sequence." << side << '-' << hf;
	  if (front)
	    os() << " front=" << quot << boolString(j != 0) << quot;
	  if (first)
	    os() << " first=" << quot << boolString(k != 0) << quot;
	  os() << '>' << RE;
	  os() << str << "</simple-page-sequence." << side << '-' << hf << '>' << RE;
	}
      }
    }
  }

}

void SgmlFOTBuilder::pageNumber()
{
  os() << "<page-number/>" << RE;
}

void SgmlFOTBuilder::startMathSequence()
{
  startSimpleFlowObj("math-sequence");
}

void SgmlFOTBuilder::endMathSequence()
{
  endFlow("math-sequence");
}

void SgmlFOTBuilder::startFractionSerial()
{
  startSimpleFlowObj("fraction");
}

void SgmlFOTBuilder::endFractionSerial()
{
  endFlow("fraction");
}

void SgmlFOTBuilder::startFractionNumerator()
{
  startPortFlow("fraction.numerator");
}

void SgmlFOTBuilder::endFractionNumerator()
{
  endFlow("fraction.numerator");
}

void SgmlFOTBuilder::startFractionDenominator()
{
  startPortFlow("fraction.denominator");
}

void SgmlFOTBuilder::endFractionDenominator()
{
  endFlow("fraction.denominator");
}

void SgmlFOTBuilder::fractionBar()
{
  simpleFlowObj("fraction.fraction-bar");
}

void SgmlFOTBuilder::startUnmath()
{
  startSimpleFlowObj("unmath");
}

void SgmlFOTBuilder::endUnmath()
{
  endFlow("unmath");
}

void SgmlFOTBuilder::startSuperscript()
{
  startSimpleFlowObj("superscript");
}

void SgmlFOTBuilder::endSuperscript()
{
  endFlow("superscript");
}

void SgmlFOTBuilder::startSubscript()
{
  startSimpleFlowObj("subscript");
}

void SgmlFOTBuilder::endSubscript()
{
  endFlow("subscript");
}

void SgmlFOTBuilder::startScriptSerial()
{
  startSimpleFlowObj("script");
  startPortFlow("script.principal");
}

void SgmlFOTBuilder::endScriptSerial()
{
  endFlow("script");
}

void SgmlFOTBuilder::startScriptPreSup()
{
  endFlow("script.principal");
  startPortFlow("script.pre-sup");
}

void SgmlFOTBuilder::endScriptPreSup()
{
  endFlow("script.pre-sup");
}

void SgmlFOTBuilder::startScriptPreSub()
{
  startPortFlow("script.pre-sub");
}

void SgmlFOTBuilder::endScriptPreSub()
{
  endFlow("script.pre-sub");
}

void SgmlFOTBuilder::startScriptPostSup()
{
  startPortFlow("script.post-sup");
}

void SgmlFOTBuilder::endScriptPostSup()
{
  endFlow("script.post-sup");
}

void SgmlFOTBuilder::startScriptPostSub()
{
  startPortFlow("script.post-sub");
}

void SgmlFOTBuilder::endScriptPostSub()
{
  endFlow("script.post-sub");
}

void SgmlFOTBuilder::startScriptMidSup()
{
  startPortFlow("script.mid-sup");
}

void SgmlFOTBuilder::endScriptMidSup()
{
  endFlow("script.mid-sup");
}

void SgmlFOTBuilder::startScriptMidSub()
{
  startPortFlow("script.mid-sub");
}

void SgmlFOTBuilder::endScriptMidSub()
{
  endFlow("script.mid-sub");
}

void SgmlFOTBuilder::startMarkSerial()
{
  startSimpleFlowObj("mark");
  startPortFlow("mark.principal");
}

void SgmlFOTBuilder::endMarkSerial()
{
  endFlow("mark");
}

void SgmlFOTBuilder::startMarkOver()
{
  endFlow("mark.principal");
  startPortFlow("mark.over-mark");
}

void SgmlFOTBuilder::endMarkOver()
{
  endFlow("mark.over-mark");
}

void SgmlFOTBuilder::startMarkUnder()
{
  startPortFlow("mark.under-mark");
}

void SgmlFOTBuilder::endMarkUnder()
{
  endFlow("mark.under-mark");
}

void SgmlFOTBuilder::startFenceSerial()
{
  startSimpleFlowObj("fence");
  startPortFlow("fence.principal");
}

void SgmlFOTBuilder::endFenceSerial()
{
  endFlow("fence");
}

void SgmlFOTBuilder::startFenceOpen()
{
  endFlow("fence.principal");
  startPortFlow("fence.open");
}

void SgmlFOTBuilder::endFenceOpen()
{
  endFlow("fence.open");
}

void SgmlFOTBuilder::startFenceClose()
{
  startPortFlow("fence.close");
}

void SgmlFOTBuilder::endFenceClose()
{
  endFlow("fence.close");
}

void SgmlFOTBuilder::startRadicalSerial()
{
  startSimpleFlowObj("radical");
}

void SgmlFOTBuilder::endRadicalSerial()
{
  endFlow("radical");
}

void SgmlFOTBuilder::startRadicalDegree()
{
  endFlow("radical.principal");
  startPortFlow("radical.degree");
}

void SgmlFOTBuilder::endRadicalDegree()
{
  endFlow("radical.degree");
}

void SgmlFOTBuilder::radicalRadical(const CharacterNIC &nic)
{
  os() << "<radical.radical";
  characterNIC(nic);
  outputIcs();
  os() << "/>" << RE;
  startPortFlow("radical.principal");
}

void SgmlFOTBuilder::radicalRadicalDefaulted()
{
  startPortFlow("radical.principal");
}

void SgmlFOTBuilder::startMathOperatorSerial()
{
  startSimpleFlowObj("math-operator");
  startPortFlow("math-operator.principal");
}

void SgmlFOTBuilder::endMathOperatorSerial()
{
  endFlow("math-operator");
}

void SgmlFOTBuilder::startMathOperatorOperator()
{
  endFlow("math-operator.principal");
  startPortFlow("math-operator.operator");
}

void SgmlFOTBuilder::endMathOperatorOperator()
{
  endFlow("math-operator.operator");
}

void SgmlFOTBuilder::startMathOperatorLowerLimit()
{
  startPortFlow("math-operator.lower-limit");
}

void SgmlFOTBuilder::endMathOperatorLowerLimit()
{
  endFlow("math-operator.lower-limit");
}

void SgmlFOTBuilder::startMathOperatorUpperLimit()
{
  startPortFlow("math-operator.upper-limit");
}

void SgmlFOTBuilder::endMathOperatorUpperLimit()
{
  endFlow("math-operator.upper-limit");
}

void SgmlFOTBuilder::startGrid(const GridNIC &nic)
{
  os() << "<grid";
  if (nic.nColumns)
    os() << " grid-n-columns=" << quot << (unsigned long)nic.nColumns << quot;
  if (nic.nRows)
    os() << " grid-n-rows=" << quot << (unsigned long)nic.nRows << quot;
  outputIcs();
  os() << '>' << RE;
}

void SgmlFOTBuilder::endGrid()
{
  endFlow("grid");
}

void SgmlFOTBuilder::startGridCell(const GridCellNIC &nic)
{
  os() << "<grid-cell";
  if (nic.columnNumber)
    os() << " column-number=" << quot << (unsigned long)nic.columnNumber << quot;
  if (nic.rowNumber)
    os() << " row-number=" << quot << (unsigned long)nic.rowNumber << quot;
  outputIcs();
  os() << '>' << RE;
}

void SgmlFOTBuilder::endGridCell()
{
  endFlow("grid-cell");
}

void SgmlFOTBuilder::externalGraphic(const ExternalGraphicNIC &nic)
{
  flushPendingElements();
  os() << "<external-graphic entity-system-id="
       << quot << Data(nic.entitySystemId) << quot
       << " notation-system-id="
       << quot << Data(nic.notationSystemId) << quot;
  if (nic.scaleType != symbolFalse)
    os() << " scale=" << quot << nic.scaleType << quot;
  else
    os() << " scale-x=" << quot << nic.scale[0] << quot
         << " scale-y=" << quot << nic.scale[1] << quot;
  if (nic.hasMaxWidth)
    os() << " max-width=" << quot << nic.maxWidth << quot;
  if (nic.hasMaxHeight)
    os() << " max-height=" << quot << nic.maxHeight << quot;
  if (nic.isDisplay) {
    os() << " display=" << quot << trueString << quot;
    displayNIC(nic);
  }
  else {
    if (nic.escapementDirection != symbolFalse)
      os() << " escapement-direction=" << quot << nic.escapementDirection << quot;
    if (nic.positionPointX)
      os() << " position-point-x=" << quot << nic.positionPointX << quot;
    if (nic.positionPointY)
      os() << " position-point-y=" << quot << nic.positionPointY << quot;
    inlineNIC(nic);
  }
  outputIcs();
  os() << "/>" << RE;
}

void SgmlFOTBuilder::rule(const RuleNIC &nic)
{
  flushPendingElements();
  const char *s = symbolName(nic.orientation);
  if (!s)
    return;
  os() << "<rule orientation=" << quot << s << quot;
  switch (nic.orientation) {
  case symbolHorizontal:
  case symbolVertical:
    displayNIC(nic);
    break;
  default:
    inlineNIC(nic);
    break;
  }
  if (nic.hasLength)
    os() << " length=" << quot << nic.length << quot;
  outputIcs();
  os() << "/>" << RE;
}

void SgmlFOTBuilder::startLeader(const LeaderNIC &nic)
{
  flushPendingElements();
  os() << "<leader";
  if (nic.hasLength)
    os() << " length=" << nic.length;
  inlineNIC(nic);
  outputIcs();
  os() << '>' << RE;
}

void SgmlFOTBuilder::endLeader()
{
  endFlow("leader");
}

void SgmlFOTBuilder::startSideline()
{
  startSimpleFlowObj("sideline");
}

void SgmlFOTBuilder::endSideline()
{
  endFlow("sideline");
}

void SgmlFOTBuilder::startBox(const BoxNIC &nic)
{
  flushPendingElements();
  os() << "<box";
  if (nic.isDisplay) {
    os() << " display=" << quot << trueString << quot;
    displayNIC(nic);
  }
  else
    inlineNIC(nic);
  outputIcs();
  os() << '>' << RE;
}

void SgmlFOTBuilder::endBox()
{
  endFlow("box");
}

void SgmlFOTBuilder::alignmentPoint()
{
  simpleFlowObj("alignment-point");
}

void SgmlFOTBuilder::startTable(const TableNIC &nic)
{
  flushPendingElements();
  os() << "<table";
  switch (nic.widthType) {
  case TableNIC::widthExplicit:
    os() << " width=" << quot << nic.width << quot;
    break;
  case TableNIC::widthMinimum:
    os() << " minimum-width=" << quot << trueString << quot;
    break;
  default:
    break;
  }
  displayNIC(nic);
  outputIcs();
  os() << '>' << RE;
}

void SgmlFOTBuilder::endTable()
{
  endFlow("table");
}

void SgmlFOTBuilder::tableBeforeRowBorder()
{
  simpleFlowObj("table.before-row-border");
}

void SgmlFOTBuilder::tableAfterRowBorder()
{
  simpleFlowObj("table.after-row-border");
}

void SgmlFOTBuilder::tableBeforeColumnBorder()
{
  simpleFlowObj("table.before-column-border");
}

void SgmlFOTBuilder::tableAfterColumnBorder()
{
  simpleFlowObj("table.after-column-border");
}

void SgmlFOTBuilder::tableColumn(const TableColumnNIC &nic)
{
  os() << "<table-column column-number="
       << quot << (unsigned long)nic.columnIndex + 1 << quot;
  if (nic.nColumnsSpanned != 1)
    os() << " n-columns-spanned=" << quot << (unsigned long)nic.nColumnsSpanned << quot;
  if (nic.hasWidth)
    os() << " width=" << quot << nic.width << quot;
  outputIcs();
  os() << "/>" << RE;
}

void SgmlFOTBuilder::startTablePartSerial(const TablePartNIC &nic)
{
  os() << "<table-part";
  displayNIC(nic);
  outputIcs();
  os() << '>' << RE;
  startPortFlow("table-part.principal");
}

void SgmlFOTBuilder::endTablePartSerial()
{
  endFlow("table-part");
}

void SgmlFOTBuilder::startTablePartHeader()
{
  endFlow("table-part.principal");
  os() << "<table-part.header>" << RE;
}

void SgmlFOTBuilder::endTablePartHeader()
{
  endFlow("table-part.header");
}

void SgmlFOTBuilder::startTablePartFooter()
{
  os() << "<table-part.footer>" << RE;
}

void SgmlFOTBuilder::endTablePartFooter()
{
  endFlow("table-part.footer");
}

void SgmlFOTBuilder::startTableRow()
{
  startSimpleFlowObj("table-row");
}

void SgmlFOTBuilder::endTableRow()
{
  endFlow("table-row");
}

void SgmlFOTBuilder::startTableCell(const TableCellNIC &nic)
{
  if (nic.missing)
    os() << "<table-cell column-number=" << quot << 0 << quot;
  else {
    os() << "<table-cell column-number="
         << quot << (unsigned long)nic.columnIndex + 1 << quot;
    if (nic.nColumnsSpanned != 1)
      os() << " n-columns-spanned=" << quot << (unsigned long)nic.nColumnsSpanned << quot;
    if (nic.nRowsSpanned != 1)
      os() << " n-rows-spanned=" << quot << (unsigned long)nic.nRowsSpanned << quot;
  }
  outputIcs();
  os() << '>' << RE;
}

void SgmlFOTBuilder::endTableCell()
{
  endFlow("table-cell");
}

void SgmlFOTBuilder::tableCellBeforeRowBorder()
{
  simpleFlowObj("table-cell.before-row-border");
}

void SgmlFOTBuilder::tableCellAfterRowBorder()
{
  simpleFlowObj("table-cell.after-row-border");
}

void SgmlFOTBuilder::tableCellBeforeColumnBorder()
{
  simpleFlowObj("table-cell.before-column-border");
}

void SgmlFOTBuilder::tableCellAfterColumnBorder()
{
  simpleFlowObj("table-cell.after-column-border");
}

void SgmlFOTBuilder::startNode(const NodePtr &node, const StringC &mode)
{
  nodeLevel_++;
  if (mode.size() != 0 || !nodeIsElement(node))
    return;
  for (size_t i = 0; i < pendingElements_.size(); i++)
    if (*pendingElements_[i] == *node)
      return;
  pendingElements_.push_back(node);
  pendingElementLevels_.push_back(nodeLevel_);
}

void SgmlFOTBuilder::endNode()
{
  // The idea is not to put out a bookmark if there were
  // no flow objects associated with the node.
  // The flow objects might have been labeled, in which case
  // we will get a startNode for the node later.
  if (pendingElements_.size() > 0 && pendingElementLevels_.back() == nodeLevel_
      && nPendingElementsNonEmpty_ < pendingElements_.size()) {
    pendingElementLevels_.resize(pendingElements_.size() - 1);
    pendingElements_.resize(pendingElements_.size() - 1);
  }
  nodeLevel_--;
}

void SgmlFOTBuilder::currentNodePageNumber(const NodePtr &node)
{
  if (!nodeIsElement(node))
    return;
  os() << "<page-number ref=" << quot;
  outputElementName(node);
  os() << quot << "/>" << RE;
}

void SgmlFOTBuilder::outputElementName(unsigned long groveIndex,
				       const Char *idData,
				       size_t idSize)
{
  if (groveIndex)
    os() << groveIndex << '.';
  os() << Data(idData, idSize);
}

void SgmlFOTBuilder::outputElementName(const NodePtr &node)
{
  GroveString id;
  if (node->getId(id) == accessOK)
    outputElementName(node->groveIndex(), id.data(), id.size());
  else {
    unsigned long n;
    n = node->groveIndex();
    if (n)
      os() << n << '.';
    if (node->elementIndex(n) == accessOK)
      os() << n;
  }
}

void SgmlFOTBuilder::flushPendingElements()
{
  if (suppressAnchors_)
    return;
  for (size_t i = 0; i < pendingElements_.size(); i++) {
    const NodePtr &node = pendingElements_[i];
    os() << "<a name=" << quot;
    outputElementName(node);
    os() << quot << "/>" << RE;
  }
  nPendingElementsNonEmpty_ = 0;
  pendingElements_.resize(0);
  pendingElementLevels_.resize(0);
}

#ifdef DSSSL_NAMESPACE
}
#endif
