// TeXFotBuilder.cxx: a Generic TeX backend for Jade
// Written by David Megginson <dmeggins@microstar.com>
// With changes from Sebastian Rahtz <s.rahtz@elsevier.co.uk>
// Last Modification: August 6th, 1998

// Table Support: Kathleen Marszalek <kmarszal@watarts.uwaterloo.ca>
// Version: 1.0b7
// Last Modification: July 7th, 1998

#include "config.h"
#include "TeXFOTBuilder.h"
#include "TeXMessages.h"
#include "MessageArg.h"
#include "TmpOutputByteStream.h"
#include <stdio.h>
#include <stdlib.h>

#undef TEXDEBUG
#undef NDEBUG
#include <assert.h>

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

// --------- TeXTmpOutputByteStream ------------------------------------------

struct TeXTmpOutputByteStream : public TmpOutputByteStream {
  TeXTmpOutputByteStream() : TmpOutputByteStream() {}
  void commit( OutputByteStream &os ) const;
};

// ----------------------------------------------------------------------------

struct LengthInPoints {
  LengthInPoints( long l_ ) : l( l_ ) {}
  long l;
};

void TeXTmpOutputByteStream::commit( OutputByteStream &os ) const {

  TmpOutputByteStream::Iter iter( *this );
  const char *s;
  size_t n;

  while( iter.next( s, n ) ) {
    os.sputn( s, n );
  }
}

// --------- TeXFOTBuilder ----------------------------------------------------

class TeXFOTBuilder : public SerialFOTBuilder {
public:
#ifdef TEXDEBUG 
  static TeXFOTBuilder &curInstance() { assert( CurInstance != NULL ); return *CurInstance; }
  // used for testing only: otherwise code is re-entrant
#endif
  struct PageFloatNIC {
    ~PageFloatNIC();
    StringC placement;
  };
  class TeXExtensionFlowObj : public FOTBuilder::ExtensionFlowObj {
  public:
    virtual void atomic(TeXFOTBuilder &, const NodePtr &) const = 0;
  };
  class TeXCompoundExtensionFlowObj : public FOTBuilder::CompoundExtensionFlowObj {
  public:
    virtual void start(TeXFOTBuilder &, const NodePtr &) const = 0;
    virtual void end(TeXFOTBuilder &) const = 0;
  };
  class PageFloatFlowObj : public TeXCompoundExtensionFlowObj {
    void start(TeXFOTBuilder &fotb, const NodePtr &) const {
      fotb.startPageFloat(nic_);
    }
    void end(TeXFOTBuilder &fotb) const {
      fotb.endPageFloat();
    }
    bool hasNIC(const StringC &name) const {
      return name == "placement" ;
    }
    void setNIC(const StringC &, const Value &value) {
      value.convertString(nic_.placement);
      }
    ExtensionFlowObj *copy() const { return new PageFloatFlowObj(*this); }
  private:
    PageFloatNIC nic_;
    StringC name_;
    StringC placement;
  };
  class PageFootnoteFlowObj : public TeXCompoundExtensionFlowObj {
    void start(TeXFOTBuilder &fotb, const NodePtr &) const {
      fotb.startPageFootnote();
    }
    void end(TeXFOTBuilder &fotb) const {
      fotb.endPageFootnote();
    }
    ExtensionFlowObj *copy() const { return new PageFootnoteFlowObj(*this); }
  private:
  };
  //////////////////////////////////////////////////////////////////////
  // Constructor and destructor.
  //////////////////////////////////////////////////////////////////////

  TeXFOTBuilder(OutputByteStream *, Messenger *mgr);
  ~TeXFOTBuilder();

  //////////////////////////////////////////////////////////////////////
  // Atomic flow objects
  //////////////////////////////////////////////////////////////////////
  void extension(const ExtensionFlowObj &fo, const NodePtr &);
  void startExtensionSerial(const CompoundExtensionFlowObj &fo, const NodePtr &nd);
  void endExtensionSerial(const CompoundExtensionFlowObj &fo);
  void start();
  void end();
  void charactersFromNode(const NodePtr &, const Char *, size_t);
  void setPreserveSdata(bool);
  void startPageFloat(const PageFloatNIC &);
  void endPageFloat();
  void setPageFloatNIC(const PageFloatNIC &);
  void startPageFootnote();
  void endPageFootnote();

  void characters(const Char *, size_t);
  void character(const CharacterNIC &);
  void paragraphBreak(const ParagraphNIC &);
  void externalGraphic(const ExternalGraphicNIC &);
  void rule(const RuleNIC &);
  void alignmentPoint();
  void pageNumber();
  void formattingInstruction(const StringC &);
  void tableColumn(const TableColumnNIC &);
  void tableCellBeforeRowBorder();
  void tableCellAfterRowBorder();
  void tableCellBeforeColumnBorder();
  void tableCellAfterColumnBorder();

  void fractionBar();
  void radicalRadical(const CharacterNIC &);
  void radicalRadicalDefaulted();
  void currentNodePageNumber(const NodePtr &);

  //////////////////////////////////////////////////////////////////////
  // Non-atomic flow objects
  //////////////////////////////////////////////////////////////////////

  void startSequence();
  void endSequence();
  void startLineField(const LineFieldNIC &);
  void endLineField();
  void startParagraph(const ParagraphNIC &);
  void endParagraph();
  void startDisplayGroup(const DisplayGroupNIC &);
  void endDisplayGroup();
  void startScroll();
  void endScroll();
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
  // Tables
  void startTable(const TableNIC &);
  void endTable();
  // A call for each border is made immediately
  // after startTable(), each preceded by any appropriate set*() calls.
  void tableBeforeRowBorder();
  void tableAfterRowBorder();
  void tableBeforeColumnBorder();
  void tableAfterColumnBorder();
  void startTablePartSerial(const TablePartNIC &);
  void endTablePartSerial();
  void startTableRow();
  void endTableRow();
  void startTableCell(const TableCellNIC &);
  void endTableCell();
  void startSimplePageSequence();
  void endSimplePageSequence();
  // Headers and footers are treated like a separate port.
  void startSimplePageSequenceHeaderFooter(unsigned);
  void endSimplePageSequenceHeaderFooter(unsigned);
  // page-number sosofo

  void startTablePartHeader();
  void endTablePartHeader();
  void startTablePartFooter();
  void endTablePartFooter();
  void startMathSequence();
  void endMathSequence();
  void startFractionSerial();
  void endFractionSerial();
  void startFractionNumerator();
  void endFractionNumerator();
  void startFractionDenominator();
  void endFractionDenominator();
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
  void startLink(const Address &);
  void endLink();


  //////////////////////////////////////////////////////////////////////
  // Inherited characteristics
  // Set the value of the characteristic for the next flow object.
  // Inherited characteristics that are not explicitly set have
  // the same value as the parent flow object.
  //////////////////////////////////////////////////////////////////////

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
  void setMarginaliaSep(const LengthSpec &);
  void setLines(Symbol);
  void setQuadding(Symbol);
  void setDisplayAlignment(Symbol);
  void setFieldAlign(Symbol);
  void setColor(const DeviceRGBColor &);
  void setBackgroundColor(); // background of #f
  void setBackgroundColor(const DeviceRGBColor &);
  void setBorderPresent(bool);
  void setLineThickness(Length);
  void setCellBeforeRowMargin(Length);
  void setCellAfterRowMargin(Length);
  void setCellBeforeColumnMargin(Length);
  void setCellAfterColumnMargin(Length);
  void setLineSep(Length);
  void setBoxSizeBefore(Length);
  void setBoxSizeAfter(Length);
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
  void setLineJoin(Symbol);
  void setLineCap(Symbol);
  void setLineNumberSide(Symbol);
  void setKernMode(Symbol);
  void setInputWhitespaceTreatment(Symbol);
  void setFillingDirection(Symbol);
  void setWritingMode(Symbol);
  void setLastLineQuadding(Symbol);
  void setMathDisplayMode(Symbol);
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
  // 0 means #f
  void setExpandTabs(long);
  void setHyphenationLadderCount(long);
  // public id or #f
  void setBackgroundTile(PublicId);
  void setLineBreakingMethod(PublicId);
  void setLineCompositionMethod(PublicId);
  void setImplicitBidiMethod(PublicId);
  void setGlyphSubstMethod(PublicId);
  void setGlyphReorderMethod(PublicId);
  void setHyphenationMethod(PublicId);
  void setTableAutoWidthMethod(PublicId);
  void setFontName(PublicId);
  // Two-letter code
  void setLanguage(Letter2);
  void setCountry(Letter2);
  // For simple page sequence
  void setPageWidth(Length);
  void setPageHeight(Length);
  void setLeftMargin(Length);
  void setRightMargin(Length);
  void setTopMargin(Length);
  void setBottomMargin(Length);
  void setHeaderMargin(Length);
  void setFooterMargin(Length);

  void setPageNumberRestart(bool);
  void setPageNumberFormat(const StringC &);
  void setPageNColumns(long);
  void setPageColumnSep(Length);
  void setPageBalanceColumns(bool);
  void setGridRowSep(Length);
  void setGridColumnSep(Length);
  void setSubscriptDepth(Length);
  void setSuperscriptHeight(Length);
  void setUnderMarkDepth(Length);
  void setOverMarkHeight(Length);
  void setHeadingLevel(long);

  void setMinPreLineSpacing(const OptLengthSpec &);
  void setMinPostLineSpacing(const OptLengthSpec &);
  void setMinLeading(const OptLengthSpec &);
  void setScriptPreAlign(Symbol);
  void setScriptPostAlign(Symbol);
  void setScriptMidSupAlign(Symbol);
  void setScriptMidSubAlign(Symbol);
  void setNumeratorAlign(Symbol);
  void setDenominatorAlign(Symbol);
  void setGridPositionCellType(Symbol);
  void setGridColumnAlignment(Symbol);
  void setGridRowAlignment(Symbol);
  void setGridEquidistantRows(bool);
  void setGridEquidistantColumns(bool);
  void setEscapementSpaceBefore(const InlineSpace &);
  void setEscapementSpaceAfter(const InlineSpace &);
  void setGlyphSubstTable(const Vector<ConstPtr<GlyphSubstTable> > &tables);

  void startDisplay( const DisplayNIC & );
  void endDisplay();

  enum FotObjectClassType { oc_Unknown, oc_Cell };

  struct Format {

    Format() : FotCurDisplaySize( 0 ),
               FotLineThickness( 1000 ),
               FotLineCap( symbolButt ),
               FotBorderPriority( 0 ),
               FotBorderPresent( true ),
               FotLineRepeat( 1 ),
               FotLineSep( 1000 ),
               FotDisplayAlignment( symbolStart ),
               FotStartIndentSpec( 0 ),
               FotEndIndentSpec( 0 ),
               FotLeftMargin( 1 ),
               FotRightMargin( 1 ),
               FotPageWidth( 72000*8 ),
               FotPageNColumns( 1 ),
               FotPageColumnSep( 72000/2 ),
               FotSpan( 1 ),
               FotCellBeforeColumnMargin( 0 ),
               FotCellAfterColumnMargin( 0 ),
               FotObjectClass( oc_Unknown ) {}

    long                 FotCurDisplaySize;
    Length               FotLineThickness;
    Symbol               FotLineCap;
    long                 FotBorderPriority;
    bool                 FotBorderPresent;
    long                 FotLineRepeat;
    Length               FotLineSep;
    Symbol               FotDisplayAlignment;
    LengthSpec           FotStartIndentSpec;
    LengthSpec           FotEndIndentSpec;
    Length               FotLeftMargin;
    Length               FotRightMargin;
    Length               FotPageWidth;
    long                 FotPageNColumns;
    Length               FotPageColumnSep;
    long                 FotSpan;
    Length               FotCellBeforeColumnMargin;
    Length               FotCellAfterColumnMargin;
    FotObjectClassType   FotObjectClass;

    static const Length INITIAL_PAGE_SIZE() { return 72000*8; }
  };

  struct CompoundFotElement;
  struct FotElement {

    FotElement(  CompoundFotElement *parent = NULL )
     : Parent( parent ), SiblingSeqIdx( -1 ) {}
    String<char> Characteristics;

    virtual void out( OutputByteStream &stream ) const
     { outProlog( stream ); outContent( stream ); outEpilog( stream ); }
    virtual void open( TeXFOTBuilder &builder );
    virtual void close( TeXFOTBuilder &builder );
    virtual bool isAtomic() const = 0;
    virtual const char *name() const = 0;
    CompoundFotElement *parent() const { return Parent; }
    String<char> &nodeInfoEpilog() { return NodeInfoEpilog; }
    void setParent( CompoundFotElement *parent ) { Parent = parent; }
    virtual FotElement *lastClosed_() = 0;
    virtual FotElement *currentlyOpen_() = 0;
    void setSiblingSeqIdx( int idx ) { SiblingSeqIdx = idx; }
    int siblingSeqIdx() const { assert( SiblingSeqIdx > -1 ); return SiblingSeqIdx; }

   protected:
    int SiblingSeqIdx;
    CompoundFotElement* Parent;
    String<char> NodeInfoProlog;
    String<char> NodeInfoEpilog;

    virtual void outContent( OutputByteStream & ) const {};
    virtual void outProlog( OutputByteStream &stream ) const = 0;
    virtual void outEpilog( OutputByteStream &stream ) const = 0;
  };

  struct FotElementState {
    FotElementState() : EnforcingStructure( true ), IsOpen( false ) {}
    bool enforcingStructure() { return IsOpen && EnforcingStructure; }

    bool EnforcingStructure;
    bool IsOpen;
    String<char> CurNodeInfoProlog;
  };
  
  struct AtomicFotElement : public FotElement {

    AtomicFotElement( CompoundFotElement *parent = NULL ) : FotElement( parent ) {}
    virtual bool isAtomic() const { return true; }
    virtual FotElement *lastClosed_() { return NULL; };
    virtual FotElement *currentlyOpen_() { return this; }
   
   protected:
    virtual void outProlog( OutputByteStream &stream ) const
     { stream << NodeInfoProlog << "\\insert" << name() << "%\n{" << Characteristics << '}'; }
    virtual void outEpilog( OutputByteStream &stream ) const
     { stream << '}' << NodeInfoEpilog; }
  };

  struct CompoundFotElement : public FotElement {

    CompoundFotElement( CompoundFotElement *parent = NULL )
     : FotElement( parent ), CurrentlyOpenChildIdx( -1 ), LastClosedChildIdx( -1 ) {}
    virtual bool isAtomic() const { return false; }
    virtual void open( TeXFOTBuilder &builder )
      { FotElement::open( builder ); builder.setCurOs( &PreContent ); };
    virtual void childJustClosed( FotElement &child )
     { CurrentlyOpenChildIdx = -1; LastClosedChildIdx = child.siblingSeqIdx(); };
    virtual void childJustOpened( FotElement &child )
     { CurrentlyOpenChildIdx = child.siblingSeqIdx(); };
    virtual FotElement &child( size_t idx ) = 0;
    static FotElement *lastClosed( CompoundFotElement &treeRoot )
     { return treeRoot.currentlyOpen( treeRoot )
        ? treeRoot.currentlyOpen( treeRoot )->lastClosed_() : (FotElement*)NULL; }
    static FotElement *currentlyOpen( CompoundFotElement &treeRoot ) {
      return treeRoot.currentlyOpen_();
    }
    virtual FotElement *currentlyOpen_()
     { return CurrentlyOpenChildIdx > -1
        ? child( (size_t)CurrentlyOpenChildIdx ).currentlyOpen_() : this; }
    virtual FotElement *lastClosed_() 
     { return LastClosedChildIdx > -1
        ? &child( (size_t)LastClosedChildIdx )
        : ( parent() ? parent()->lastClosed_() : (FotElement*)NULL ); }
    
   protected:
    virtual void outProlog( OutputByteStream &stream ) const {
      #ifdef TEXDEBUG
       stream <<  "\nELEMENT\n" << "\nPRO\n" << NodeInfoProlog << "\nEND_PRO\n" << "\\" << name() << "%\n{" << Characteristics << '}';
      #else
       stream <<  NodeInfoProlog << "\\" << name() << "%\n{" << Characteristics << '}';
      #endif
       PreContent.commit( stream );
     }
    virtual void outEpilog( OutputByteStream &stream ) const {
      #ifdef TEXDEBUG
        stream << "\\end" << name() << "{}" << "\nEPI\n" << NodeInfoEpilog << "\nEND_EPI\nEND_ELEMENT\n" ;
      #else
        stream << "\\end" << name() << "{}" << NodeInfoEpilog << "%\n" ;
      #endif
    }

    int CurrentlyOpenChildIdx;
    int LastClosedChildIdx;
    TeXTmpOutputByteStream PreContent;
  };
  
  struct Border : public AtomicFotElement {

    Border( CompoundFotElement *parent = NULL, bool cellBorder_ = true )
     : AtomicFotElement( parent ), cellBorder( cellBorder_ ), borderPresent( false ) {}
    long   borderPriority;
    Length lineThickness;
    bool   borderPresent;
    long   lineRepeat;
    Length lineSep;
    bool   cellBorder;

    void resolve( Border &adjacentBorder );
    void setFromFot( TeXFOTBuilder &builder );
  };

  struct CellBeforeRowBorder : public Border {
    CellBeforeRowBorder( CompoundFotElement *parent = NULL ) : Border( parent ) {}
    virtual const char *name() const { return "TableCellBeforeRowBorder"; }
  };
  struct CellAfterRowBorder : public Border {
    CellAfterRowBorder( CompoundFotElement *parent = NULL ) : Border( parent ) {}
    virtual const char *name() const { return "TableCellAfterRowBorder"; }
  };
  struct CellBeforeColumnBorder : public Border {
    CellBeforeColumnBorder( CompoundFotElement *parent = NULL ) : Border( parent ) {}
    virtual const char *name() const { return "TableCellBeforeColumnBorder"; }
  };
  struct CellAfterColumnBorder : public Border {
    CellAfterColumnBorder( CompoundFotElement *parent = NULL ) : Border( parent ) {}
    virtual const char *name() const { return "TableCellAfterColumnBorder"; }
  };
  struct TableBeforeRowBorder : public Border {
    TableBeforeRowBorder( CompoundFotElement *parent = NULL ) : Border( parent, false ) {}
    virtual const char *name() const { return "TableBeforeRowBorder"; }
  };
  struct TableAfterRowBorder : public Border {
    TableAfterRowBorder( CompoundFotElement *parent = NULL ) : Border( parent, false ) {}
    virtual const char *name() const { return "TableAfterRowBorder"; }
  };
  struct TableBeforeColumnBorder : public Border {
    TableBeforeColumnBorder( CompoundFotElement *parent = NULL ) : Border( parent, false ) {}
    virtual const char *name() const { return "TableBeforeColumnBorder"; }
  };
  struct TableAfterColumnBorder : public Border {
    TableAfterColumnBorder( CompoundFotElement *parent = NULL ) : Border( parent, false ) {}
    virtual const char *name() const { return "TableAfterColumnBorder"; }
  };
  
  struct Column {

    Column() : hasWidth( 0 ), computedWidth( 0 ), defaultTeXLeftBorder( 0 ),
             defaultTeXRightBorder( 0 ), displayAlignment( symbolStart ),
             isExplicit( false ) {}
    bool isExplicit;
    bool hasWidth;
    TableLengthSpec width;
    long computedWidth;
    Symbol displayAlignment;
    int defaultTeXLeftBorder; // also used as column border count;
    int defaultTeXRightBorder; //
  };

  struct TablePart;
  struct Cell : public CompoundFotElement {

    Cell( CompoundFotElement *parent = NULL )
     : CompoundFotElement( parent ), missing( false ), OverlappingCell( NULL ),
       nRowsSpanned( 1 ), nColumnsSpanned( 1 ), displaySize( 0 ),
       beforeRowBorder(), afterRowBorder(), beforeColumnBorder(),
       afterColumnBorder(), TeXTableRowIdx( -1 ), TeXTableColumnIdx( -1 ),
       displayAlignment( symbolStart ), effectiveAlignment( symbolStart ),
       paragraphChildrenNum( 0 )  {}

    bool missing;
    int TeXTableRowIdx;
    int TeXTableColumnIdx;
    unsigned nColumnsSpanned;
    unsigned nRowsSpanned;
    CellBeforeRowBorder beforeRowBorder;
    CellAfterRowBorder afterRowBorder;
    CellBeforeColumnBorder beforeColumnBorder;
    CellAfterColumnBorder afterColumnBorder;
    Symbol displayAlignment;
    Symbol effectiveAlignment;
    long displaySize;
    long paragraphChildrenNum;

    Cell *OverlappingCell;
    OutputByteStream &content() { return Content; }
    virtual void open( TeXFOTBuilder &builder )
     { CompoundFotElement::open( builder );
       builder.setCurOs( &Content );
       builder.curFotElementState().EnforcingStructure = false; }
    virtual void close( TeXFOTBuilder &builder )
     { computeMultiParFlag();
       CompoundFotElement::close( builder );
       builder.curFotElementState().EnforcingStructure = true; }
    virtual const char *name() const { return "TableCell"; }
    bool singleRowBeforeRowBorderPresent() const;
    bool singleRowAfterRowBorderPresent() const;
    bool singleColumnBeforeColumnBorderPresent() const;
    bool singleColumnAfterColumnBorderPresent() const;
    void computeOverridingTeXColumnBorders( TablePart &tablePart );
    void computeOverridingTeXDisplayAlignment( TablePart &tablePart );
    void computeMultiParFlag();

    bool isOverlapped() const { return OverlappingCell == this ? false : true; }
    virtual FotElement &child( size_t ) { assert( false ); return *this; }

   protected:
    virtual void outProlog( OutputByteStream &stream ) const;
    virtual void outEpilog( OutputByteStream &stream ) const;
    void outContent( OutputByteStream &stream ) const { Content.commit( stream ); }
    TeXTmpOutputByteStream Content;
  };

  struct Row : public CompoundFotElement {

    Row( CompoundFotElement *parent = NULL ) : CompoundFotElement( parent ) {}
    virtual const char *name() const { return "TableRow"; }
    Vector<Cell> Cells;
    static void outVerticalBorders
      ( const Row *upperRow, const Row *lowerRow, OutputByteStream &stream );
    virtual FotElement &child( size_t idx )
     { assert( idx < Cells.size() ); return Cells[idx]; }

   protected:
    void outContent( OutputByteStream &stream ) const;
  };

  struct Table;
  struct TablePart : public CompoundFotElement {

    TablePart( CompoundFotElement *parent = NULL )
     : CompoundFotElement( parent ), columnsProcessed( false ),
       needsColumnReprocessing( false ), isExplicit( true ) {}

    bool isExplicit;

    Vector<Column> Columns;
    String<char> HeaderProlog;
    Vector<Row> Header;
    String<char> HeaderEpilog;
    Vector<Row> Body;
    String<char> FooterProlog;
    Vector<Row> Footer;                
    String<char> FooterEpilog;

    void processColumns( TeXFOTBuilder &builder );
    void computeTeXColumnBordersAndDisplayAlignment();
    void normalizeRows();
    void begin();

    Table &parentTable() const
     { assert( Parent != NULL ); return *(Table*)Parent; }
    virtual const char *name() const { return "TablePart"; }
    virtual FotElement &child( size_t idx );

    bool columnsProcessed;
    bool needsColumnReprocessing;

   protected:
    virtual void outProlog( OutputByteStream &stream ) const
     { if( isExplicit ) CompoundFotElement::outProlog( stream ); }
    virtual void outEpilog( OutputByteStream &stream ) const
     { if( isExplicit ) CompoundFotElement::outEpilog( stream ); }
    Row &siblingSeqIdxToRow( int idx ) const;
    void outContent( OutputByteStream &stream ) const;
  };

  struct Table : public CompoundFotElement {

    Table(  CompoundFotElement *parent = NULL )
     : CompoundFotElement( parent ), beforeRowBorder(), afterRowBorder(),
       beforeColumnBorder(), afterColumnBorder(), CurCell( NULL ),
       CurTablePart( NULL ), NoTablePartsSeen( true ) {}

    Vector<TablePart> TableParts;

    TableBeforeRowBorder beforeRowBorder;
    TableAfterRowBorder afterRowBorder;
    TableBeforeColumnBorder beforeColumnBorder;
    TableAfterColumnBorder afterColumnBorder;
    Length tableWidth;
    Symbol displayAlignment;
    Length startIndent;

    virtual const char *name() const { return "Table"; }
    virtual FotElement &child( size_t idx )
     { assert( idx < TableParts.size() ); return TableParts[idx]; }
    void resolveBorders( Vector<Row> *preceedingRows,
                         Vector<Row> &rows,
                         Vector<Row> *followingRows,
                         unsigned startingRowIdx,
                         bool hasFirstTableRow, bool hasLastTableRow );
    void begin();
    void end( TeXFOTBuilder &builder );

    Vector<Row> &curRows() { assert( CurRows != NULL ); return *CurRows; }
    TablePart &curTablePart() { assert( CurTablePart != NULL ); return *CurTablePart; }
    Cell &curCell() { assert( CurCell != NULL ); return *CurCell; }

    TablePart *CurTablePart;
    Cell *CurCell;
    Vector<Row> *CurRows;
    
    bool NoTablePartsSeen;

  protected:
    void outContent( OutputByteStream &stream ) const;
  };

  long computeLengthSpec( const LengthSpec &spec ) const;
  const Format &curFormat() const { assert( FormatStack.size() > 0 ); return FormatStack.back(); }
  Table &curTable() { return CurTable; }
  FotElementState &curFotElementState() { return CurFotElementState; }
  void setCurOs( OutputByteStream *to ) { CurOs = to; }
  void elementStart( FotObjectClassType objectClassType );
  
  OutputByteStream *fileout_;
private:
				// Variables.
  OutputByteStream *CurOs;
  StrOutputByteStream stringout_;
  Messenger *mgr_;
  bool preserveSdata_;

  Vector<size_t> DisplayBoxLevels;
  Vector<Format> FormatStack;
  Format NextFormat; 
  Table CurTable;
  FotElementState CurFotElementState;

				// Functions.
  OutputByteStream &os();
  void insertAtomic(const char *name);
  void insertAtomic(FotElement &fotElement);
  void startGroup(FotElement &fotElement);
  void startGroup(const char *name, String<char> *output = NULL );
  void endGroup(const char *name, String<char> *output = NULL );
  void closeopenBrace(const char *name, String<char> *output = NULL );
  void startBrace(const char *name, String<char> *output = NULL );
  void endBrace(const char *name, String<char> *output = NULL );
  void startSimpleGroup(const char *name, String<char> *output = NULL );
  void endSimpleGroup(String<char> *output = NULL);
  void endGroup() {};

  void setlength(const char *,Length);
  void set(const char *,const StringC &);
  void set(const char *,const GroveString &);
  void set(const char *,Symbol);
  void set(const char *,const LengthSpec &);
  void set(const char *,double);
  void set(const char *,const DeviceRGBColor &);
  void set(const char *,bool);
  void set(const char *,long);
  void set(const char *,long unsigned int);
  void set(const char *name,unsigned int n) {
    set(name, (unsigned long)n);
  }
  void set(const char *,PublicId);
  void setletter2(const char *,Letter2);
  void set(const char *,const DisplaySpace &);
  void set(const char *,const GlyphId &);

  void set(const char *,const OptLengthSpec &);
  void set(const char *,const InlineSpace &);

  // Structures for non-inherited characteristics,
  // in the order specified in style/FOTBuilder.h.

  void setDisplayNIC(const DisplayNIC &);
  void setInlineNIC(const InlineNIC &);
  void setDisplayGroupNIC(const DisplayGroupNIC &);
  void setExternalGraphicNIC(const ExternalGraphicNIC &);
  void setBoxNIC(const BoxNIC &);
  void setRuleNIC(const RuleNIC &);
  void setLeaderNIC(const LeaderNIC &);
  void setParagraphNIC(const ParagraphNIC &);
  void setCharacterNIC(const CharacterNIC &);
  void setLineFieldNIC(const LineFieldNIC &);
  void setTableNIC(const TableNIC &);
  void setTablePartNIC(const TablePartNIC &);
  void setTableColumnNIC(const TableColumnNIC &);
  void setTableCellNIC(const TableCellNIC &);
  void setGridNIC(const GridNIC &);
  void setGridCellNIC(const GridCellNIC &);

  void dumpInherited();

  void message(const MessageType0 &);
};

#ifdef TEXDEBUG
  TeXFOTBuilder *TeXFOTBuilder::CurInstance = NULL;
#endif

// --------- OutputByteStream operators --------------------------------------

OutputByteStream &operator<<( OutputByteStream &os, LengthInPoints length ) {

  char buf[32];
  int i;
  sprintf( buf, "%li.%.3i%n", long(length.l)/1000, abs(long(length.l)%1000), &i );
  while( buf[--i] == '0' ) {}; if( buf[i] == '.' ) i--;
    buf[i+1] = '\0';
  os << buf << "\\p@";
  return os;
}

// --------- TeXFOTBuilder::FotElement ---------------------------------------

void TeXFOTBuilder::FotElement::open( TeXFOTBuilder &builder ) {

  builder.curFotElementState().IsOpen = true;
  if( parent() )
    parent()->childJustOpened( *this );

  if( builder.curFotElementState().CurNodeInfoProlog.size() > 0 ) {
    NodeInfoProlog = builder.curFotElementState().CurNodeInfoProlog;
    builder.curFotElementState().CurNodeInfoProlog.resize( 0 );
  }
}

void TeXFOTBuilder::FotElement::close( TeXFOTBuilder &builder ) {

  if( parent() )
    parent()->childJustClosed( *this );
  else {
    builder.curFotElementState().IsOpen = false;
    builder.setCurOs( NULL );
  }
}  

TeXFOTBuilder::FotElement *TeXFOTBuilder::FotElement::lastClosed_() {

  return parent() ? parent()->lastClosed_() : (FotElement*)NULL;
}

// --------- TeXFOTBuilder Standard Display/Element Handling -----------------

void TeXFOTBuilder::elementStart( FotObjectClassType objectClassType ) {

  NextFormat.FotObjectClass = objectClassType;
  FormatStack.push_back( NextFormat );
}

void TeXFOTBuilder::start() {

  NextFormat.FotObjectClass = oc_Unknown;
  FormatStack.push_back( NextFormat );
}

void TeXFOTBuilder::end() {

  assert( FormatStack.size() > 0 );
  FormatStack.resize( FormatStack.size()-1 );

  assert( FormatStack.size() > 0 );
  NextFormat = FormatStack.back();
}

void TeXFOTBuilder::startDisplay( const DisplayNIC & ) {

  if( curTable().CurCell != NULL )
      NextFormat.FotCurDisplaySize = curTable().CurCell->displaySize;
  else if( NextFormat.FotSpan > 1 )
      NextFormat.FotCurDisplaySize
       = NextFormat.FotPageWidth - NextFormat.FotLeftMargin - NextFormat.FotRightMargin;
  else
      NextFormat.FotCurDisplaySize
       = ( NextFormat.FotPageWidth - NextFormat.FotLeftMargin - NextFormat.FotRightMargin
            - NextFormat.FotPageColumnSep * ( NextFormat.FotPageNColumns - 1 ) )  
          / NextFormat.FotPageNColumns;

}

void TeXFOTBuilder::endDisplay() {
}

// --------- TeXFOTBuilder Misc ----------------------------------------------

long TeXFOTBuilder::computeLengthSpec( const LengthSpec &spec ) const {

    if( spec.displaySizeFactor == 0.0 ) {
        return spec.length;
    } else {
        double tem = curFormat().FotCurDisplaySize * spec.displaySizeFactor;
        return spec.length + long( tem >= 0.0 ? tem +.5 : tem - .5 );
    }
}

// --------- TeXFOTBuilder::Table ---------------------------------------------

void TeXFOTBuilder::Table::resolveBorders
 ( Vector<Row> *preceedingRows, Vector<Row> &rows,
   Vector<Row> *, unsigned startingRowIdx, bool hasFirstTableRow,
   bool hasLastTableRow ) {

    bool isFirstRow;
    bool isLastRow;
    bool isFirstColumn;
    bool isLastColumn;
    Cell *cell = NULL;
    size_t r, c, rr, cc;
    bool leftEdge, topEdge;

    #ifdef TEXDEBUG
      *TeXFOTBuilder::curInstance().fileout_ << "RESOLVING_BORDERS\n";
    #endif

    for( r = 0; r < rows.size(); r++ ) {
      for( c = 0; c < rows[r].Cells.size()-1; c++ ) {
        cell = &rows[r].Cells[c];
        if( cell->OverlappingCell == NULL ) {
          for( rr = r; rr < r + cell->nRowsSpanned; rr++ ) {
            for( cc = c, leftEdge = true; cc < c + cell->nColumnsSpanned; cc++ ) {
              rows[rr].Cells[cc].OverlappingCell = cell;
              rows[rr].Cells[cc].TeXTableRowIdx = rr + startingRowIdx;
              rows[rr].Cells[cc].TeXTableColumnIdx = cc;
            }
          }
        }
      }
    }

    for( r = 0; r < rows.size(); r++ ) {
        #ifdef TEXDEBUG
          *TeXFOTBuilder::curInstance().fileout_ << "ROW " << r << "\n";
        #endif
        for( c = 0; c < rows[r].Cells.size() - 1; c++ ) {
            #ifdef TEXDEBUG
              *TeXFOTBuilder::curInstance().fileout_ << "    COL " << c << "\n";
            #endif
            cell = &rows[r].Cells[c];
            if( cell->OverlappingCell == cell ) {
                for( rr = r, topEdge = true; rr < r + cell->nRowsSpanned; rr++ ) {
                    isFirstRow = ( rr == 0 ) ? true : false;
                    isLastRow = ( rr == rows.size() - 1 ) ? true : false;
                    for( cc = c, leftEdge = true; cc < c + cell->nColumnsSpanned; cc++ ) {
                        isFirstColumn = ( cc == 0 ) ? true : false;
                        isLastColumn = ( cc == rows[rr].Cells.size() - 2 ) ? true : false;

                        if( leftEdge )
                            if( isFirstColumn ) {
                                #ifdef TEXDEBUG
                                  *TeXFOTBuilder::curInstance().fileout_
                                   << "LEFT_TABLE_BORDER_RES \n";
                                #endif
                                cell->beforeColumnBorder.resolve( beforeColumnBorder );
                            } else
                                cell->beforeColumnBorder.resolve
                                 ( rows[rr].Cells[cc-1].OverlappingCell->afterColumnBorder );

                        if( topEdge )
                            if( isFirstRow && hasFirstTableRow )
                                cell->beforeRowBorder.resolve( beforeRowBorder );
                            else
                                if( !isFirstRow )
                                    cell->beforeRowBorder.resolve
                                     ( rows[rr-1].Cells[cc].OverlappingCell->afterRowBorder );
                                else if( preceedingRows != NULL ) {
                                    assert( preceedingRows->size() > 0 );
                                    cell->beforeRowBorder.resolve
                                     ( (*preceedingRows)[preceedingRows->size()-1].Cells[cc]
                                        .OverlappingCell->afterRowBorder );
                                }

                        if( isLastColumn )
                            cell->afterColumnBorder.resolve( afterColumnBorder );

                        if( isLastRow && hasLastTableRow )
                            cell->afterRowBorder.resolve( afterRowBorder );
                        
                        leftEdge = false;
                    }
                    topEdge = false;
                }
            }                
        }
    }
}

void TeXFOTBuilder::Table::begin() {

    CurCell = NULL;
    NoTablePartsSeen = true;

    TableParts.resize( 0 );
    TableParts.resize( 1 );
    TableParts.back().setSiblingSeqIdx( 0 );
    TableParts.back().setParent( this );
    TableParts.back().begin();
}

void TeXFOTBuilder::Table::end( TeXFOTBuilder &builder ) {

  bool firstPart, lastPart, hasHeader, hasBody, hasFooter;
  for( size_t i = 0; i < TableParts.size(); i++ ) {
      firstPart = ( i == 0 ) ? true : false;
      lastPart = ( i == TableParts.size() - 1 ) ? true : false;
      TablePart &tablePart = TableParts[i];
      tablePart.normalizeRows();
      hasHeader = tablePart.Header.size() > 0 ? true : false;
      hasBody = tablePart.Body.size() > 0 ? true : false;
      hasFooter = tablePart.Footer.size() > 0 ? true : false;
      if( hasHeader )
          resolveBorders( (Vector<Row>*)NULL,
                          tablePart.Header,
                          hasBody ? &tablePart.Body : (Vector<Row>*)NULL,
                          0, firstPart ? true : false, false );
      resolveBorders( hasHeader ? &tablePart.Header : (Vector<Row>*)NULL,
                      tablePart.Body,
                      hasFooter ? &tablePart.Footer : (Vector<Row>*)NULL,
                      tablePart.Header.size(), hasHeader ? false : true,
                      hasFooter ? false : true );
      if( hasFooter )
          resolveBorders( hasBody ? &tablePart.Body : (Vector<Row>*)NULL,
                          tablePart.Footer,
                          (Vector<Row>*)NULL, 
                          tablePart.Header.size() + tablePart.Body.size(),
                          false, lastPart ? true : false );

      if( tablePart.needsColumnReprocessing )
        tablePart.processColumns( builder );

      tablePart.computeTeXColumnBordersAndDisplayAlignment();
  }
}

void TeXFOTBuilder::Table::outContent( OutputByteStream &stream ) const {

  for( size_t i = 0; i < TableParts.size(); i++ )
      TableParts[i].out( stream );
};

// --------- TeXFOTBuilder::TablePart -----------------------------------------

void TeXFOTBuilder::TablePart::begin() {

    Columns.resize( 0 );
    Header.resize( 0 );
    Body.resize( 0 );
    Footer.resize( 0 );
    
    columnsProcessed = false;
    needsColumnReprocessing = false;

    parentTable().CurRows = &Body;    
    parentTable().CurTablePart = this;
}

TeXFOTBuilder::FotElement &TeXFOTBuilder::TablePart::child( size_t idx ) {

  if( idx < Header.size() )
    return Header[idx];

  idx -= Header.size();
  if( idx < Body.size() )
    return Body[idx];

  assert( idx < Footer.size() );
  idx -= Body.size();
  return Footer[idx];
}

void TeXFOTBuilder::TablePart::computeTeXColumnBordersAndDisplayAlignment() {

  Vector<Row> *rows;  
  for( int step = 0; step < 3; step++ ) {
    switch( step ) {
      case 0: rows = &Header; break;
      case 1: rows = &Body; break;
      default: rows = &Footer;
    }
    for( size_t r = 0; r < rows->size(); r++ ) {
      if( (*rows)[r].Cells.size()-1 > Columns.size() )
        Columns.resize( (*rows)[r].Cells.size()-1 ); 
      for( size_t c = 0; c < (*rows)[r].Cells.size()-1; c++ ) {
        if( (*rows)[r].Cells[c].singleColumnBeforeColumnBorderPresent() )
          Columns[c].defaultTeXLeftBorder++;
        if( (*rows)[r].Cells[c].singleColumnAfterColumnBorderPresent() )
          Columns[c].defaultTeXRightBorder++;
      }
    }
  }

  #ifdef TEXDEBUG
    for( int step = 0; step < 3; step++ ) {
      switch( step ) {
        case 0: rows = &Header; break;
        case 1: rows = &Body; break;
        default: rows = &Footer;
      }
      for( size_t r = 0; r < rows->size(); r++ ) {
        *TeXFOTBuilder::curInstance().fileout_
         << "\nROW " << ( step == 0 ? "Header" : ( step == 1 ? "Body" : "Footer" ) ) << "\n";
        for( size_t c = 0; c < (*rows)[r].Cells.size()-1; c++ ) {
          *TeXFOTBuilder::curInstance().fileout_
           << "\n    CELL " << c
           << " LB: " << (*rows)[r].Cells[c].beforeColumnBorder.borderPresent
           << " RB: " << (*rows)[r].Cells[c].beforeColumnBorder.borderPresent
           << " OVERLAPPED: "
           << ( (*rows)[r].Cells[c].isOverlapped() ? "YES" : "NO" ) << "\n";
        }
      }
    }
  #endif

  size_t TeXTableRowsNum = Header.size() + Body.size() + Footer.size();
  for( size_t c = 0; c < Columns.size(); c++ ) {
    Columns[c].defaultTeXLeftBorder
     = ((size_t)Columns[c].defaultTeXLeftBorder)*2 >= TeXTableRowsNum ? 1 : 0;
    Columns[c].defaultTeXRightBorder
      = ((size_t)Columns[c].defaultTeXRightBorder)*2 >= TeXTableRowsNum ? 1 : 0;
  }

  for( int step = 0; step < 3; step++ ) {
    switch( step ) {
      case 0: rows = &Header; break;
      case 1: rows = &Body; break;
      default: rows = &Footer;
    }
    for( size_t r = 0; r < rows->size(); r++ ) {
      for( size_t c = 0; c < (*rows)[r].Cells.size()-1; c++ ) {
        if( !(*rows)[r].Cells[c].isOverlapped() ) {
          (*rows)[r].Cells[c].computeOverridingTeXColumnBorders( *this );
          (*rows)[r].Cells[c].computeOverridingTeXDisplayAlignment( *this );
        }
      }
    }
  }
}
  
void TeXFOTBuilder::TablePart::processColumns( TeXFOTBuilder &builder ) {

  long totalNonproportionalWidth = 0L;
  double totalProportionalUnits = 0.0;
  size_t nonWidthCellsNum = 0;
  for( size_t i = 0; i < Columns.size(); i++ ) {
    if( Columns[i].hasWidth ) {
      if( Columns[i].width.tableUnitFactor ) {
        totalProportionalUnits += Columns[i].width.tableUnitFactor;
      } else {
        Columns[i].computedWidth = builder.computeLengthSpec( Columns[i].width );
        totalNonproportionalWidth += Columns[i].computedWidth;
      }
    } else
      nonWidthCellsNum++;
  }

  if( totalProportionalUnits > 0 )
    totalProportionalUnits += nonWidthCellsNum;
  // cannot predict width of some cells, but their width is needed to 
  // compute widths of proportional cells - force non-with cells to be 1 table-unit

  double proportionalUnit = 0.0;
  if( totalProportionalUnits )
      proportionalUnit
       = ( parentTable().tableWidth - totalNonproportionalWidth ) / totalProportionalUnits;
  
  for( size_t i = 0; i < Columns.size(); i++ ) {
      if( Columns[i].hasWidth ) {
          if( Columns[i].width.tableUnitFactor )
              Columns[i].computedWidth
               = long(proportionalUnit * Columns[i].width.tableUnitFactor);
      } else if( totalProportionalUnits > 0 )
          Columns[i].computedWidth = long(proportionalUnit);
  }

  columnsProcessed = true;
}

void TeXFOTBuilder::TablePart::outContent( OutputByteStream &stream ) const {

  stream << "\\TeXTable%\n{" << LengthInPoints( parentTable().tableWidth ) 
         << "}{" << Columns.size() << '}';

  stream << '{';
  for( size_t i = 0; i < Columns.size(); i++ ) {
    #ifdef TEXDEBUG
      stream << "\nCOLUMN " << i << " DEF_LEFT_B: " <<  Columns[i].defaultTeXLeftBorder
             << " DEF_RIGHT_B: " <<  Columns[i].defaultTeXRightBorder << "\n";
    #endif
    if( i == 0 && Columns[i].defaultTeXLeftBorder )
      stream << '|';
    if( Columns[i].computedWidth > 0 ) {
      switch( Columns[i].displayAlignment ) {
        case symbolInside:  
        case symbolStart:
          stream << "L{" << LengthInPoints( Columns[i].computedWidth ) << '}';
          break;
        case symbolOutside: 
        case symbolEnd:
          stream << "R{" << LengthInPoints( Columns[i].computedWidth ) << '}';
          break;
        case symbolCenter: default:
          stream << "C{" << LengthInPoints( Columns[i].computedWidth ) << '}';
      }
    } else {
      switch( Columns[i].displayAlignment ) {
        case symbolOutside: 
        case symbolEnd:     stream << 'r'; break;
        case symbolCenter:  stream << 'c'; break;
        case symbolInside:  
        case symbolStart:
        default:            stream << 'l'; break;
      }
    }
    if( Columns[i].defaultTeXRightBorder )
      stream << '|';
  }
  stream << "}%\n";

  stream << HeaderProlog;
  const Row *recentRow = NULL;
  for( size_t i = 0; i < Header.size(); i++ ) {
    Row::outVerticalBorders( recentRow, &Header[i], stream );
    Header[i].out( stream );
    recentRow = &Header[i];
  }
  stream << HeaderEpilog;

  for( size_t i = 0; i < Body.size(); i++ ) {
    Row::outVerticalBorders( recentRow, &Body[i], stream );
    Body[i].out( stream );
    recentRow = &Body[i];
  }

  stream << FooterProlog;
  for( size_t i = 0; i < Footer.size(); i++ ) {
    Row::outVerticalBorders( recentRow, &Footer[i], stream );
    Footer[i].out( stream );
    recentRow = &Footer[i];
  }
  stream << FooterEpilog;

  if( recentRow )
    Row::outVerticalBorders( recentRow, NULL, stream );

  stream << "\\endTeXTable" << "{}%\n";
}

void TeXFOTBuilder::TablePart::normalizeRows() {

  size_t maxCellsInRow = Columns.size()+1;
  Vector<Row> *rows;  
  for( int step = 0; step < 2; step++ ) {
    for( int rowType = 0; rowType < 3; rowType++ ) {
      switch( rowType ) {
        case 0: rows = &Header; break;
        case 1: rows = &Body; break;
        default: rows = &Footer;
      }
      for( size_t r = 0; r < rows->size(); r++ ) {
        if( step == 0 ) {
          if( (*rows)[r].Cells.size() > 1 ) {
            size_t lastCellIdx = (*rows)[r].Cells.size()-2;
            Cell &lastCell = (*rows)[r].Cells[lastCellIdx];
            if( !lastCell.missing
                  &&
                lastCellIdx+lastCell.nColumnsSpanned+1 > maxCellsInRow )
              maxCellsInRow = lastCellIdx+lastCell.nColumnsSpanned+1;
          }
        } else if( (*rows)[r].Cells.size() < maxCellsInRow )
          (*rows)[r].Cells.resize( maxCellsInRow );
      }
    }
  }
}

// --------- TeXFOTBuilder::Row -----------------------------------------------

void TeXFOTBuilder::Row::outContent( OutputByteStream &stream ) const {

  bool first = true;
  for( size_t i = 0; i + 1 < Cells.size(); i++ ) {
    if( !Cells[i].isOverlapped() ) {
      if( !first )
        stream << "&";  
      else
        first = false;
      Cells[i].out( stream );
    }
  }
}

void TeXFOTBuilder::Row::outVerticalBorders
 ( const TeXFOTBuilder::Row *upperRow, 
   const TeXFOTBuilder::Row *lowerRow, OutputByteStream &stream ) {

  assert( upperRow != NULL || lowerRow != NULL );
  size_t colNum = upperRow ? upperRow->Cells.size()-1 : lowerRow->Cells.size()-1;
  int borderStartIdx = -1;
  int borderEndIdx;
  for( size_t i = 0; i < colNum; i++ ) {
    #ifdef TEXDEBUG
      stream << "\nROW: " << "COL " << i << " " <<
       ( ( ( upperRow && upperRow->Cells[i].singleRowAfterRowBorderPresent() )
           ||
          ( lowerRow && lowerRow->Cells[i].singleRowBeforeRowBorderPresent() ) )
          ? "Border" : "No Border" );    
    #endif
    if( ( upperRow && upperRow->Cells[i].singleRowAfterRowBorderPresent() )
         ||
        ( lowerRow && lowerRow->Cells[i].singleRowBeforeRowBorderPresent() ) ) {
      if( borderStartIdx < 0 )
        borderStartIdx = i;
      if( i == colNum-1 ) {
        borderEndIdx = i;
        goto OUT_BORDER;
      }  
   } else if( borderStartIdx > -1 ) {
      borderEndIdx = i-1;
      goto OUT_BORDER;
   }
   continue;

    OUT_BORDER:    
      if( borderStartIdx == 0 && i == colNum-1 )
        stream << "\\Hline%\n";
      else
        stream << "\\Cline{" << borderStartIdx+1 << '-' << borderEndIdx+1 << "}%\n";
      borderStartIdx = -1;
  }
}

// --------- TeXFOTBuilder::Cell ----------------------------------------------

void TeXFOTBuilder::Cell::outProlog( OutputByteStream &stream ) const {

  if( nColumnsSpanned > 1 ) {
    stream << "\\multicolumn%\n{" << nColumnsSpanned << "}{";
    if( beforeColumnBorder.borderPresent )
      stream << '|';
    switch( effectiveAlignment ) {
      case symbolOutside: 
      case symbolEnd:     stream << 'r'; break;
      case symbolCenter:  stream << 'c'; break;
      case symbolInside:  
      case symbolStart:
      default:            stream << 'l'; break;
    }
    if( afterColumnBorder.borderPresent )
      stream << '|';
    stream << "}{";
  }

  CompoundFotElement::outProlog( stream );
}

void TeXFOTBuilder::Cell::outEpilog( OutputByteStream &stream ) const {

  CompoundFotElement::outEpilog( stream );
  if( nColumnsSpanned > 1 )
    stream << "}%\n";
}

void TeXFOTBuilder::Cell::computeMultiParFlag() {

  if( paragraphChildrenNum > 1 ) {
    StrOutputByteStream str;
    str << "\\def\\MultiPar{1}";
    String<char> s;
    str.extractString( s );
    Characteristics += s;
  }
}

void TeXFOTBuilder::Cell::computeOverridingTeXColumnBorders( TablePart &tablePart ) {

  StrOutputByteStream str;

  if( tablePart.Columns[TeXTableColumnIdx].defaultTeXLeftBorder
       != beforeColumnBorder.borderPresent ) 
    str << "\\def\\TeXTableCellBeforeColumnBorder{"  << beforeColumnBorder.borderPresent << '}';

  if( tablePart.Columns[TeXTableColumnIdx+nColumnsSpanned-1].defaultTeXRightBorder
       != afterColumnBorder.borderPresent ) 
    str << "\\def\\TeXTableCellAfterColumnBorder{"  << afterColumnBorder.borderPresent << '}';

  String<char> s;
  str.extractString( s );
  Characteristics += s;
}

void TeXFOTBuilder::Cell::computeOverridingTeXDisplayAlignment( TablePart &tablePart ) {

  effectiveAlignment
   = tablePart.Columns[TeXTableColumnIdx].isExplicit
      ? tablePart.Columns[TeXTableColumnIdx].displayAlignment : displayAlignment;

  StrOutputByteStream str;
  if( effectiveAlignment != tablePart.Columns[TeXTableColumnIdx].displayAlignment ) {
    str << "\\def\\TeXTableCellDisplayAlignment{";
    switch( effectiveAlignment ) {
      case symbolOutside: 
      case symbolEnd:     str << 'r'; break;
      case symbolCenter:  str << 'c'; break;
      case symbolInside:  
      case symbolStart:
      default:            str << 'l'; break;
    }
    str << '}';
    String<char> s;
    str.extractString( s );
    Characteristics += s;
  }
}

bool TeXFOTBuilder::Cell::singleRowBeforeRowBorderPresent() const {

  assert( OverlappingCell != NULL );
  if( OverlappingCell->TeXTableRowIdx == TeXTableRowIdx )
    return OverlappingCell->beforeRowBorder.borderPresent;
  else
    return false;    
}

bool TeXFOTBuilder::Cell::singleRowAfterRowBorderPresent() const {

  assert( OverlappingCell != NULL );
  assert( TeXTableRowIdx > -1 );
  if( OverlappingCell->TeXTableRowIdx + OverlappingCell->nRowsSpanned
       == ((unsigned)TeXTableRowIdx)+1 )
    return OverlappingCell->afterRowBorder.borderPresent;
  else
    return false;    
}

bool TeXFOTBuilder::Cell::singleColumnBeforeColumnBorderPresent() const {

  assert( OverlappingCell != NULL );
  assert( TeXTableRowIdx > -1 );
  if( OverlappingCell->TeXTableColumnIdx
       == TeXTableColumnIdx )
    return OverlappingCell->beforeColumnBorder.borderPresent;
  else
    return false;    
}

bool TeXFOTBuilder::Cell::singleColumnAfterColumnBorderPresent() const {

  assert( OverlappingCell != NULL );
  if( OverlappingCell->TeXTableColumnIdx + (int)OverlappingCell->nColumnsSpanned
       == TeXTableColumnIdx+1 )
    return OverlappingCell->afterColumnBorder.borderPresent;
  else
    return false;    
}

// --------- TeXFOTBuilder::Border --------------------------------------------

void TeXFOTBuilder::Border::setFromFot( TeXFOTBuilder &builder ) {

  const TeXFOTBuilder::Format &f = builder.curFormat();
  borderPriority = f.FotBorderPriority;
  borderPresent = f.FotBorderPresent;
  lineThickness = f.FotLineThickness;
  lineRepeat = f.FotLineRepeat;
  lineSep  = f.FotLineSep;
}

void TeXFOTBuilder::Border::resolve( Border &adjacentBorder ) {
    
  if( adjacentBorder.borderPriority > borderPriority
     ||
    (   adjacentBorder.borderPriority == borderPriority 
     && !adjacentBorder.cellBorder
     && adjacentBorder.borderPresent ) ) {

    lineThickness = adjacentBorder.lineThickness;
    borderPresent = adjacentBorder.borderPresent;
    lineRepeat = adjacentBorder.lineRepeat;
    lineSep = adjacentBorder.lineSep;

    if( adjacentBorder.cellBorder )
        adjacentBorder.borderPresent = false;
  }

  #ifdef TEXDEBUG
    *TeXFOTBuilder::curInstance().fileout_
     << "RESOLVE_RESULT: " << borderPresent << "\n";
  #endif
}

// --------- End Tables -------------------------------------------------------

#define MAYBESET(name,value,default) (value!=default?(set(name,value),0):0)

//
// Get the current output stream.
//
inline
OutputByteStream &TeXFOTBuilder::os()
{
  return CurOs == NULL ? *fileout_ : *CurOs;
}

//
// Define an output operator for StringC
//
// FIXME This won't work for Unicode characters.

static OutputByteStream &operator<<(OutputByteStream &os, const StringC &s)
{
  for (size_t i = 0; i < s.size(); i++)
    os << char(s[i]);
  return os;
}


//
// Define an output operator for GroveString
//
static OutputByteStream &operator<<(OutputByteStream &os, const GroveString &s)
{
  for (size_t i = 0; i < s.size(); i++)
    os << char(s[i]);
  return os;
}

static OutputByteStream &operator<<(OutputByteStream &os, double d)
{
  char buf[64];
  sprintf(buf, "%g", d);
  return os << buf;
}

FOTBuilder *makeTeXFOTBuilder(OutputByteStream *os, Messenger *mgr,
			      const FOTBuilder::Extension *&ext)
{  
  static const TeXFOTBuilder::PageFloatFlowObj pageFloat;
  static const TeXFOTBuilder::PageFootnoteFlowObj pageFootnote;
  static const FOTBuilder::Extension extensions[] = {
    {
      "UNREGISTERED::Sebastian Rahtz//Flow Object Class::page-float",
      0,
      0,
      0,
      0,
      &pageFloat
    },
    {
      "UNREGISTERED::Sebastian Rahtz//Flow Object Class::page-footnote",
      0,
      0,
      0,
      0,
      &pageFootnote
    },
    {
      "UNREGISTERED::James Clark//Characteristic::page-number-format",
      0,
      (void (FOTBuilder::*)(const StringC &))&TeXFOTBuilder::setPageNumberFormat,
      0,
      0
    },
    {
      "UNREGISTERED::James Clark//Characteristic::page-number-restart?",
      (void (FOTBuilder::*)(bool))&TeXFOTBuilder::setPageNumberRestart,
      0,
      0
    },
    {
      "UNREGISTERED::James Clark//Characteristic::page-n-columns",
      0,
      0,
      (void (FOTBuilder::*)(long))&TeXFOTBuilder::setPageNColumns,
      0
    },
    {
      "UNREGISTERED::James Clark//Characteristic::page-column-sep",
      0,
      0,
      0,
      (void (FOTBuilder::*)(FOTBuilder::Length))&TeXFOTBuilder::setPageColumnSep,
    },
    {
      "UNREGISTERED::James Clark//Characteristic::page-balance-columns?",
      (void (FOTBuilder::*)(bool))&TeXFOTBuilder::setPageBalanceColumns,
      0,
      0
    },
    {
      "UNREGISTERED::James Clark//Characteristic::subscript-depth",
      0,
      0,
      0,
      (void (FOTBuilder::*)(FOTBuilder::Length))&TeXFOTBuilder::setSubscriptDepth,
    },
    {
      "UNREGISTERED::James Clark//Characteristic::over-mark-height",
      0,
      0,
      0,
      (void (FOTBuilder::*)(FOTBuilder::Length))&TeXFOTBuilder::setOverMarkHeight,
    },
    {
      "UNREGISTERED::James Clark//Characteristic::under-mark-depth",
      0,
      0,
      0,
      (void (FOTBuilder::*)(FOTBuilder::Length))&TeXFOTBuilder::setUnderMarkDepth,
    },
    {
      "UNREGISTERED::James Clark//Characteristic::superscript-height",
      0,
      0,
      0,
      (void (FOTBuilder::*)(FOTBuilder::Length))&TeXFOTBuilder::setSuperscriptHeight,
    },
    {
      "UNREGISTERED::James Clark//Characteristic::grid-row-sep",
      0,
      0,
      0,
      (void (FOTBuilder::*)(FOTBuilder::Length))&TeXFOTBuilder::setGridRowSep,
    },
    {
      "UNREGISTERED::James Clark//Characteristic::grid-column-sep",
      0,
      0,
      0,
      (void (FOTBuilder::*)(FOTBuilder::Length))&TeXFOTBuilder::setGridColumnSep,
    },
    {
      "UNREGISTERED::James Clark//Characteristic::heading-level",
      0,
      0,
      (void (FOTBuilder::*)(long))&TeXFOTBuilder::setHeadingLevel,
      0
    },
    {
      "UNREGISTERED::James Clark//Characteristic::preserve-sdata?",
      (void (FOTBuilder::*)(bool))&TeXFOTBuilder::setPreserveSdata,
      0,
      0,
      0,
      0
    },

    { 0, 0, 0}
  };
  ext = extensions;
  return new TeXFOTBuilder(os, mgr);
}

////////////////////////////////////////////////////////////////////////
// Constructor and Destructor
////////////////////////////////////////////////////////////////////////

TeXFOTBuilder::TeXFOTBuilder(OutputByteStream *o, Messenger *mgr)
: fileout_(o), mgr_(mgr), CurOs( NULL ), preserveSdata_(1)
{
  #ifdef TEXDEBUG
    CurInstance = this;
  #endif
  NextFormat.FotCurDisplaySize = Format::INITIAL_PAGE_SIZE();
  FormatStack.push_back( NextFormat );
  os() << "\\FOT{2}";
}

TeXFOTBuilder::~TeXFOTBuilder()
{
  os() << "\\endFOT{}";
}

//////////////////////////////////////////////////////////////////////
// Atomic flow objects
//////////////////////////////////////////////////////////////////////

void TeXFOTBuilder::characters(const Char *s, size_t n)
{
  for (; n > 0; n--, s++) {
				// Since TeX has only 256 characters
				// by default, two-byte characters
				// will need special treatment.
    if (*s > 255) {
      //      set("Ch",(unsigned long)(*s));
      // insertAtomic("Character");
      //
      // Don't complicate matters, just give the character!
       os() << "\\Character{" << (unsigned long)(*s) << "}";
    } else {
				// Otherwise, check for special
				// TeX escapes.
      switch(*s) {
      default:
	os() << char(*s);
	break;
      case '\\':
      case '^':
      case '_':
      case '~':
	os() << "\\char" << int(*s) << "{}";
	break;
      case '{':
      case '}':
      case '$':
      case '&':
      case '#':
      case '%':
	os() << "\\" << char(*s);
	break;
      case '\r':
	os() << '\n';
	break;
      case '\n':
	break;
      }
    }
  }
}

void TeXFOTBuilder::character(const CharacterNIC &nic)
{
  setCharacterNIC(nic);
  //  insertAtomic("Character");
  dumpInherited();
  os() << "\\Character{" << (unsigned long)(nic.ch) << "}";
}

void TeXFOTBuilder::paragraphBreak(const ParagraphNIC &nic)
{
  setParagraphNIC(nic);
  insertAtomic("ParagraphBreak");
}

void TeXFOTBuilder::externalGraphic(const ExternalGraphicNIC &nic)
{
  setExternalGraphicNIC(nic);
  insertAtomic("ExternalGraphic");
}

void TeXFOTBuilder::rule(const RuleNIC &nic)
{
  setRuleNIC(nic);
  insertAtomic("Rule");
}

void TeXFOTBuilder::alignmentPoint()
{
  insertAtomic("AlignmentPoint");
}

// page-number sosofo
void TeXFOTBuilder::pageNumber()
{
  insertAtomic("PageNumber");
}

void TeXFOTBuilder::formattingInstruction (const StringC &instr)
{
  os() << instr;
}
  
void TeXFOTBuilder::tableColumn(const TableColumnNIC &nic)
{
  setTableColumnNIC(nic);

  if( nic.columnIndex >= curTable().curTablePart().Columns.size() )
      curTable().curTablePart().Columns.resize( nic.columnIndex + 1 );

  Column &col = curTable().curTablePart().Columns[nic.columnIndex];

  col.isExplicit = true;
  col.hasWidth = nic.hasWidth;
  if( nic.hasWidth )
      col.width = nic.width;

  col.displayAlignment = curFormat().FotDisplayAlignment;

  insertAtomic("TableColumn");
}

void TeXFOTBuilder::tableCellBeforeRowBorder()
{
  start();
  curTable().curCell().beforeRowBorder.setFromFot( *this );
  insertAtomic( curTable().curCell().beforeRowBorder );
  end();
}

void TeXFOTBuilder::tableCellAfterRowBorder()
{
  start();
  curTable().curCell().afterRowBorder.setFromFot( *this );
  insertAtomic( curTable().curCell().afterRowBorder );
  end();
}

void TeXFOTBuilder::tableCellBeforeColumnBorder()
{
  start();
  curTable().curCell().beforeColumnBorder.setFromFot( *this );
  insertAtomic( curTable().curCell().beforeColumnBorder );
  end();
}

void TeXFOTBuilder::tableCellAfterColumnBorder()
{
  start();
  curTable().curCell().afterColumnBorder.setFromFot( *this );
  insertAtomic( curTable().curCell().afterColumnBorder );
  end();
}

void TeXFOTBuilder::fractionBar()
{
  insertAtomic("FractionBar");
}

void TeXFOTBuilder::radicalRadical(const CharacterNIC &c)
{
  setCharacterNIC(c);
  insertAtomic("RadicalRadical");
}

void TeXFOTBuilder::radicalRadicalDefaulted()
{
  insertAtomic("RadicalRadicalDefaulted");
}

void TeXFOTBuilder::currentNodePageNumber(const NodePtr &node)
{
  GroveString id;
  unsigned long ei;

  // FIX ME!
  // Only PARTIALLY supported -- I currently allow cross-references
  // only to elements.
  if (node->getId(id) == accessOK) {
    set("Label",id);
  } else if (node->elementIndex(ei) == accessOK) {
    set("Element",ei);
  } else {
    message(TeXMessages::unsupportedPageNumberNonElement);
    return;
  }
  unsigned long g = node->groveIndex();
  if (g) {
    set("GroveIndex",g);
  }
  insertAtomic("CurrentNodePageNumber");
}


//////////////////////////////////////////////////////////////////////
// Non-atomic flow objects
//////////////////////////////////////////////////////////////////////

void TeXFOTBuilder::startSequence()
{
  start();
  if( !curFotElementState().enforcingStructure() )
    startGroup("Seq");
}

void TeXFOTBuilder::endSequence()
{
  if( !curFotElementState().enforcingStructure() )
    endGroup("Seq");
  end();
}

void TeXFOTBuilder::startLineField(const LineFieldNIC &nic)
{
  start();
  setLineFieldNIC(nic);
  startGroup("LineField");
}

void TeXFOTBuilder::endLineField()
{
  endGroup("LineField");
  end();
}

void TeXFOTBuilder::startParagraph(const ParagraphNIC &nic)
{
  startDisplay( nic );
  if( curFormat().FotObjectClass == oc_Cell )
    curTable().curCell().paragraphChildrenNum++;
  start();
  setParagraphNIC(nic);
  startGroup("Par");
}

void TeXFOTBuilder::endParagraph()
{
  endGroup("Par");
  end();
  endDisplay();
}

void TeXFOTBuilder::startDisplayGroup(const DisplayGroupNIC &nic)
{
  startDisplay( nic );
  start();
  setDisplayGroupNIC(nic);
  startGroup("DisplayGroup");
}

void TeXFOTBuilder::endDisplayGroup()
{
  endGroup("DisplayGroup");
  end();
  endDisplay();
}

void TeXFOTBuilder::startScroll()
{
  start();
  startGroup("Scroll");
}

void TeXFOTBuilder::endScroll()
{
  endGroup("Scroll");
  end();
}

void TeXFOTBuilder::startScore(Char ch)
{
  start();
  set("ScoreCharacter",(unsigned long)ch);
  startGroup("Score");
}

void TeXFOTBuilder::startScore(const LengthSpec &len)
{
  start();
  set("ScoreLength",len);
  startGroup("Score");
}

void TeXFOTBuilder::startScore(Symbol type)
{
  start();
  set("ScoreType",type);
  startGroup("Score");
}

void TeXFOTBuilder::endScore()
{
  endGroup("Score");
  end();
}

void TeXFOTBuilder::startLeader(const LeaderNIC &nic)
{
  start();
  setLeaderNIC(nic);
  startGroup("Leader");
}

void TeXFOTBuilder::endLeader()
{
  endGroup("Leader");
  end();
}

void TeXFOTBuilder::startSideline()
{
  start();
  startGroup("SideLine");
}

void TeXFOTBuilder::endSideline()
{
  endGroup("SideLine");
  end();
}

void TeXFOTBuilder::startBox(const BoxNIC &nic)
{
  if( nic.isDisplay ) {
    DisplayBoxLevels.push_back( FormatStack.size() );
    startDisplay(nic);
  }
  start();
  setBoxNIC(nic);
  startGroup("BOX");
}

void TeXFOTBuilder::endBox()
{
  endGroup("BOX");
  end();
  if( DisplayBoxLevels.size() > 0 && DisplayBoxLevels.back() == FormatStack.size() ) {
    DisplayBoxLevels.resize( DisplayBoxLevels.size() - 1 );
    endDisplay();
  }
}

// Tables
void TeXFOTBuilder::startTable(const TableNIC &nic)
{
  #ifdef TEXDEBUG
    *fileout_ << "\nTABLE_START\n";
  #endif
  startDisplay( nic );
  start();

  setTableNIC(nic);

  Length curStartIndent = computeLengthSpec( curFormat().FotStartIndentSpec );
  curTable().startIndent = curStartIndent;

  curTable().begin();
  curTable().displayAlignment = curFormat().FotDisplayAlignment;

  if( nic.widthType == TableNIC::widthExplicit )
      curTable().tableWidth = computeLengthSpec( nic.width );
  else
      curTable().tableWidth
       = curFormat().FotCurDisplaySize - curStartIndent
          - computeLengthSpec( curFormat().FotEndIndentSpec );

  startGroup( curTable() );
  curTable().curTablePart().open( *this );
}

void TeXFOTBuilder::endTable()
{
  #ifdef TEXDEBUG
    *fileout_ << "\nTABLE_END\n";
  #endif

  if( curTable().NoTablePartsSeen ) {
    curTable().curTablePart().close( *this );
    curTable().curTablePart().isExplicit = false;
  }

  curTable().end( *this );
  curTable().close( *this );
  curTable().out( os() );
  
  endGroup();
  end();
  endDisplay();
}

// A call for each border is made immediately
// after startTable(), each preceded by any appropriate set*() calls.
void TeXFOTBuilder::tableBeforeRowBorder()
{
  start();
  curTable().beforeRowBorder.setFromFot( *this );
  insertAtomic( curTable().beforeRowBorder );
  end();
}

void TeXFOTBuilder::tableAfterRowBorder()
{
  start();
  curTable().afterRowBorder.setFromFot( *this );
  insertAtomic( curTable().afterRowBorder );
  end();
}

void TeXFOTBuilder::tableBeforeColumnBorder()
{
  start();
  curTable().beforeColumnBorder.setFromFot( *this );
  insertAtomic( curTable().beforeColumnBorder );
  end();
}

void TeXFOTBuilder::tableAfterColumnBorder()
{
  start();
  curTable().afterColumnBorder.setFromFot( *this );
  insertAtomic( curTable().afterColumnBorder );
  end();
}

void TeXFOTBuilder::startTablePartSerial(const TablePartNIC &nic)
{
  #ifdef TEXDEBUG
    *fileout_ << "\nTABLE_PART_START\n";
  #endif

  startDisplay( nic );
  start();

  setTablePartNIC(nic);

  if( curTable().NoTablePartsSeen )
      curTable().NoTablePartsSeen = false;
  else {
      curTable().TableParts.resize( curTable().TableParts.size()+1 );
      curTable().TableParts.back().setSiblingSeqIdx( curTable().TableParts.size()-1 );
      curTable().TableParts.back().setParent( &curTable() );    
  }

  curTable().TableParts.back().begin();    
  curTable().TableParts.back().open( *this );    

  startGroup( curTable().curTablePart() );
}

void TeXFOTBuilder::endTablePartSerial()
{
  curTable().curTablePart().close( *this );
  curTable().CurTablePart = NULL;
  endGroup();
  end();
  endDisplay();

  #ifdef TEXDEBUG
    *fileout_ << "\nTABLE_PART_END\n";
  #endif
}

void TeXFOTBuilder::startTableRow()
{
  #ifdef TEXDEBUG
    *fileout_ << "\nTABLE_ROW_START\n";
  #endif

  curTable().curRows().resize( curTable().curRows().size() + 1 );
  curTable().curRows().back().setSiblingSeqIdx( curTable().curRows().size()-1 );
  curTable().curRows().back().setParent( &curTable().curTablePart() );
  startGroup( curTable().curRows().back() );
  curTable().curRows().back().open( *this );
}

void TeXFOTBuilder::endTableRow()
{
  curTable().curRows().back().close( *this );
  endGroup();

  #ifdef TEXDEBUG
    *fileout_ << "\nTABLE_ROW_END\n";
  #endif
}

void TeXFOTBuilder::startTableCell(const TableCellNIC &nic)
{
  #ifdef TEXDEBUG
    *fileout_ << "\nTABLE_CELL_START index: " << nic.columnIndex
              << " missing: " << nic.missing << "\n";
  #endif

  setTableCellNIC(nic);

  TablePart &tp = curTable().curTablePart();
  if( !tp.columnsProcessed )
      tp.processColumns( *this );
 
  Vector<Cell> &Cells = curTable().curRows().back().Cells;
  { size_t curSize = Cells.size();
    if( nic.columnIndex >= curSize ) {
        Cells.resize( nic.columnIndex + 1 );
        for( size_t i = curSize; i < Cells.size(); i++ )
          Cells[i].setSiblingSeqIdx( i );
    }
  }

  Cell &cell = Cells[nic.columnIndex];
  curTable().CurCell = &cell;
  cell.missing = nic.missing;
  cell.setParent( &curTable().curRows().back() );

  if( nic.nColumnsSpanned != 1 )
      cell.nColumnsSpanned = nic.nColumnsSpanned;

  if( nic.nRowsSpanned != 1 )
      cell.nRowsSpanned = nic.nRowsSpanned;
  
  long newDisplaySize = 0;
  for( size_t i = nic.columnIndex; i < nic.columnIndex + nic.nColumnsSpanned; i++ )
      if( i < tp.Columns.size() ) {
          if( tp.Columns[i].hasWidth )
              newDisplaySize
               += computeLengthSpec( tp.Columns[i].width );
      } else
      if( !nic.missing ) {
          tp.Columns.resize( tp.Columns.size() + 1 ); 
          tp.Columns.back().hasWidth = false;
      }

  if( newDisplaySize > 0 ) {
      newDisplaySize -= NextFormat.FotCellBeforeColumnMargin
                         + NextFormat.FotCellAfterColumnMargin;
      NextFormat.FotCurDisplaySize = newDisplaySize;
  }

  cell.displaySize = NextFormat.FotCurDisplaySize;
  cell.displayAlignment = NextFormat.FotDisplayAlignment;
  
  elementStart( oc_Cell );
  startGroup( cell );
  cell.open( *this );
}

void TeXFOTBuilder::endTableCell()
{
  curTable().curCell().close( *this );
  curTable().CurCell = NULL;
  endGroup();
  end();

  #ifdef TEXDEBUG
    *fileout_ << "\nTABLE_CELL_END\n";
  #endif
}

void TeXFOTBuilder::startSimplePageSequence()
{
  NextFormat.FotCurDisplaySize
   = ( NextFormat.FotPageWidth - NextFormat.FotLeftMargin - NextFormat.FotRightMargin
        - NextFormat.FotPageColumnSep * ( NextFormat.FotPageNColumns - 1 ) )  
      / NextFormat.FotPageNColumns;

  start();
  startGroup("SpS");
}

void TeXFOTBuilder::endSimplePageSequence()
{
  endGroup("SpS");
  end();
}

// These aren't real flow objects, so handle them a little
// differently.
void TeXFOTBuilder::startSimplePageSequenceHeaderFooter(unsigned flags)
{
  os() << "\n\\SpS";
  if ((flags & (firstHF|otherHF)) == firstHF)
    os() << "First";
  else
    os() << "Other";
  if ((flags & (frontHF|backHF)) == frontHF)
    os() << "Front";
  else
    os() << "Back";
  switch (flags & (leftHF|centerHF|rightHF)) {
  case leftHF:
    os() << "Left";
    break;
  case centerHF:
    os() << "Center";
    break;
  case rightHF:
    os() << "Right";
    break;
  }
  if ((flags & (headerHF|footerHF)) == headerHF)
    os() << "Header";
  else
    os() << "Footer";
  os() << "%\n{";
}

void TeXFOTBuilder::endSimplePageSequenceHeaderFooter(unsigned)
{
  endSimpleGroup();
}

void TeXFOTBuilder::startTablePartHeader()
{
  curTable().CurRows = &curTable().curTablePart().Header; 
  startGroup( "TablePartHeader", &curTable().curTablePart().HeaderProlog );
}

void TeXFOTBuilder::endTablePartHeader()
{
  curTable().CurRows = &curTable().curTablePart().Body; 
  endGroup( "TablePartHeader", &curTable().curTablePart().HeaderEpilog );
}

void TeXFOTBuilder::startTablePartFooter()
{
  curTable().CurRows = &curTable().curTablePart().Footer; 
  startGroup( "TablePartFooter", &curTable().curTablePart().FooterProlog );
}

void TeXFOTBuilder::endTablePartFooter()
{
  curTable().CurRows = &curTable().curTablePart().Body; 
  endGroup( "TablePartFooter", &curTable().curTablePart().FooterEpilog );
}

void TeXFOTBuilder::startMathSequence()
{
  startGroup("MathSeq");
}

void TeXFOTBuilder::endMathSequence()
{
  endGroup("MathSeq");
}

void TeXFOTBuilder::startFractionSerial()
{
  startGroup("FractionSerial");
}

void TeXFOTBuilder::endFractionSerial()
{
  endGroup("FractionSerial");
}

void TeXFOTBuilder::startFractionNumerator()
{
  startGroup("FractionNumerator");
}

void TeXFOTBuilder::endFractionNumerator()
{
  endGroup("FractionNumerator");
}

void TeXFOTBuilder::startFractionDenominator()
{
  startGroup("FractionDenominator");
}

void TeXFOTBuilder::endFractionDenominator()
{
  endGroup("FractionDenominator");
}

void TeXFOTBuilder::startUnmath()
{
  startBrace("Unmath");
}

void TeXFOTBuilder::endUnmath()
{
  endBrace("Unmath");
}

void TeXFOTBuilder::startSuperscript()
{ 
   startBrace("Superscript");
}

void TeXFOTBuilder::endSuperscript()
{
  endBrace("Superscript");
}

void TeXFOTBuilder::startSubscript()
{
  startBrace("Subscript");
}

void TeXFOTBuilder::endSubscript()
{
  endBrace("Subscript");
}
void TeXFOTBuilder::startScriptSerial()
{
  startBrace("ScriptSerial");
}

void TeXFOTBuilder::endScriptSerial()
{
}

void TeXFOTBuilder::startScriptPreSup()
{
  closeopenBrace("ScriptPreSup"); // ends brace started in startScript Serial
}

void TeXFOTBuilder::endScriptPreSup()
{
  endSimpleGroup();
}

void TeXFOTBuilder::startScriptPreSub()
{
  startSimpleGroup("ScriptPreSub");
}

void TeXFOTBuilder::endScriptPreSub()
{
  endSimpleGroup();
}

void TeXFOTBuilder::startScriptPostSup()
{
  startSimpleGroup("ScriptPostSup");
}

void TeXFOTBuilder::endScriptPostSup()
{
  endSimpleGroup();
}

void TeXFOTBuilder::startScriptPostSub()
{
  startSimpleGroup("ScriptPostSub");
}

void TeXFOTBuilder::endScriptPostSub()
{
  endSimpleGroup();
}

void TeXFOTBuilder::startScriptMidSup()
{
  startSimpleGroup("ScriptMidSup");
}

void TeXFOTBuilder::endScriptMidSup()
{
  endSimpleGroup();
}

void TeXFOTBuilder::startScriptMidSub()
{
  startSimpleGroup("ScriptMidSub");
}

void TeXFOTBuilder::endScriptMidSub()
{
  endSimpleGroup();
}

void TeXFOTBuilder::startMarkSerial()
{
  startGroup("MarkSerial");
}

void TeXFOTBuilder::endMarkSerial()
{
  endGroup("MarkSerial");
}

void TeXFOTBuilder::startMarkOver()
{
  startGroup("MarkOver");
}

void TeXFOTBuilder::endMarkOver()
{
  endGroup("MarkOver");
}

void TeXFOTBuilder::startMarkUnder()
{
  startGroup("MarkUnder");
}

void TeXFOTBuilder::endMarkUnder()
{
  endGroup("MarkUnder");
}

void TeXFOTBuilder::startFenceSerial()
{
  startBrace("FenceSerial");
}

void TeXFOTBuilder::endFenceSerial()
{
    //  endGroup("FenceSerial");
}

void TeXFOTBuilder::startFenceOpen()
{
  // Extra closing brace for end of fence body
  closeopenBrace("FenceOpen");
}

void TeXFOTBuilder::endFenceOpen()
{
  endSimpleGroup();
}

void TeXFOTBuilder::startFenceClose()
{
  startSimpleGroup("FenceClose");
}

void TeXFOTBuilder::endFenceClose()
{
  endSimpleGroup();
}

void TeXFOTBuilder::startRadicalSerial()
{
  startGroup("RadicalSerial");
}

void TeXFOTBuilder::endRadicalSerial()
{
  endGroup("RadicalSerial");
}

void TeXFOTBuilder::startRadicalDegree()
{
  startGroup("RadicalDegree");
}

void TeXFOTBuilder::endRadicalDegree()
{
  endGroup("RadicalDegree");
}

void TeXFOTBuilder::startMathOperatorSerial()
{
  startGroup("MathOperatorSerial");
}

void TeXFOTBuilder::endMathOperatorSerial()
{
  endGroup("MathOperatorSerial");
}

void TeXFOTBuilder::startMathOperatorOperator()
{
  startGroup("MathOperatorOperator");
}

void TeXFOTBuilder::endMathOperatorOperator()
{
  endGroup("MathOperatorOperator");
}

void TeXFOTBuilder::startMathOperatorLowerLimit()
{
  startGroup("MathOperatorLowerLimit");
}

void TeXFOTBuilder::endMathOperatorLowerLimit()
{
  endGroup("MathOperatorLowerLimit");
}

void TeXFOTBuilder::startMathOperatorUpperLimit()
{
  startGroup("MathOperatorUpperLimit");
}

void TeXFOTBuilder::endMathOperatorUpperLimit()
{
  endGroup("MathOperatorUpperLimit");
}

void TeXFOTBuilder::startGrid(const GridNIC &nic)
{
  setGridNIC(nic);
  startGroup("Grid");
}

void TeXFOTBuilder::endGrid()
{
  endGroup("Grid");
}

void TeXFOTBuilder::startGridCell(const GridCellNIC &nic)
{
  setGridCellNIC(nic);
  startGroup("GridCell");
}

void TeXFOTBuilder::endGridCell()
{
  endGroup("GridCell");
}

void TeXFOTBuilder::startNode(const NodePtr &node,
			      const StringC &processingMode)
{
  GroveString id;
  unsigned long ei;

  if (node->getId(id) == accessOK) {
    set("Label",id);
  }
  else if (node->elementIndex(ei) == accessOK) {
    set("Element", ei);
  }
  unsigned long g = node->groveIndex();
  if (g) {
    set("GroveIndex", g);
  }
  if (processingMode.size()) {
    set("ProcessingMode", processingMode);
  }

  if( curFotElementState().enforcingStructure() ) {
    startGroup( "Node", &(curFotElementState().CurNodeInfoProlog) );
    #ifdef TEXDEBUG
      *fileout_ << "\nSTART_NODE " << ei << "\n";
    #endif
  } else 
    startGroup("Node");
}

void TeXFOTBuilder::endNode()
{
  if( curFotElementState().enforcingStructure() ) {
    curFotElementState().CurNodeInfoProlog.resize( 0 );
    FotElement *lastClosed = CompoundFotElement::lastClosed( curTable() );
    if( lastClosed != NULL )
      endGroup( "Node", &lastClosed->nodeInfoEpilog() );
    #ifdef TEXDEBUG
      *fileout_ << "\nEND_NODE\n";
    #endif
  } else
    endGroup("Node");
}

void TeXFOTBuilder::startLink(const Address &addr)
{
  GroveString id;
  unsigned long ei;

  // FIX ME!
  // This needs a lot of work -- for now, it supports only links to
  // elements.

  switch (addr.type) {
  case Address::none:
    break;
  case Address::resolvedNode:
    if (addr.node->getId(id) == accessOK) {
      set("Label",id);
    } else if (addr.node->elementIndex(ei) == accessOK) {
      set("Element", ei);
    }
    else {
      message(TeXMessages::unsupportedLinkNonElement);
    }
    break;
  case Address::idref:
				// just the first IDREF for now
    set("Label",addr.params[0]);
    break;
  case Address::entity:
    message(TeXMessages::unsupportedLinkEntity);
    break;
  case Address::sgmlDocument:
    message(TeXMessages::unsupportedLinkSgmlDoc);
    break;
  case Address::hytimeLinkend:
    message(TeXMessages::unsupportedLinkHyTime);
    break;
  case Address::tei:
    message(TeXMessages::unsupportedLinkTei);
    break;
  case Address::html:
    message(TeXMessages::unsupportedLinkHtml);
    break;
  }
  if (addr.node) {
    unsigned long g = addr.node->groveIndex();
    if (g) {
      set("GroveIndex",g);
    }
  }
  startGroup("Link");
}

void TeXFOTBuilder::endLink()
{
  endGroup("Link");
}

/////////////////////////////////////////////////////////////////////
// Inherited characteristics
// Set the value of the characteristic for the next flow object.
// Inherited characteristics that are not explicitly set have
// the same value as the parent flow object.
//////////////////////////////////////////////////////////////////////

void TeXFOTBuilder::setFontSize(Length size)
{
  setlength("fSize",size);
}

void TeXFOTBuilder::setFontFamilyName(const StringC &name)
{
  stringout_ << "\\def\\fFamName{";
  for (size_t i = 0; i < name.size(); i++)
    {
      switch(name[i]) {
      case ' ': 
	stringout_ << '-';
        break;
      default:
	stringout_ << char(name[i]);
	break;
       }
      }
      stringout_ << "}";
}

void TeXFOTBuilder::setFontWeight(Symbol weight)
{
  set("fWeight",weight);
}

void TeXFOTBuilder::setFontPosture(Symbol posture)
{
  set("fPosture",posture);
}

void TeXFOTBuilder::setStartIndent(const LengthSpec &indent)
{
  NextFormat.FotStartIndentSpec = indent;
  set("StartIndent",indent);
}

void TeXFOTBuilder::setEndIndent(const LengthSpec &indent)
{
  NextFormat.FotEndIndentSpec = indent;
  set("EndIndent",indent);
}

void TeXFOTBuilder::setFirstLineStartIndent(const LengthSpec &indent)
{
  set("FirstLineStartIndent",indent);
}

void TeXFOTBuilder::setLastLineEndIndent(const LengthSpec &indent)
{
  set("LastLineEndIndent",indent);
}

void TeXFOTBuilder::setLineSpacing(const LengthSpec &spacing)
{
  set("LineSpacing",spacing);
}

void TeXFOTBuilder::setFieldWidth(const LengthSpec &width)
{
  set("FieldWidth",width);
}

void TeXFOTBuilder::setMarginaliaSep(const LengthSpec &sep)
{
  set("MarginaliaSep",sep);
}

void TeXFOTBuilder::setLines(Symbol lines)
{
  set("Lines",lines);
}

void TeXFOTBuilder::setQuadding(Symbol quadding)
{
  set("Quadding",quadding);
}

void TeXFOTBuilder::setDisplayAlignment(Symbol align)
{
  NextFormat.FotDisplayAlignment = align;
  set("DisplayAlignment",align);
}

void TeXFOTBuilder::setFieldAlign(Symbol align)
{
  set("FieldAlign",align);
}

void TeXFOTBuilder::setColor(const DeviceRGBColor &color)
{
  set("Color",color);
}

void TeXFOTBuilder::setBackgroundColor()
{
  set("BackgroundColor",symbolFalse);
}

void TeXFOTBuilder::setBackgroundColor(const DeviceRGBColor &color)
{
  set("BackgroundColor",color);
}

void TeXFOTBuilder::setBorderPresent(bool flag)
{
  NextFormat.FotBorderPresent = flag;
  set("BorderPresent",flag);
}

void TeXFOTBuilder::setLineThickness(Length thickness)
{
  NextFormat.FotLineThickness = thickness;
  setlength("LineThickness",thickness);
}

void TeXFOTBuilder::setCellBeforeRowMargin(Length margin)
{
  setlength("CellBeforeRowMargin",margin);
}

void TeXFOTBuilder::setCellAfterRowMargin(Length margin)
{
  setlength("CellAfterRowMargin",margin);
}

void TeXFOTBuilder::setCellBeforeColumnMargin(Length margin)
{
  NextFormat.FotCellBeforeColumnMargin = margin;
  setlength("CellBeforeColumnMargin",margin);
}

void TeXFOTBuilder::setCellAfterColumnMargin(Length margin)
{
  NextFormat.FotCellAfterColumnMargin = margin;
  setlength("CellAfterColumnMargin",margin);
}

void TeXFOTBuilder::setLineSep(Length sep)
{
  NextFormat.FotLineSep = sep;
  setlength("LineSep",sep);
}

void TeXFOTBuilder::setBoxSizeBefore(Length size)
{
  setlength("BoxSizeBefore",size);
}

void TeXFOTBuilder::setBoxSizeAfter(Length size)
{
  setlength("BoxSizeAfter",size);
}

void TeXFOTBuilder::setPositionPointShift(const LengthSpec &shift)
{
  set("PositionPointShift",shift);
}

void TeXFOTBuilder::setStartMargin(const LengthSpec &margin)
{
  set("StartMargin",margin);
}

void TeXFOTBuilder::setEndMargin(const LengthSpec &margin)
{
  set("EndMargin",margin);
}

void TeXFOTBuilder::setSidelineSep(const LengthSpec &sep)
{
  set("SidelineSep",sep);
}

void TeXFOTBuilder::setAsisWrapIndent(const LengthSpec &indent)
{
  set("AsisWrapIndent",indent);
}

void TeXFOTBuilder::setLineNumberSep(const LengthSpec &sep)
{
  set("LineNumberSep",sep);
}

void TeXFOTBuilder::setLastLineJustifyLimit(const LengthSpec &limit)
{
  set("LastLineJustifyLimit",limit);
}

void TeXFOTBuilder::setJustifyGlyphSpaceMaxAdd(const LengthSpec &max)
{
  set("JustifyGlyphSpaceMaxAdd",max);
}

void TeXFOTBuilder::setJustifyGlyphSpaceMaxRemove(const LengthSpec &max)
{
  set("JustifyGlyphSpaceMaxRemove",max);
}

void TeXFOTBuilder::setTableCornerRadius(const LengthSpec &radius)
{
  set("TableCornerRadius",radius);
}

void TeXFOTBuilder::setBoxCornerRadius(const LengthSpec &radius)
{
  set("BoxCornerRadius",radius);
}

void TeXFOTBuilder::setInhibitLineBreaks(bool flag)
{
  set("InhibitLineBreaks",flag);
}

void TeXFOTBuilder::setHyphenate(bool flag)
{
  set("Hyphenate",flag);
}

void TeXFOTBuilder::setKern(bool flag)
{
  set("Kern",flag);
}

void TeXFOTBuilder::setLigature(bool flag)
{
  set("Ligature",flag);
}

void TeXFOTBuilder::setScoreSpaces(bool flag)
{
  set("ScoreSpaces",flag);
}

void TeXFOTBuilder::setFloatOutMarginalia(bool flag)
{
  set("FloatOutMarginalia",flag);
}

void TeXFOTBuilder::setFloatOutSidelines(bool flag)
{
  set("FloatOutSidelines",flag);
}

void TeXFOTBuilder::setFloatOutLineNumbers(bool flag)
{
  set("FloatOutLineNumbers",flag);
}

void TeXFOTBuilder::setCellBackground(bool flag)
{
  set("CellBackground",flag);
}

void TeXFOTBuilder::setSpanWeak(bool flag)
{
  set("SpanWeak",flag);
}

void TeXFOTBuilder::setIgnoreRecordEnd(bool flag)
{
  set("IgnoreRecordEnd",flag);
}

void TeXFOTBuilder::setNumberedLines(bool flag)
{
  set("NumberedLines",flag);
}

void TeXFOTBuilder::setHangingPunct(bool flag)
{
  set("HangingPunct",flag);
}

void TeXFOTBuilder::setBoxOpenEnd(bool flag)
{
  set("BoxOpenEnd",flag);
}

void TeXFOTBuilder::setTruncateLeader(bool flag)
{
  set("TruncateLeader",flag);
}

void TeXFOTBuilder::setAlignLeader(bool flag)
{
  set("AlignLeader",flag);
}

void TeXFOTBuilder::setTablePartOmitMiddleHeader(bool flag)
{
  set("TablePartOmitMiddleHeader",flag);
}

void TeXFOTBuilder::setTablePartOmitMiddleFooter(bool flag)
{
  set("TablePartOmitMiddleFooter",flag);
}

void TeXFOTBuilder::setBorderOmitAtBreak(bool flag)
{
  set("BorderOmitAtBreak",flag);
}

void TeXFOTBuilder::setPrincipalModeSimultaneous(bool flag)
{
  set("PrincipalModeSimultaneous",flag);
}

void TeXFOTBuilder::setMarginaliaKeepWithPrevious(bool flag)
{
  set("MarginaliaKeepWithPrevious",flag);
}

void TeXFOTBuilder::setLineJoin(Symbol join)
{
  set("LineJoin",join);
}

void TeXFOTBuilder::setLineCap(Symbol cap)
{
  NextFormat.FotLineCap = cap;
  set("LineCap",cap);
}

void TeXFOTBuilder::setLineNumberSide(Symbol side)
{
  set("LineNumberSide",side);
}

void TeXFOTBuilder::setKernMode(Symbol mode)
{
  set("KernMode",mode);
}

void TeXFOTBuilder::setInputWhitespaceTreatment(Symbol treatment)
{
  set("InputWhitespaceTreatment",treatment);
}

void TeXFOTBuilder::setFillingDirection(Symbol direction)
{
  set("FillingDirection",direction);
}

void TeXFOTBuilder::setWritingMode(Symbol mode)
{
  set("WritingMode",mode);
}

void TeXFOTBuilder::setLastLineQuadding(Symbol quadding)
{
  set("LastLineQuadding",quadding);
}

void TeXFOTBuilder::setMathDisplayMode(Symbol mode)
{
  set("MathDisplayMode",mode);
}

void TeXFOTBuilder::setBoxType(Symbol type)
{
  set("BoxType",type);
}

void TeXFOTBuilder::setGlyphAlignmentMode(Symbol mode)
{
  set("GlyphAlignmentMode",mode);
}

void TeXFOTBuilder::setBoxBorderAlignment(Symbol align)
{
  set("BoxBorderAlignment",align);
}

void TeXFOTBuilder::setCellRowAlignment(Symbol align)
{
  set("CellRowAlignment",align);
}

void TeXFOTBuilder::setBorderAlignment(Symbol align)
{
  set("BorderAlignment",align);
}

void TeXFOTBuilder::setSidelineSide(Symbol side)
{
  set("SidelineSide",side);
}

void TeXFOTBuilder::setHyphenationKeep(Symbol keep)
{
  set("HyphenationKeep",keep);
}

void TeXFOTBuilder::setFontStructure(Symbol structure)
{
  set("fStructure",structure);
}

void TeXFOTBuilder::setFontProportionateWidth(Symbol width)
{
  set("fProportionateWidth",width);
}

void TeXFOTBuilder::setCellCrossed(Symbol crossed)
{
  set("CellCrossed",crossed);
}

void TeXFOTBuilder::setMarginaliaSide(Symbol side)
{
  set("MarginaliaSide",side);
}

void TeXFOTBuilder::setLayer(long n)
{
  set("Layer",n);
}

void TeXFOTBuilder::setBackgroundLayer(long n)
{
  set("BackgroundLayer",n);
}

void TeXFOTBuilder::setBorderPriority(long n)
{
  NextFormat.FotBorderPriority = n;
  set("BorderPriority",n);
}

void TeXFOTBuilder::setLineRepeat(long n)
{
  NextFormat.FotLineRepeat = n;
  set("LineRepeat",n);
}

void TeXFOTBuilder::setSpan(long n)
{
  NextFormat.FotSpan = n;
  set("Span",n);
}

void TeXFOTBuilder::setMinLeaderRepeat(long n)
{
  set("MinLeaderRepeat",n);
}

void TeXFOTBuilder::setHyphenationRemainCharCount(long n)
{
  set("HyphenationRemainCharCount",n);
}

void TeXFOTBuilder::setHyphenationPushCharCount(long n)
{
  set("HyphenationPushCharCount",n);
}

void TeXFOTBuilder::setWidowCount(long n)
{
  set("WidowCount",n);
}

void TeXFOTBuilder::setOrphanCount(long n)
{
  set("OrphanCount",n);
}

// 0 means #f
void TeXFOTBuilder::setExpandTabs(long n)
{
  set("ExpandTabs",n);
}

void TeXFOTBuilder::setHyphenationLadderCount(long n)
{
  set("HyphenationLadderCount",n);
}

// public id or #f
void TeXFOTBuilder::setBackgroundTile(PublicId id)
{
  set("BackgroundTile",id);
}

void TeXFOTBuilder::setLineBreakingMethod(PublicId id)
{
  set("LineBreakingMethod",id);
}

void TeXFOTBuilder::setLineCompositionMethod(PublicId id)
{
  set("LineCompositionMethod",id);
}

void TeXFOTBuilder::setImplicitBidiMethod(PublicId id)
{
  set("ImplicitBidiMethod",id);
}

void TeXFOTBuilder::setGlyphSubstMethod(PublicId id)
{
  set("GlyphSubstMethod",id);
}

void TeXFOTBuilder::setGlyphReorderMethod(PublicId id)
{
  set("GlyphReorderMethod",id);
}

void TeXFOTBuilder::setHyphenationMethod(PublicId id)
{
  set("HyphenationMethod",id);
}

void TeXFOTBuilder::setTableAutoWidthMethod(PublicId id)
{
  set("TableAutoWidthMethod",id);
}

void TeXFOTBuilder::setFontName(PublicId id)
{
  set("fName",id);
}

// Two-letter code
void TeXFOTBuilder::setLanguage(Letter2 language)
{
  setletter2("Language",language);
}

void TeXFOTBuilder::setCountry(Letter2 country)
{
  setletter2("Country",country);
}

// For simple page sequence
void TeXFOTBuilder::setPageWidth(Length width)
{
  NextFormat.FotPageWidth = width;
  setlength("PageWidth",width);
}

void TeXFOTBuilder::setPageHeight(Length height)
{
  setlength("PageHeight",height);
}

void TeXFOTBuilder::setLeftMargin(Length margin)
{
  NextFormat.FotLeftMargin = margin;
  setlength("LeftMargin",margin);
}

void TeXFOTBuilder::setRightMargin(Length margin)
{
  NextFormat.FotRightMargin = margin;
  setlength("RightMargin",margin);
}

void TeXFOTBuilder::setTopMargin(Length margin)
{
  setlength("TopMargin",margin);
}

void TeXFOTBuilder::setBottomMargin(Length margin)
{
  setlength("BottomMargin",margin);
}

void TeXFOTBuilder::setHeaderMargin(Length margin)
{
  setlength("HeaderMargin",margin);
}

void TeXFOTBuilder::setFooterMargin(Length margin)
{
  setlength("FooterMargin",margin);
}

				// New inherited characteristics
				// added 1 March/97 with math support.
void TeXFOTBuilder::setMinPreLineSpacing(const OptLengthSpec &len)
{
  set("MinPreLineSpacing",len);
}


void TeXFOTBuilder::setMinPostLineSpacing(const OptLengthSpec &len)
{
  set("MinPostLineSpacing",len);
}


void TeXFOTBuilder::setMinLeading(const OptLengthSpec &len)
{
  set("MinLeading",len);
}


void TeXFOTBuilder::setScriptPreAlign(Symbol sym)
{
  set("ScriptPreAlign",sym);
}


void TeXFOTBuilder::setScriptPostAlign(Symbol sym)
{
  set("ScriptPostAlign",sym);
}


void TeXFOTBuilder::setScriptMidSupAlign(Symbol sym)
{
  set("ScriptMidSupAlign",sym);
}


void TeXFOTBuilder::setScriptMidSubAlign(Symbol sym)
{
  set("ScriptMidSubAlign",sym);
}


void TeXFOTBuilder::setNumeratorAlign(Symbol sym)
{
  set("NumeratorAlign",sym);
}


void TeXFOTBuilder::setDenominatorAlign(Symbol sym)
{
  set("DenominatorAlign",sym);
}


void TeXFOTBuilder::setGridPositionCellType(Symbol sym)
{
  set("GridPositionCellType",sym);
}


void TeXFOTBuilder::setGridColumnAlignment(Symbol sym)
{
  set("GridColumnAlignment",sym);
}


void TeXFOTBuilder::setGridRowAlignment(Symbol sym)
{
  set("GridRowAlignment",sym);
}


void TeXFOTBuilder::setGridEquidistantRows(bool flag)
{
  set("GridEquidistantRows",flag);
}


void TeXFOTBuilder::setGridEquidistantColumns(bool flag)
{
  set("GridEquidistantColumns",flag);
}


void TeXFOTBuilder::setEscapementSpaceBefore(const InlineSpace &space)
{
  set("EscapementSpaceBefore",space);
}


void TeXFOTBuilder::setEscapementSpaceAfter(const InlineSpace &space)
{
  set("EscapementSpaceAfter",space);
}


void TeXFOTBuilder::setGlyphSubstTable(const Vector<ConstPtr<GlyphSubstTable> > &)
{
  // FIX ME!
  message(TeXMessages::unsupportedGlyphSubstTable);
  // set("GlyphSubstTable",tables);
}

////////////////////////////////////////////////////////////////////////
// Private member functions.
////////////////////////////////////////////////////////////////////////

//
// Insert an atomic flow object.
//
void TeXFOTBuilder::insertAtomic(const char *name)
{
  os() << "\\insert" << name << "%\n{";
  dumpInherited();
  os() << '}';
}

void TeXFOTBuilder::insertAtomic( TeXFOTBuilder::FotElement &fotElement )
{
  stringout_.extractString( fotElement.Characteristics );
}

//
// Start a non-atomic flow object.
//
void TeXFOTBuilder::startGroup(const char *name, String<char> *output )
{
  if( output ) {
    String<char> s;
    stringout_.extractString( s );
    StrOutputByteStream out;
    out << "\\" << name << "%\n{" << s << '}';
    out.extractString( s );
    *output += s;
  } else {
    os() << "\\" << name << "%\n{";
    dumpInherited();
    os() << '}';
  }
}

void TeXFOTBuilder::startGroup( TeXFOTBuilder::FotElement &fotElement ) {

  stringout_.extractString( fotElement.Characteristics );
}

//
// Start a non-atomic flow object, with the content delimited by braces,
// but no macro name at all; we just emit all the characteristics in
// the stream after the brace.
//
void TeXFOTBuilder::startSimpleGroup(const char *name, String<char> *output )
{
  if( output ) {
    String<char> s;
    stringout_.extractString( s );
    StrOutputByteStream out;
    out << "%\n{" << s ;
    out.extractString( s );
    *output += s;
  } else {
    os() << "%\n{";
    dumpInherited();
  }
}

//
// End with just a closing brace
//
void TeXFOTBuilder::endSimpleGroup(String<char> *output )
{
  if( output ) {
    StrOutputByteStream out;
    out << "}"; 
    String<char> s;
    out.extractString( s );
    *output += s;
  }
  else
    os() << "}";
}
//
// Stop and start a brace, but note characteristics
//
void TeXFOTBuilder::closeopenBrace(const char *name, String<char> *output )
{
  if( output ) {
    String<char> s;
    stringout_.extractString( s );
    StrOutputByteStream out;
    out << "}{" << s ;
    out.extractString( s );
    *output += s;
  } else {
    os() << "}{";
    dumpInherited();
  }
}
//
// Start a non-atomic flow object, with the content delimited by braces
//
void TeXFOTBuilder::startBrace(const char *name, String<char> *output )
{
  if( output ) {
    String<char> s;
    stringout_.extractString( s );
    StrOutputByteStream out;
    out << "\\" << name << "%\n{" << s << "}{";
    out.extractString( s );
    *output += s;
  } else {
    os() << "\\" << name << "%\n{";
    dumpInherited();
    os() << "}{";
  }
}

//
// End a non-atomic flow object with just a closing brace
//
void TeXFOTBuilder::endBrace(const char *name, String<char> *output )
{
  if( output ) {
    StrOutputByteStream out;
    out << "}"; 
    String<char> s;
    out.extractString( s );
    *output += s;
  }
  else
    os() << "}";
}

//
// End a non-atomic flow object.
//
void TeXFOTBuilder::endGroup(const char *name, String<char> *output )
{
  if( output ) {
    StrOutputByteStream out;
    out << "\\end" << name << "{}";
    String<char> s;
    out.extractString( s );
    *output += s;
  }
  else
    os() << "\\end" << name << "{}";
}

//
// Set a Length (needs a different name to avoid conflict
// with long.
//
void TeXFOTBuilder::setlength(const char *name,Length size)
{
  stringout_ << "\\def\\" << name << "%\n{"
	      << float(size/1000.0)
	      << "\\p@}";
}

//
// Set a StringC.
//
void TeXFOTBuilder::set(const char *name,const StringC &value)
{
  stringout_ << "\\def\\" << name << "%\n{"
	      << value
	      << '}';
}

//
// Set a GroveString
//
void TeXFOTBuilder::set(const char *name,const GroveString &value)
{
  stringout_ << "\\def\\" << name << "%\n{"
	      << value
	      << '}';
}

//
// Set a Symbol.
//
void TeXFOTBuilder::set(const char *name,Symbol sym)
{
  const char * symbolName = "";

  switch (sym) {
  case symbolFalse:
    symbolName = "false";
    break;
  case symbolTrue:
    symbolName = "true";
    break;
  case symbolNotApplicable:
    symbolName = "notapplicable";
    break;
  case symbolUltraCondensed:
    symbolName = "ultracondensed";
    break;
  case symbolExtraCondensed:
    symbolName = "extracondensed";
    break;
  case symbolCondensed:
    symbolName = "condensed";
    break;
  case symbolSemiCondensed:
    symbolName = "semicondensed";
    break;
  case symbolUltraLight:
    symbolName = "ultralight";
    break;
  case symbolExtraLight:
    symbolName = "extralight";
    break;
  case symbolLight:
    symbolName = "light";
    break;
  case symbolSemiLight:
    symbolName = "semilight";
    break;
  case symbolMedium:
    symbolName = "medium";
    break;
  case symbolSemiExpanded:
    symbolName = "semiexpanded";
    break;
  case symbolExpanded:
    symbolName = "expanded";
    break;
  case symbolExtraExpanded:
    symbolName = "extraexpanded";
    break;
  case symbolUltraExpanded:
    symbolName = "ultraexpanded";
    break;
  case symbolSemiBold:
    symbolName = "semibold";
    break;
  case symbolBold:
    symbolName = "bold";
    break;
  case symbolExtraBold:
    symbolName = "extrabold";
    break;
  case symbolUltraBold:
    symbolName = "ultrabold";
    break;
  case symbolUpright:
    symbolName = "upright";
    break;
  case symbolOblique:
    symbolName = "oblique";
    break;
  case symbolBackSlantedOblique:
    symbolName = "backslantedoblique";
    break;
  case symbolItalic:
    symbolName = "italic";
    break;
  case symbolBackSlantedItalic:
    symbolName = "backslanteditalic";
    break;
  case symbolStart:
    symbolName = "start";
    break;
  case symbolEnd:
    symbolName = "end";
    break;
  case symbolCenter:
    symbolName = "center";
    break;
  case symbolJustify:
    symbolName = "justify";
    break;
  case symbolSpreadInside:
    symbolName = "spreadinside";
    break;
  case symbolSpreadOutside:
    symbolName = "spreadoutside";
    break;
  case symbolPageInside:
    symbolName = "pageinside";
    break;
  case symbolPageOutside:
    symbolName = "pageoutside";
    break;
  case symbolWrap:
    symbolName = "wrap";
    break;
  case symbolAsis:
    symbolName = "asis";
    break;
  case symbolAsisWrap:
    symbolName = "asiswrap";
    break;
  case symbolAsisTruncate:
    symbolName = "asistruncate";
    break;
  case symbolNone:
    symbolName = "none";
    break;
  case symbolBefore:
    symbolName = "before";
    break;
  case symbolThrough:
    symbolName = "through";
    break;
  case symbolAfter:
    symbolName = "after";
    break;
  case symbolTopToBottom:
    symbolName = "toptobottom";
    break;
  case symbolLeftToRight:
    symbolName = "lefttoright";
    break;
  case symbolBottomToTop:
    symbolName = "bottomtotop";
    break;
  case symbolRightToLeft:
    symbolName = "righttoleft";
    break;
  case symbolInside:
    symbolName = "inside";
    break;
  case symbolOutside:
    symbolName = "outside";
    break;
  case symbolHorizontal:
    symbolName = "horizontal";
    break;
  case symbolVertical:
    symbolName = "vertical";
    break;
  case symbolEscapement:
    symbolName = "escapement";
    break;
  case symbolLineProgression:
    symbolName = "lineprogression";
    break;
  case symbolMath:
    symbolName = "math";
    break;
  case symbolOrdinary:
    symbolName = "ordinary";
    break;
  case symbolOperator:
    symbolName = "operator";
    break;
  case symbolBinary:
    symbolName = "binary";
    break;
  case symbolRelation:
    symbolName = "relation";
    break;
  case symbolOpening:
    symbolName = "opening";
    break;
  case symbolClosing:
    symbolName = "closing";
    break;
  case symbolPunctuation:
    symbolName = "punctuation";
    break;
  case symbolInner:
    symbolName = "inner";
    break;
  case symbolSpace:
    symbolName = "space";
    break;
  case symbolPage:
    symbolName = "page";
    break;
  case symbolPageRegion:
    symbolName = "pageregion";
    break;
  case symbolColumnSet:
    symbolName = "columnset";
    break;
  case symbolColumn:
    symbolName = "column";
    break;
  case symbolMax:
    symbolName = "max";
    break;
  case symbolMaxUniform:
    symbolName = "maxuniform";
    break;
  case symbolMiter:
    symbolName = "miter";
    break;
  case symbolRound:
    symbolName = "round";
    break;
  case symbolBevel:
    symbolName = "bevel";
    break;
  case symbolButt:
    symbolName = "butt";
    break;
  case symbolSquare:
    symbolName = "square";
    break;
  case symbolLoose:
    symbolName = "loose";
    break;
  case symbolNormal:
    symbolName = "normal";
    break;
  case symbolKern:
    symbolName = "kern";
    break;
  case symbolTight:
    symbolName = "tight";
    break;
  case symbolTouch:
    symbolName = "touch";
    break;
  case symbolPreserve:
    symbolName = "preserve";
    break;
  case symbolCollapse:
    symbolName = "collapse";
    break;
  case symbolIgnore:
    symbolName = "ignore";
    break;
  case symbolRelative:
    symbolName = "relative";
    break;
  case symbolDisplay:
    symbolName = "display";
    break;
  case symbolInline:
    symbolName = "inline";
    break;
  case symbolBorder:
    symbolName = "border";
    break;
  case symbolBackground:
    symbolName = "background";
    break;
  case symbolBoth:
    symbolName = "both";
    break;
  case symbolBase:
    symbolName = "base";
    break;
  case symbolFont:
    symbolName = "font";
    break;
  case symbolTop:
    symbolName = "top";
    break;
  case symbolBottom:
    symbolName = "bottom";
    break;
  case symbolSpread:
    symbolName = "spread";
    break;
  case symbolSolid:
    symbolName = "solid";
    break;
  case symbolOutline:
    symbolName = "outline";
    break;
  case symbolWith:
    symbolName = "with";
    break;
  case symbolAgainst:
    symbolName = "against";
    break;
  case symbolForce:
    symbolName = "force";
    break;
  case symbolIndependent:
    symbolName = "independent";
    break;
  case symbolPile:
    symbolName = "pile";
    break;
  case symbolSupOut:
    symbolName = "supout";
    break;
  case symbolSubOut:
    symbolName = "subout";
    break;
  case symbolLeadEdge:
    symbolName = "leadedge";
    break;
  case symbolTrailEdge:
    symbolName = "trailedge";
    break;
  case symbolExplicit:
    symbolName = "explicit";
    break;
  case symbolRowMajor:
    symbolName = "rowmajor";
    break;
  case symbolColumnMajor:
    symbolName = "columnmajor";
    break;
  }

  stringout_ << "\\def\\" << name << "%\n{" << symbolName << '}';
}

//
// Set a LengthSpec.
//
void TeXFOTBuilder::set(const char *name,const LengthSpec &spec)
{
  stringout_ << "\\def\\" << name << "%\n{"
	      << float(spec.length/1000.0)
	      << "\\p@}";
  stringout_ << "\\def\\" << name << "Factor%\n{"
	      << spec.displaySizeFactor
	      << '}';
}

//
// Set a double.
//
void TeXFOTBuilder::set(const char *name,double n)
{
  stringout_ << "\\def\\" << name << "%\n{" << n << '}';
}

//
// Set a DeviceRGBColor.
//
void TeXFOTBuilder::set(const char *name, const DeviceRGBColor &color)
{
  stringout_ << "\\def\\" << name << "Red%\n{" << int(color.red) << '}';
  stringout_ << "\\def\\" << name << "Green%\n{" << int(color.green) << '}';
  stringout_ << "\\def\\" << name << "Blue%\n{" << int(color.blue) << '}';
}

//
// Set a bool.
//
void TeXFOTBuilder::set(const char *name,bool flag)
{
  stringout_ << "\\def\\" << name << "%\n{"
	      << (flag ? 1 : 0)
	      << '}';
}

//
// Set a long.
//
void TeXFOTBuilder::set(const char *name,long n)
{
  stringout_ << "\\def\\" << name << "%\n{" << n << '}';
}

//
// Set an unsigned long.
//
void TeXFOTBuilder::set(const char *name,long unsigned int n)
{
  stringout_ << "\\def\\" << name << "%\n{" << n << '}';
}

//
// Set a PublicId.
//
void TeXFOTBuilder::set(const char *name,PublicId id)
{
  stringout_ << "\\def\\" << name << "%\n{" << id << '}';
}

//
// Set a Letter2.
//
void TeXFOTBuilder::setletter2(const char *name,Letter2 code)
{
  char letter1 = (code & 0xff00) >> 8;
  char letter2 = (code & 0xff);
  stringout_ << "\\def\\" << name << "%\n{" << letter1 << letter2 << '}';
}

//
// This one is a problem because it duplications functionality from
// above.
//
void TeXFOTBuilder::set(const char *name,const DisplaySpace &space)
{
  if (space.nominal.length != 0 || space.min.length != 0
      || space.max.length != 0) {
    stringout_ << "\\def\\" << name << "Nom%\n{"
		<< (space.nominal.length/1000.0) << "\\p@}";
    if (space.nominal.displaySizeFactor != 0)
      stringout_ << "\\def\\" << name << "NomFactor%\n{"
		  << space.nominal.displaySizeFactor << '}';
    if (space.min.length != 0)
      stringout_ << "\\def\\" << name << "Min%\n{"
		  << (space.min.length/1000.0) << "\\p@}";
    if (space.min.displaySizeFactor != 0)
      stringout_ << "\\def\\" << name << "MinFactor%\n{"
		  << space.min.displaySizeFactor << '}';
    if (space.max.length != 0)
      stringout_ << "\\def\\" << name << "Max%\n{"
		  << (space.max.length/1000.0) << "\\p@}";
    if (space.max.displaySizeFactor != 0)
      stringout_ << "\\def\\" << name << "MaxFactor%\n{"
		  << space.max.displaySizeFactor << '}';
    if (space.priority != 0)
      stringout_ << "\\def\\" << name << "Priority%\n{"
		  << space.priority << '}';
    if (space.conditional)
      stringout_ << "\\def\\" << name << "Conditional%\n{"
		  << (space.conditional ? 1 : 0) << '}';
    if (space.force)
      stringout_ << "\\def\\" << name << "Force%\n{"
		  << (space.force ? 1 : 0) << '}';
  }
}

void TeXFOTBuilder::set(const char *name,const GlyphId &glyphId)
{
  stringout_ << "\\def\\" << name << "%\n{";
  if (glyphId.publicId) {
    stringout_ << glyphId.publicId;
    if (glyphId.suffix)
      stringout_ << "::" << glyphId.suffix;
  }
  stringout_ << '}';
}

void TeXFOTBuilder::set(const char *name,const OptLengthSpec &spec)
{
  if (spec.hasLength) {
    set(name,spec.length);
  }
}

// This one is also a problem because it duplicates functionality.
void TeXFOTBuilder::set(const char *name,const InlineSpace &space)
{
  if (space.nominal.length != 0 || space.min.length != 0
      || space.max.length != 0) {
    stringout_ << "\\def\\" << name << "Nom%\n{"
		<< (space.nominal.length/1000.0) << "\\p@}";
    if (space.nominal.displaySizeFactor != 0)
      stringout_ << "\\def\\" << name << "NomFactor%\n{"
		  << space.nominal.displaySizeFactor << '}';
    if (space.min.length != 0)
      stringout_ << "\\def\\" << name << "Min%\n{"
		  << (space.min.length/1000.0) << "\\p@}";
    if (space.min.displaySizeFactor != 0)
      stringout_ << "\\def\\" << name << "MinFactor%\n{"
		  << space.min.displaySizeFactor << '}';
    if (space.max.length != 0)
      stringout_ << "\\def\\" << name << "Max%\n{"
		  << (space.max.length/1000.0) << "\\p@}";
    if (space.max.displaySizeFactor != 0)
      stringout_ << "\\def\\" << name << "MaxFactor%\n{"
		  << space.max.displaySizeFactor << '}';
  }
}

//
// Structures for non-inherited characters, in the order specified
// in /style/FOTBuilder.h.
//

void TeXFOTBuilder::setDisplayNIC(const DisplayNIC &nic)
{
  set("sb",nic.spaceBefore);
  set("sa",nic.spaceAfter);
  MAYBESET("PositionPreference",nic.positionPreference,symbolFalse);
  MAYBESET("Keep",nic.keep,symbolFalse);
  MAYBESET("BreakBefore",nic.breakBefore,symbolFalse);
  MAYBESET("BreakAfter",nic.breakAfter,symbolFalse);
  MAYBESET("KeepWithPrevious",nic.keepWithPrevious,symbolFalse);
  MAYBESET("KeepWithNext",nic.keepWithNext,symbolFalse);
  MAYBESET("MayViolateKeepBefore",nic.mayViolateKeepBefore,symbolFalse);
  MAYBESET("MayViolateKeepAfter",nic.mayViolateKeepAfter,symbolFalse);
}

void TeXFOTBuilder::setInlineNIC(const InlineNIC &nic)
{
  MAYBESET("BreakBeforePriority",nic.breakBeforePriority,0);
  MAYBESET("BreakAfterPriority",nic.breakAfterPriority,0);
}

void TeXFOTBuilder::setDisplayGroupNIC(const DisplayGroupNIC &nic)
{
  setDisplayNIC(nic);
  if (nic.hasCoalesceId) 
    set("CoalesceId",nic.coalesceId);
}

void TeXFOTBuilder::setExternalGraphicNIC(const ExternalGraphicNIC &nic)
{
  setDisplayNIC(nic);
  setInlineNIC(nic);

  MAYBESET("IsDisplay",nic.isDisplay,symbolFalse);
  MAYBESET("ScaleType",nic.scaleType,symbolMaxUniform);
  if (nic.scaleType == symbolFalse) {
    set("ScaleX",nic.scale[0]);
    set("ScaleY",nic.scale[1]);
  }
  set("EntitySystemId",nic.entitySystemId);
  set("NotationSystemId",nic.notationSystemId);
  if(nic.hasMaxWidth)
    set("MaxWidth",nic.maxWidth);
  if (nic.hasMaxHeight)
    set("MaxHeight",nic.maxHeight);
  if (!nic.isDisplay) {
    set("EscapementDirection",nic.escapementDirection);
    set("PositionPointX",nic.positionPointX);
    set("PositionPointY",nic.positionPointY);
  }
}

void TeXFOTBuilder::setBoxNIC(const BoxNIC &nic)
{
  setDisplayNIC(nic);
  setInlineNIC(nic);
				// BoxNIC
  MAYBESET("IsDisplay",nic.isDisplay,symbolFalse);
}

void TeXFOTBuilder::setRuleNIC(const RuleNIC &nic)
{
  setDisplayNIC(nic);
  setInlineNIC(nic);
				// Rule NIC
  set("Orientation",nic.orientation);
  if (nic.hasLength)
    set("Length",nic.length);
}

void TeXFOTBuilder::setLeaderNIC(const LeaderNIC &nic)
{
  setInlineNIC(nic);
  if (nic.hasLength)
    set("Length",nic.length);
}

void TeXFOTBuilder::setParagraphNIC(const ParagraphNIC &nic)
{
  setDisplayNIC(nic);
}

void TeXFOTBuilder::setCharacterNIC(const CharacterNIC &nic)
{
  if (nic.specifiedC & (1 << CharacterNIC::cChar))
    set("Ch",(unsigned long)nic.ch);
  if (nic.specifiedC & (1 << CharacterNIC::cGlyphId))
    set("GlyphId",nic.glyphId);
  if (nic.specifiedC & (1 << CharacterNIC::cBreakBeforePriority))
    set("BreakBeforePriority",nic.breakBeforePriority);
  if (nic.specifiedC & (1 << CharacterNIC::cBreakAfterPriority))
    set("BreakAfterPriority",nic.breakAfterPriority);
  if (nic.specifiedC & (1 << CharacterNIC::cMathClass))
    set("MathClass",nic.mathClass);
  if (nic.specifiedC & (1 << CharacterNIC::cMathFontPosture))
    set("MathFontPosture",nic.mathFontPosture);
  if (nic.specifiedC & (1 << CharacterNIC::cScript))
    set("Script",(long unsigned int)nic.script);
  if (nic.specifiedC & (1 << CharacterNIC::cIsDropAfterLineBreak))
    set("IsDropAfterLineBreak",nic.isDropAfterLineBreak);
  if (nic.specifiedC & (1 << CharacterNIC::cIsDropUnlessBeforeLineBreak))
    set("IsDropUnlessBeforeLineBreak",nic.isDropUnlessBeforeLineBreak);
  if (nic.specifiedC & (1 << CharacterNIC::cIsPunct))
    set("IsPunct",nic.isPunct);
  if (nic.specifiedC & (1 << CharacterNIC::cIsInputWhitespace))
    set("IsInputWhiteSpace",nic.isInputWhitespace);
  if (nic.specifiedC & (1 << CharacterNIC::cIsInputTab))
    set("IsInputTab",nic.isInputTab);
  if (nic.specifiedC & (1 << CharacterNIC::cIsRecordEnd))
    set("IsRecordEnd",nic.isRecordEnd);
  if (nic.specifiedC & (1 << CharacterNIC::cIsSpace))
    set("IsSpace",nic.isSpace);
  MAYBESET("StretchFactor",nic.stretchFactor,1.0);
}

void TeXFOTBuilder::setLineFieldNIC(const LineFieldNIC &nic)
{
  setInlineNIC(nic);
}

void TeXFOTBuilder::setTableNIC(const TableNIC &nic)
{
  setDisplayNIC(nic);
				// TableNIC
  switch (nic.widthType) {
  case TableNIC::widthFull:
    set("TableWidth","full");
    break;
  case TableNIC::widthMinimum:
    set("TableWidth","minimum");
    break;
  case TableNIC::widthExplicit:
    set("TableWidth",nic.width);
    break;
  }
}

void TeXFOTBuilder::setTablePartNIC(const TablePartNIC &nic)
{
  setDisplayNIC(nic);
}

void TeXFOTBuilder::setTableColumnNIC(const TableColumnNIC &nic)
{
  MAYBESET("ColumnIndex",long(nic.columnIndex),0);
  MAYBESET("NColumnsSpanned",long(nic.nColumnsSpanned),1);
  if (nic.hasWidth)
    set("Width",nic.width);
}

void TeXFOTBuilder::setTableCellNIC(const TableCellNIC &nic)
{
  // FIX ME!
  // does not deal with "missing" bool yet.
  MAYBESET("ColumnIndex",long(nic.columnIndex),0);
  MAYBESET("NColumnsSpanned",long(nic.nColumnsSpanned),1);
  MAYBESET("NRowsSpanned",long(nic.nRowsSpanned),1);
}

void TeXFOTBuilder::setGridNIC(const GridNIC &nic)
{
  set("NColumns",nic.nColumns);
  set("NRows",nic.nRows);
}

void TeXFOTBuilder::setGridCellNIC(const GridCellNIC &nic)
{
  set("ColumnNumber",nic.columnNumber);
  set("RowNumber",nic.rowNumber);
}


// Dump all accumulated inherited characteristics.

void TeXFOTBuilder::dumpInherited()
{
  String<char> tem;
  stringout_.extractString(tem);
  os() << tem;
}

void TeXFOTBuilder::message(const MessageType0 &msg)
{
  mgr_->message(msg);
}

void TeXFOTBuilder::setPageNumberFormat(const StringC &name)
{
  set("PageNumberFormat",name);
}

void TeXFOTBuilder::setPageNColumns(long n)
{
  NextFormat.FotPageNColumns = n;
  set("PageNColumns",n);
}

void TeXFOTBuilder::setPageColumnSep(Length w)
{
  NextFormat.FotPageColumnSep = w;
  setlength("PageColumnSep",w);
}

void TeXFOTBuilder::setPageBalanceColumns(bool flag)
{
 set("PageBalanceColumns",flag);
}

void TeXFOTBuilder::setSubscriptDepth(Length w)
{
 setlength("SubScriptDepth",w);
}

void TeXFOTBuilder::setOverMarkHeight(Length w)
{
 setlength("OverMarkHeight",w);
}

void TeXFOTBuilder::setUnderMarkDepth(Length w)
{
 setlength("UnderMarkDepth",w);
}

void TeXFOTBuilder::setSuperscriptHeight(Length w)
{
 setlength("SuperscriptHeight",w);
}

void TeXFOTBuilder::setGridRowSep(Length  w)
{
 setlength("GridRowsep",w);
}

void TeXFOTBuilder::setGridColumnSep(Length w)
{
 setlength("GridColumnSep",w);
}

void TeXFOTBuilder::setHeadingLevel(long n)
{
 set("HeadingLevel",n);
}

void TeXFOTBuilder::setPageNumberRestart(bool flag)
{
 set("PageNumberRestart",flag);
}

void TeXFOTBuilder::startPageFloat(const PageFloatNIC &nic)
{
  setPageFloatNIC(nic);
  startGroup("PageFloat");
}

void TeXFOTBuilder::endPageFloat()
{
  endGroup("PageFloat");
}

void TeXFOTBuilder::startPageFootnote()
{
  startBrace("PageFootnote");
}

void TeXFOTBuilder::endPageFootnote()
{
  endBrace("PageFootnote");
}

void TeXFOTBuilder::setPageFloatNIC(const PageFloatNIC &nic)
{
  set("placement",nic.placement);
}

TeXFOTBuilder::PageFloatNIC::~PageFloatNIC()
{
}

void TeXFOTBuilder::extension(const ExtensionFlowObj &fo, const NodePtr &nd)
{
  ((const TeXExtensionFlowObj &)fo).atomic(*this, nd);
}

void TeXFOTBuilder::startExtensionSerial(const CompoundExtensionFlowObj &fo, const NodePtr &nd)
{
  ((const TeXCompoundExtensionFlowObj &)fo).start(*this, nd);
}

void TeXFOTBuilder::endExtensionSerial(const CompoundExtensionFlowObj &fo)
{
  ((const TeXCompoundExtensionFlowObj &)fo).end(*this);
}

void TeXFOTBuilder::setPreserveSdata(bool b)
{
  preserveSdata_ = b;
}

void TeXFOTBuilder::charactersFromNode(const NodePtr &nd, const Char *s, size_t n)
{
  GroveString name;
  if (preserveSdata_ && n == 1 && nd->getEntityName(name) == accessOK)
    os() << "\\Entity{" << name << '}';
  else
    TeXFOTBuilder::characters(s, n);
}

#ifdef DSSSL_NAMESPACE
}
#endif

#include "TeXFOTBuilder_inst.cxx"
