/* Copyright (c) 1998 ISOGEN International Corp.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
``Software''), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED ``AS IS'', WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL ISOGEN INTERNATIONAL CORP. BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of ISOGEN International
Corp. shall not be used in advertising or otherwise to promote the
sale, use or other dealings in this Software without prior written
authorization from ISOGEN International Corp. */

/* Created by Kathleen Marszalek and Paul Prescod. */

#include "config.h"

#ifdef JADE_MIF

#include "MifFOTBuilder.h"
#include "MifMessages.h"
#include "TmpOutputByteStream.h"
#include "CharMap.h"
#include "CmdLineApp.h"
#include "ErrnoMessageArg.h"
#include "FOTBuilder.h"
#include "Hash.h"
#include "HashTable.h"
#include "IList.h"
#include "IQueue.h"
#include "InputSource.h"
#include "Message.h"
#include "OutputByteStream.h"
#include "Owner.h"
#include "PointerTable.h"
#include "StorageManager.h"
#include "StringC.h"
#include "Vector.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef DSSSL_NAMESPACE
namespace DSSSL_NAMESPACE {
#endif

//#define assert(exp) (void)( (exp) || (throw "assertion_failed", 0) )

#define PROPERTY( propertyType, propertyName ) \
    propertyType propertyName; \
    void set ## propertyName ( propertyType p ) \
     { propertyName = p; setProperties |= f ## propertyName ;} 

#define PROPERTY_OUT( propertyName ) \
    if( properties & f ## propertyName ) \
        os << '\n' << MifOutputByteStream::INDENT \
           << "<" #propertyName " " << propertyName << ">"; 

#define VECTOR_OF_PROPERTIES( propertyType, propertyName ) \
    Vector< propertyType > propertyName ## s; \
    void propertyName ## sAreSet() \
     { setProperties |= f ## propertyName ## s ;}

#define STATIC_PROPERTY( propertyType, propertyName ) \
    static propertyType propertyName; \
    static void set ## propertyName ( propertyType p ) \
     { propertyName = p; setProperties |= f ## propertyName ;}

class StringHash {
public:
  static unsigned long hash( const String<char> & );
};

class MifOutputByteStream;
class MifTmpOutputByteStream;
class MifDoc {

  public:

    MifDoc( const String<CmdLineApp::AppChar> &fileLoc, CmdLineApp *app );
    ~MifDoc();
    static MifDoc &curInstance() { assert( CurInstance != NULL ); return *CurInstance; }
    static MifDoc *CurInstance;

    struct T_indent {

        T_indent( unsigned i ) : data( i ) {}
        long data;
        operator unsigned() const { return data; }
    };

    struct T_dimension {

        T_dimension( long u = 0 ) : data( u ) {}
        long data;
        operator long() const { return data; }
        T_dimension &operator=( long s ) { data = s; return *this; }
    };

    struct T_string : public String<char> {

        T_string() : String<char>() {};
        T_string( const String<char> &s ) : String<char>( s ) {};
        T_string( String<CmdLineApp::AppChar> &s ) : String<char>() 
          { for( size_t i = 0; i < s.size(); i++ ) *this += char( s[i] ); }
        T_string( const char *cS ) : String<char>( cS, strlen( cS ) ) {};
        T_string &operator=( String<char> s ) { String<char>::operator=( s ); return *this; }
        void escapeSpecialChars();
    };

    struct T_boolean {

        T_boolean( bool b = false ) : data( b ) {}
        bool data;
        operator bool() const { return data; }
        T_boolean &operator=( bool b ) { data = b; return *this; }
    };

    typedef T_string        T_tagstring;
    typedef T_string        T_pathname;
    typedef String<char>    T_keyword;

    typedef long        T_integer;
    typedef unsigned    T_ID;
    typedef long        T_degrees;
    typedef double      T_percentage;
    typedef long        T_metric;

    struct T_WH { T_dimension w, h; // width, height      
                  T_WH( T_dimension w_, T_dimension h_ ) : w( w_ ), h( h_ ) {}  
                  T_WH() : w( 0L ), h( 0L ) {} };
    struct T_XY { T_dimension x, y;
                  T_XY( T_dimension x_, T_dimension y_ ) : x( x_ ), y( y_ ) {}  
                  T_XY() : x( 0L ), y( 0L ) {} };
    struct T_LTRB { T_dimension l, t, r, b; // left, top, right, bottom
                    T_LTRB( T_dimension l_, T_dimension t_, T_dimension r_, T_dimension b_ )
                     : l( l_ ), t( t_ ), r( r_ ), b( b_ ) {}
                    T_LTRB() : l( 0L ), t( 0L ), r( 0L ), b( 0L ) {} };
    struct T_LTWH { T_dimension l, t, w, h; // left, top, width, height
                    T_LTWH( T_dimension l_, T_dimension t_, T_dimension w_, T_dimension h_ )
                     : l( l_ ), t( t_ ), w( w_ ), h( h_ ) {}
                    T_LTWH() : l( 0L ), t( 0L ), w( 0L ), h( 0L ) {} };
    struct T_XYWH { T_dimension x, y, w, h; };          
 

    // special characters
    static const String<char> sTab;
    static const String<char> sHardSpace;
    static const String<char> sSoftHyphen;
    static const String<char> sHardHyphen;
    static const String<char> sHardReturn;
    static const String<char> sNumberSpace;
    static const String<char> sThinSpace;
    static const String<char> sEnSpace;
    static const String<char> sEmSpace;
    static const String<char> sCent;
    static const String<char> sPound;
    static const String<char> sYen;
    static const String<char> sEnDash;
    static const String<char> sEmDash;
    static const String<char> sDagger;
    static const String<char> sDoubleDagger;
    static const String<char> sBullet;

    // for tagged text flows
    static const String<char> sA;

    // FWeight
    static const String<char> sRegular;
    static const String<char> sBold;

    // FAngle
    // sRegular;
    static const String<char> sItalic;

    // FVar
    // sRegular;

    // FColor
    static const String<char> sWhite;
    static const String<char> sBlack;

    // FUnderlining
    static const String<char> sFNoUnderlining;
    static const String<char> sFSingle;
    static const String<char> sFDouble;
    static const String<char> sFNumeric;

    // FPosition
    static const String<char> sFNormal;
    static const String<char> sFSuperscript;
    static const String<char> sFSubscript;

    // FCase
    static const String<char> sFAsTyped;
    static const String<char> sFSmallCaps;
    static const String<char> sFLowercase;
    static const String<char> sFUppercase;

    // FFamily
    static const String<char> sTimesNewRoman;

    struct FontFormat : public Link {

        enum { fNone = 0, fFFamily = 0x1, fFAngle = 0x2, fFWeight = 0x4, fFVar = 0x8,
               fFSize = 0x10, fFColor = 0x20, fFUnderlining = 0x40, fFOverline = 0x80,
               fFStrike = 0x100, fFPosition = 0x200, fFPairKern = 0x400, fFCase = 0x800,
               fFDX = 0x1000, fFDY = 0x2000, fFDW = 0x4000, fAll = 0x7FFF };
        unsigned setProperties;

        FontFormat() : setProperties( 0 ) {}

        // font name
        PROPERTY( T_string, FFamily );
        PROPERTY( T_string, FAngle );
        PROPERTY( T_string, FWeight );
        PROPERTY( T_string, FVar );

        // font size and color
        PROPERTY( T_dimension, FSize );
        PROPERTY( T_tagstring, FColor );
                
        // font style
        PROPERTY( T_keyword, FUnderlining );
        PROPERTY( T_boolean, FOverline );
        PROPERTY( T_boolean, FStrike );
        PROPERTY( T_keyword, FPosition );
        PROPERTY( T_boolean, FPairKern );
        PROPERTY( T_keyword, FCase );

        // kerning information
        PROPERTY( T_percentage, FDX );
        PROPERTY( T_percentage, FDY );
        PROPERTY( T_percentage, FDW );

        void setDSSSLDefaults() {
        
            setFFamily( sTimesNewRoman );
            setFAngle( sRegular );
            setFWeight( sRegular );
            setFVar( sRegular );
            setFSize( 10000 );
            setFColor( sBlack ),
            setFUnderlining( sFNoUnderlining );
            setFOverline( false );
            setFStrike( false );
            setFPosition( sFNormal );
            setFPairKern( false );
            setFCase( sFAsTyped );
            setFDX( 0 );
            setFDY( 0 );
            setFDW( 0 );
        }

        enum FontStatement { stPgfFont, stFont };

        unsigned compare( FontFormat &f );
        void setFrom( FontFormat &f, unsigned properties );
        void out( MifOutputByteStream &os, unsigned properties, FontStatement fontStatement );
        void ffOut( MifOutputByteStream &os, unsigned properties, FontStatement fontStatement )
              { out( os, properties, fontStatement ); }
        void out( MifOutputByteStream &os, FontStatement fontStatement )
            { out( os, setProperties, fontStatement ); }
        void ffOut( MifOutputByteStream &os, FontStatement fontStatement )
            { out( os, fontStatement ); }
        void updateFrom( FontFormat & );
        void ffUpdateFrom( FontFormat &f ) { updateFrom( f ); }
        void clearSetProperties() { setProperties = 0; }
    };

    static const String<char> sNONE;

    // paragraph formats
    static const String<char> sDefaultPgfFormat;
    static const String<char> sHeader;
    static const String<char> sFooter;

    // PgfAlignment
    static const String<char> sLeftRight;
    static const String<char> sLeft;
    static const String<char> sRight;
    static const String<char> sCenter;

    // PgfLineSpacing
    static const String<char> sFixed;
    static const String<char> sProportional;

    // PgfCellAlignment
    static const String<char> sTop;
    static const String<char> sMiddle;
    static const String<char> sBottom;

    // PgfPlacement
    static const String<char> sAnywhere;
    static const String<char> sColumnTop;
    static const String<char> sPageTop;
    static const String<char> sLPageTop;
    static const String<char> sRPageTop;

    // PgfPlacementStyle
    static const String<char> sNormal;
    static const String<char> sStraddle;

    // PgfLanguage
    static const String<char> sNoLanguage;
    static const String<char> sUSEnglish;
    static const String<char> sUKEnglish;
    static const String<char> sGerman;
    static const String<char> sSwissGerman;
    static const String<char> sFrench;
    static const String<char> sCanadianFrench;
    static const String<char> sSpanish;
    static const String<char> sCatalan;
    static const String<char> sItalian;
    static const String<char> sPortuguese;
    static const String<char> sBrazilian;
    static const String<char> sDanish;
    static const String<char> sDutch;
    static const String<char> sNorwegian;
    static const String<char> sNynorsk;
    static const String<char> sFinnish;
    static const String<char> sSwedish;


    // TStype
    // sLeft;
    // sRight;
    // sCenter;
    static const String<char> sDecimal;

    // TSLeaderStr;
    static const String<char> sSPACE;

    struct TabStop {

        enum { fNone = 0, fTSX = 0x1, fTSType = 0x2, fTSLeaderStr = 0x4,
               fTSDecimalChar = 0x8, fAll = 0xF };
        unsigned setProperties;

        TabStop( T_keyword type = sLeft, T_dimension x = 0, T_string tSLeaderStr = sSPACE )
         : TSX( x ), TSType( type ), setProperties( fTSType | fTSX )
          { setTSLeaderStr( tSLeaderStr ); }

        PROPERTY( T_dimension, TSX );
        PROPERTY( T_keyword, TSType );
        PROPERTY( T_string, TSLeaderStr );
        PROPERTY( T_integer, TSDecimalChar );

        void out( MifOutputByteStream &os );
    };

    struct ParagraphFormat : public FontFormat {
    
		enum { fNone = 0, fPgfFIndent = 0x1, fPgfLIndent = 0x2, fPgfRIndent = 0x4,
               fPgfAlignment = 0x8, fPgfSpBefore = 0x10, fPgfSpAfter = 0x20,
               fPgfLineSpacing = 0x40, fPgfLeading = 0x80, fPgfWithPrev = 0x100,
               fPgfWithNext = 0x200, fPgfBlockSize = 0x400, fPgfAutoNum = 0x800,
               fPgfNumFormat = 0x1000, fPgfNumberFont = 0x2000, fPgfHyphenate = 0x4000,
               fHyphenMaxLines = 0x8000, fHyphenMinPrefix = 0x10000,
               fHyphenMinSuffix = 0x20000, fHyphenMinWord = 0x40000,
               fPgfLetterSpace = 0x80000, fPgfLanguage = 0x100000,
               fPgfCellAlignment = 0x200000, fPgfCellMargins = 0x400000,
               fPgfCellLMarginFixed = 0x800000, fPgfCellTMarginFixed = 0x1000000,
               fPgfCellRMarginFixed = 0x2000000,  fPgfCellBMarginFixed = 0x4000000,
               fPgfTag = 0x8000000, fTabStops = 0x10000000, fPgfPlacement = 0x20000000,
               fPgfNumTabs = 0x40000000, fPgfPlacementStyle = 0x80000000,
               fAll = 0xFFFFFFFF };
        unsigned setProperties;

        ParagraphFormat() : setProperties( 0 ) {}

        // basic properties
        PROPERTY( T_tagstring, PgfTag );
        PROPERTY( T_dimension, PgfFIndent );
        PROPERTY( T_dimension, PgfLIndent );
        PROPERTY( T_dimension, PgfRIndent );
        PROPERTY( T_keyword, PgfAlignment );
        PROPERTY( T_dimension, PgfSpBefore );
        PROPERTY( T_dimension, PgfSpAfter );
        PROPERTY( T_keyword, PgfLineSpacing );
        PROPERTY( T_dimension, PgfLeading );
        PROPERTY( T_integer, PgfNumTabs );
        Vector<TabStop> TabStops;

        // default font properties

        // pagination properties
        PROPERTY( T_keyword, PgfPlacement );
        PROPERTY( T_keyword, PgfPlacementStyle );
        PROPERTY( T_boolean, PgfWithPrev );
        PROPERTY( T_boolean, PgfWithNext );
        PROPERTY( T_integer, PgfBlockSize );

        // numbering properties
        PROPERTY( T_boolean, PgfAutoNum );
        PROPERTY( T_string, PgfNumFormat );
        PROPERTY( T_tagstring, PgfNumberFont );

        // advanced properties
        PROPERTY( T_boolean, PgfHyphenate );
        PROPERTY( T_integer, HyphenMaxLines );
        PROPERTY( T_integer, HyphenMinPrefix );
        PROPERTY( T_integer, HyphenMinSuffix );
        PROPERTY( T_integer, HyphenMinWord );
        PROPERTY( T_boolean, PgfLetterSpace );
        PROPERTY( T_keyword, PgfLanguage );
            
        // table cell properties
        PROPERTY( T_keyword, PgfCellAlignment );
        PROPERTY( T_LTRB, PgfCellMargins );
        PROPERTY( T_boolean, PgfCellLMarginFixed );
        PROPERTY( T_boolean, PgfCellTMarginFixed );
        PROPERTY( T_boolean, PgfCellRMarginFixed );
        PROPERTY( T_boolean, PgfCellBMarginFixed );

        // miscellaneous properties
        // none

        void setDSSSLDefaults() {
        
            FontFormat::setDSSSLDefaults();
            setPgfFIndent( 0 );
            setPgfLIndent( 0 );
            setPgfRIndent( 0 );
            setPgfAlignment( sLeft );
            setPgfSpBefore( 0 );
            setPgfSpAfter( 0 );
            setPgfLineSpacing( sFixed );
            setPgfLeading( 0 );
            setPgfNumTabs( 0 );
            setPgfWithPrev( false );
            setPgfWithNext( false );
            setPgfBlockSize( 2 );
            setPgfAutoNum( false );
            // setPgfNumFormat( sNONE );
            // setPgfNumberFont( sNONE );
            setPgfHyphenate( false );
            setHyphenMaxLines( 999 );
            setHyphenMinPrefix( 2 );
            setHyphenMinSuffix( 2 );
            setHyphenMinWord( 2 );
            setPgfLetterSpace( false );
            setPgfLanguage( sNoLanguage );
            setPgfCellAlignment( sTop );
            setPgfCellMargins( T_LTRB( 0, 0, 0, 0 ) );
            setPgfCellLMarginFixed( true );
            setPgfCellTMarginFixed( true );
            setPgfCellRMarginFixed( true );
            setPgfCellBMarginFixed( true );
            // setPgfTag( sNONE );
            setPgfPlacement( sAnywhere );
            setPgfPlacementStyle( sNormal );
        }

        void forceSetProperties( unsigned properties, unsigned fontProperties )
            { setProperties = properties; FontFormat::setProperties = fontProperties; }
        unsigned &ffSetProperties() { return FontFormat::setProperties; }
        unsigned compare( ParagraphFormat & );
        void setFrom( ParagraphFormat &, unsigned, unsigned );
        void out( MifOutputByteStream &os, unsigned, unsigned,
                  bool excludeCellProperties = true );
        void out( MifOutputByteStream &os, bool excludeCellProperties = true )
            { out( os, setProperties, (unsigned)FontFormat::setProperties,
                   excludeCellProperties ); }
        void updateFrom( ParagraphFormat & );
        void clearSetProperties() { setProperties = 0; FontFormat::clearSetProperties(); }
    };

    // DPageNumStyle
    static const String<char> sArabic;
    static const String<char> sUCRoman;
    static const String<char> sLCRoman;
    static const String<char> sUCAlpha;
    static const String<char> sLCAlpha;

    // DParity
    static const String<char> sFirstLeft;
    static const String<char> sFirstRight;

    struct Document {

        enum { fNone = 0, fDPageSize = 0x1, fDStartPage = 0x2, fDPageNumStyle = 0x4,
               fDTwoSides = 0x8, fDParity = 0x10, fDMargins = 0x20, fDColumns = 0x40,
               fDColumnGap = 0x80, fAll = 0xFF };
        unsigned setProperties;

        Document() : setProperties( fNone ) {}

        // column properties
        PROPERTY( T_LTRB, DMargins );
        PROPERTY( T_integer, DColumns );
        PROPERTY( T_dimension, DColumnGap );
        PROPERTY( T_WH, DPageSize );

        // pagination
        PROPERTY( T_integer, DStartPage );
        PROPERTY( T_keyword, DPageNumStyle );
        PROPERTY( T_boolean, DTwoSides );
        PROPERTY( T_keyword, DParity );

        void out( MifOutputByteStream &os );
    };

    struct Object {

        enum { fNone = 0, fID = 0x1, fPen = 0x2, fFill = 0x4, fPenWidth = 0x8,
               fObjectNext = 0x10, fObColor = 0x20 };
        unsigned setProperties;

        Object( T_integer pen = 15, T_integer fill = 15, T_dimension penWidth = 0,
                const T_tagstring &obColor = sBlack )
         : setProperties( 0 )
         { setID( ++IDCnt );
           setPen( pen ); setFill( fill ); setPenWidth( penWidth ); setObColor( obColor ); }

        static T_ID IDCnt;
        PROPERTY( T_integer, ID );
        PROPERTY( T_integer, Pen );
        PROPERTY( T_integer, Fill );
        PROPERTY( T_dimension, PenWidth );
        PROPERTY( T_tagstring, ObColor );

        void outObjectProperties( MifOutputByteStream &os );
        virtual void out( MifOutputByteStream &os ) = 0;
    };

    // HeadCap/TailCap
    static const String<char> sButt;
    static const String<char> sRound;
    static const String<char> sSquare;
    
    struct PolyLine : public Object {

        enum { fHeadCap = fObjectNext << 1, fTailCap = fHeadCap << 1 };

        PolyLine( T_keyword cap, T_integer pen = 15, T_integer fill = 15,
                  T_dimension penWidth = 0,
                  const T_tagstring &obColor = sBlack )
         : Object( pen, fill, penWidth, obColor ) { setHeadCap( cap ); setTailCap( cap ); }

        PROPERTY( T_keyword, HeadCap );
        PROPERTY( T_keyword, TailCap );
        Vector<T_XY> Points;

        void out( MifOutputByteStream &os );
    };
    
    struct ImportObject : public Object {

        enum { fImportObFileDI = fObjectNext << 1, fBitMapDpi = fImportObFileDI << 1,
               fShapeRect = fBitMapDpi << 1, fImportObFixedSize = fShapeRect << 1,
               fNativeOrigin = fImportObFixedSize << 1 };

        ImportObject( T_pathname importObFileDI, T_LTWH shapeRect,
                      T_boolean importObFixedSize = true, T_integer bitMapDpi = 72 ) : Object()
         { setImportObFileDI( importObFileDI ); setShapeRect( shapeRect );
           setImportObFixedSize( importObFixedSize ); setBitMapDpi( bitMapDpi ); }

        PROPERTY( T_pathname, ImportObFileDI );
        PROPERTY( T_integer, BitMapDpi );
        PROPERTY( T_LTWH, ShapeRect );
        PROPERTY( T_boolean, ImportObFixedSize );
        PROPERTY( T_XY, NativeOrigin );

        void out( MifOutputByteStream &os );
    };

    // FrameType
    static const String<char> sInline;
    static const String<char> sRunIntoParagraph;
    static const String<char> sBelow;

    // AnchorAlign
    // Left
    // Center
    // Right
    // Inside
    // Outside

    struct Frame : public Object {

        enum { fShapeRect = fObjectNext << 1, fFrameType = fShapeRect << 1, 
               fNSOffset = fFrameType << 1, fBLOffset = fNSOffset << 1, 
               fAnchorAlign = fBLOffset << 1 };

        Frame() : Object() {}

        PROPERTY( T_LTWH, ShapeRect );
        PROPERTY( T_keyword, FrameType );
        PROPERTY( T_dimension, NSOffset );
        PROPERTY( T_dimension, BLOffset );
        PROPERTY( T_keyword, AnchorAlign );
        Vector<Object*> Objects;

        void out( MifOutputByteStream &os );
    };

    struct TextRect : public Object {

        enum { fID = fObjectNext << 1, fShapeRect = fID << 1, fTRNumColumns = fShapeRect << 1,
               fTRColumnGap = fTRNumColumns << 1, fTRColumnBalance = fTRColumnGap << 1  };

        TextRect() : Object()  {}
        TextRect( T_LTWH shapeRect, T_integer tRNumColumns = 1, T_dimension tRColumnGap = 0,
                  bool tRColumnBalance = false )
         : Object() { setShapeRect( shapeRect ); setTRNumColumns( tRNumColumns );
                      setTRColumnGap( tRColumnGap ); setTRColumnBalance( tRColumnBalance ); }

        PROPERTY( T_LTWH, ShapeRect );
        PROPERTY( T_integer, TRNumColumns );
        PROPERTY( T_dimension, TRColumnGap );
        PROPERTY( T_boolean, TRColumnBalance );

        void out( MifOutputByteStream &os );
    };

    // PageType
    static const String<char> sLeftMasterPage;
    static const String<char> sRightMasterPage;
    static const String<char> sOtherMasterPage;
    static const String<char> sBodyPage;

    // PageTag
    static const String<char> sFirst;
    // sLeft
    // sRight

    struct Page {

        enum { fNone = 0, fPageType = 0x1, fPageTag = 0x2, fPageBackground = 0x4 };
        unsigned setProperties;

        Page( T_keyword pageType, T_tagstring pageTag = sNONE,
              T_tagstring pageBackground = sNONE )
         : PageType( pageType ), PageTag( pageTag ), PageBackground( pageBackground ),
           setProperties( fPageType | fPageTag )
         { if( PageTag != sNONE ) setProperties |= fPageTag;
           if( PageBackground != sNONE ) setProperties |= fPageBackground; }
        Page() : PageType( MifDoc::sRightMasterPage ), setProperties( fPageType ) {}

        PROPERTY( T_keyword, PageType );
        PROPERTY( T_tagstring, PageTag );
        PROPERTY( T_tagstring, PageBackground ); // keyword in MIF
        Vector<TextRect> TextRects;

        void out( MifOutputByteStream &os );
    };

    struct TagStream {

        enum TagStreamClass { tsTagStream, tsTextFlow, tsCell, tsPara } TagStreamClass;

        TagStream( T_indent osIndent = 2 );
        ~TagStream();

        T_tagstring InitialPgfTag;
        bool PgfTagUsed;

        ParagraphFormat &format() { return Format; }
        MifTmpOutputByteStream &content() { assert( Content != NULL ); return *Content; }
        void setParagraphFormat( ParagraphFormat &pf ) { Format = pf; }

      protected:
        MifTmpOutputByteStream *Content;               
        ParagraphFormat Format;
    };

    struct TextFlow : public TagStream {

        enum { fNone = 0, fTFTag = 0x1, fTFAutoConnect = 0x02 };
        unsigned setProperties;

        TextFlow() : setProperties( fNone ), TextRectIDUsed( false )
         { TagStreamClass = tsTextFlow; }
        TextFlow( const TextRect &textRect, bool body,
                  ParagraphFormat *format = NULL, T_tagstring pgfTag = sNONE );

        T_ID TextRectID; // for ParaLine (not in MIF)
        bool TextRectIDUsed;
        PROPERTY( T_tagstring, TFTag );
        PROPERTY( T_boolean, TFAutoConnect );

        const TextRect *textRect() { assert( TextRect_ != NULL ); return TextRect_; }
        void out( MifOutputByteStream &os, bool resolveCrossReferences = false );

      protected:
        const TextRect *TextRect_;
    };

    struct Para : public TagStream {

        enum { fNone = 0, fPgfTag = 0x1, fParagraphFormat = 0x2 };
        unsigned setProperties;

        Para( T_indent osIndent = 2 )
         : TagStream( osIndent ), setProperties( fParagraphFormat ), leaderTabsSet( false )
          { TagStreamClass = tsPara; }

        static bool currentlyOpened;
        bool leaderTabsSet;
        PROPERTY( T_tagstring, PgfTag );

        ParagraphFormat &curFormat() { return CurFormat; }

        void out( MifOutputByteStream &os );
        void outProlog( MifOutputByteStream &os );

        static void outSimpleProlog( MifOutputByteStream &os );
        static void outEpilog( MifOutputByteStream &os );

      protected:
        ParagraphFormat CurFormat;

    };

    struct ParaLine {

        enum { fNone = 0, fTextRectID = 0x1, fATbl = 0x2 };
        static unsigned setProperties;

        STATIC_PROPERTY( T_ID, TextRectID );
        STATIC_PROPERTY( T_ID, ATbl );

        static void outProlog( MifOutputByteStream &os );
        static void outEpilog( MifOutputByteStream &os );
    };

    struct PgfCatalog {

        Vector<ParagraphFormat> ParaFormats;
        void out( MifOutputByteStream &os );
    };

    struct Color {

        enum { fColorTag = 0x1, fColorCyan = 0x2, fColorMagenta = 0x4, fColorYellow = 0x8,
               fColorBlack = 0x10 };
        unsigned setProperties;

        Color() : setProperties( 0 ) {}
        Color( unsigned char red, unsigned char green, unsigned char blue );
  
        PROPERTY( T_tagstring, ColorTag );
        PROPERTY( T_percentage, ColorCyan );
        PROPERTY( T_percentage, ColorMagenta );
        PROPERTY( T_percentage, ColorYellow );
        PROPERTY( T_percentage, ColorBlack );

        static const String<char> &key( Color &color );
        void out( MifOutputByteStream &os ) const;
    };

    struct ColorCatalog {

        PointerTable<Color*,String<char>,StringHash,Color> Colors;
        void out( MifOutputByteStream &os );
    };

    // TblAlignment
    // Left
    // Center
    // Right
    static const String<char> sInside;
    static const String<char> sOutside;

    struct Ruling {

        enum { fNone = 0, fRulingTag = 0x1, fRulingPenWidth = 0x2, fRulingGap = 0x4,
               fRulingPen = 0x8, fRulingLines = 0x10, fRulingColor = 0x20 };
        unsigned setProperties;

        Ruling() : setProperties( 0 ), RulingPenWidth( 1000 ), RulingGap( 1000 ),
                   RulingPen( 0 ), RulingLines( 1 ) {};
        Ruling( T_tagstring RulingTag_ ) 
         : setProperties( 0 ), RulingPenWidth( 1000 ), RulingGap( 1000 ),
           RulingPen( 0 ), RulingLines( 1 )
        { setRulingTag( RulingTag_ ); }

        PROPERTY( T_tagstring, RulingTag );
        PROPERTY( T_dimension, RulingPenWidth );
        PROPERTY( T_dimension, RulingGap );
        PROPERTY( T_tagstring, RulingColor );
        PROPERTY( T_integer, RulingPen );
        PROPERTY( T_integer, RulingLines );

        static const String<char> &key( Ruling & );
        void out( MifOutputByteStream &os ) const;
        String<char> Key;        
      
    };

    struct RulingCatalog {

        PointerTable<Ruling*,String<char>,StringHash,Ruling> Rulings;
        void out( MifOutputByteStream &os );
    };

    struct Cell : public TagStream {

        enum { fNone = 0, fCellLRuling = 0x1, fCellBRuling = 0x2, fCellRRuling = 0x4,
               fCellTRuling = 0x8, fCellColumns = 0x10, fCellRows = 0x20,
               fCellFill = 0x40, fCellColor = 0x80 };
        unsigned setProperties;

        Cell( T_indent osIndent = 12 )
         : TagStream( osIndent ), setProperties( 0 ) { TagStreamClass = tsCell; }

        PROPERTY( T_integer, CellFill );
        PROPERTY( T_tagstring, CellColor );
        PROPERTY( T_tagstring, CellLRuling );
        PROPERTY( T_tagstring, CellBRuling );
        PROPERTY( T_tagstring, CellRRuling );
        PROPERTY( T_tagstring, CellTRuling );
        PROPERTY( T_integer, CellColumns );
        PROPERTY( T_integer, CellRows );

        void out( MifOutputByteStream &os, bool resolveCrossReferences = false );
    };

    struct Row {

        Vector<Cell> Cells;

        void out( MifOutputByteStream &os, bool resolveCrossReferences = false );
    };

    struct TblColumn {

        enum { fNone = 0, fTblColumnNum = 0x1, fTblColumnWidth = 0x2 };
        unsigned setProperties;

        TblColumn() : setProperties( 0 ), TblColumnNum( 0 ), TblColumnWidth( 0 ) {};
        TblColumn( T_integer TblColumnNum_, T_dimension TblColumnWidth_ )
         : setProperties( 0 )
         { setTblColumnNum( TblColumnNum_ ); setTblColumnWidth( TblColumnWidth_ ); }

        PROPERTY( T_integer, TblColumnNum );
        PROPERTY( T_dimension, TblColumnWidth );

        void out( MifOutputByteStream &os );
    };

    // TblTitlePlacement
    static const String<char> sNone;

    struct TblFormat {

        enum { fNone = 0, fTblTag = 0x1, fTblLIndent = 0x2, fTblRIndent = 0x4,
               fTblSpBefore = 0x8, fTblSpAfter = 0x10, fTblAlignment = 0x20,
               fTblCellMargins = 0x40, fTblWidth = 0x80, fTblTitlePlacement = 0x100,
               fAll = 0x1ff };
        unsigned setProperties;

        TblFormat() : setProperties( 0 ) {}
        TblFormat( T_tagstring TblTag_ ) : setProperties( 0 ) { setTblTag( TblTag_ ); }

        void setDSSSLDefaults() {
        
            setTblLIndent( 0 );
            setTblRIndent( 0 );
            setTblSpBefore( 0 );
            setTblSpAfter( 0 );
            setTblAlignment( sLeft );
            setTblCellMargins( T_LTRB( 0, 0, 0, 0 ) );
            setTblTitlePlacement( sNone );
        }

        // basic properties
        PROPERTY( T_tagstring, TblTag );
        PROPERTY( T_dimension, TblLIndent );
        PROPERTY( T_dimension, TblRIndent );
        PROPERTY( T_dimension, TblSpBefore );
        PROPERTY( T_dimension, TblSpAfter );
        PROPERTY( T_keyword, TblAlignment );
        PROPERTY( T_LTRB, TblCellMargins );
        PROPERTY( T_keyword, TblTitlePlacement );

        // column properties
        PROPERTY( T_dimension, TblWidth );
        Vector<TblColumn> TblColumns;

        void out( MifOutputByteStream &os );
    };

    struct TblCatalog {

        Vector<TblFormat> TblFormats;
        void out( MifOutputByteStream &os );
    };

    // TblTag
    static const String<char> sDefaultTblFormat;

    struct Tbl {

        enum { fNone = 0, fTblID = 0x1, fTblTag = 0x2, fTblNumColumns = 0x4,
               fTblColumnWidths = 0x8, fTblFormat = 0x10 };
        unsigned setProperties;

        Tbl() : setProperties( 0 ), TblIDUsed( false )
          { setTblID( MifDoc::CurInstance->nextID() ); setTblTag( MifDoc::sDefaultTblFormat ); }

        PROPERTY( T_ID, TblID );
        PROPERTY( T_tagstring, TblTag );
        bool TblIDUsed;
        TblFormat tblFormat;

        // table columns
        PROPERTY( T_integer, TblNumColumns );
        VECTOR_OF_PROPERTIES( T_dimension, TblColumnWidth );

        // table rows
        Vector<Row> TblH;
        Vector<Row> TblBody;
        Vector<Row> TblF;

        void out( MifOutputByteStream &os, bool resolveCrossReferences = false );
    };

    // paragraph formats
    static const String<char> sPageNumXRefFormatName;

    struct XRefFormat {

        enum { fXRefName = 0x1, fXRefDef = 0x2 };
        unsigned setProperties;

        XRefFormat() {}
        XRefFormat( T_string XRefName_, T_string XRefDef_ )
         { setXRefName( XRefName_ ); setXRefDef( XRefDef_ ); }

        PROPERTY( T_string, XRefName );
        PROPERTY( T_string, XRefDef );

        void out( MifOutputByteStream &os );
    };

    static char escapeChar() { return '\x0'; }

    struct CrossRefInfo {

        enum InfoType { PotentialMarker, XRef, HypertextLink } Type;
        CrossRefInfo() {}
        CrossRefInfo
         ( unsigned long groveIndex_, unsigned long elementIndex_, int tagIndent_,
           InfoType type_, const Char *id, size_t idLen )
          : groveIndex( groveIndex_ ), elementIndex( elementIndex_ ), tagIndent( tagIndent_ ),
            Type( type_ ) { if( id != NULL && idLen > 0 ) sgmlId.assign( id, idLen ); }
        InfoType type() { return Type; }
        void out( MifOutputByteStream &os );
        T_string crossRefText() const;
        
        unsigned long groveIndex;
        unsigned long elementIndex;
        int tagIndent;
        StringC sgmlId;
    };

    struct XRef {

        enum { fXRefName = 0x1, fXRefSrcFile = 0x2, fXRefText = 0x4, fXRefSrcText };
        unsigned setProperties;

        XRef() {}
        XRef( CrossRefInfo &crossRefInfo );
        XRef( T_tagstring XRefName_, T_string XRefSrcText_, T_string XRefText_,
              T_pathname XRefSrcFile_ )
         { setXRefName( XRefName_ ); setXRefSrcText( XRefSrcText_ ); setXRefText( XRefText_ );
           setXRefSrcFile( XRefSrcFile_ ); }

        PROPERTY( T_tagstring, XRefName );
        PROPERTY( T_string, XRefSrcText );
        PROPERTY( T_pathname, XRefSrcFile );
        PROPERTY( T_string, XRefText );

        void out( MifOutputByteStream &os );
    };

    struct Marker {

        enum Type { XRef = 9, Index = 2, Hypertext = 8, HypertextLink = 100,
                    HypertextDestination = 101 };
        enum { fMType = 0x1, fMText = 0x2 };
        unsigned setProperties;

        Marker() {}
        Marker( CrossRefInfo &crossRefInfo, bool linkDestinationMode = false );
        Marker( T_string MText_, Type MType_ = XRef )
         { setMText( MText_ ); setMType( T_integer( MType_ ) ); }

        PROPERTY( T_integer, MType );
        PROPERTY( T_string, MText );

        void out( MifOutputByteStream &os );
    };

    struct BookComponent {
    
        BookComponent() : epilogOs( NULL ), pageNumXRefFormatGenerated( false ) {}
        BookComponent( MifTmpOutputByteStream *epilogOs );
        void commit( const String<CmdLineApp::AppChar> &dirName,
                     bool resolveCrossReferences = true );
        
        String<CmdLineApp::AppChar> FileName;
        Vector<XRefFormat> XRefFormats;
        Document document;
        ColorCatalog colorCatalog;
        PgfCatalog pgfCatalog;
        TblCatalog tblCatalog;
        Vector<Frame> AFrames;
        Vector<Tbl> Tbls;
        Vector<Page> Pages;
        Vector<TextFlow> TextFlows;
        RulingCatalog rulingCatalog;

        bool pageNumXRefFormatGenerated;

        MifTmpOutputByteStream *epilogOs;             
    };

    struct ElementSet {

        enum ReferenceType
         { AnyReference = 0xC000, LinkReference = 0x8000, PageReference = 0x4000 };
        static unsigned short LINK_TYPE_MASK( ReferenceType refType )
         { return (unsigned short)( refType ); }
        static const unsigned short BOOK_COMPONENT_INDEX_M() { return 0x3FFF; }
        ElementSet();

        struct SgmlIdInfo {
            SgmlIdInfo( const StringC &sgmlId_, unsigned long groveIndex_ )
             : sgmlId( sgmlId_ ), groveIndex( groveIndex_ ), flags( 0 ) {}
            StringC sgmlId;
            unsigned short flags;
            unsigned long groveIndex;
            static const StringC &key( SgmlIdInfo &sgmlIdInfo)
             { return sgmlIdInfo.sgmlId; }
        };

        SgmlIdInfo &enforceSgmlId( const StringC &sgmlId, unsigned long groveIndex );
        void setReferencedFlag( ReferenceType, unsigned long groveIndex, unsigned long n );
        void setReferencedFlag( ReferenceType, unsigned long groveIndex, const StringC &sgmlId );
        void setBookComponentIndex( unsigned long groveIndex, unsigned long n, int i );
        void setBookComponentIndex( unsigned long groveIndex, const StringC &sgmlId, int i );
        bool hasBeenReferenced( ReferenceType, unsigned long groveIndex, unsigned long n ) const;
        bool hasBeenReferenced( ReferenceType, unsigned long groveIndex, const StringC &sgmlId );
        int bookComponentIndex( unsigned long groveIndex, unsigned long n ) const;
        int bookComponentIndex( unsigned long groveIndex, const StringC &sgmlId );

      private:
        PointerTable<SgmlIdInfo*,StringC,Hash,SgmlIdInfo> SgmlIdInfos;
        void add( unsigned long groveIndex, unsigned long n, unsigned short flags );
        bool getFlags( unsigned long groveIndex, unsigned long n, unsigned short &result ) const;
        Vector<Vector<unsigned short> > Flags;
    };

    void commit();

    void outTagEnd();

    void outHexChar( unsigned code, MifOutputByteStream *o = NULL );
    void outHexChar( unsigned code, String<char> &targetString );
    void outSpecialChar( const String<char> &charName, MifOutputByteStream *o = NULL ); 
    void beginParaLine();
    void endParaLine();
    void outPageNumber();
    void outBreakingPara( T_keyword pgfPlacement );
    void outAFrame( T_ID ID, MifOutputByteStream &os );

    Vector<BookComponent> &bookComponents() { return BookComponents; };
    BookComponent &bookComponent()
     { assert( bookComponents().size() > 0 ); return bookComponents().back(); }

    void setCurOs( MifOutputByteStream &os ) { curOs = &os; } 
    MifOutputByteStream &os() { assert( curOs != NULL ); return *curOs; }

    void setCurTextFlow( TextFlow *tf ) { CurTextFlow = tf; } 
    TextFlow *curTextFlow() { return CurTextFlow; }

    void setCurCell( Cell *c ) { CurCell = c; } 
    Cell *curCell() { return CurCell; }

    void setCurTblNum( size_t n ) { CurTblNum = n; } 
    Tbl *curTbl( bool assertNotNull = true )
     { if( assertNotNull ) assert( CurTblNum > 0 );
       return CurTblNum > 0 ? &bookComponent().Tbls[CurTblNum-1] : (Tbl *)NULL; }

    void setCurPara( Para *p ) {  CurPara = p; } 
    Para *curPara( bool assertNotNull = true )
     { if( assertNotNull ) assert( CurPara != NULL ); return CurPara; }

    void setCurParagraphFormat( ParagraphFormat &pf ) { CurFormat = pf; } 
    ParagraphFormat &curFormat() { return CurFormat; }

    TagStream &curTagStream()
      { assert( TagStreamStack.size() > 0 ); return *TagStreamStack.back(); }

    void enterTextFlow( TextFlow &textFlow  ) { enterTagStream( textFlow ); }
    void exitTextFlow() { setCurTextFlow( NULL ); exitTagStream(); }

    void enterTableCell( Cell &cell ) { enterTagStream( cell ); }
    void exitTableCell() { setCurCell( NULL ); exitTagStream(); }

    void enterPara( Para &p ) { enterTagStream( p ); }
    void exitPara() { setCurPara( NULL ); exitTagStream(); }

    void enterBookComponent();
    void exitBookComponent();
    void enterTagStream( TagStream &tagStream );
    void exitTagStream();
    T_ID nextID() { return ++NextID; }

    Document &document() { return bookComponent().document; }
    PgfCatalog &pgfCatalog() { return bookComponent().pgfCatalog; }
    TblCatalog &tblCatalog() { return bookComponent().tblCatalog; }
    RulingCatalog &rulingCatalog() { return bookComponent().rulingCatalog; }
    ColorCatalog &colorCatalog() { return bookComponent().colorCatalog; }
    Vector<Tbl> &tbls() { return bookComponent().Tbls; }
    Vector<Page> &pages() { return bookComponent().Pages; }
    Vector<TextFlow> &textFlows() { return bookComponent().TextFlows; }
    Vector<Frame> &aFrames() { return bookComponent().AFrames; }
    ElementSet &elements() { return Elements; }
    Vector<CrossRefInfo> &crossRefInfos() { return CrossRefInfos; }

    const String<CmdLineApp::AppChar> &rootOutputFileLoc() { return RootOutputFileLoc; }

    CmdLineApp *App;

  protected:

    Vector<CrossRefInfo> CrossRefInfos;
    ElementSet Elements;
    void switchToTagStream( TagStream &tagStream, bool startWithDefaultPgfFormat = true );

    String<CmdLineApp::AppChar> RootOutputFileLoc;
    T_ID NextID;

    MifOutputByteStream *curOs;    

    Vector<BookComponent> BookComponents;

    size_t CurTblNum;
    ParagraphFormat CurFormat;
    TextFlow *CurTextFlow;
    Cell *CurCell;
    Para *CurPara;

    Vector<TagStream*> TagStreamStack;
};

class MifOutputByteStream {

    OutputByteStream *os;

  public:
    
    int CurTagIndent;
    static MifDoc::T_indent INDENT;

    void indent() { CurTagIndent += 2; }
    void undent() { CurTagIndent -= 2; assert( CurTagIndent >= 0 ); }
    MifOutputByteStream( MifDoc::T_indent i = 0 )
     : os( NULL ), CurTagIndent( i ) {};
    MifOutputByteStream( OutputByteStream &os, MifDoc::T_indent i = 0 )
     : os( &os ), CurTagIndent( i ) {};
    void setStream( OutputByteStream &s ) { os = &s; }
    OutputByteStream &stream() { assert( os != NULL ); return *os; }
    MifOutputByteStream &operator<<( unsigned n ) { stream() << n; return *this; }
    MifOutputByteStream &operator<<( long n ) { stream() << n; return *this; }
    MifOutputByteStream &operator<<( char c ) { stream() << c; return *this; }
    MifOutputByteStream &operator<<( const char *s ) { stream() << s; return *this; }
#ifdef SP_WIDE_SYSTEM
    MifOutputByteStream &operator<<( const String<CmdLineApp::AppChar> &s )
     { for( size_t i = 0; i < s.size(); i++ ) stream() << char( s[i] ); return *this; }
#endif
    MifOutputByteStream &operator<<( const String<char> &s ) { stream() << s; return *this; }
    MifOutputByteStream &operator<<( const MifDoc::T_string &s )
     { stream() << '`'; stream() << s; stream() << '\''; return *this; }
    MifOutputByteStream &operator<<( const MifDoc::T_percentage &p ) {
        char buf[32];
        sprintf( buf, "%.6f", p );
        stream() << buf;
        return *this;
    }
    MifOutputByteStream &operator<<( const MifDoc::T_dimension &d ) {
        char buf[32];
        int i;
        sprintf( buf, "%li.%.3i%n", long(d)/1000, abs(long(d)%1000), &i );
        while( buf[--i] == '0' ) {}; if( buf[i] == '.' ) i--;
        buf[i+1] = '\0';
        stream() << buf << "pt";
        return *this;
    }
    MifOutputByteStream &operator<<( const MifDoc::CrossRefInfo &cri ) {
        Vector<MifDoc::CrossRefInfo> &crossRefInfos = MifDoc::CurInstance->crossRefInfos();
        unsigned idx = crossRefInfos.size();
        crossRefInfos.resize( crossRefInfos.size() + 1 );
        crossRefInfos.back() = cri;
        stream() << MifDoc::escapeChar();
        stream().sputn( (char*)&idx, sizeof( idx ) );
        return *this;
    }
    MifOutputByteStream &operator<<( const MifDoc::T_LTRB &b )
     { *this << b.l << " " << b.t << " " << b.r << " " << b.b; return *this; }
    MifOutputByteStream &operator<<( const MifDoc::T_LTWH &b )
     { *this << b.l << " " << b.t << " " << b.w << " " << b.h; return *this; }
    MifOutputByteStream &operator<<( const MifDoc::T_WH &s )
     { *this << s.w << " " << s.h; return *this; }
    MifOutputByteStream &operator<<( const MifDoc::T_XY &s )
     { *this << s.x << " " << s.y; return *this; }
    MifOutputByteStream &operator<<( const MifDoc::T_boolean &b )
     { if( bool(b) ) *this << "Yes"; else *this << "No"; return *this; }
    MifOutputByteStream &operator<<( const MifDoc::T_indent &i )
     { unsigned cnt = (i==INDENT) ? CurTagIndent : unsigned( i );
       for( ; cnt; cnt-- ) stream() << ' '; return *this; }
};

class MifTmpOutputByteStream : public TmpOutputByteStream {

    MifOutputByteStream os;    

  public:
    MifTmpOutputByteStream( MifDoc::T_indent osIndent = 0 )
     : TmpOutputByteStream(), os( osIndent ) { os.setStream( *this ); }
    void commit( OutputByteStream &os, bool resolveCrossReferences = false );
    void commit( String<char> &str );
    MifOutputByteStream  &stream() { return os; }
};

class MifFOTBuilder : public SerialFOTBuilder {

  public:
    MifFOTBuilder( const String<CmdLineApp::AppChar> &fileLoc,
                   const Ptr<ExtendEntityManager> &, const CharsetInfo &, CmdLineApp *app );
    ~MifFOTBuilder() {}
    static MifFOTBuilder &curInstance() { assert( CurInstance != NULL ); return *CurInstance; }
    static MifFOTBuilder *CurInstance;

    struct IndexEntryNIC {
        IndexEntryNIC()
         : pageNumber( true ), startsPageRange( false ), endsPageRange( false ) {}
        StringC sortString;
        Vector<StringC> components;
        bool pageNumber;
        bool startsPageRange;
        bool endsPageRange;
    };

    class MifExtensionFlowObj : public FOTBuilder::ExtensionFlowObj {
      public:
        virtual void atomic( MifFOTBuilder &, const NodePtr & ) const = 0;
    };

    class IndexEntryFlowObj : public MifExtensionFlowObj {
      public:
        void atomic( MifFOTBuilder &fb, const NodePtr & ) const
         { fb.indexEntry( nic ); }
        bool hasNIC( const StringC &name ) const;
        void setNIC( const StringC &name, const Value &value );
        ExtensionFlowObj *copy() const { return new IndexEntryFlowObj(*this); }
      private:
        IndexEntryNIC nic;
    };

    void extension( const ExtensionFlowObj &fo, const NodePtr & );
    void start();
    void end();

    void startSimplePageSequence();
    void endSimplePageSequence();
    void startSimplePageSequenceHeaderFooter( unsigned );
    void endSimplePageSequenceHeaderFooter( unsigned );
    void endAllSimplePageSequenceHeaderFooter();

    void indexEntry( const IndexEntryNIC & );

    void setPageNColumns( long );
    void setPageColumnSep( Length );
    void setPageBalanceColumns( bool );

    void startNode( const NodePtr &, const StringC & );
    void endNode();
    void currentNodePageNumber( const NodePtr & );

    void startLink( const Address & );
    void endLink();
    void startLineField( const LineFieldNIC & );
    void endLineField();
    void doStartParagraph( const ParagraphNIC &, bool servesAsWrapper = false,
                           MifDoc::T_dimension = 0, bool allowNegativeLeading = false );
    void startParagraph( const ParagraphNIC &nic ) { doStartParagraph( nic ); }
    void doEndParagraph( bool sustainFormatStack = false, bool sustainDisplayStack = false,
                         bool paragraphBreakTest = true, bool discardThisPara = false );
    void endParagraph() { doEndParagraph(); }
    void startDisplayGroup( const DisplayGroupNIC & );
    void endDisplayGroup();
    void characters( const Char *, size_t );
    void paragraphBreak( const ParagraphNIC & );
    void externalGraphic( const ExternalGraphicNIC & );
    void rule( const RuleNIC & );
    void pageNumber();
    void startScore( Symbol );
    void endScore();
    void startLeader( const LeaderNIC & );
    void endLeader();

    void startTable(const TableNIC &);
    void endTable();
    void startTablePartSerial(const TablePartNIC &);
    void endTablePartSerial();
    void startTablePartHeader();
    void endTablePartHeader();
    void startTablePartFooter();
    void endTablePartFooter();
    void tableColumn(const TableColumnNIC &);
    void startTableRow();
    void endTableRow();
    void startTableCell(const TableCellNIC &);
    void endTableCell();

    void tableBeforeRowBorder();
    void tableAfterRowBorder();
    void tableBeforeColumnBorder();
    void tableAfterColumnBorder();
    void tableCellBeforeRowBorder();
    void tableCellAfterRowBorder();
    void tableCellBeforeColumnBorder();
    void tableCellAfterColumnBorder();
//  void setCellBeforeRowMargin(Length);
//  void setCellAfterRowMargin(Length);
//  void setCellBeforeColumnMargin(Length);
//  void setCellAfterColumnMargin(Length);
    void setCellBackground( bool );
    void setCellRowAlignment( Symbol );
    void setFontSize(Length);
    void setFontFamilyName( const StringC & );
    void setFontWeight( Symbol );
    void setFontPosture( Symbol );
    void setStartIndent( const LengthSpec & );
    void setEndIndent( const LengthSpec & );
    void setFirstLineStartIndent( const LengthSpec & );
//  void setLastLineEndIndent(const LengthSpec &);
    void setLineSpacing( const LengthSpec & );
    void setFieldWidth( const LengthSpec & );
    void setLines( Symbol );
    void setQuadding( Symbol );
    void setDisplayAlignment( Symbol );
    void setFieldAlign( Symbol );
    void setColor( const DeviceRGBColor & );
    void setBackgroundColor( const DeviceRGBColor & );
    void setBackgroundColor();
    void setPageWidth( Length );
    void setPageHeight( Length );
    void setLeftMargin( Length );
    void setRightMargin( Length );
    void setTopMargin( Length );
    void setBottomMargin( Length );
    void setHeaderMargin( Length );
    void setFooterMargin( Length );
    void setBorderPresent(bool);
    void setLineThickness(Length);
    void setCellBeforeRowMargin( Length );
    void setCellAfterRowMargin( Length );
    void setCellBeforeColumnMargin( Length );
    void setCellAfterColumnMargin( Length );
    void setLineSep(Length);
//  void setBoxSizeBefore(Length);
//  void setBoxSizeAfter(Length);
    void setPositionPointShift( const LengthSpec & );
//  void setStartMargin(const LengthSpec &);
//  void setEndMargin(const LengthSpec &);
//  void setSidelineSep(const LengthSpec &);
//  void setAsisWrapIndent(const LengthSpec &);
//  void setLineNumberSep(const LengthSpec &);
//  void setLastLineJustifyLimit(const LengthSpec &);
//  void setJustifyGlyphSpaceMaxAdd(const LengthSpec &);
//  void setJustifyGlyphSpaceMaxRemove(const LengthSpec &);
//  void setTableCornerRadius(const LengthSpec &);
//  void setBoxCornerRadius(const LengthSpec &);
//  void setMarginaliaSep(const LengthSpec &);
//  void setMinPreLineSpacing(const OptLengthSpec &);
//  void setMinPostLineSpacing(const OptLengthSpec &);
    void setMinLeading( const OptLengthSpec & );
//  void setInhibitLineBreaks(bool);
    void setHyphenate( bool );
    void setKern( bool );
//  void setLigature(bool);
//  void setScoreSpaces(bool);
//  void setFloatOutMarginalia(bool);
//  void setFloatOutSidelines(bool);
//  void setFloatOutLineNumbers(bool);
//  void setCellBackground(bool);
//  void setSpanWeak(bool);
//  void setIgnoreRecordEnd(bool);
//  void setNumberedLines(bool);
//  void setHangingPunct(bool);
//  void setBoxOpenEnd(bool);
//  void setTruncateLeader(bool);
//  void setAlignLeader(bool);
//  void setTablePartOmitMiddleHeader(bool);
//  void setTablePartOmitMiddleFooter(bool);
    void setBorderOmitAtBreak(bool);
//  void setPrincipalModeSimultaneous(bool);
//  void setMarginaliaKeepWithPrevious(bool);
//  void setGridEquidistantRows(bool);
//  void setGridEquidistantColumns(bool);
//  void setLineJoin(Symbol);
    void setLineCap( Symbol );
//  void setLineNumberSide(Symbol);
//  void setKernMode(Symbol);
    void setInputWhitespaceTreatment( Symbol );
//  void setFillingDirection(Symbol);
//  void setWritingMode(Symbol);
//  void setLastLineQuadding(Symbol);
//  void setMathDisplayMode(Symbol);
//  void setScriptPreAlign(Symbol);
//  void setScriptPostAlign(Symbol);
//  void setScriptMidSupAlign(Symbol);
//  void setScriptMidSubAlign(Symbol);
//  void setNumeratorAlign(Symbol);
//  void setDenominatorAlign(Symbol);
//  void setGridPositionCellType(Symbol);
//  void setGridColumnAlignment(Symbol);
//  void setGridRowAlignment(Symbol);
//  void setBoxType(Symbol);
//  void setGlyphAlignmentMode(Symbol);
//  void setBoxBorderAlignment(Symbol);
//  void setCellRowAlignment(Symbol);
//  void setBorderAlignment(Symbol);
//  void setSidelineSide(Symbol);
//  void setHyphenationKeep(Symbol);
//  void setPositionPreference(Symbol);
//  void setFontStructure(Symbol);
//  void setFontProportionateWidth(Symbol);
//  void setCellCrossed(Symbol);
//  void setMarginaliaSide(Symbol);
//  void setLayer(long);
//  void setBackgroundLayer(long);
    void setBorderPriority( long );
    void setLineRepeat( long );
    void setSpan( long );
//  void setMinLeaderRepeat(long);
    void setHyphenationRemainCharCount( long );
    void setHyphenationPushCharCount( long );
    void setWidowCount( long );
    void setOrphanCount( long );
//  void setExpandTabs(long);
    void setHyphenationLadderCount( long );
//  void setBackgroundTile(PublicId);
//  void setLineBreakingMethod(PublicId);
//  void setLineCompositionMethod(PublicId);
//  void setImplicitBidiMethod(PublicId);
//  void setGlyphSubstMethod(PublicId);
//  void setGlyphReorderMethod(PublicId);
//  void setHyphenationMethod(PublicId);
//  void setTableAutoWidthMethod(PublicId);
//  void setFontName(PublicId);
    void setLanguage (Letter2 );
    void setCountry( Letter2 );
//  void setEscapementSpaceBefore(const InlineSpace &);
//  void setEscapementSpaceAfter(const InlineSpace &);
//  void setGlyphSubstTable(const Vector<ConstPtr<GlyphSubstTable> > &);

    struct DisplayInfo : public Link {

        DisplayInfo( const DisplayNIC &nic, DisplayInfo *parentDs )
         : spaceBefore( nic.spaceBefore), spaceAfter( nic.spaceAfter ), keep( nic.keep ),
           breakAfter( nic.breakAfter ), breakBefore( nic.breakBefore ),
           keepWithPrevious( nic.keepWithPrevious ), keepWithNext( nic.keepWithNext ),
           mayViolateKeepBefore( nic.mayViolateKeepBefore ), isParagraph( false ),
           mayViolateKeepAfter( nic.mayViolateKeepAfter ), firstParaOutputed( false ),
           paragraphClosedInMif( false )
            { keepWithinPageInEffect = (nic.keep==symbolPage)
                                        ? true
                                        : (parentDs
                                            ? parentDs->keepWithinPageInEffect
                                            : false ); }
       
        DisplaySpace spaceBefore;
        DisplaySpace spaceAfter;
        Symbol keep;
        Symbol breakBefore;
        Symbol breakAfter;
        bool keepWithPrevious;
        bool keepWithNext;
        bool mayViolateKeepBefore;
        bool mayViolateKeepAfter;
        bool firstParaOutputed;
        bool isParagraph;
        bool paragraphClosedInMif;

        bool keepWithinPageInEffect;
    };

    struct DisplaySpaceInfo : public Link {

        DisplaySpaceInfo( DisplaySpace space_, Symbol breakType_, bool breakIsAfter_ )
         : space( space_ ), breakType( breakType_ ), breakIsAfter( breakIsAfter_ ) {}

        DisplaySpace space;
        Symbol breakType;
        bool breakIsAfter;
    };

    struct EffectiveDisplaySpace {

        long nominal;
        long min;
        long max;
        long priority;
        bool conditional;
        bool force;

        EffectiveDisplaySpace() { set(); }
        EffectiveDisplaySpace( long) { set(); }

        void set( long nominal_ = 0, long min_ = 0, long max_ = 0, long priority_ = 0,
                  bool conditional_ = true, bool force_ = false )
              { nominal = nominal_; min = min_; max = max_; priority = priority_;
                conditional = conditional_; force = force_; }
        void combine( const EffectiveDisplaySpace );
        void clear() { set(); }
    };

    struct TFotSimplePageSequence {

         TFotSimplePageSequence() : BodyTextFlow( NULL ), FirstHeaderTextFlow( NULL ),
                                    FirstFooterTextFlow( NULL ), LeftHeaderTextFlow( NULL ),
                                    LeftFooterTextFlow( NULL ), RightHeaderTextFlow( NULL ),
                                    RightFooterTextFlow( NULL ) {}
        MifDoc::TextFlow *BodyTextFlow;
        MifDoc::TextFlow *FirstHeaderTextFlow;
        MifDoc::TextFlow *FirstFooterTextFlow;
        MifDoc::TextFlow *LeftHeaderTextFlow;
        MifDoc::TextFlow *LeftFooterTextFlow;
        MifDoc::TextFlow *RightHeaderTextFlow;
        MifDoc::TextFlow *RightFooterTextFlow;

        MifDoc::ParagraphFormat paragraphFormat;

    };

    struct Border {

        Border( bool cellBorder_ = true )
         : cellBorder( cellBorder_ ), borderPresent( false ) {}
        long   borderPriority;
        Length lineThickness;
        bool   borderPresent;
        long   lineRepeat;
        Length lineSep;
        bool cellBorder;
        MifDoc::T_tagstring color;

        MifDoc::T_tagstring makeMifRuling( MifDoc &mifDoc );
        void resolve( Border &adjacentBorder );
        void setFromFot();
    };

    struct Column {

        Column() : hasWidth( 0 ) {}
        bool hasWidth;
        TableLengthSpec width;
    };

    struct Cell {

        Cell() : missing( false ), MifCell( new MifDoc::Cell ), OverlappingCell( NULL ),
                 nRowsSpanned( 1 ), nColumnsSpanned( 1 ), displaySize( 0 ) {}
        bool missing;

        unsigned nColumnsSpanned;
        unsigned nRowsSpanned;
        Border beforeRowBorder;
        Border afterRowBorder;
        Border beforeColumnBorder;
        Border afterColumnBorder;
        long displaySize;

        Cell *OverlappingCell;
        MifDoc::Cell &mifCell()
         { assert( MifCell != NULL ); return *MifCell; }
        void translate( MifDoc::Cell &mifCell, MifDoc &mifDoc );

      protected:
        MifDoc::Cell *MifCell;    
    };

    struct Row {

        Row() {}
        Vector<Cell> Cells;

        void translate( Vector<MifDoc::Row> &mifRows, MifDoc &mifDoc );
    };

    struct Table;
    struct TablePart {

        TablePart()
         : MifTableNum( 0 ), ParentTable( NULL ), columnsProcessed( false ),
           needsColumnReprocessing( false ) {}

        Vector<Column> Columns;
        Vector<Row> Header;
        Vector<Row> Body;
        Vector<Row> Footer;                

        void translate( MifDoc &mifDoc );
        void processColumns();
        void normalizeRows();
        void begin( Table &parentTable, MifDoc &mifDoc );

        MifDoc::Tbl &mifTable( MifDoc &mifDoc )
         { assert( MifTableNum > 0 ); return mifDoc.tbls()[MifTableNum-1]; }
        Table &parentTable() { assert( ParentTable != NULL ); return *ParentTable; }

        size_t MifTableNum;
        Table *ParentTable;
        bool columnsProcessed;
        bool needsColumnReprocessing;
    };

    struct Table {

        Table() : beforeRowBorder( false ), afterRowBorder( false ), beforeColumnBorder( false ),
                  afterColumnBorder( false ), CurCell( NULL ), CurTablePart( NULL ),
                  DefaultTblFormatGenerated( false ), NoTablePartsSeen( true ) {}

        Vector<TablePart> TableParts;

        Border beforeRowBorder;
        Border afterRowBorder;
        Border beforeColumnBorder;
        Border afterColumnBorder;
//        TableNIC nic;
        Length tableWidth;
        Symbol displayAlignment;
        Length startIndent;

        void resolveBorders( Vector<Row> &rows, bool hasFirstTableRow, bool hasLastTableRow );
        void begin( MifDoc &mifDoc );

        Vector<Row> &curRows() { assert( CurRows != NULL ); return *CurRows; }
        TablePart &curTablePart() { assert( CurTablePart != NULL ); return *CurTablePart; }
        Cell &curCell() { assert( CurCell != NULL ); return *CurCell; }

        TablePart *CurTablePart;
        Cell *CurCell;
        Vector<Row> *CurRows;
        
        bool DefaultTblFormatGenerated;
        bool NoTablePartsSeen;

    };

    struct Format : public MifDoc::ParagraphFormat {

        Format() : MifDoc::ParagraphFormat(), FotLineSpacingSpec( 12000 ),
                   FotFirstLineStartIndentSpec( 0 ), FotStartIndentSpec( 0 ),
                   FotEndIndentSpec( 0 ), FotLanguage( 0 ), FotCurDisplaySize( 0 ),
                   FotLineThickness( 1000 ), FotBorderPresent( true ), FotLineRepeat( 1 ),
                   FotLineSep( 1000 ), FotBorderPriority( 0 ),
                   FotDisplayAlignment( symbolStart ), FotLeftMargin( 1 ), FotRightMargin( 1 ),
                   FotTopMargin( 0 ), FotBottomMargin( 0 ), FotHeaderMargin( 0 ),
                   FotFooterMargin( 0 ), FotPageWidth( 72000*8 ),
                   FotPageHeight( (72000*23)/2 ), FotLineCap( symbolButt ),
                   FotPositionPointShiftSpec( 0 ), FotMinLeading(),
                   FotFieldAlign( symbolStart ), FotFieldWidth( 0 ), FotLines( symbolWrap ),
                   FotInputWhitespaceTreatment( symbolPreserve ), FotPageNColumns( 1 ),
                   FotPageColumnSep( 72000/2 ), FotSpan( 1 ), FotPageBalanceColumns( false ),
                   FotCellBackground( false ) {}

        LengthSpec    FotFirstLineStartIndentSpec;
        LengthSpec    FotStartIndentSpec;
        LengthSpec    FotEndIndentSpec;
        LengthSpec    FotLineSpacingSpec;
        LengthSpec    FotPositionPointShiftSpec;
        LengthSpec    FotFieldWidth;
        OptLengthSpec FotMinLeading;
        long          FotCurDisplaySize;
        Length        FotLineThickness;
        Symbol        FotLineCap;
        long          FotBorderPriority;
        bool          FotBorderPresent;
        long          FotLineRepeat;
        Length        FotLineSep;
        Symbol        FotDisplayAlignment;
        Length        FotLeftMargin;
        Length        FotRightMargin;
        Length        FotTopMargin;
        Length        FotBottomMargin;
        Length        FotHeaderMargin;
        Length        FotFooterMargin;
        Length        FotPageWidth;
        Length        FotPageHeight;
        Symbol        FotFieldAlign;
        Symbol        FotInputWhitespaceTreatment;
        Symbol        FotLines;
        long          FotPageNColumns;
        Length        FotPageColumnSep;
        long          FotSpan;
        bool          FotPageBalanceColumns;
        bool          FotCellBackground;

        MifDoc::T_tagstring MifBackgroundColor;

        Letter2 FotLanguage;
        Letter2 FotCountry;
        void computePgfLanguage();

        static const Length INITIAL_PAGE_SIZE() { return 72000*8; }
    };

    struct NodeInfo {

        NodeInfo() {}
        NodeInfo( NodePtr node_, unsigned nodeLevel_ )
        : node( node_ ), nodeLevel( nodeLevel_ ) {}
        NodePtr node;
        unsigned nodeLevel;
        static unsigned nonEmptyElementsOpened;
        static unsigned curNodeLevel;
    };

    struct LinkInfo {

        LinkInfo( MifDoc::CrossRefInfo *crossRefInfo_ = NULL ) 
         : crossRefInfo( crossRefInfo_ ), openedInMif( false ) {}
        ~LinkInfo() { if( crossRefInfo != NULL ) delete crossRefInfo; }

        bool forcesNoLink() const { return crossRefInfo != NULL ? false : true; }

        MifDoc::CrossRefInfo *crossRefInfo;
        bool openedInMif;
        static unsigned pendingMifClosings;
    };

    struct SymbolFont {
        const char *name;
        Char mapping[256];
    };

    enum { nSymbolFonts = 3 };

    void synchronizeFontFormat();
    long computeLengthSpec( const LengthSpec &spec ) const;
    Format &format() const
        { Format *result = formatStack.head(); assert( result != NULL ); return *result; }
    Table &curTable() { return CurTable; }
    void setCurLeaderStream( MifTmpOutputByteStream *s, bool doDelete = true )
     { if( doDelete && CurLeaderStream ) delete CurLeaderStream; CurLeaderStream = s; }
    MifTmpOutputByteStream &curLeaderStream()
     { assert( CurLeaderStream != NULL ); return *CurLeaderStream; }

    CmdLineApp *App;
    const CharsetInfo *SystemCharset;
    Ptr<ExtendEntityManager> EntityManager;
    bool paragraphBreakInEffect;
    bool inLeader;
    bool lastFlowObjectWasWhitespace;
    bool firstHeaderFooter;
    bool inSimplePageSequence;
    bool bookComponentOpened;
    bool bookComponentAvailable;

  protected:

    TFotSimplePageSequence FotSimplePageSequence;
    Table CurTable;

    MifDoc::T_pathname systemIdToMifPathname( const StringC &systemId );
    bool systemIdToMifPathname( const StringC &systemId, MifDoc::T_pathname & );
    int  systemIdFilename( const StringC &systemId, StringC &filename );
    void initMifBookComponent();
    void setupSimplePageSequence();
    void beginHeaderFooter( bool header );
    void beginHeader() { beginHeaderFooter( true ); }
    void beginFooter() { beginHeaderFooter( false ); };
    void endHeaderFooter();
    void makeEmptyTextFlow( MifDoc::TextRect & );
    void checkForParagraphReopening();
    void outPendingInlineStatements();
    void outString( const Char *s, size_t n, MifTmpOutputByteStream *o = NULL,
                    bool inParagraph = true, String<char> *targetString = NULL );

    MifDoc::Frame &makeAnchoredFrame
     ( MifDoc::T_keyword frameType, long width, long height,
       MifDoc::T_keyword anchorAlign = MifDoc::sLeft );
    void setupHeaderFooterParagraphFormat( MifDoc::ParagraphFormat &,
                                           MifDoc::ParagraphFormat &,
                                           MifDoc::T_dimension );
    OutputByteStream &os() { return *outputStream; }

    MifDoc mifDoc;
    OutputByteStream *outputStream;
    MifTmpOutputByteStream *CurLeaderStream;
    EffectiveDisplaySpace createEffectiveDisplaySpace( const DisplaySpace &ds ) const;
    IList<DisplayInfo> displayStack;
    IList<Format> formatStack;
    IQueue<DisplaySpaceInfo> displaySpaceQueue;
    Vector<NodeInfo> nodeStack;
    Vector<LinkInfo> linkStack;
    Vector<MifDoc::Marker> indexEntryStack;

    Format nextFormat;

    Symbol pendingBreak;
    EffectiveDisplaySpace pendingEffectiveDisplaySpace;
    void processDisplaySpaceStack();

    void startDisplay( const DisplayNIC & );
    void endDisplay();

    // sytem font code from James Clark
    enum { CHAR_TABLE_CHAR_BITS = 16 };
    enum { CHAR_TABLE_SYMBOL_FLAG = 1U << 31 };
    CharMap<Unsigned32> CharTable;

    static const SymbolFont SymbolFonts[nSymbolFonts];
    static const Char FrameCharsetMap[128];
};

MifFOTBuilder *MifFOTBuilder::CurInstance = NULL;
unsigned MifFOTBuilder::NodeInfo::nonEmptyElementsOpened = 0;
unsigned MifFOTBuilder::NodeInfo::curNodeLevel = 0;
unsigned MifFOTBuilder::LinkInfo::pendingMifClosings = 0;

int operator ==(struct FOTBuilder::DisplayNIC, struct FOTBuilder::DisplayNIC) { return 0; }

void MifFOTBuilder::start() {

    NodeInfo::nonEmptyElementsOpened = nodeStack.size();

    Format *effectiveFormat = new Format( nextFormat );
    if( nextFormat.FSize > 0 )
        effectiveFormat->setFDY
         ( double( computeLengthSpec( nextFormat.FotPositionPointShiftSpec ) * -100 )
            / nextFormat.FSize );

    formatStack.insert( effectiveFormat );
}

void MifFOTBuilder::end() {

    assert( formatStack.head() != NULL );
    delete formatStack.get();

    assert( formatStack.head() != NULL );
    nextFormat = *formatStack.head();
}

void MifFOTBuilder::Format::computePgfLanguage() {

    switch( FotLanguage ) {
      case SP_LETTER2( 'E', 'N' ):
        switch( FotCountry ) {
          case SP_LETTER2( 'G', 'B' ):
            PgfLanguage = MifDoc::sUKEnglish;
            break;
          default:
            PgfLanguage = MifDoc::sUSEnglish;
        }
        break;
      case SP_LETTER2( 'D', 'E' ):
        switch( FotCountry ) {
          case SP_LETTER2( 'C', 'H' ):
            PgfLanguage = MifDoc::sSwissGerman;
            break;
          default:
            PgfLanguage = MifDoc::sGerman;
        }
      case SP_LETTER2( 'F', 'R' ):
        switch( FotCountry ) {
          case SP_LETTER2( 'C', 'A' ):
            PgfLanguage = MifDoc::sCanadianFrench;
            break;
          default:
            PgfLanguage = MifDoc::sFrench;
        }
        break;
      case SP_LETTER2( 'E', 'S' ):
        PgfLanguage = MifDoc::sSpanish;
        break;
      case SP_LETTER2( 'C', 'A' ):
        PgfLanguage = MifDoc::sCatalan;
        break;
      case SP_LETTER2( 'I', 'T' ):
        PgfLanguage = MifDoc::sItalian;
        break;
      case SP_LETTER2( 'P', 'T' ):
        PgfLanguage = MifDoc::sPortuguese;
        break;
//      case SP_LETTER2( '', '' ):
//        PgfLanguage = sBrazilian;
//        break;
//      case SP_LETTER2( '', '' ):
//        PgfLanguage = sDanish;
//        break;
      case SP_LETTER2( 'N', 'L' ):
        PgfLanguage = MifDoc::sDutch;
        break;
      case SP_LETTER2( 'N', 'O' ):
        PgfLanguage = MifDoc::sNorwegian;
        break;
 //     case SP_LETTER2( '', '' ):
 //       PgfLanguage = sNynorsk;
 //       break;
      case SP_LETTER2( 'F', 'I' ):
        PgfLanguage = MifDoc::sFinnish;
        break;
      case SP_LETTER2( 'S', 'V' ):
        PgfLanguage = MifDoc::sSwedish;
        break;
      default:
        PgfLanguage = MifDoc::sNoLanguage;
    }
}

MifFOTBuilder::MifFOTBuilder( const String<CmdLineApp::AppChar> &fileLoc,
                              const Ptr<ExtendEntityManager> &entityManager,
 		            	      const CharsetInfo &systemCharset,
                              CmdLineApp *app )
 : App( app ), EntityManager( entityManager ), SystemCharset( &systemCharset ),
   mifDoc( fileLoc, app ), paragraphBreakInEffect( false ), inLeader( false ),
   CurLeaderStream( NULL ), CharTable( 0 ), lastFlowObjectWasWhitespace( false ),
   pendingBreak( symbolFalse ), firstHeaderFooter( true ),  inSimplePageSequence( false ),
   bookComponentOpened( false ), bookComponentAvailable( false ) {

    CurInstance = this;

    nextFormat.setDSSSLDefaults();
    nextFormat.FotCurDisplaySize = Format::INITIAL_PAGE_SIZE() - 2;
     // 2 is for margins (MIF doesn't accept zero-sized ones)
    formatStack.insert( new Format( nextFormat ) );

    initMifBookComponent();
    bookComponentOpened = true;
    bookComponentAvailable = true;

    for( int i = 0; i < 128; i++ ) {
        Char c = FrameCharsetMap[i];
        if( c ) {
            if( !CharTable[i+0x80] )
	        CharTable.setChar( i+0x80, c );
	    else if ((CharTable[i+0x80] & ((1 << CHAR_TABLE_CHAR_BITS) - 1)) == c)
	        CharTable.setChar(i+0x80, CharTable[i+0x80] | (1 << (i + CHAR_TABLE_CHAR_BITS)));
        }
    }

    // symbol character code from James Clark
    for( int i = 0; i < nSymbolFonts; i++ ) {
        for( int j = 0; j < 256; j++ ) {
            Char c = SymbolFonts[i].mapping[j];
            if( c && !CharTable[c] )
            	CharTable.setChar( c, j | (i << CHAR_TABLE_CHAR_BITS) | CHAR_TABLE_SYMBOL_FLAG );
        }
    }
}

void MifFOTBuilder::initMifBookComponent() {

   MifDoc::ParagraphFormat defaultParaFormat;
   defaultParaFormat.setDSSSLDefaults();
   defaultParaFormat.setPgfTag( MifDoc::sDefaultPgfFormat );
   mifDoc.pgfCatalog().ParaFormats.push_back( defaultParaFormat );
 
   mifDoc.document().setDPageSize
    ( MifDoc::T_WH( MifDoc::T_dimension( format().FotPageWidth ),
                    MifDoc::T_dimension( format().FotPageHeight ) ) );
   mifDoc.document().setDMargins( MifDoc::T_LTRB( 1, 1, 1, 1 ) );
    // MIF doesn't accept zeros
   mifDoc.document().setDColumns( 1 );
}

long MifFOTBuilder::computeLengthSpec( const LengthSpec &spec ) const {

    if( spec.displaySizeFactor == 0.0 ) {
        return spec.length;
    } else {
        double tem = format().FotCurDisplaySize * spec.displaySizeFactor;
        return spec.length + long( tem >= 0.0 ? tem +.5 : tem - .5 );
    }
}

MifFOTBuilder::EffectiveDisplaySpace
MifFOTBuilder::createEffectiveDisplaySpace( const DisplaySpace &ds ) const {

    EffectiveDisplaySpace result;
    result.set( computeLengthSpec( ds.nominal ), computeLengthSpec( ds.min ),
                computeLengthSpec( ds.max ), ds.priority, ds.conditional, ds.force );
    return result;
}

void MifFOTBuilder::EffectiveDisplaySpace::combine( const EffectiveDisplaySpace eds ) {

    if( eds.force ) {
        if( force ) {
            nominal += eds.nominal;
            min += eds.min;
            max += eds.max;
        } else
            *this = eds;
    } else {
        if( eds.priority > priority )
            *this = eds;
        else {
            if( eds.priority == priority ) {
                if( eds.nominal > nominal )
                    *this = eds;
                else
                    if( eds.nominal == nominal ) {
                        if( eds.min < min ) {
                            min = eds.min;
                        }
                        if( eds.max > max ) {
                            max = eds.max;
                        }
                    }
            }
        }
    }
}

void MifFOTBuilder::synchronizeFontFormat() {

    if( mifDoc.curPara( false ) != NULL ) {
//        mifDoc.curPara()->curFormat().ffUpdateFrom( format() );
        mifDoc.curPara()->curFormat().ffUpdateFrom( nextFormat );
        mifDoc.curPara()->curFormat().ffOut( mifDoc.os(), MifDoc::FontFormat::stFont );
    } else {
//        mifDoc.curFormat().ffUpdateFrom( format() );
        mifDoc.curFormat().ffUpdateFrom( nextFormat );
        mifDoc.curFormat().ffOut( mifDoc.os(), MifDoc::FontFormat::stFont );
    }
    outPendingInlineStatements();
}

void MifFOTBuilder::outString
 ( const Char *s, size_t n, MifTmpOutputByteStream *o, bool inParagraph,
   String<char> *targetString ) {

    MifOutputByteStream *outS
     = ( o != NULL ) ? &o->stream()
                     : ( ( targetString != NULL ) ? (MifOutputByteStream *)NULL : &mifDoc.os() );

    enum { nStr, nCode, nChar, nNothing, nHardReturn, nSpecialChar } nextOut;
    union { const char *str; unsigned long code; char chr;  } out;
    String<char> out_specialChar;

    MifDoc::ParagraphFormat *curPFormat
     = inParagraph 
        ? ( mifDoc.curPara( false ) != NULL ? &mifDoc.curPara()->curFormat() : &mifDoc.curFormat() ) 
        : (MifDoc::ParagraphFormat*)NULL;
    MifDoc::T_string paraFFamily;
    if( curPFormat != NULL )
        paraFFamily = curPFormat->FFamily;
    bool stringOpened = false;
    int prevSymbolFontNum = 0;
    int curSymbolFontNum = 0;
    bool thisFlowObjectIsWhitespace;
    for( ; n > 0; s++, n-- ) {
        thisFlowObjectIsWhitespace = false;
        nextOut = nNothing;
        switch( *s ) {
          case Char( '\n' ): break;
          case Char( '\r' ):
            if( !inParagraph ) {
                out.chr = ' '; nextOut = nChar;
            } else {
                switch( format().FotLines ) {
                  case symbolNone:
                  case symbolWrap:
                    switch( format().FotInputWhitespaceTreatment ) {
                      case symbolIgnore: break;
                      case symbolCollapse:
                        if( lastFlowObjectWasWhitespace )
                            break;
                      case symbolPreserve: default:            
                        out.chr = ' '; nextOut = nChar;
                    }  
                    break;
                  default:
                    nextOut = nHardReturn;
                }
                thisFlowObjectIsWhitespace = true;
            }
            break;
          case Char( '\t' ):
            if( !inParagraph ) {
                out.str = "\\t"; nextOut = nStr;
            } else {
                switch( format().FotInputWhitespaceTreatment ) {
                  case symbolIgnore: break;
                  case symbolCollapse:
                    if( lastFlowObjectWasWhitespace )
                        break;
                  case symbolPreserve: default:            
                    out.str = "\\t"; nextOut = nStr;
                }  
                thisFlowObjectIsWhitespace = true;
            }
            break;
          case Char( '>' ):  out.str = "\\>"; nextOut = nStr; break;
          case Char( '\'' ): out.str = "\\q"; nextOut = nStr; break;
          case Char( '`' ):  out.str = "\\Q"; nextOut = nStr; break;
          case Char( '\\' ): out.str = "\\\\"; nextOut = nStr; break;
          case Char( ' ' ):
            if( !inParagraph ) {
                out.chr = ' '; nextOut = nChar;
            } else {
                switch( format().FotInputWhitespaceTreatment ) {
                  case symbolIgnore: break;
                  case symbolCollapse:
                    if( lastFlowObjectWasWhitespace )
                        break;
                  case symbolPreserve: default:            
                    out.chr = ' '; nextOut = nChar;
                }  
                thisFlowObjectIsWhitespace = true;
            }
            break;
          case 0x00A0:
            out_specialChar = MifDoc::sHardSpace; nextOut = nSpecialChar;
            break;
          case 0x00A2:
            out_specialChar = MifDoc::sCent; nextOut = nSpecialChar;
            break;
          case 0x00A3:
          case 0x20A4:
            out_specialChar = MifDoc::sPound; nextOut = nSpecialChar;
            break;
          case 0x00A5:
            out_specialChar = MifDoc::sYen; nextOut = nSpecialChar;
            break;
          case 0x2002:
            out_specialChar = MifDoc::sEnSpace; nextOut = nSpecialChar;
            break;
          case 0x2003:
            out_specialChar = MifDoc::sEmSpace; nextOut = nSpecialChar;
            break;
          case 0x2009:
            out_specialChar = MifDoc::sThinSpace; nextOut = nSpecialChar;
            break;
          case 0x2010:
            out_specialChar = MifDoc::sSoftHyphen; nextOut = nSpecialChar;
            break;
          case 0x2011:
            out_specialChar = MifDoc::sHardHyphen; nextOut = nSpecialChar;
            break;
          case 0x2013:
            out_specialChar = MifDoc::sEnDash; nextOut = nSpecialChar;
            break;
          case 0x2014:
            out_specialChar = MifDoc::sEmDash; nextOut = nSpecialChar;
            break;
          case 0x2020:
            out_specialChar = MifDoc::sDagger; nextOut = nSpecialChar;
            break;
          case 0x2021:
            out_specialChar = MifDoc::sDoubleDagger; nextOut = nSpecialChar;
            break;
          case 0x2022:
            out_specialChar = MifDoc::sBullet; nextOut = nSpecialChar;
            break;
          default:
            if( *s >= 0x80 ) {
                unsigned long code = CharTable[*s];
                if( code & CHAR_TABLE_SYMBOL_FLAG ) {
                    if( curPFormat != NULL ) {
                        curSymbolFontNum = ( ( code & ~CHAR_TABLE_SYMBOL_FLAG )
                                              >> CHAR_TABLE_CHAR_BITS ) + 1;
                        assert( curSymbolFontNum < nSymbolFonts );
                        out.code = code & 0xff; nextOut = nCode;
                    }
                } else
                    if( code ) {
                        out.code = code & 0xff; nextOut = nCode;
                    }
            } else {
                out.chr = char( *s ); nextOut = nChar;
            }
        }

        if( nextOut != nNothing ) {
            if( curSymbolFontNum != prevSymbolFontNum ) {
                if( stringOpened ) {
                    *outS << "'>";
                    stringOpened = false;
                }
                MifDoc::T_string fontFamily( curSymbolFontNum == 0 
                                      ? paraFFamily
                                      : MifDoc::T_string( 
                                         SymbolFonts[ curSymbolFontNum - 1 ].name ) );
                curPFormat->setFFamily( fontFamily );
                curPFormat->ffOut( *outS, MifDoc::FontFormat::stFont );
            }
            if( curPFormat != NULL && !stringOpened ) {
                *outS << '\n' << MifOutputByteStream::INDENT << "<String `";
                stringOpened = true;
            }
            switch( nextOut ) {
              case nChar:
                  if( targetString != NULL )
                    *targetString += out.chr;
                  else
                    *outS << out.chr;
                  break;

              case nHardReturn:
                if( inParagraph ) {
                    if( stringOpened ) {
                        *outS << "'>";
                        stringOpened = false;
                    }
                    mifDoc.outSpecialChar( MifDoc::sHardReturn, outS );
                    MifDoc::ParaLine::outEpilog( *outS );
                    MifDoc::ParaLine::outProlog( *outS );
                }
                break;

              case nSpecialChar:
                if( inParagraph ) {
                    if( stringOpened ) {
                        *outS << "'>";
                        stringOpened = false;
                    }
                    mifDoc.outSpecialChar( out_specialChar, outS );
                }
                break;

              case nCode:
                if( targetString != NULL )
                   mifDoc.outHexChar( out.code, *targetString );
                else
                   mifDoc.outHexChar( out.code, outS ); break;
                break;          

              case nStr:
                if( targetString != NULL )
                    targetString->append( out.str, strlen( out.str ) );
                else
                    *outS << out.str;
                break;
              case nNothing: break;
            }
        }

        prevSymbolFontNum = curSymbolFontNum;    
        lastFlowObjectWasWhitespace = thisFlowObjectIsWhitespace;
    }

    if( stringOpened )
        *outS << "'>";

    if( curSymbolFontNum != 0 ) {
        curPFormat->setFFamily( paraFFamily );
        curPFormat->ffOut( *outS, MifDoc::FontFormat::stFont );
        curPFormat->ffSetProperties() &= ~MifDoc::FontFormat::fFFamily;
    }
}

void MifFOTBuilder::characters( const Char *s, size_t n ) {

    checkForParagraphReopening();
    if( MifDoc::Para::currentlyOpened ) {
        if( inLeader ) {
            outString( s, n, &curLeaderStream(), false );
        } else {
            synchronizeFontFormat();
            outString( s, n, NULL, true );
        }
    }
}

void MifFOTBuilder::checkForParagraphReopening() {

   DisplayInfo *curDs = displayStack.head();
//   assert( curDs != NULL );
   if( curDs != NULL && curDs->paragraphClosedInMif ) {

        Format f = format();

        f.setPgfWithPrev( curDs->firstParaOutputed ? curDs->keepWithinPageInEffect : false );
        f.setPgfWithNext( false ); // TMAI
        curDs->firstParaOutputed = true;

        processDisplaySpaceStack();
        switch( pendingBreak ) {
            case symbolPage:
                f.setPgfPlacement( MifDoc::sPageTop );
                break;
            case symbolColumn:
                f.setPgfPlacement( MifDoc::sColumnTop );
                break;
            default:
                f.setPgfPlacement( MifDoc::sAnywhere );
        }
        pendingBreak = symbolFalse;

        f.setPgfSpBefore( pendingEffectiveDisplaySpace.nominal );
         pendingEffectiveDisplaySpace.clear();
        f.setPgfFIndent( nextFormat.PgfLIndent );

        MifDoc::Para *p
         = new MifDoc::Para( mifDoc.curTagStream().content().stream().CurTagIndent + 4 );
        p->setParagraphFormat( mifDoc.curFormat() );
        p->format().updateFrom( f );
        p->format().FSize = mifDoc.curFormat().FSize;
        p->format().ffSetProperties() &= ~MifDoc::FontFormat::fFSize;
        Length lineSpacing = computeLengthSpec( f.FotLineSpacingSpec.length );
        if( p->format().FSize != lineSpacing )
            p->format().setFSize( lineSpacing );
        p->curFormat() = p->format();
        // assert( mifDoc.curPara( false ) == NULL );
        mifDoc.setCurPara( p );
        mifDoc.enterPara( *mifDoc.curPara() );

        curDs->paragraphClosedInMif = false;
   }
}

void MifFOTBuilder::processDisplaySpaceStack() {

    pendingBreak = symbolFalse;
    EffectiveDisplaySpace effectiveDisplaySpace;
    DisplaySpaceInfo *curDSI;
    while( !displaySpaceQueue.empty() ) {
        curDSI = displaySpaceQueue.get();
        if( curDSI->breakType == symbolPage || curDSI->breakType == symbolColumn ) {
            effectiveDisplaySpace.clear();
            if( pendingBreak != symbolFalse )
                mifDoc.outBreakingPara( curDSI->breakType == symbolPage
                                         ? MifDoc::sPageTop : MifDoc::sColumnTop );
            pendingBreak = curDSI->breakType;
            if( !curDSI->breakIsAfter )
                effectiveDisplaySpace.combine( createEffectiveDisplaySpace( curDSI->space ) );
        } else
            effectiveDisplaySpace.combine( createEffectiveDisplaySpace( curDSI->space ) );
        delete curDSI;
    }

    pendingEffectiveDisplaySpace = effectiveDisplaySpace;
}


void MifFOTBuilder::doStartParagraph
      ( const ParagraphNIC &nic, bool servesAsWrapper, MifDoc::T_dimension height,
        bool allowNegativeLeading ) {

    startDisplay( nic );

    DisplayInfo *curDs = displayStack.head();
    assert( curDs != NULL );
    curDs->isParagraph = true;

    nextFormat.setPgfWithPrev( curDs->mayViolateKeepBefore
                                ? curDs->keepWithPrevious
                                : ( curDs->firstParaOutputed
                                     ? curDs->keepWithinPageInEffect
                                     : false ) );
    nextFormat.setPgfWithNext( curDs->keepWithNext ); 
    curDs->firstParaOutputed = true;

    processDisplaySpaceStack();
    switch( pendingBreak ) {
        case symbolPage:
            nextFormat.setPgfPlacement( MifDoc::sPageTop );
            break;
        case symbolColumn:
            nextFormat.setPgfPlacement( MifDoc::sColumnTop );
            break;
        default:
            nextFormat.setPgfPlacement( MifDoc::sAnywhere );
    }
    pendingBreak = symbolFalse;

    Length lineSpacing;
    Length extraSpaceBefore = 0; 
    if( servesAsWrapper ) {
        if( height < 2000 ) {   // FrameMaker minimum is 2pt    
            if( allowNegativeLeading )
                nextFormat.setPgfLeading( height - 2000 );
            else
                pendingEffectiveDisplaySpace.nominal
                  -= ( pendingEffectiveDisplaySpace.nominal > 2000 )
                        ? 2000 : pendingEffectiveDisplaySpace.nominal;
                 // try to steal as much as possible from space before
            height = 2000;
        }
        //if( effectiveDisplaySpace.nominal <= 0 )
         //    extraSpaceBefore = 1; // FrameMaker bug workaround    
        //nextFormat.setFSize( height );
        lineSpacing = height;
        nextFormat.setFColor( MifDoc::sWhite );
        nextFormat.setPgfLineSpacing( MifDoc::sFixed );
    } else {
        //nextFormat.setFSize( computeLengthSpec( nextFormat.FotLineSpacingSpec.length ) );
        lineSpacing = computeLengthSpec( nextFormat.FotLineSpacingSpec.length );
        if( nextFormat.FotMinLeading.hasLength ) // but ignore the actual min-leading value
            nextFormat.PgfLineSpacing = MifDoc::sProportional;
        else
            nextFormat.PgfLineSpacing = MifDoc::sFixed;
    }
    
    nextFormat.setPgfSpBefore( pendingEffectiveDisplaySpace.nominal + extraSpaceBefore );
    pendingEffectiveDisplaySpace.clear();

    nextFormat.setPgfLIndent( computeLengthSpec( nextFormat.FotStartIndentSpec ) );
    nextFormat.setPgfFIndent( computeLengthSpec( nextFormat.FotFirstLineStartIndentSpec )
                               + nextFormat.PgfLIndent );
    nextFormat.setPgfRIndent( computeLengthSpec( nextFormat.FotEndIndentSpec ) );

    nextFormat.setPgfPlacementStyle
     ( nextFormat.FotSpan > 1 ? MifDoc::sStraddle : MifDoc::sNormal );
    start();

    MifDoc::Para *p
     = new MifDoc::Para( mifDoc.curTagStream().content().stream().CurTagIndent + 4 );
    p->setParagraphFormat( mifDoc.curFormat() );
    p->format().updateFrom( format() );
    p->format().FSize = mifDoc.curFormat().FSize;
    p->format().ffSetProperties() &= ~MifDoc::FontFormat::fFSize;
    if( p->format().FSize != lineSpacing )
        p->format().setFSize( lineSpacing );
    p->curFormat() = p->format();
    // assert( mifDoc.curPara( false ) == NULL );
    mifDoc.setCurPara( p );
    mifDoc.enterPara( *mifDoc.curPara() );
    lastFlowObjectWasWhitespace = false;
    outPendingInlineStatements();

    // mifDoc.curFormat().out( mifDoc.os() );
    // mifDoc.beginParaLine();
}

void MifFOTBuilder::doEndParagraph
 ( bool sustainFormatStack, bool sustainDisplayStack, bool paragraphBreakTest,
   bool discardThisPara ) {

//    mifDoc.endParaLine();
//    MifDoc::Para::outEpilog( mifDoc.os() );

    DisplayInfo *curDs = displayStack.head();
    assert( curDs != NULL );

    if( !sustainFormatStack )
        end();
    if( !sustainDisplayStack )
        endDisplay();

    if( paragraphBreakTest && paragraphBreakInEffect ) {
        paragraphBreakInEffect = false;
        end();
    }

    if( !curDs->paragraphClosedInMif ) {
        MifDoc::Para *p = mifDoc.curPara();
        mifDoc.exitPara();
        if( !discardThisPara ) {
            p->out( mifDoc.os() );
            mifDoc.curFormat().updateFrom( p->format() );
        }
        delete p;
    }
}

void MifFOTBuilder::startDisplayGroup( const DisplayGroupNIC &nic ) {

    startDisplay( nic );
    start();
}

void MifFOTBuilder::endDisplayGroup() {

    end();
    endDisplay();
}

void MifFOTBuilder::startDisplay( const DisplayNIC &nic ) {

    if( !inSimplePageSequence ) {
        if( !bookComponentOpened ) {
            mifDoc.enterBookComponent();
            initMifBookComponent();
            bookComponentOpened = true;
        }
        bookComponentAvailable = false;
    }

    displaySpaceQueue.append
     ( new DisplaySpaceInfo( nic.spaceBefore, nic.breakBefore, false ) );

    DisplayInfo *di = displayStack.head();
    if( di != NULL && di->isParagraph && !di->paragraphClosedInMif ) {
        if( mifDoc.curPara()->content().isEmpty() ) {
            doEndParagraph( true, true, true, true );
        } else
            doEndParagraph( true, true );
        di->paragraphClosedInMif = true;
    }

    if( curTable().CurCell != NULL )
        nextFormat.FotCurDisplaySize = curTable().CurCell->displaySize;
    else if( nextFormat.FotSpan > 1 )
        nextFormat.FotCurDisplaySize
         = nextFormat.FotPageWidth - nextFormat.FotLeftMargin - nextFormat.FotRightMargin;
    else
        nextFormat.FotCurDisplaySize
         = ( nextFormat.FotPageWidth - nextFormat.FotLeftMargin - nextFormat.FotRightMargin
              - nextFormat.FotPageColumnSep * ( nextFormat.FotPageNColumns - 1 ) )  
            / nextFormat.FotPageNColumns;

    displayStack.insert( new DisplayInfo( nic, displayStack.head() ) );
}

void MifFOTBuilder::endDisplay() {

    DisplayInfo *di = displayStack.get();
    
    displaySpaceQueue.append
     ( new DisplaySpaceInfo( di->spaceAfter, di->breakAfter, true ) );

    delete di;
}

void MifFOTBuilder::startSimplePageSequence() {

    inSimplePageSequence = true;
    firstHeaderFooter = true;

    bool openBookComponent = true;
    if( bookComponentOpened ) {
        if( bookComponentAvailable ) {
            openBookComponent = false;
            bookComponentAvailable = false;
        } else
            mifDoc.exitBookComponent();
    }

    if( openBookComponent ) {
        mifDoc.enterBookComponent();
        initMifBookComponent();
        bookComponentOpened = true;
        bookComponentAvailable = false;
    }

    nextFormat.FotCurDisplaySize
     = ( nextFormat.FotPageWidth - nextFormat.FotLeftMargin - nextFormat.FotRightMargin
          - nextFormat.FotPageColumnSep * ( nextFormat.FotPageNColumns - 1 ) )  
        / nextFormat.FotPageNColumns;

    mifDoc.document().setDColumns( nextFormat.FotPageNColumns );
    if( nextFormat.FotPageNColumns > 1 )
        mifDoc.document().setDColumnGap( nextFormat.FotPageColumnSep );

    mifDoc.document().setProperties &= ~MifDoc::Document::fDMargins;

    start();

    FotSimplePageSequence.paragraphFormat = format();
}

void MifFOTBuilder::endSimplePageSequence() {

    end();
    mifDoc.exitTextFlow();
    mifDoc.exitBookComponent();
    inSimplePageSequence = false;
    bookComponentOpened = false;
    bookComponentAvailable = false;
}

void MifFOTBuilder::makeEmptyTextFlow( MifDoc::TextRect &textRect ) {

    MifDoc::TextFlow &textFlow = *(new MifDoc::TextFlow( textRect, true ));
    mifDoc.textFlows().push_back( textFlow );

    mifDoc.enterTextFlow( textFlow );
    MifDoc::Para::outSimpleProlog( mifDoc.os() );
    MifDoc::ParaLine::outProlog( mifDoc.os() );
    MifDoc::ParaLine::outEpilog( mifDoc.os() );
    MifDoc::Para::outEpilog( mifDoc.os() );
    mifDoc.exitTextFlow();
}

void MifFOTBuilder::beginHeaderFooter( bool header ) {

    start();

    MifDoc::Para p;
    p.setPgfTag( header ? MifDoc::sHeader : MifDoc::sFooter );
    p.setProperties &= ~MifDoc::Para::fParagraphFormat;
    p.outProlog( mifDoc.os() );

    MifDoc::FontFormat ff;
    ff.setFSize( format().FSize );
    ff.out( mifDoc.os(), MifDoc::FontFormat::fFSize, MifDoc::FontFormat::stFont );

    mifDoc.beginParaLine();
}

void MifFOTBuilder::endHeaderFooter() {

    mifDoc.endParaLine();
    MifDoc::Para::outEpilog( mifDoc.os() );

    end();
}

void MifFOTBuilder::setupSimplePageSequence() {

    MifDoc::Page &firstMasterPage
     = *(new MifDoc::Page( MifDoc::sOtherMasterPage, MifDoc::sFirst ));
    MifDoc::Page &rightMasterPage
     = *(new MifDoc::Page( MifDoc::sRightMasterPage, MifDoc::sRight ));
    MifDoc::Page &leftMasterPage
     = *(new MifDoc::Page( MifDoc::sLeftMasterPage, MifDoc::sLeft ));
    MifDoc::Page &bodyPage
     = *(new MifDoc::Page( MifDoc::sBodyPage, MifDoc::sNONE, MifDoc::sFirst ));

    MifDoc::T_LTWH bodyRect;
    MifDoc::T_LTWH headerRect;
    MifDoc::T_LTWH footerRect;

    bodyRect.l = format().FotLeftMargin;
    bodyRect.t = format().FotTopMargin;
    bodyRect.w = format().FotPageWidth - format().FotLeftMargin - format().FotRightMargin;
    bodyRect.h = format().FotPageHeight - format().FotTopMargin - format().FotBottomMargin;

    headerRect.l = format().FotLeftMargin;
    headerRect.t = 0;
    headerRect.w = bodyRect.w;
    headerRect.h = format().FotTopMargin;

    footerRect.l = format().FotLeftMargin;
    footerRect.t = format().FotPageHeight - format().FotBottomMargin;

    footerRect.w = bodyRect.w;
    footerRect.h = format().FotBottomMargin;
   {
    MifDoc::TextRect &firstBodyTextRect
     = *(new MifDoc::TextRect( bodyRect, format().FotPageNColumns, format().FotPageColumnSep,
                               format().FotPageBalanceColumns ));
    MifDoc::TextRect &rightBodyTextRect
     = *(new MifDoc::TextRect( bodyRect, format().FotPageNColumns, format().FotPageColumnSep,
                               format().FotPageBalanceColumns));
    MifDoc::TextRect &leftBodyTextRect
     = *(new MifDoc::TextRect( bodyRect, format().FotPageNColumns, format().FotPageColumnSep,
                               format().FotPageBalanceColumns ));
    MifDoc::TextRect &bodyTextRect
     = *(new MifDoc::TextRect( bodyRect, format().FotPageNColumns, format().FotPageColumnSep,
                               format().FotPageBalanceColumns ));
    MifDoc::TextRect &firstHeaderTextRect = *(new MifDoc::TextRect( headerRect ));
    MifDoc::TextRect &rightHeaderTextRect = *(new MifDoc::TextRect( headerRect ));
    MifDoc::TextRect &leftHeaderTextRect = *(new MifDoc::TextRect( headerRect ));
    MifDoc::TextRect &firstFooterTextRect = *(new MifDoc::TextRect( footerRect ));
    MifDoc::TextRect &rightFooterTextRect = *(new MifDoc::TextRect( footerRect ));
    MifDoc::TextRect &leftFooterTextRect = *(new MifDoc::TextRect( footerRect ));

    firstMasterPage.TextRects.push_back( firstHeaderTextRect );
    firstMasterPage.TextRects.push_back( firstBodyTextRect );
    firstMasterPage.TextRects.push_back( firstFooterTextRect );

    rightMasterPage.TextRects.push_back( rightHeaderTextRect );
    rightMasterPage.TextRects.push_back( rightBodyTextRect );
    rightMasterPage.TextRects.push_back( rightFooterTextRect );

    leftMasterPage.TextRects.push_back( leftHeaderTextRect );
    leftMasterPage.TextRects.push_back( leftBodyTextRect );
    leftMasterPage.TextRects.push_back( leftFooterTextRect );

    bodyPage.TextRects.push_back( bodyTextRect );
    mifDoc.pages().push_back( bodyPage );
    mifDoc.pages().push_back( firstMasterPage );
    mifDoc.pages().push_back( rightMasterPage );
    mifDoc.pages().push_back( leftMasterPage );

    MifDoc::ParagraphFormat headerPF;
    headerPF.setDSSSLDefaults();
    MifDoc::ParagraphFormat footerPF;
    footerPF.setDSSSLDefaults();
    setupHeaderFooterParagraphFormat( headerPF, footerPF, bodyRect.w );

    FotSimplePageSequence.BodyTextFlow
     = new MifDoc::TextFlow( bodyTextRect, true, &FotSimplePageSequence.paragraphFormat,
                             MifDoc::sDefaultPgfFormat );
    FotSimplePageSequence.FirstHeaderTextFlow
     = new MifDoc::TextFlow( firstHeaderTextRect, false, &headerPF, MifDoc::sHeader );
    FotSimplePageSequence.FirstFooterTextFlow
     = new MifDoc::TextFlow( firstFooterTextRect, false, &footerPF, MifDoc::sFooter );
    FotSimplePageSequence.LeftHeaderTextFlow
     = new MifDoc::TextFlow( leftHeaderTextRect, false, &headerPF, MifDoc::sHeader );
    FotSimplePageSequence.LeftFooterTextFlow
     = new MifDoc::TextFlow( leftFooterTextRect, false, &footerPF, MifDoc::sFooter );
    FotSimplePageSequence.RightHeaderTextFlow
     = new MifDoc::TextFlow( rightHeaderTextRect, false, &headerPF, MifDoc::sHeader );
    FotSimplePageSequence.RightFooterTextFlow
     = new MifDoc::TextFlow( rightFooterTextRect, false, &footerPF, MifDoc::sFooter );

    makeEmptyTextFlow( firstBodyTextRect );
    makeEmptyTextFlow( leftBodyTextRect );
    makeEmptyTextFlow( rightBodyTextRect );

    mifDoc.textFlows().push_back( *FotSimplePageSequence.BodyTextFlow );
    mifDoc.textFlows().push_back( *FotSimplePageSequence.FirstHeaderTextFlow );
    mifDoc.textFlows().push_back( *FotSimplePageSequence.FirstFooterTextFlow );
    mifDoc.textFlows().push_back( *FotSimplePageSequence.LeftHeaderTextFlow );
    mifDoc.textFlows().push_back( *FotSimplePageSequence.LeftFooterTextFlow );
    mifDoc.textFlows().push_back( *FotSimplePageSequence.RightHeaderTextFlow );
    mifDoc.textFlows().push_back( *FotSimplePageSequence.RightFooterTextFlow );

    mifDoc.document().setDTwoSides( true );
    mifDoc.document().setDParity( MifDoc::sFirstRight );
  }

  mifDoc.enterTextFlow( *FotSimplePageSequence.BodyTextFlow );
}

void MifFOTBuilder::startSimplePageSequenceHeaderFooter( unsigned hfPart ) {

    if( firstHeaderFooter ) { setupSimplePageSequence(); firstHeaderFooter = false; }

    MifDoc::TextFlow *curTextFlow;

    if( hfPart & firstHF )
        if( hfPart & frontHF )
            if( hfPart & headerHF )
                curTextFlow = FotSimplePageSequence.FirstHeaderTextFlow;
            else
                curTextFlow = FotSimplePageSequence.FirstFooterTextFlow;
        else
            return;
    else
        if( hfPart & frontHF )
            if( hfPart & headerHF )
                curTextFlow = FotSimplePageSequence.RightHeaderTextFlow;
            else
                curTextFlow = FotSimplePageSequence.RightFooterTextFlow;
        else
            if( hfPart & headerHF )
                curTextFlow = FotSimplePageSequence.LeftHeaderTextFlow;
            else
                curTextFlow = FotSimplePageSequence.LeftFooterTextFlow;

    mifDoc.enterTextFlow( *curTextFlow );

    if( hfPart & (centerHF | rightHF) )
        mifDoc.outSpecialChar( MifDoc::sTab );
    else // leftHF
        (hfPart & headerHF) ? beginHeader() : beginFooter();
}

void MifFOTBuilder::endSimplePageSequenceHeaderFooter( unsigned hfPart ) {

    if( hfPart & rightHF && (hfPart & frontHF || !(hfPart & firstHF)) ) {
        endHeaderFooter();
    }

    if( !(hfPart & firstHF) || hfPart & frontHF )
        mifDoc.exitTextFlow();
}

void MifFOTBuilder::setupHeaderFooterParagraphFormat
 ( MifDoc::ParagraphFormat &hpf, MifDoc::ParagraphFormat &fpf,
   MifDoc::T_dimension textRectWidth ) {

    MifDoc::TabStop centerTS( MifDoc::sCenter, textRectWidth / 2 );
    MifDoc::TabStop rightTS( MifDoc::sRight, textRectWidth );

    hpf.setFrom( FotSimplePageSequence.paragraphFormat, 0, MifDoc::FontFormat::fAll );
    hpf.TabStops.push_back( centerTS );
    hpf.TabStops.push_back( rightTS );
    hpf.setProperties |= MifDoc::ParagraphFormat::fTabStops;
    fpf = hpf;
    hpf.setFSize( ( ( format().FotBottomMargin - format().FotFooterMargin ) * 3 ) / 2 );
    fpf.setFSize( ( format().FotHeaderMargin * 3 ) / 2 );
    hpf.setPgfTag( MifDoc::sHeader );
    fpf.setPgfTag( MifDoc::sFooter );

    mifDoc.pgfCatalog().ParaFormats.push_back( hpf );
    mifDoc.pgfCatalog().ParaFormats.push_back( fpf );

    hpf.clearSetProperties();
    fpf.clearSetProperties();
}

void MifFOTBuilder::endAllSimplePageSequenceHeaderFooter() {

//    mifDoc.enterTextFlow( *FotSimplePageSequence.BodyTextFlow );
}

inline void MifFOTBuilder::pageNumber() {

    mifDoc.outPageNumber();
}

// --------- MifFOTBuilder::Table ---------------------------------------------

void MifFOTBuilder::Table::resolveBorders
      ( Vector<Row> &rows, bool hasFirstTableRow, bool hasLastTableRow ) {

    bool isFirstRow;
    bool isLastRow;
    bool isFirstColumn;
    bool isLastColumn;
    Cell *cell = NULL;
    size_t r, c, rr, cc;
    bool leftEdge, topEdge;

    for( r = 0; r < rows.size(); r++ ) {
      for( c = 0; c < rows[r].Cells.size()-1; c++ ) {
        cell = &rows[r].Cells[c];
        if( cell->OverlappingCell == NULL ) {
          for( rr = r; rr < r + cell->nRowsSpanned; rr++ ) {
            for( cc = c, leftEdge = true; cc < c + cell->nColumnsSpanned; cc++ ) {
              rows[rr].Cells[cc].OverlappingCell = cell;
            }
          }
        }
      }
    }

    for( r = 0; r < rows.size(); r++ ) {
        for( c = 0; c < rows[r].Cells.size() - 1; c++ ) {
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

void MifFOTBuilder::Table::begin( MifDoc &mifDoc ) {

    CurCell = NULL;
    NoTablePartsSeen = true;

    TableParts.resize( 0 );
    TableParts.resize( 1 );
    TableParts.back().begin( *this, mifDoc );
}

// --------- MifFOTBuilder::TablePart -----------------------------------------

void MifFOTBuilder::TablePart::begin( Table &parent, MifDoc &mifDoc ) {

    Columns.resize( 0 );
    Header.resize( 0 );
    Body.resize( 0 );
    Footer.resize( 0 );
    
    columnsProcessed = false;
    needsColumnReprocessing = false;

    ParentTable = &parent;
    parentTable().CurRows = &Body;    
    parentTable().CurTablePart = this;
    
    if( MifTableNum == 0 ) {
        mifDoc.tbls().resize( mifDoc.tbls().size() + 1 );
        MifTableNum = mifDoc.tbls().size();
        MifDoc::CurInstance->setCurTblNum( MifTableNum );
    }
}

void MifFOTBuilder::TablePart::translate( MifDoc &mifDoc ) {

    if( needsColumnReprocessing )
        processColumns();

    MifDoc::Tbl &mifTbl = mifTable( mifDoc );
    if( parentTable().startIndent != 0 ) { // DSSSL default
        mifTbl.tblFormat.setTblLIndent( parentTable().startIndent );
        mifTbl.setProperties |= MifDoc::Tbl::fTblFormat;
    }

    if( parentTable().displayAlignment != symbolStart ) {
        mifTbl.setProperties |= MifDoc::Tbl::fTblFormat;
        MifDoc::T_keyword mifAlignment = MifDoc::sLeft;
        switch( parentTable().displayAlignment ) {
            case symbolStart:   mifAlignment = MifDoc::sLeft; break;
            case symbolEnd:     mifAlignment = MifDoc::sRight; break;
            case symbolCenter:  mifAlignment = MifDoc::sCenter; break;
            case symbolInside:  mifAlignment = MifDoc::sInside; break;
            case symbolOutside: mifAlignment = MifDoc::sOutside; break;
            default: assert( false );
        }
        mifTbl.tblFormat.setTblAlignment( mifAlignment );
    }

    bool putHeaderInBody = Body.size() == 0 && Header.size() > 0;
    bool putFooterInBody = !putHeaderInBody && Body.size() == 0 && Footer.size() > 0;
    for( size_t i = 0; i < Header.size(); i++ )
        Header[i].translate( putHeaderInBody ? mifTbl.TblBody : mifTbl.TblH, mifDoc );
    for( size_t i = 0; i < Body.size(); i++)
        Body[i].translate( mifTbl.TblBody, mifDoc );
    for( size_t i = 0; i < Footer.size(); i++ )
        Footer[i].translate( putFooterInBody ? mifTbl.TblBody : mifTbl.TblF, mifDoc );
}

void MifFOTBuilder::TablePart::processColumns() {

    MifDoc::Tbl &mifTbl = mifTable( *MifDoc::CurInstance );
    mifTbl.setTblNumColumns( 0 );
    mifTbl.setTblNumColumns( Columns.size() );
    mifTbl.TblColumnWidths.resize( 0 );
    mifTbl.TblColumnWidths.resize( Columns.size() );
    mifTbl.TblColumnWidthsAreSet();

    long totalNonproportionalWidth = 0L;
    double totalProportionalUnits = 0.0;
    for( size_t i = 0; i < Columns.size(); i++ ) {
        if( Columns[i].hasWidth ) {
            if( Columns[i].width.tableUnitFactor ) {
                totalProportionalUnits += Columns[i].width.tableUnitFactor;
            } else {
                mifTbl.TblColumnWidths[i]
                 = MifFOTBuilder::curInstance().computeLengthSpec( Columns[i].width );
                totalNonproportionalWidth += mifTbl.TblColumnWidths[i];
            }
        }
    }

    double proportionalUnit = 0.0;
    if( totalProportionalUnits )
        proportionalUnit
         = ( parentTable().tableWidth - totalNonproportionalWidth ) / totalProportionalUnits;
    
    for( size_t i = 0; i < Columns.size(); i++ ) {
        if( Columns[i].hasWidth ) {
            if( Columns[i].width.tableUnitFactor )
                mifTbl.TblColumnWidths[i]
                 = long(proportionalUnit * Columns[i].width.tableUnitFactor);
        } else 
            mifTbl.TblColumnWidths[i] = long(proportionalUnit);
    }

    columnsProcessed = true;
}

void MifFOTBuilder::TablePart::normalizeRows() {

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

// --------- MifFOTBuilder::Row -----------------------------------------------

void MifFOTBuilder::Row::translate( Vector<MifDoc::Row> &mifRows, MifDoc &mifDoc ) {

    mifRows.resize( mifRows.size() + 1 );
    MifDoc::Row &mifRow = mifRows.back();
    mifRow.Cells.resize( Cells.size() - 1 );
    for( size_t i = 0; i + 1 < Cells.size(); i++ ) {
        Cells[i].translate( Cells[i].mifCell(), mifDoc );
        mifRow.Cells[i] = Cells[i].mifCell();    
    }
}

// --------- MifFOTBuilder::Cell ----------------------------------------------

void MifFOTBuilder::Cell::translate( MifDoc::Cell &mifCell, MifDoc &mifDoc ) {

   MifDoc::T_tagstring rulingTag; 

   rulingTag = beforeRowBorder.makeMifRuling( mifDoc );
   if( rulingTag.size() )
       mifCell.setCellTRuling( rulingTag );

   rulingTag = afterRowBorder.makeMifRuling( mifDoc );
   if( rulingTag.size() )
       mifCell.setCellBRuling( rulingTag );

   rulingTag = beforeColumnBorder.makeMifRuling( mifDoc );
   if( rulingTag.size() )
       mifCell.setCellLRuling( rulingTag );

   rulingTag = afterColumnBorder.makeMifRuling( mifDoc );
   if( rulingTag.size() )
       mifCell.setCellRRuling( rulingTag );
}

// --------- MifFOTBuilder::Border --------------------------------------------

void MifFOTBuilder::Border::setFromFot() {

    MifFOTBuilder::Format &f = MifFOTBuilder::curInstance().format();
    borderPriority = f.FotBorderPriority;
    borderPresent = f.FotBorderPresent;
    lineThickness = f.FotLineThickness;
    lineRepeat = f.FotLineRepeat;
    lineSep  = f.FotLineSep;
    color = f.FColor;
}

MifDoc::T_tagstring MifFOTBuilder::Border::makeMifRuling( MifDoc &mifDoc ) {

    MifDoc::T_tagstring result;

    if( borderPresent ) {

        MifDoc::Ruling *mifRuling = new MifDoc::Ruling;
    
        mifRuling->setRulingPenWidth( lineThickness );
        mifRuling->setRulingLines( lineRepeat >= 2 ? 2 : lineRepeat );
        mifRuling->setRulingGap( lineRepeat >= 2 ? lineSep - lineThickness : 0 );
        mifRuling->setRulingColor( color );
    
        result = MifDoc::T_tagstring( mifRuling->key( *mifRuling ) );
        mifRuling->setRulingTag( result );

        if( mifDoc.rulingCatalog().Rulings.lookup( result ) == NULL ) {
            mifDoc.rulingCatalog().Rulings.insert( mifRuling );    
        } else
            delete mifRuling;
    }

    return result;
}

void MifFOTBuilder::Border::resolve( Border &adjacentBorder ) {
    
    if( adjacentBorder.borderPriority > borderPriority
         ||
        (   adjacentBorder.borderPriority == borderPriority 
         && !adjacentBorder.cellBorder
         && adjacentBorder.borderPresent ) ) {

        lineThickness = adjacentBorder.lineThickness;
        borderPresent = adjacentBorder.borderPresent;
        lineRepeat = adjacentBorder.lineRepeat;
        lineSep = adjacentBorder.lineSep;
        color = adjacentBorder.color;

        if( adjacentBorder.cellBorder )
            adjacentBorder.borderPresent = false;
    }
}

//-----------------------------------------------------------------------------

void MifFOTBuilder::setFontSize( long n ) {

    nextFormat.FSize = n;
}

void MifFOTBuilder::setLineSpacing( const LengthSpec &l ) {

    nextFormat.FotLineSpacingSpec = l;
}

void MifFOTBuilder::setFontWeight( Symbol weight ) {

    if( weight > symbolMedium ) {
        nextFormat.FWeight = MifDoc::sBold;
    } else {
        nextFormat.FWeight = MifDoc::sRegular;
    }
}

void MifFOTBuilder::setFontPosture( Symbol posture ) {

    switch( posture ) {

      case symbolOblique:
      case symbolBackSlantedOblique:
      case symbolItalic:
      case symbolBackSlantedItalic:

        nextFormat.FAngle = MifDoc::sItalic;
        break;

      default:
        nextFormat.FAngle = MifDoc::sRegular;
    }
}

inline void MifFOTBuilder::setStartIndent( const LengthSpec &l ) {

    nextFormat.FotStartIndentSpec = l;
}

inline void MifFOTBuilder::setEndIndent( const LengthSpec &l ) {

    nextFormat.FotEndIndentSpec = l;
}

inline void MifFOTBuilder::setFirstLineStartIndent( const LengthSpec &l ) {

    nextFormat.FotFirstLineStartIndentSpec = l;
}

void MifFOTBuilder::setQuadding( Symbol quadding ) {

    switch( quadding ) {

      case symbolEnd:

        nextFormat.PgfAlignment = MifDoc::sRight;
        break;

      case symbolCenter:

        nextFormat.PgfAlignment = MifDoc::sCenter;
        break;

      case symbolJustify:

        nextFormat.PgfAlignment = MifDoc::sLeftRight;
        break;

      default: // symbolStart
        nextFormat.PgfAlignment = MifDoc::sLeft;
    }
}

inline void MifFOTBuilder::setKern( bool kern ) {

    nextFormat.FPairKern = kern;
}

inline void MifFOTBuilder::setLanguage( Letter2 code ) {

    nextFormat.FotLanguage = code;
    nextFormat.computePgfLanguage();
}

inline void MifFOTBuilder::setCountry( Letter2 code ) {

    nextFormat.FotCountry = code;
    nextFormat.computePgfLanguage();
}

inline void MifFOTBuilder::setHyphenate( bool hyphenate ) {

    nextFormat.PgfHyphenate = hyphenate;
}

inline void MifFOTBuilder::setHyphenationRemainCharCount( long n ) {

    nextFormat.HyphenMinPrefix = n;
}

inline void MifFOTBuilder::setHyphenationPushCharCount( long n ) {

    nextFormat.HyphenMinSuffix = n;
}

inline void MifFOTBuilder::setHyphenationLadderCount( long n ) {

    nextFormat.HyphenMaxLines = n;
}

void MifFOTBuilder::setFontFamilyName( const StringC &s ) {

    nextFormat.FFamily.resize( 0 );
    for( size_t i = 0; i < s.size(); i++ )
        nextFormat.FFamily += char(s[i]);
}

inline void MifFOTBuilder::setWidowCount( long n ) {

    nextFormat.PgfBlockSize = n;
}

inline void MifFOTBuilder::setOrphanCount( long n ) {

    nextFormat.PgfBlockSize = n;
}

inline void MifFOTBuilder::setPageWidth( Length pWidth ) {

    nextFormat.FotPageWidth = pWidth;
}

inline void MifFOTBuilder::setPageHeight( Length pHeight ) {

    nextFormat.FotPageHeight = pHeight;
}

inline void MifFOTBuilder::setLeftMargin( Length leftM ) {

    nextFormat.FotLeftMargin = leftM;
}

inline void MifFOTBuilder::setRightMargin( Length rightM ) {

    nextFormat.FotRightMargin = rightM;
}

inline void MifFOTBuilder::setTopMargin( Length topM ) {

    nextFormat.FotTopMargin = topM;
}

inline void MifFOTBuilder::setBottomMargin( Length bottomM ) {

    nextFormat.FotBottomMargin = bottomM;
}

void MifFOTBuilder::setHeaderMargin( Length headerM ) {

    nextFormat.FotHeaderMargin = headerM;
}

void MifFOTBuilder::setFooterMargin( Length footerM ) {

    nextFormat.FotFooterMargin = footerM;
}

void MifFOTBuilder::setDisplayAlignment( Symbol alignment ) {

    nextFormat.FotDisplayAlignment = alignment;
}

//--------- LEADERS -----------------------------------------------------------

void MifFOTBuilder::startLeader( const LeaderNIC & ) {

    checkForParagraphReopening();
    lastFlowObjectWasWhitespace = false;

    mifDoc.outSpecialChar( MifDoc::sTab );
    inLeader = true;
    setCurLeaderStream( new MifTmpOutputByteStream() );
}

void MifFOTBuilder::endLeader() {

    if( !mifDoc.curPara()->leaderTabsSet ) {
 
        String<char> leaderStr;
        curLeaderStream().commit( leaderStr );

        int numTabs = mifDoc.curFormat().PgfNumTabs + 1;
        mifDoc.curFormat().setPgfNumTabs( numTabs );
        mifDoc.curPara()->format().TabStops.push_back
         ( MifDoc::TabStop( MifDoc::sRight, format().FotCurDisplaySize
                                             - mifDoc.curFormat().PgfRIndent - 1,
                            MifDoc::T_string( leaderStr ) ) );
        mifDoc.curPara()->format().setPgfNumTabs( numTabs );
        mifDoc.curPara()->curFormat().setPgfNumTabs( numTabs );
        mifDoc.curPara()->format().setProperties |= MifDoc::ParagraphFormat::fTabStops;

        mifDoc.curPara()->leaderTabsSet = true;
    }

    if( CurLeaderStream ) {
        delete CurLeaderStream;
        CurLeaderStream = NULL;
    }
    inLeader = false;
    lastFlowObjectWasWhitespace = false;
}

//--------- LINE FIELDS -------------------------------------------------------

void MifFOTBuilder::startLineField( const LineFieldNIC & ) {

    checkForParagraphReopening();
    lastFlowObjectWasWhitespace = false;

    Length fieldWidth = computeLengthSpec( nextFormat.FotFieldWidth );
    Length firstLineIndent = mifDoc.curPara()->format().PgfFIndent;
    bool leadingTab = true;
    switch( nextFormat.FotFieldAlign ) {
        case symbolStart: default:
            mifDoc.curPara()->format().TabStops.push_back
             ( MifDoc::TabStop( MifDoc::sLeft, firstLineIndent + fieldWidth ) );
            leadingTab = false;
            break;
        case symbolEnd:
            mifDoc.curPara()->format().TabStops.push_back
             ( MifDoc::TabStop( MifDoc::sRight, firstLineIndent + fieldWidth ) );
            mifDoc.curPara()->format().TabStops.push_back
             ( MifDoc::TabStop( MifDoc::sLeft, firstLineIndent + fieldWidth + 1 ) );
            break;
        case symbolCenter:
            mifDoc.curPara()->format().TabStops.push_back
             ( MifDoc::TabStop( MifDoc::sCenter, firstLineIndent + fieldWidth / 2 ) );
            mifDoc.curPara()->format().TabStops.push_back
             ( MifDoc::TabStop( MifDoc::sLeft, firstLineIndent + fieldWidth + 1 ) );
    }


    int numTabs = mifDoc.curFormat().PgfNumTabs + leadingTab ? 2 : 1;
    mifDoc.curFormat().setPgfNumTabs( numTabs );
    mifDoc.curPara()->format().setPgfNumTabs( numTabs );
    mifDoc.curPara()->curFormat().setPgfNumTabs( numTabs );
    mifDoc.curPara()->format().setProperties |= MifDoc::ParagraphFormat::fTabStops;

    start();

    if( leadingTab )
        mifDoc.outSpecialChar( MifDoc::sTab );
}

void MifFOTBuilder::endLineField() {

    mifDoc.outSpecialChar( MifDoc::sTab );
    end();
    lastFlowObjectWasWhitespace = false;
}

//--------- PARAGRAPH BREAKS --------------------------------------------------

void MifFOTBuilder::paragraphBreak( const ParagraphNIC &nic ) {

    if( MifDoc::Para::currentlyOpened ) {
        if( paragraphBreakInEffect )
            doEndParagraph( false, false, false );
        else {
            doEndParagraph( true, false, false );
            paragraphBreakInEffect = true;
        }
        doStartParagraph( nic );
    }
}

//--------- SCORES ------------------------------------------------------------

void MifFOTBuilder::startScore( Symbol scoreType ) {

    checkForParagraphReopening();

    switch( scoreType ) {
        case symbolBefore:
            nextFormat.setFOverline( true );
            break;
        case symbolThrough:
            nextFormat.setFStrike( true );
            break;
        case symbolAfter: default:
            nextFormat.setFUnderlining
             ( nextFormat.FotLineRepeat > 1 ? MifDoc::sFDouble : MifDoc::sFSingle );
            break;
    }

    start();
}

void MifFOTBuilder::endScore() {

    end();
}

//--------- ANCHORED FRAMES (GENERAL) -----------------------------------------

MifDoc::Frame &MifFOTBuilder::makeAnchoredFrame
 ( MifDoc::T_keyword frameType, long width, long height, MifDoc::T_keyword anchorAlign ) {

    mifDoc.aFrames().resize( mifDoc.aFrames().size() + 1 );
    MifDoc::Frame &frame = mifDoc.aFrames().back();

    frame.setFrameType( frameType );
    frame.setAnchorAlign( anchorAlign );
    frame.setShapeRect( MifDoc::T_LTWH( 0,  0, width, height ) );
 
    return frame;
}

//--------- EXTERNAL GRAPHICS -------------------------------------------------

void MifFOTBuilder::externalGraphic( const ExternalGraphicNIC &nic ) {

    bool isInline = MifDoc::Para::currentlyOpened ? true : false;

    if( !isInline )
        startDisplay( nic );
    start();

    MifDoc::T_pathname mifPathname;
    if( systemIdToMifPathname( nic.entitySystemId, mifPathname ) ) {


        MifDoc::T_keyword mifAlignment = MifDoc::sLeft;
        switch( format().FotDisplayAlignment ) {
            case symbolStart:   mifAlignment = MifDoc::sLeft; break;
            case symbolEnd:     mifAlignment = MifDoc::sRight; break;
            case symbolCenter:  mifAlignment = MifDoc::sCenter; break;
            case symbolInside:  mifAlignment = MifDoc::sInside; break;
            case symbolOutside: mifAlignment = MifDoc::sOutside; break;
            default: assert( false );
        }

        MifDoc::Frame &frame
         = makeAnchoredFrame( isInline ? MifDoc::sInline : MifDoc::sBelow,
                              nic.hasMaxWidth ? computeLengthSpec( nic.maxWidth ) : 72000,
                              nic.hasMaxHeight ? computeLengthSpec( nic.maxHeight ) : 72000,
                              mifAlignment );

        MifDoc::ImportObject *importObject
         = new MifDoc::ImportObject( mifPathname, frame.ShapeRect );        
        frame.Objects.resize( frame.Objects.size() + 1 );
        frame.Objects.back() = importObject;

        if( !isInline )
            doStartParagraph( nic, true, 0, false );
        else
            checkForParagraphReopening();
        mifDoc.outAFrame( frame.ID, mifDoc.os() );
        if( !isInline )
            endParagraph();
        else
            lastFlowObjectWasWhitespace = false;
    }

    end();
    if( !isInline )
        endDisplay();
}

enum TComponent { cName, cUp, cRoot, cRootDrive };
void addComponent( MifDoc::T_pathname &target, TComponent cType, StringC &component ) {

    target += '<';
    target += cType == cName ? 'c' : ( cType == cUp ? 'u' : 'r' );
    target += '\\';
    target += '>';
    if( cType == cName || cType == cRootDrive )
        for( size_t i = 0; i < component.size(); i++ )
            target += char( component[i] );
}

bool MifFOTBuilder::systemIdToMifPathname
 ( const StringC &systemId, MifDoc::T_pathname &mifPathname ) {

    StringC filename;
    StringC component;
    int result;

    if( ( result = systemIdFilename( systemId, filename ) ) < 0 ) {
        App->message( MifMessages::systemIdNotFilename, StringMessageArg( systemId ) );
        return false;
    } else {
        bool firstComponent = true;
        size_t i = 0;
        do {
            component.resize( 0 );
            while( i < filename.size() && filename[i] != '\\' && filename[i] != '/' )
                    component += char( filename[i++] );
            switch( component.size() ) {
                case 2:
                    if( firstComponent && component[1] == ':' )
                        addComponent( mifPathname, cRootDrive, component );
                    else
                    if( component[0] == '.' && component[1] == '.' )
                        addComponent( mifPathname, cUp, component );
                    else 
                        goto add_component;
                    break;
                case 1:
                    if( component[0] != '.' )
                        goto add_component;
                    break;
                case 0:
                    if( firstComponent && filename.size() > 0 )
                        addComponent( mifPathname, cRoot, component );
                    break;
                add_component: default:
                    addComponent( mifPathname, cName, component );
            }
            firstComponent = false;
            i++;
        } while( i < filename.size() );
    }

    return result == 0 ? false : true;
}

// System Id Resolution from James Clark

// Return -1 if an error should be generated
// FIXME in some cases should copy the entity into a file in the same
// directory as the output file.

int MifFOTBuilder::systemIdFilename(const StringC &systemId, StringC &filename)
{
  if (systemId.size() == 0)
    return -1;
  Owner<InputSource> in(EntityManager->open(systemId,
					     *SystemCharset,
					     InputSourceOrigin::make(),
					     0,
					     *App));
  if (!in)
    return 0;
  Xchar c = in->get(*App);
  StorageObjectLocation soLoc;
  ParsedSystemId parsedBuf;
  if (c == InputSource::eE && in->accessError()) {
    if (!EntityManager->parseSystemId(systemId, *SystemCharset, 0, 0, *App, parsedBuf))
      return 0;
    if (parsedBuf.size() != 1 || parsedBuf[0].baseId.size())
      return 0;
    soLoc.storageObjectSpec = &parsedBuf[0];
    soLoc.actualStorageId = parsedBuf[0].specId;
  }
  else {
    const Location &loc = in->currentLocation();
    if (loc.origin().isNull())
      return -1;
    const InputSourceOrigin *tem = loc.origin()->asInputSourceOrigin();
    if (!tem)
      return -1;
    const ParsedSystemId *psi
      = ExtendEntityManager::externalInfoParsedSystemId(tem->externalInfo());
    if (!psi || psi->size() != 1)
      return -1;
    if (!ExtendEntityManager::externalize(tem->externalInfo(), 0, soLoc))
      return -1;
  }
  if (strcmp(soLoc.storageObjectSpec->storageManager->type(), "OSFILE") != 0)
    return -1;
  filename = soLoc.actualStorageId;
  return 1;
}

//--------- RULES -------------------------------------------------------------

void MifFOTBuilder::rule( const RuleNIC &nic ) {

    bool isInline = ( nic.orientation == symbolHorizontal || nic.orientation == symbolVertical )
                     ? false : true;

    if( isInline )
        checkForParagraphReopening();

    if( !isInline )
        startDisplay( nic );
    start();

    long indentlessDisplaySize
          = format().FotCurDisplaySize - computeLengthSpec( format().FotStartIndentSpec )
            - computeLengthSpec( format().FotEndIndentSpec );

    MifDoc::T_dimension ruleHeight
     = format().FotLineThickness + format().FotLineSep * ( format().FotLineRepeat - 1 );
    if( ruleHeight < 0 )
        ruleHeight = 0;
        
    MifDoc::T_dimension ruleLength;
    MifDoc::T_dimension ruleOffset;

    if( nic.hasLength && ( ruleLength = computeLengthSpec( nic.length ) ) > 0 ) {
        switch( format().FotDisplayAlignment ) {
            case symbolStart: default:
                ruleOffset = 0;
                break;
            case symbolCenter:
                ruleOffset = ( indentlessDisplaySize - ruleLength ) / 2;
                break;
            case symbolEnd:
                ruleOffset = indentlessDisplaySize - ruleLength;
        }
    } else {
        ruleOffset = 0;
        ruleLength = indentlessDisplaySize;
    }

    MifDoc::Frame &frame
     = makeAnchoredFrame( MifDoc::sInline, isInline ? long(ruleLength) : indentlessDisplaySize,
                          ruleHeight );

   if( isInline )
        frame.setBLOffset( computeLengthSpec( format().FotPositionPointShiftSpec ) );
    else
        frame.setBLOffset( -ruleHeight / 2 + 4000 / 3 ); // min font baseline correction

    MifDoc::T_keyword capType;
    switch( format().FotLineCap ) {
        case symbolButt: default: capType = MifDoc::sButt; break;
        case symbolRound: capType = MifDoc::sRound; break;
        case symbolSquare: capType = MifDoc::sSquare;
    }
    MifDoc::PolyLine *polyLine;
    Length curLineVOffset = format().FotLineThickness / 2;
    for( long i = format().FotLineRepeat; i > 0; i--, curLineVOffset += format().FotLineSep ) {

        polyLine = new MifDoc::PolyLine( capType, 0, 0, format().FotLineThickness,
                                         format().FColor );        
        frame.Objects.resize( frame.Objects.size() + 1 );
        frame.Objects.back() = polyLine;

        polyLine->setHeadCap( capType );
        polyLine->setTailCap( capType );
        polyLine->Points.resize( polyLine->Points.size() + 1 );
        polyLine->Points.back() = MifDoc::T_XY( ruleOffset, curLineVOffset );
        polyLine->Points.resize( polyLine->Points.size() + 1 );
        polyLine->Points.back() = MifDoc::T_XY( ruleOffset + ruleLength, curLineVOffset );
    }

    if( !isInline ) doStartParagraph( nic, true, 0, true );
    mifDoc.outAFrame( frame.ID, mifDoc.os() );
    if( !isInline )
        endParagraph();
    else
        lastFlowObjectWasWhitespace = false;

    end();
    if( !isInline )
        endDisplay();
}

//--------- TABLES ------------------------------------------------------------

void MifFOTBuilder::startTable( const TableNIC &nic ) {

    startDisplay( nic );
    start();

    if( !curTable().DefaultTblFormatGenerated ) {
    
        MifDoc::TblFormat defaultTblFormat( MifDoc::sDefaultTblFormat );
        defaultTblFormat.setDSSSLDefaults();
        mifDoc.tblCatalog().TblFormats.push_back( defaultTblFormat );
        curTable().DefaultTblFormatGenerated = true;
    }

    Length curStartIndent = computeLengthSpec( format().FotStartIndentSpec );
    curTable().startIndent = curStartIndent;

    curTable().begin( mifDoc );
//    curTable().nic = nic;
    curTable().displayAlignment = format().FotDisplayAlignment;

    if( nic.widthType == TableNIC::widthExplicit )
        curTable().tableWidth = computeLengthSpec( nic.width );
    else
        curTable().tableWidth
         = format().FotCurDisplaySize - curStartIndent
            - computeLengthSpec( format().FotEndIndentSpec );


    doStartParagraph( nic, true, 0 );
    endParagraph();
}

void MifFOTBuilder::endTable() {

    bool firstPart, lastPart, hasHeader, hasFooter;
    for( size_t i = 0; i < curTable().TableParts.size(); i++ ) {
        firstPart = ( i == 0 ) ? true : false;
        lastPart = ( i == curTable().TableParts.size() - 1 ) ? true : false;
        TablePart &tablePart = curTable().TableParts[i];
        tablePart.normalizeRows();
        hasHeader = tablePart.Header.size() > 0 ? true : false;
        hasFooter = tablePart.Footer.size() > 0 ? true : false;
        if( hasHeader )
            curTable().resolveBorders( tablePart.Header,
                                       firstPart ? true : false, false );
        curTable().resolveBorders( tablePart.Body,
                                   hasHeader ? false : true, hasFooter ? false : true );
        if( hasFooter )
            curTable().resolveBorders( tablePart.Footer,
                                       false, lastPart ? true : false );
        tablePart.translate( mifDoc );
    }

    MifDoc::CurInstance->setCurTblNum( 0 );

    endDisplay();
    end();
}

void MifFOTBuilder::tableColumn( const TableColumnNIC &nic ) {

    if( nic.columnIndex >= curTable().curTablePart().Columns.size() )
        curTable().curTablePart().Columns.resize( nic.columnIndex + 1 );
  
    curTable().curTablePart().Columns[nic.columnIndex].hasWidth = nic.hasWidth;
    if( nic.hasWidth ) {
        curTable().curTablePart().Columns[nic.columnIndex].width = nic.width;
    }
}

void MifFOTBuilder::startTableRow() {

    curTable().curRows().resize( curTable().curRows().size() + 1 );
}

void MifFOTBuilder::endTableRow() {

}

void MifFOTBuilder::startTableCell( const TableCellNIC &nic ) {

    start();

    TablePart &tp = curTable().curTablePart();
    if( !tp.columnsProcessed )
        tp.processColumns();
   
    Vector<Cell> &Cells = curTable().curRows().back().Cells;
    if( nic.columnIndex >= Cells.size() )
        Cells.resize( nic.columnIndex + 1 );

    Cell &cell = Cells[nic.columnIndex];
    curTable().CurCell = &cell;
    cell.missing = nic.missing;

    if( nic.nColumnsSpanned != 1 ) {
        cell.nColumnsSpanned = nic.nColumnsSpanned;
        cell.mifCell().setCellColumns( nic.nColumnsSpanned );
    }

    if( nic.nRowsSpanned != 1 ) {
        cell.nRowsSpanned = nic.nRowsSpanned;
        cell.mifCell().setCellRows( nic.nRowsSpanned );
    }

    if( format().FotCellBackground && format().MifBackgroundColor.size() > 0 ) {
        cell.mifCell().setCellFill( 0 );
        cell.mifCell().setCellColor( format().MifBackgroundColor );
    }

    long newDisplaySize = 0;
    for( size_t i = nic.columnIndex; i < nic.columnIndex + nic.nColumnsSpanned; i++ )
        if( i < tp.Columns.size() ) {
            if( tp.Columns[i].hasWidth )
                newDisplaySize
                 += computeLengthSpec( tp.Columns[i].width );
        } else
        if( !nic.missing ) {
            App->message( MifMessages::missingTableColumnFlowObject );  
            // NOTE: at this point there's already a danger of not realizing
            // right display space sizes inside cells
            tp.Columns.resize( tp.Columns.size() + 1 ); 
            tp.Columns.back().hasWidth = true;
            TableLengthSpec tls; tls.tableUnitFactor = 1.0;
            tp.Columns.back().width = tls;
            tp.needsColumnReprocessing = true;
            if( (MifDoc::T_integer)i > tp.mifTable( mifDoc ).TblNumColumns )
                tp.mifTable( mifDoc ).setTblNumColumns( i );
        }

    newDisplaySize -= format().PgfCellMargins.l + format().PgfCellMargins.r;

    if( newDisplaySize > 0 )
        format().FotCurDisplaySize = newDisplaySize;

    cell.displaySize = format().FotCurDisplaySize;
    mifDoc.enterTableCell( cell.mifCell() ); 
}

void MifFOTBuilder::endTableCell() {

    mifDoc.exitTableCell();
    end();

    curTable().CurCell = NULL;
}

void MifFOTBuilder::startTablePartSerial( const TablePartNIC &nic ) {

    startDisplay( nic );
    start();

    if( curTable().NoTablePartsSeen )
        curTable().NoTablePartsSeen = false;
    else
        curTable().TableParts.resize( curTable().TableParts.size() + 1 );

    curTable().TableParts.back().begin( curTable(), mifDoc );    

    doStartParagraph( nic, true, 0 );
    endParagraph();
}
 
void MifFOTBuilder::endTablePartSerial() {

    curTable().CurTablePart = NULL;
    endDisplay();
    end();
}

void MifFOTBuilder::startTablePartHeader() {

    curTable().CurRows = &curTable().curTablePart().Header; 
}

void MifFOTBuilder::endTablePartHeader() {

    curTable().CurRows = &curTable().curTablePart().Body; 
}

void MifFOTBuilder::startTablePartFooter() {

    curTable().CurRows = &curTable().curTablePart().Footer; 
}

void MifFOTBuilder::endTablePartFooter() {

    curTable().CurRows = &curTable().curTablePart().Body; 
}

void MifFOTBuilder::setCellBeforeRowMargin( Length l ) {

    nextFormat.PgfCellMargins.t = l;
}

void MifFOTBuilder::setCellAfterRowMargin( Length l ) {

    nextFormat.PgfCellMargins.b = l;
}

void MifFOTBuilder::setCellBeforeColumnMargin( Length l ) {

    nextFormat.PgfCellMargins.l = l;
}

void MifFOTBuilder::setCellAfterColumnMargin( Length l ) {

    nextFormat.PgfCellMargins.r = l;
}

void MifFOTBuilder::setCellRowAlignment( Symbol alignment ) {

    switch( alignment ) {
        case symbolEnd:     nextFormat.PgfCellAlignment = MifDoc::sBottom; break;
        case symbolCenter:  nextFormat.PgfCellAlignment = MifDoc::sMiddle; break;
        default:            nextFormat.PgfCellAlignment = MifDoc::sTop; break;
    }
}

//--------- TABLE BORDERS -----------------------------------------------------

void MifFOTBuilder::setBorderPresent( bool b ) {

    nextFormat.FotBorderPresent = b;
}

void MifFOTBuilder::setBorderPriority( long p ) {

    nextFormat.FotBorderPriority = p;
}

void MifFOTBuilder::setBorderOmitAtBreak(bool) {
}

void MifFOTBuilder::tableBeforeRowBorder() {

    start();
    curTable().beforeRowBorder.setFromFot();
    end();
}

void MifFOTBuilder::tableAfterRowBorder() {

    start();
    curTable().afterRowBorder.setFromFot();
    end();
}

void MifFOTBuilder::tableBeforeColumnBorder() {

    start();
    curTable().beforeColumnBorder.setFromFot();
    end();
}

void MifFOTBuilder::tableAfterColumnBorder() {

    start();
    curTable().afterColumnBorder.setFromFot();
    end();
}

void MifFOTBuilder::tableCellBeforeRowBorder() {

    start();
    curTable().curCell().beforeRowBorder.setFromFot();
    end();
}

void MifFOTBuilder::tableCellAfterRowBorder() {

    start();
    curTable().curCell().afterRowBorder.setFromFot();
    end();
}

void MifFOTBuilder::tableCellBeforeColumnBorder() {

    start();
    curTable().curCell().beforeColumnBorder.setFromFot();
    end();
}

void MifFOTBuilder::tableCellAfterColumnBorder() {

    start();
    curTable().curCell().afterColumnBorder.setFromFot();
    end();
}


//--------- CROSS-REFERENCES, LINKS, AND INDEX-ENTRIES ------------------------

void MifFOTBuilder::outPendingInlineStatements() {

    if(    linkStack.size() > 1
        && linkStack[linkStack.size()-2].openedInMif
        && !linkStack[linkStack.size()-2].forcesNoLink() ) {
        LinkInfo::pendingMifClosings++;
        linkStack[linkStack.size()-2].openedInMif = false;
    }

    for( ; LinkInfo::pendingMifClosings > 0; LinkInfo::pendingMifClosings-- ) {
        MifDoc::Marker marker( MifDoc::T_string( "" ), MifDoc::Marker::Hypertext );
        marker.out( mifDoc.os() );
    }

    if( indexEntryStack.size() > 0 ) {
        indexEntryStack.back().out( mifDoc.os() );
        indexEntryStack.resize( indexEntryStack.size() - 1 );
    }

    for( size_t i = 0; i < nodeStack.size(); i++ ) {
        unsigned long n;
        if( nodeStack[i].node->elementIndex( n ) == accessOK ) {
            GroveString id; 
            nodeStack[i].node->getId( id );
            unsigned long groveIndex = nodeStack[i].node->groveIndex();
            mifDoc.os() << MifDoc::CrossRefInfo
                          ( groveIndex, n, mifDoc.os().CurTagIndent,
                            MifDoc::CrossRefInfo::PotentialMarker, id.data(), id.size() );
            if( id.size() > 0 )
                mifDoc.elements().setBookComponentIndex
                 ( groveIndex, StringC( id.data(), id.size() ),
                   mifDoc.bookComponents().size() - 1 );
            else
                mifDoc.elements().setBookComponentIndex
                 ( groveIndex, n, mifDoc.bookComponents().size() - 1 );
        }
    }
    NodeInfo::nonEmptyElementsOpened = 0;
    nodeStack.resize(0);

    if(    linkStack.size() > 0
        && !linkStack.back().openedInMif
        && !linkStack.back().forcesNoLink() ) {
        linkStack.back().crossRefInfo->tagIndent = mifDoc.os().CurTagIndent;
        mifDoc.os() << *linkStack.back().crossRefInfo;
        linkStack.back().openedInMif = true;
    }
}

//--------- LINKS -------------------------------------------------------------

void MifFOTBuilder::startLink( const Address &address ) {

    switch( address.type ) {
      case Address::resolvedNode: {
        unsigned long n;
        if( address.node->elementIndex( n ) == accessOK ) {
            GroveString id; 
            address.node->getId( id );
            unsigned long groveIndex = address.node->groveIndex();
            linkStack.resize( linkStack.size() + 1 );
            linkStack.back().crossRefInfo
             = new MifDoc::CrossRefInfo
                    ( groveIndex, n, 0, MifDoc::CrossRefInfo::HypertextLink,
                      id.data(), id.size() );
            if( id.size() > 0 )
                mifDoc.elements().setReferencedFlag
                 ( MifDoc::ElementSet::LinkReference, groveIndex,
                   StringC( id.data(), id.size() ) );
            else
                mifDoc.elements().setReferencedFlag
                 ( MifDoc::ElementSet::LinkReference, groveIndex, n );
        }
        break; }

      case Address::idref: {
        const StringC &id = address.params[0];
        size_t i;
        for( i = 0; i < id.size(); i++ )
            if( id[i] == ' ')
                break;
        linkStack.resize( linkStack.size() + 1 );
        linkStack.back().crossRefInfo
         = new MifDoc::CrossRefInfo
                ( address.node->groveIndex(), 0, 0, MifDoc::CrossRefInfo::HypertextLink,
                  id.data(), i );
        mifDoc.elements().setReferencedFlag
          ( MifDoc::ElementSet::LinkReference, address.node->groveIndex(),
            StringC( id.data(), i ) );
        break; }

      case Address::none: default:
        linkStack.resize( linkStack.size() + 1 );
    }
}

void MifFOTBuilder::endLink() {

    assert( linkStack.size() > 0 );
    if( linkStack.back().openedInMif )
        LinkInfo::pendingMifClosings++;
    linkStack.resize( linkStack.size() - 1 );

    // MifDoc::Marker marker( MifDoc::T_string( "" ), MifDoc::Marker::Hypertext );
    // marker.out( mifDoc.os() );
}

//--------- CROSS-REFERENCES --------------------------------------------------

void MifFOTBuilder::startNode( const NodePtr &node, const StringC &mode ) {

    NodeInfo::curNodeLevel++;
    if( mode.size() == 0 )
        nodeStack.push_back( NodeInfo( node, NodeInfo::curNodeLevel ) );
}

void MifFOTBuilder::endNode() {

    if( nodeStack.size() > 0 && nodeStack.back().nodeLevel == NodeInfo::curNodeLevel
         && NodeInfo::nonEmptyElementsOpened < nodeStack.size() )
        nodeStack.resize( nodeStack.size() - 1 );
    NodeInfo::curNodeLevel--;
}

void MifFOTBuilder::currentNodePageNumber( const NodePtr &node ) {

    unsigned long n;
    if( node->elementIndex( n ) == accessOK ) {
        GroveString id; 
        node->getId( id );
        if( !mifDoc.bookComponent().pageNumXRefFormatGenerated ) {
            mifDoc.bookComponent().XRefFormats.push_back
             ( MifDoc::XRefFormat( MifDoc::sPageNumXRefFormatName, "<$pagenum\\>" ) );
            mifDoc.bookComponent().pageNumXRefFormatGenerated = true;
        }
        unsigned long groveIndex = node->groveIndex();
        mifDoc.os() << MifDoc::CrossRefInfo
                        ( groveIndex, n, mifDoc.os().CurTagIndent,
                          MifDoc::CrossRefInfo::XRef, id.data(), id.size() );
        if( id.size() > 0 )
            mifDoc.elements().setReferencedFlag
             ( MifDoc::ElementSet::PageReference, groveIndex, StringC( id.data(), id.size() ) );
        else
            mifDoc.elements().setReferencedFlag
             ( MifDoc::ElementSet::PageReference, groveIndex, n );
    }
}

//--------- INDEX-ENTRIES -----------------------------------------------------

void MifFOTBuilder::indexEntry( const IndexEntryNIC &nic ) {

    MifDoc::T_string mText;
    if( nic.components.size() > 0 ) {
        if( !nic.pageNumber )
            mText += MifDoc::T_string( "<$nopage>" );
        if( nic.startsPageRange )
            mText += MifDoc::T_string( "<$startrange>" );
        if( nic.endsPageRange )
            mText += MifDoc::T_string( "<$endrange>" );
        bool first = true;
        for( size_t i = 0; i < nic.components.size(); first = false, i++ ) {
            if( !first ) mText += ':';
            for( size_t ii = 0; ii < nic.components[i].size(); ii++ )
                mText += char( nic.components[i][ii] );
        }
        if( nic.sortString.size() > 0 ) {
            mText += '[';
            for( size_t i = 0; i < nic.sortString.size(); i++ )
                mText += char( nic.sortString[i] );
            mText += ']';
        }
        indexEntryStack.resize( indexEntryStack.size() + 1 );
        indexEntryStack.back() = MifDoc::Marker( mText, MifDoc::Marker::Index );
    }
}

//-----------------------------------------------------------------------------

void MifFOTBuilder::setCellBackground( bool to ) {

    nextFormat.FotCellBackground = to;
}

void MifFOTBuilder::setBackgroundColor( const DeviceRGBColor &rgbColor ) {

    MifDoc::Color *color = new MifDoc::Color( rgbColor.red, rgbColor.green, rgbColor.blue );
    nextFormat.MifBackgroundColor = color->ColorTag;
    if( mifDoc.colorCatalog().Colors.lookup( color->ColorTag ) == NULL ) {
        mifDoc.colorCatalog().Colors.insert( color );    
    } else
        delete color;
}

void MifFOTBuilder::setBackgroundColor() {

    nextFormat.MifBackgroundColor = MifDoc::T_tagstring();
}

void MifFOTBuilder::setColor( const DeviceRGBColor &rgbColor ) {

    MifDoc::Color *color = new MifDoc::Color( rgbColor.red, rgbColor.green, rgbColor.blue );
    nextFormat.FColor = color->ColorTag;
    if( mifDoc.colorCatalog().Colors.lookup( color->ColorTag ) == NULL ) {
        mifDoc.colorCatalog().Colors.insert( color );    
    } else
        delete color;
}

void MifFOTBuilder::setSpan( long n ) {

    nextFormat.FotSpan = n;
}

void MifFOTBuilder::setPageNColumns( long n ) {

    nextFormat.FotPageNColumns = n;
    mifDoc.document().setDColumns( n );
}

void MifFOTBuilder::setPageColumnSep( Length l ) {

    nextFormat.FotPageColumnSep = l;
    mifDoc.document().setDColumnGap( l );
}

void MifFOTBuilder::setPageBalanceColumns( bool b ) {

    nextFormat.FotPageBalanceColumns = b;
}

void MifFOTBuilder::setLines( Symbol s ) {

    nextFormat.FotLines = s;
}

void MifFOTBuilder::setInputWhitespaceTreatment( Symbol s ) {

    nextFormat.FotInputWhitespaceTreatment = s;
}

void MifFOTBuilder::setMinLeading( const OptLengthSpec &ol ) {

    nextFormat.FotMinLeading = ol;
}

void MifFOTBuilder::setPositionPointShift( const LengthSpec &l ) {

    nextFormat.FotPositionPointShiftSpec = l;
}

void MifFOTBuilder::setLineThickness( Length l ) {

    nextFormat.FotLineThickness = l;
}

void MifFOTBuilder::setLineRepeat( long n ) {

    nextFormat.FotLineRepeat = n;
}

void MifFOTBuilder::setLineSep( Length l ) {

    nextFormat.FotLineSep = l;
}

void MifFOTBuilder::setFieldWidth( const LengthSpec &l ) {

    nextFormat.FotFieldWidth = l;
}

void MifFOTBuilder::setLineCap( Symbol s ) {

    nextFormat.FotLineCap = s;
}

void MifFOTBuilder::setFieldAlign( Symbol s ) {

    nextFormat.FotFieldAlign = s;
}

//---------------------------------------------------------------------------------------------

void MifFOTBuilder::extension( const ExtensionFlowObj &fo, const NodePtr &node ) {

  ((const MifExtensionFlowObj &)fo).atomic( *this, node );
}

bool MifFOTBuilder::IndexEntryFlowObj::hasNIC( const StringC &name ) const {

    return name == "components" || name == "page-number?" || name == "sort-string"
           || name == "starts-page-range?" || name == "ends-page-range?" ;
}

void MifFOTBuilder::IndexEntryFlowObj::setNIC( const StringC &name, const Value &value ) {

    if( name == "components" )
        value.convertStringList( nic.components );
    else if( name == "page-number?" )
        value.convertBoolean( nic.pageNumber );
    else if( name == "sort-string" ) {
        bool hasString;
        value.convertBoolean( hasString );
        if( hasString )
            value.convertString( nic.sortString );
    } else if( name == "starts-page-range?" )
        value.convertBoolean( nic.startsPageRange );
    else if( name == "ends-page-range?" )
        value.convertBoolean( nic.endsPageRange );
}

//---------------------------------------------------------------------------------------------

unsigned long StringHash::hash( const String<char> &str ) {

  const char *p = str.data();
  unsigned long h = 0;
  for( size_t n = str.size(); n > 0; n-- )
    h = (h << 5) + h + *p++;	// from Chris Torek
  return h;
}

FOTBuilder *makeMifFOTBuilder( const String<CmdLineApp::AppChar> &fileLoc,
                               const Ptr<ExtendEntityManager> &entityManager,
 			                   const CharsetInfo &systemCharset,
                               CmdLineApp *app,
                               const FOTBuilder::Extension *&ext ) {

    
    MifFOTBuilder::IndexEntryFlowObj *indexEntryFlowObject
     = new MifFOTBuilder::IndexEntryFlowObj;

    static const FOTBuilder::Extension extensions[] = {
    {
      "UNREGISTERED::ISOGEN//Flow Object Class::index-entry",
      0,
      0,
      0,
      0,
      indexEntryFlowObject
    },
    {
      "UNREGISTERED::James Clark//Characteristic::page-n-columns",
      0,
      0,
      (void (FOTBuilder::*)(long))&MifFOTBuilder::setPageNColumns,
      0
    },
    {
      "UNREGISTERED::James Clark//Characteristic::page-column-sep",
      0,
      0,
      0,
      (void (FOTBuilder::*)(FOTBuilder::Length))&MifFOTBuilder::setPageColumnSep,
    },
    {
      "UNREGISTERED::James Clark//Characteristic::page-balance-columns?",
      (void (FOTBuilder::*)(bool))&MifFOTBuilder::setPageBalanceColumns,
      0,
      0
    },
    { 0, 0, 0}
  };
  ext = extensions;
  return new MifFOTBuilder( fileLoc, entityManager, systemCharset, app );
}

const Char MifFOTBuilder::FrameCharsetMap[] = { // Latin-1 -> Frame Charset
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x00c1, 0x00a2, 0x00a3, 0x00db, 0x00b4, 0x007c, 0x00a4,
    0x00ac, 0x00a9, 0x00bb, 0x00c7, 0x00c2, 0x002d, 0x00a8, 0x00f8,
    0x00fb, 0x0000, 0x0000, 0x0000, 0x00ab, 0x0000, 0x00a6, 0x00a5,
    0x00fc, 0x0000, 0x00bc, 0x00c8, 0x0000, 0x0000, 0x0000, 0x00c0,
    0x00cb, 0x00e7, 0x00e5, 0x00cc, 0x0080, 0x0081, 0x00ae, 0x0082,
    0x00e9, 0x0083, 0x00e6, 0x00e9, 0x00ed, 0x00ea, 0x00eb, 0x00ec,
    0x0000, 0x0084, 0x00f1, 0x00ee, 0x00ef, 0x00cd, 0x0085, 0x0000,
    0x00af, 0x00f4, 0x00f2, 0x00f3, 0x0086, 0x0000, 0x0000, 0x00a7,
    0x0088, 0x0087, 0x0089, 0x008b, 0x008a, 0x008c, 0x00be, 0x008d,
    0x008f, 0x008e, 0x0090, 0x0091, 0x0093, 0x0092, 0x0094, 0x0095,
    0x0000, 0x0096, 0x0098, 0x0097, 0x0099, 0x009b, 0x009a, 0x0000,
    0x00bf, 0x009d, 0x009c, 0x009e, 0x009f, 0x0000, 0x0000, 0x00d8
};

const MifFOTBuilder::SymbolFont
MifFOTBuilder::SymbolFonts[MifFOTBuilder::nSymbolFonts] = {
  { "Symbol", {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0020, 0x0021, 0x2200, 0x0023, 0x2203, 0x0025, 0x0026, 0x220B,
    0x0028, 0x0029, 0x2217, 0x002B, 0x002C, 0x2212, 0x002E, 0x002F,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    0x2245, 0x0391, 0x0392, 0x03A7, 0x2206, 0x0395, 0x03A6, 0x0393,
    0x0397, 0x0399, 0x03D1, 0x039A, 0x039B, 0x039C, 0x039D, 0x039F,
    0x03A0, 0x0398, 0x03A1, 0x03A3, 0x03A4, 0x03A5, 0x03C2, 0x2126,
    0x039E, 0x03A8, 0x0396, 0x005B, 0x2234, 0x005D, 0x22A5, 0x005F,
    0x203E, 0x03B1, 0x03B2, 0x03C7, 0x03B4, 0x03B5, 0x03C6, 0x03B3,
    0x03B7, 0x03B9, 0x03D5, 0x03BA, 0x03BB, 0x03BC, 0x03BD, 0x03BF,
    0x03C0, 0x03B8, 0x03C1, 0x03C3, 0x03C4, 0x03C5, 0x03D6, 0x03C9,
    0x03BE, 0x03C8, 0x03B6, 0x007B, 0x007C, 0x007D, 0x223C, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x03D2, 0x2032, 0x2264, 0x2215, 0x221E, 0x0192, 0x2663,
    0x2666, 0x2665, 0x2660, 0x2194, 0x2190, 0x2191, 0x2192, 0x2193,
    0x00B0, 0x00B1, 0x2033, 0x2265, 0x00D7, 0x221D, 0x2202, 0x2022,
    0x00F7, 0x2260, 0x2261, 0x2248, 0x2026, 0x0000, 0x0000, 0x21B5,
    0x2135, 0x2111, 0x211C, 0x2118, 0x2297, 0x2295, 0x2205, 0x2229,
    0x222A, 0x2283, 0x2287, 0x2284, 0x2282, 0x2286, 0x2208, 0x2209,
    0x2220, 0x2207, 0x00AE, 0x00A9, 0x2122, 0x220F, 0x221A, 0x22C5,
    0x00AC, 0x2227, 0x2228, 0x21D4, 0x21D0, 0x21D1, 0x21D2, 0x21D3,
    0x25CA, 0x2329, 0x00AE, 0x00A9, 0x2122, 0x2211, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x232A, 0x222B, 0x2320, 0x0000, 0x2321, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 }
  },
  // I created this table by hand.  It's sometimes not obvious what
  // the appropriate mapping is (James Clark)
  { "Wingdings", {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x2702, 0x2701, 0x0000, 0x0000, 0x0000, 0x0000,
    0x260e, 0x2706, 0x2709, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2328,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2707, 0x270d,
    0x0000, 0x270c, 0x0000, 0x0000, 0x0000, 0x261c, 0x261e, 0x261d,
    0x261f, 0x0000, 0x263a, 0x0000, 0x2639, 0x0000, 0x2620, 0x0000,
    0x0000, 0x2708, 0x263c, 0x0000, 0x2744, 0x0000, 0x271e, 0x0000,
    0x2720, 0x2721, 0x262a, 0x262f, 0x0950, 0x2638, 0x2648, 0x2649,
    0x264a, 0x264b, 0x264c, 0x264d, 0x264e, 0x264f, 0x2650, 0x2651,
    0x2652, 0x2653, 0x0000, 0x0000, 0x25cf, 0x274d, 0x25a0, 0x25a1,
    0x0000, 0x2751, 0x2752, 0x0000, 0x0000, 0x25c6, 0x2756, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x2780, 0x2781, 0x2782, 0x2783, 0x2784, 0x2785, 0x2786,
    0x2787, 0x2788, 0x2789, 0x0000, 0x278a, 0x278b, 0x278c, 0x278d,
    0x278e, 0x278f, 0x2790, 0x2791, 0x2792, 0x2793, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x25cb, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x25aa,
    0x0000, 0x0000, 0x2726, 0x2605, 0x2736, 0x0000, 0x2739, 0x0000,
    0x0000, 0x0000, 0x2727, 0x0000, 0x0000, 0x272a, 0x2730, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x232b, 0x2326, 0x0000,
    0x27a2, 0x0000, 0x0000, 0x0000, 0x27b2, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x25ab, 0x2718, 0x2714, 0x2612, 0x2611, 0x0000 }
  },
  { "ZapfDingbats", {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x2701, 0x2702, 0x2703, 0x2704, 0x260E, 0x2706, 0x2707,
    0x2708, 0x2709, 0x261B, 0x261E, 0x270C, 0x270D, 0x270E, 0x270F,
    0x2710, 0x2711, 0x2712, 0x2713, 0x2714, 0x2715, 0x2716, 0x2717,
    0x2718, 0x2719, 0x271A, 0x271B, 0x271C, 0x271D, 0x271E, 0x271F,
    0x2720, 0x2721, 0x2722, 0x2723, 0x2724, 0x2725, 0x2726, 0x2727,
    0x2605, 0x2729, 0x272A, 0x272B, 0x272C, 0x272D, 0x272E, 0x272F,
    0x2730, 0x2731, 0x2732, 0x2733, 0x2734, 0x2735, 0x2736, 0x2737,
    0x2738, 0x2739, 0x273A, 0x273B, 0x273C, 0x273D, 0x273E, 0x273F,
    0x2740, 0x2741, 0x2742, 0x2743, 0x2744, 0x2745, 0x2746, 0x2747,
    0x2748, 0x2749, 0x274A, 0x274B, 0x0000, 0x274D, 0x25A0, 0x274F,
    0x2750, 0x2751, 0x2752, 0x25B2, 0x25BC, 0x25C6, 0x2756, 0x0000,
    0x2758, 0x2759, 0x275A, 0x275B, 0x275C, 0x275D, 0x275E, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x2761, 0x2762, 0x2763, 0x2764, 0x2765, 0x2766, 0x2767,
    0x2663, 0x2666, 0x2665, 0x2660, 0x2460, 0x2461, 0x2462, 0x2463,
    0x2464, 0x2465, 0x2466, 0x2467, 0x2468, 0x2469, 0x2776, 0x2777,
    0x2778, 0x2779, 0x277A, 0x277B, 0x277C, 0x277D, 0x277E, 0x277F,
    0x2780, 0x2781, 0x2782, 0x2783, 0x2784, 0x2785, 0x2786, 0x2787,
    0x2788, 0x2789, 0x278A, 0x278B, 0x278C, 0x278D, 0x278E, 0x278F,
    0x2790, 0x2791, 0x2792, 0x2793, 0x2794, 0x2192, 0x2194, 0x2195,
    0x2798, 0x2799, 0x279A, 0x279B, 0x279C, 0x279D, 0x279E, 0x279F,
    0x27A0, 0x27A1, 0x27A2, 0x27A3, 0x27A4, 0x27A5, 0x27A6, 0x27A7,
    0x27A8, 0x27A9, 0x27AA, 0x27AB, 0x27AC, 0x27AD, 0x27AE, 0x27AF,
    0x0000, 0x27B1, 0x27B2, 0x27B3, 0x27B4, 0x27B5, 0x27B6, 0x27B7,
    0x27B8, 0x27B9, 0x27BA, 0x27BB, 0x27BC, 0x27BD, 0x27BE, 0x0000 }
    },
};

// --------- static -----------------------------------------------------------

#define CONST_STRING( symbolName ) \
const String<char> MifDoc::s ## symbolName ( #symbolName, sizeof( #symbolName ) - 1 )

const String<char> MifDoc::sNONE;
const String<char> MifDoc::sSPACE( " ", sizeof( " " ) - 1 );
const String<char> MifDoc::sDefaultTblFormat
                    ( "Default Tbl Format", sizeof( "Default Tbl Format" ) - 1 );
const String<char> MifDoc::sDefaultPgfFormat
                    ( "Default Pgf Format", sizeof( "Default Pgf Format" ) - 1 );
const String<char> MifDoc::sTimesNewRoman
                    ( "Times New Roman", sizeof( "Times New Roman" ) - 1 );
const String<char> MifDoc::sPageNumXRefFormatName
                    ( "Page Number Format", sizeof( "Page Number Format" ) - 1 );
CONST_STRING( A );
CONST_STRING( Regular );
CONST_STRING( Bold );
CONST_STRING( Italic );
CONST_STRING( FNoUnderlining );
CONST_STRING( FSingle );
CONST_STRING( FDouble );
CONST_STRING( FNumeric );
CONST_STRING( FNormal );
CONST_STRING( FSuperscript );
CONST_STRING( FSubscript );
CONST_STRING( FAsTyped );
CONST_STRING( FSmallCaps );
CONST_STRING( FLowercase );
CONST_STRING( FUppercase );
CONST_STRING( LeftRight );
CONST_STRING( Left );
CONST_STRING( Right );
CONST_STRING( Center );
CONST_STRING( Fixed );
CONST_STRING( Proportional );
CONST_STRING( Top );
CONST_STRING( Middle );
CONST_STRING( Bottom );
CONST_STRING( NoLanguage );
CONST_STRING( USEnglish );
CONST_STRING( UKEnglish );
CONST_STRING( German );
CONST_STRING( SwissGerman );
CONST_STRING( French );
CONST_STRING( CanadianFrench );
CONST_STRING( Spanish );
CONST_STRING( Catalan );
CONST_STRING( Italian );
CONST_STRING( Portuguese );
CONST_STRING( Brazilian );
CONST_STRING( Danish );
CONST_STRING( Dutch );
CONST_STRING( Norwegian );
CONST_STRING( Nynorsk );
CONST_STRING( Finnish );
CONST_STRING( Swedish );
CONST_STRING( LeftMasterPage );
CONST_STRING( RightMasterPage );
CONST_STRING( OtherMasterPage );
CONST_STRING( BodyPage );
CONST_STRING( Decimal );
CONST_STRING( Tab );
CONST_STRING( HardSpace );
CONST_STRING( SoftHyphen );
CONST_STRING( HardHyphen );
CONST_STRING( HardReturn );
CONST_STRING( NumberSpace );
CONST_STRING( ThinSpace );
CONST_STRING( EnSpace );
CONST_STRING( EmSpace );
CONST_STRING( Header );
CONST_STRING( Footer );
CONST_STRING( Arabic );
CONST_STRING( UCRoman );
CONST_STRING( LCRoman );
CONST_STRING( UCAlpha );
CONST_STRING( LCAlpha );
CONST_STRING( FirstLeft );
CONST_STRING( FirstRight );
CONST_STRING( Anywhere );
CONST_STRING( ColumnTop );
CONST_STRING( PageTop );
CONST_STRING( LPageTop );
CONST_STRING( RPageTop );
CONST_STRING( First );
CONST_STRING( Inside );
CONST_STRING( Outside );
CONST_STRING( None );
CONST_STRING( RunIntoParagraph );
CONST_STRING( Inline );
CONST_STRING( Below );
CONST_STRING( Butt );
CONST_STRING( Round );
CONST_STRING( Square );
CONST_STRING( White );
CONST_STRING( Black );
CONST_STRING( Normal );
CONST_STRING( Straddle );
CONST_STRING( Cent );
CONST_STRING( Pound );
CONST_STRING( Yen );
CONST_STRING( EnDash );
CONST_STRING( EmDash );
CONST_STRING( Dagger );
CONST_STRING( DoubleDagger );
CONST_STRING( Bullet);

MifDoc *MifDoc::CurInstance = NULL;

MifDoc::T_ID MifDoc::Object::IDCnt = 0;

MifDoc::T_indent MifOutputByteStream::INDENT = MifDoc::T_indent( 0xFFF );

bool MifDoc::Para::currentlyOpened = false;
unsigned MifDoc::ParaLine::setProperties = 0;
MifDoc::T_ID MifDoc::ParaLine::TextRectID;
MifDoc::T_ID MifDoc::ParaLine::ATbl;

// ----------------------------------------------------------------------------

int operator!=( const MifDoc::T_LTRB &o1, const MifDoc::T_LTRB &o2 ) {

    return o1.l != o2.l || o1.t != o2.t || o1.r != o2.r || o1.b != o2.b;
}

// --------- MifTmpOutputByteStream -------------------------------------------

void MifTmpOutputByteStream::commit( OutputByteStream &os, bool resolveCrossReferences ) {

    MifOutputByteStream outS( os );
    TmpOutputByteStream::Iter iter( *this );
    const char *s;
    size_t n;

    unsigned crossRefInfoIdx;
    char *byteDestPtr = NULL;
    size_t neededBytesNum = 0;
    enum { normal, waitingForCrossRefInfoIdx } state = normal;
    while( iter.next( s, n ) ) {
        while( n > 0 ) {
            switch( state ) {
              case waitingForCrossRefInfoIdx: {
                int bytesCopiedNum = neededBytesNum > n ? n : neededBytesNum;
                memcpy( byteDestPtr, s, bytesCopiedNum );
                byteDestPtr += bytesCopiedNum; n -= bytesCopiedNum; s += bytesCopiedNum;
                neededBytesNum -= bytesCopiedNum;
                if( neededBytesNum == 0 ) {
                    assert( crossRefInfoIdx < MifDoc::CurInstance->crossRefInfos().size() );
                    MifDoc::CrossRefInfo &crossRefInfo
                     = MifDoc::CurInstance->crossRefInfos()[crossRefInfoIdx];   
                    crossRefInfo.out( outS );
                    state = normal;
                } }
                break;

              case normal:
                if( resolveCrossReferences ) {
                    const char *escapeCharPtr
                     = (char*)memchr( s, MifDoc::escapeChar(), n );
                    if( escapeCharPtr != NULL ) {
                        outS.stream().sputn( s, escapeCharPtr - s);
                        n -= (escapeCharPtr - s); s = escapeCharPtr; n--; s++;
                        state = waitingForCrossRefInfoIdx;
                        byteDestPtr = (char*)&crossRefInfoIdx;
                        neededBytesNum = sizeof( unsigned );
                        break;
                    }
                }
                outS.stream().sputn( s, n );
                n = 0;
            }
        }
    }

    assert( neededBytesNum == 0 );
}

void MifTmpOutputByteStream::commit( String<char> &str ) {

    TmpOutputByteStream::Iter iter( *this );
    const char *s;
    size_t n;

    while( iter.next( s, n ) ) {
        str.append( s, n );
    }
}

// --------- MifDoc::T_string -------------------------------------------------

void MifDoc::T_string::escapeSpecialChars() {

    T_string newValue;

    bool changes = false;
    for( size_t i = 0; i < size(); i++ ) {
        switch( (*this)[i] ) {
          case '\n': case '\r': changes = true; break;
          case '\t': newValue.append( "\\t", 2 ); changes = true; break;
          case '>': newValue.append( "\\>", 2 ); changes = true; break;
          case '\'': newValue.append( "\\q", 2 ); changes = true; break;
          case '`':  newValue.append( "\\Q", 2 ); changes = true; break;
          case '\\': newValue.append( "\\\\", 2 );  changes = true; break;
          default: 
            if( (unsigned)(*this)[i] >= 0x80 ) {
                changes = true;
                MifDoc::CurInstance->outHexChar( (*this)[i], newValue );
            } else
                newValue += (*this)[i];
        }
    }
    if( changes )
        *this = newValue;
}

// --------- MifDoc::CrossRefInfo ---------------------------------------------

MifDoc::T_string MifDoc::CrossRefInfo::crossRefText() const {

    char buf[32];
    if( sgmlId.size() > 0 ) {
        sprintf( buf, "NODE%lu.", groveIndex );
        T_string result( buf );
        for( size_t i = 0; i < sgmlId.size(); i++ )
            result += char( sgmlId[i] );
        return result;
    } else {    
        sprintf( buf, "NODE%lu.%lu", groveIndex, elementIndex );
        return T_string( buf );
    }
}

void MifDoc::CrossRefInfo::out( MifOutputByteStream &os ) {

    switch( type() ) {
      case PotentialMarker: {
        bool regularMarkerOut = false;
        bool linkMarkerOut = false;
        if( sgmlId.size() > 0 ) {
            if( MifDoc::CurInstance->elements().hasBeenReferenced
                 ( MifDoc::ElementSet::PageReference, groveIndex, sgmlId ) )
                regularMarkerOut = true;
            if( MifDoc::CurInstance->elements().hasBeenReferenced
                 ( MifDoc::ElementSet::LinkReference, groveIndex, sgmlId ) )
                linkMarkerOut = true;
        } else {
            if( MifDoc::CurInstance->elements().hasBeenReferenced
                 ( MifDoc::ElementSet::PageReference, groveIndex, elementIndex ) )
                regularMarkerOut = true;
            if( MifDoc::CurInstance->elements().hasBeenReferenced
                 ( MifDoc::ElementSet::LinkReference, groveIndex, elementIndex ) ) 
                linkMarkerOut = true;
        }
        if( regularMarkerOut ) {
            os.CurTagIndent = tagIndent;
            MifDoc::Marker marker( *this );
            marker.out( os );
        }
        if( linkMarkerOut ) {
            os.CurTagIndent = tagIndent;
            MifDoc::Marker marker( *this, true );
            marker.out( os );
        } }
        break;

      case HypertextLink: {
        os.CurTagIndent = tagIndent;
        MifDoc::Marker marker( *this );
        marker.out( os ); }
        break;

      case XRef: {
        os.CurTagIndent = tagIndent;
        MifDoc::XRef xRef( *this );
        xRef.out( os ); }
        break;

      default: assert( false );        
    }
}

// ----------------------------------------------------------------------------

#define CHECK_PROPERTY( propertyName ) \
    \
    if( setProperties & f ## propertyName ) \
        os << '\n' << MifOutputByteStream::INDENT \
           << "<" #propertyName " " << propertyName << ">"; 

#define CHECK_VECTOR_OF_PROPERTIES( propertyName ) \
    \
    if( setProperties & f ## propertyName ## s) \
        for( size_t i = 0; i < propertyName ## s.size(); i++ ) { \
            os << '\n' << MifOutputByteStream::INDENT \
               << "<" #propertyName " " << propertyName ## s[i] << ">"; \
        }

// --------- MifDoc::TagStream ------------------------------------------------

MifDoc::TagStream::TagStream( MifDoc::T_indent osIndent )
 : Format(), Content( new MifTmpOutputByteStream( osIndent ) ), PgfTagUsed( false ),
   InitialPgfTag( sDefaultPgfFormat ), TagStreamClass( tsTagStream ) {
}

MifDoc::TagStream::~TagStream() {

    if( Content )
        delete Content;
}

// --------- MifDoc::TextFlow -------------------------------------------------

MifDoc::TextFlow::TextFlow( const TextRect &textRect, bool body, ParagraphFormat *format, 
                            T_tagstring pgfTag )
 : TagStream(), TextRectID( textRect.ID ), setProperties( 0 ), TextRectIDUsed( false ),
   TextRect_( &textRect ) {

    TagStreamClass = tsTextFlow;
    InitialPgfTag = pgfTag;

    if( format )
        Format = *format;

    if( body ) {
        setTFTag( MifDoc::sA );
        setTFAutoConnect( true );
    }
}

void MifDoc::TextFlow::out( MifOutputByteStream &os, bool resolveCrossReferences ) {

    os << '\n' << MifOutputByteStream::INDENT << "<TextFlow ";
    os.indent();
    CHECK_PROPERTY( TFTag );
    CHECK_PROPERTY( TFAutoConnect );
    content().commit( os.stream(), resolveCrossReferences );
    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}


// --------- MifDoc::Color ----------------------------------------------------

MifDoc::Color::Color( unsigned char red, unsigned char green, unsigned char blue ) 
 : setProperties( 0 ) {

    char colorTag[64];

    sprintf( colorTag, "RGB-%i-%i-%i", int( red ), int( green ), int( blue ) );
    setColorTag( T_tagstring( colorTag ) );

    if( red >= green && red >= blue ) {
        setColorBlack( double( 255 - red ) / 2.55 );
        setColorYellow( double( red - blue ) / 2.55 );
        setColorMagenta( double( red - green ) / 2.55 );
        setColorCyan( 0.0 );
    } else
    if( green >= red && green >= blue ) {
        setColorBlack( double( 255 - green ) / 2.55 );
        setColorCyan( double( green - red ) / 2.55 );
        setColorYellow( double( green - blue ) / 2.55 );
        setColorMagenta( 0.0 );
    } else { // blue is largest
        setColorBlack( double( 255 - blue ) / 2.55 );
        setColorCyan( double( blue - red ) / 2.55 );
        setColorMagenta( double( blue - green ) / 2.55 );
        setColorYellow( 0.0 );
    }
}

void MifDoc::Color::out( MifOutputByteStream &os ) const {

    os << '\n' << MifOutputByteStream::INDENT << "<Color ";
    os.indent();
    CHECK_PROPERTY( ColorTag );
    CHECK_PROPERTY( ColorCyan );
    CHECK_PROPERTY( ColorMagenta );
    CHECK_PROPERTY( ColorYellow );
    CHECK_PROPERTY( ColorBlack );
    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}

const String<char> &MifDoc::Color::key( Color &color ) {

    return color.ColorTag;
}

// --------- MifDoc::ColorCatalog ---------------------------------------------

void MifDoc::ColorCatalog::out( MifOutputByteStream &os ) {

    if( Colors.count() > 0 ) {
        os << '\n' << MifOutputByteStream::INDENT << "<ColorCatalog ";
        os.indent();
    
        PointerTableIter<Color*,String<char>,StringHash,Color> iter( Colors );
        const Color *color;
        while( ( color = iter.next() ) != NULL )
            color->out( os );
    
        os.undent();
        os << '\n' << MifOutputByteStream::INDENT << ">";
    }
}

// --------- MifDoc::Ruling ---------------------------------------------------

const String<char> &MifDoc::Ruling::key( Ruling &r ) {

    if( r.Key.size() < 1 ) {
        char keyStr[ 48 ];
        sprintf( keyStr, "Ruling-%li-%li-%li-%li-",
                 long( r.RulingPenWidth ), long( r.RulingGap ), r.RulingPen, r.RulingLines );
        r.Key.assign( keyStr, strlen( keyStr ) );
        r.Key += r.RulingColor;
     }

     return r.Key;       
}

void MifDoc::Ruling::out( MifOutputByteStream &os ) const {

    os << '\n' << MifOutputByteStream::INDENT << "<Ruling ";
    os.indent();
    CHECK_PROPERTY( RulingTag );
    CHECK_PROPERTY( RulingPenWidth );
    CHECK_PROPERTY( RulingGap );
    CHECK_PROPERTY( RulingColor );
    CHECK_PROPERTY( RulingPen );
    CHECK_PROPERTY( RulingLines );
    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}

// --------- MifDoc::RulingCatalog --------------------------------------------

void MifDoc::RulingCatalog::out( MifOutputByteStream &os ) {

    if( Rulings.count() > 0 ) {
        os << '\n' << MifOutputByteStream::INDENT << "<RulingCatalog ";
        os.indent();
    
        PointerTableIter<Ruling*,String<char>,StringHash,Ruling> iter( Rulings );
        const Ruling *ruling;
        while( ( ruling = iter.next() ) != NULL )
            ruling->out( os );
    
        os.undent();
        os << '\n' << MifOutputByteStream::INDENT << ">";
    }
}

// --------- MifDoc::Cell -----------------------------------------------------

void MifDoc::Cell::out( MifOutputByteStream &os, bool resolveCrossReferences ) {

    os << '\n' << MifOutputByteStream::INDENT << "<Cell ";
    os.indent();

    CHECK_PROPERTY( CellFill );
    CHECK_PROPERTY( CellColor );
    CHECK_PROPERTY( CellLRuling );
    CHECK_PROPERTY( CellBRuling );
    CHECK_PROPERTY( CellRRuling );
    CHECK_PROPERTY( CellTRuling );
    CHECK_PROPERTY( CellColumns );
    CHECK_PROPERTY( CellRows );

    os << '\n' << MifOutputByteStream::INDENT << "<CellContent ";
    os.indent();
    content().commit( os.stream(), resolveCrossReferences );
    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";

    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}

// --------- MifDoc::Row ------------------------------------------------------

void MifDoc::Row::out( MifOutputByteStream &os, bool resolveCrossReferences ) {

    os << '\n' << MifOutputByteStream::INDENT << "<Row ";
    os.indent();

    for( size_t i = 0; i < Cells.size(); i++ ) Cells[i].out( os, resolveCrossReferences );

    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}

// --------- MifDoc::TblColumn ------------------------------------------------

void MifDoc::TblColumn::out( MifOutputByteStream &os ) {

    os << '\n' << MifOutputByteStream::INDENT << "<TblFormat ";
    os.indent();

    CHECK_PROPERTY( TblColumnNum );
    CHECK_PROPERTY( TblColumnWidth );

    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}

// --------- MifDoc::TblFormat ------------------------------------------------

void MifDoc::TblFormat::out( MifOutputByteStream &os ) {

    os << '\n' << MifOutputByteStream::INDENT << "<TblFormat ";
    os.indent();

    CHECK_PROPERTY( TblTag );
    CHECK_PROPERTY( TblLIndent );
    CHECK_PROPERTY( TblRIndent );
    CHECK_PROPERTY( TblSpBefore );
    CHECK_PROPERTY( TblSpAfter );
    CHECK_PROPERTY( TblAlignment );
    CHECK_PROPERTY( TblCellMargins );
    CHECK_PROPERTY( TblTitlePlacement );
    CHECK_PROPERTY( TblWidth );
    for( size_t i = 0; i < TblColumns.size(); i++ ) TblColumns[i].out( os );

    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}

// --------- MifDoc::Tbl ------------------------------------------------------

void MifDoc::Tbl::out( MifOutputByteStream &os, bool resolveCrossReferences ) {

    os << '\n' << MifOutputByteStream::INDENT << "<Tbl ";
    os.indent();
    CHECK_PROPERTY( TblID );
    CHECK_PROPERTY( TblTag );

    if( setProperties & fTblFormat )
        tblFormat.out( os );

    CHECK_PROPERTY( TblNumColumns );
    CHECK_VECTOR_OF_PROPERTIES( TblColumnWidth );

    size_t i;

    if( TblH.size() ) {
        os << '\n' << MifOutputByteStream::INDENT << "<TblH ";
        os.indent();
        for( i = 0; i < TblH.size(); i++ ) TblH[i].out( os, resolveCrossReferences );
        os.undent();
        os << '\n' << MifOutputByteStream::INDENT << ">";
    }

    if( TblBody.size() ) {
        os << '\n' << MifOutputByteStream::INDENT << "<TblBody ";
        os.indent();
        for( i = 0; i < TblBody.size(); i++ ) TblBody[i].out( os, resolveCrossReferences );
        os.undent();
        os << '\n' << MifOutputByteStream::INDENT << ">";
    }

    if( TblF.size() ) {
        os << '\n' << MifOutputByteStream::INDENT << "<TblF ";
        os.indent();
        for( i = 0; i < TblF.size(); i++ ) TblF[i].out( os, resolveCrossReferences );
        os.undent();
        os << '\n' << MifOutputByteStream::INDENT << ">";
    }
    
    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}

// --------- MifDoc::Document -------------------------------------------------

void MifDoc::Document::out( MifOutputByteStream &os ) {

    os << '\n' << MifOutputByteStream::INDENT << "<Document ";
    os.indent();
    CHECK_PROPERTY( DMargins );
    CHECK_PROPERTY( DColumns );
    CHECK_PROPERTY( DColumnGap );
    CHECK_PROPERTY( DPageSize );
    CHECK_PROPERTY( DStartPage );
    CHECK_PROPERTY( DPageNumStyle );
    CHECK_PROPERTY( DTwoSides );
    CHECK_PROPERTY( DParity );
    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}

// --------- MifDoc::XRefFormat -----------------------------------------------

void MifDoc::XRefFormat::out( MifOutputByteStream &os ) {

    os << '\n' << MifOutputByteStream::INDENT << "<XRefFormat ";
    os.indent();
    CHECK_PROPERTY( XRefName );
    CHECK_PROPERTY( XRefDef );
    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}

// --------- MifDoc::XRef -----------------------------------------------------

MifDoc::XRef::XRef( CrossRefInfo &crossRefInfo ) {

    switch( crossRefInfo.type() ) {
      case CrossRefInfo::XRef: {
        MifDoc::T_pathname targetFileName( "<c\\>" );
        int bookComponentIdx 
         = crossRefInfo.sgmlId.size() > 0
            ? MifDoc::CurInstance->elements().bookComponentIndex
                  ( crossRefInfo.groveIndex, crossRefInfo.sgmlId )
            : MifDoc::CurInstance->elements().bookComponentIndex
                  ( crossRefInfo.groveIndex, crossRefInfo.elementIndex );
        targetFileName
         += MifDoc::T_pathname
             ( MifDoc::CurInstance->bookComponents()[bookComponentIdx].FileName );
        setXRefSrcFile( targetFileName );
        setXRefName( MifDoc::sPageNumXRefFormatName );
        setXRefSrcText( crossRefInfo.crossRefText() );
        setXRefText( "000" );
        break; }
      default: assert( false );
    }
}

void MifDoc::XRef::out( MifOutputByteStream &os ) {

    os << '\n' << MifOutputByteStream::INDENT << "<XRef ";
    os.indent();
    CHECK_PROPERTY( XRefName );
    if( setProperties & fXRefSrcText )
        XRefSrcText.escapeSpecialChars();
    CHECK_PROPERTY( XRefSrcText );
    CHECK_PROPERTY( XRefSrcFile );
    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
    if( setProperties & fXRefText )
        os << '\n' << MifOutputByteStream::INDENT
           << "<String " << XRefText << ">"; 
    os << '\n' << MifOutputByteStream::INDENT << "<XRefEnd>";
}

// --------- MifDoc::Marker ---------------------------------------------------

MifDoc::Marker::Marker( CrossRefInfo &crossRefInfo, bool linkDestinationMode ) {

    switch( crossRefInfo.type() ) {
      case CrossRefInfo::PotentialMarker:
        if( linkDestinationMode ) {
            setMType( Hypertext );
            setMText( "newlink " );
            MText += crossRefInfo.crossRefText();
        } else {
            setMType( XRef );
            setMText( crossRefInfo.crossRefText() );
        }
        break;

      case CrossRefInfo::HypertextLink: {
        setMType( Hypertext );
        MifDoc::T_pathname targetFileName( "<c>" );
        int bookComponentIdx 
         = crossRefInfo.sgmlId.size() > 0
            ? MifDoc::CurInstance->elements().bookComponentIndex
                  ( crossRefInfo.groveIndex, crossRefInfo.sgmlId )
            : MifDoc::CurInstance->elements().bookComponentIndex
                  ( crossRefInfo.groveIndex, crossRefInfo.elementIndex );
        targetFileName
         += MifDoc::T_pathname
             ( MifDoc::CurInstance->bookComponents()[bookComponentIdx].FileName );
        setMText( T_string( "gotolink " ) );
        MText += targetFileName;
        MText += ':';
        MText += crossRefInfo.crossRefText();
        break; }

      case HypertextDestination:
        setMText( T_string( "newlink " ) );
        MText += crossRefInfo.crossRefText();
        break;

      default: assert( false );
    }
}

void MifDoc::Marker::out( MifOutputByteStream &os ) {

    os << '\n' << MifOutputByteStream::INDENT << "<Marker ";
    os.indent();
    CHECK_PROPERTY( MType );
    if( setProperties & fMText )
        MText.escapeSpecialChars();
    CHECK_PROPERTY( MText );
    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}

// --------- MifDoc::Object ---------------------------------------------------

void MifDoc::Object::outObjectProperties( MifOutputByteStream &os ) {

    CHECK_PROPERTY( ID );
    CHECK_PROPERTY( Pen );
    CHECK_PROPERTY( Fill );
    CHECK_PROPERTY( PenWidth );
    CHECK_PROPERTY( ObColor );
}

// --------- MifDoc::PolyLine -------------------------------------------------

void MifDoc::PolyLine::out( MifOutputByteStream &os ) {

    os << '\n' << MifOutputByteStream::INDENT << "<PolyLine ";
    os.indent();
    outObjectProperties( os );
    CHECK_PROPERTY( HeadCap );
    CHECK_PROPERTY( TailCap );
    for( size_t i = 0; i < Points.size(); i++ )
        os << '\n' << MifOutputByteStream::INDENT << "<Point " << Points[i] << ">";
    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}

// --------- MifDoc::ImportObject ---------------------------------------------

void MifDoc::ImportObject::out( MifOutputByteStream &os ) {

    os << '\n' << MifOutputByteStream::INDENT << "<ImportObject ";
    os.indent();
    outObjectProperties( os );
    CHECK_PROPERTY( ImportObFileDI );
    CHECK_PROPERTY( BitMapDpi );
    CHECK_PROPERTY( ShapeRect );
    CHECK_PROPERTY( ImportObFixedSize );
    CHECK_PROPERTY( NativeOrigin );
    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}

// --------- MifDoc::Frame ----------------------------------------------------

void MifDoc::Frame::out( MifOutputByteStream &os ) {

    os << '\n' << MifOutputByteStream::INDENT << "<Frame ";
    os.indent();
    outObjectProperties( os );
    CHECK_PROPERTY( ShapeRect );
    CHECK_PROPERTY( FrameType );
    CHECK_PROPERTY( NSOffset );
    CHECK_PROPERTY( BLOffset );
    CHECK_PROPERTY( AnchorAlign );
    for( size_t i = 0; i < Objects.size(); i++ ) Objects[i]->out( os );
    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}

// --------- MifDoc::TextRect -------------------------------------------------

void MifDoc::TextRect::out( MifOutputByteStream &os ) {

    os << '\n' << MifOutputByteStream::INDENT << "<TextRect ";
    os.indent();
    outObjectProperties( os );
    CHECK_PROPERTY( ShapeRect );
    CHECK_PROPERTY( TRNumColumns );
    CHECK_PROPERTY( TRColumnGap );
    CHECK_PROPERTY( TRColumnBalance );
    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}

// --------- MifDoc::Page -----------------------------------------------------

void MifDoc::Page::out( MifOutputByteStream &os ) {

    os << '\n' << MifOutputByteStream::INDENT << "<Page ";
    os.indent();
    CHECK_PROPERTY( PageType );
    CHECK_PROPERTY( PageTag );
    CHECK_PROPERTY( PageBackground );
    for( size_t i = 0; i < TextRects.size(); i++ )
        TextRects[i].out( os );
    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}                                      

// --------- MifDoc::TabStop --------------------------------------------------

void MifDoc::TabStop::out( MifOutputByteStream &os ) {

    os << '\n' << MifOutputByteStream::INDENT << "<TabStop ";
    os.indent();
    CHECK_PROPERTY( TSX );
    CHECK_PROPERTY( TSType );
    CHECK_PROPERTY( TSLeaderStr );
    CHECK_PROPERTY( TSDecimalChar );
    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}

// --------- MifDoc::Para -----------------------------------------------------

void MifDoc::Para::out( MifOutputByteStream &os ) {

    outProlog( os );
    MifDoc::ParaLine::outProlog( os );
    content().commit( os.stream() );
    MifDoc::ParaLine::outEpilog( os );
    outEpilog( os );
}

void MifDoc::Para::outSimpleProlog( MifOutputByteStream &os ) {

    currentlyOpened = true;
    os << '\n' << MifOutputByteStream::INDENT << "<Para ";
    os.indent();
}

void MifDoc::Para::outProlog( MifOutputByteStream &os ) {

    currentlyOpened = true;
    outSimpleProlog( os );
    CHECK_PROPERTY( PgfTag );
    if( setProperties & fParagraphFormat )
        format().out( os );
}

void MifDoc::Para::outEpilog( MifOutputByteStream &os ) {

    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
    currentlyOpened = false;
}

// --------- MifDoc::ParaLine -------------------------------------------------

void MifDoc::ParaLine::outProlog( MifOutputByteStream &os ) {

    os << '\n' << MifOutputByteStream::INDENT << "<ParaLine ";
    os.indent();
    
    MifDoc::TextFlow *tf = MifDoc::CurInstance->curTextFlow();
    if( tf != NULL && !tf->TextRectIDUsed ) {
        setTextRectID( tf->TextRectID );
        CHECK_PROPERTY( TextRectID );
        setProperties &= ~fTextRectID;
        tf->TextRectIDUsed = true;
    }

    MifDoc::Tbl *tbl = MifDoc::CurInstance->curTbl( false );
    if( tbl != NULL && !tbl->TblIDUsed ) {
        setATbl( tbl->TblID );
        CHECK_PROPERTY( ATbl );
        setProperties &= ~fATbl;
        tbl->TblIDUsed = true;
    }
}

void MifDoc::ParaLine::outEpilog( MifOutputByteStream &os ) {

    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}


// --------- MifDoc::PgfCatalog -----------------------------------------------

void MifDoc::PgfCatalog::out( MifOutputByteStream &os ) {

    os << '\n' << MifOutputByteStream::INDENT << "<PgfCatalog ";
    os.indent();
    for( size_t i = 0; i < ParaFormats.size(); i++ ) ParaFormats[i].out( os, false );
    os.undent();
    os << '\n' << MifOutputByteStream::INDENT << ">";
}

// --------- MifDoc::TblCatalog -----------------------------------------------

void MifDoc::TblCatalog::out( MifOutputByteStream &os ) {

    if( TblFormats.size() > 0 ) {
        os << '\n' << MifOutputByteStream::INDENT << "<TblCatalog ";
        os.indent();
        for( size_t i = 0; i < TblFormats.size(); i++ ) TblFormats[i].out( os );
        os.undent();
        os << '\n' << MifOutputByteStream::INDENT << ">";
    }
}

// ----------------------------------------------------------------------------

#define COMPARE_PROPERTY( propertyName ) \
\
    if( propertyName != f.propertyName ) \
        differingProperties |= f ## propertyName;        

#define COND_SET_PROPERTY( propertyName ) \
\
    if( properties & f ## propertyName ) \
        propertyName =  f.propertyName;        

#define PROPERTY_OUT( propertyName ) \
\
    if( properties & f ## propertyName ) \
        os << '\n' << MifOutputByteStream::INDENT \
           << "<" #propertyName " " << propertyName << ">"; 

// --------- MifDoc::FontFormat -----------------------------------------------

unsigned MifDoc::FontFormat::compare( FontFormat &f ) {

    unsigned differingProperties = 0;

    COMPARE_PROPERTY( FFamily );
    COMPARE_PROPERTY( FAngle );
    COMPARE_PROPERTY( FWeight );
    COMPARE_PROPERTY( FVar );
    COMPARE_PROPERTY( FSize );
    COMPARE_PROPERTY( FColor );
    COMPARE_PROPERTY( FUnderlining );
    COMPARE_PROPERTY( FOverline );
    COMPARE_PROPERTY( FStrike );
    COMPARE_PROPERTY( FPosition );
    COMPARE_PROPERTY( FPairKern );
    COMPARE_PROPERTY( FCase );
    COMPARE_PROPERTY( FDX );
    COMPARE_PROPERTY( FDY );
    COMPARE_PROPERTY( FDW );

    return differingProperties;
}

void MifDoc::FontFormat::setFrom( FontFormat &f, unsigned properties ) {

    COND_SET_PROPERTY( FFamily );
    COND_SET_PROPERTY( FAngle );
    COND_SET_PROPERTY( FWeight );
    COND_SET_PROPERTY( FVar );
    COND_SET_PROPERTY( FSize );
    COND_SET_PROPERTY( FColor );
    COND_SET_PROPERTY( FUnderlining );
    COND_SET_PROPERTY( FOverline );
    COND_SET_PROPERTY( FStrike );
    COND_SET_PROPERTY( FPosition );
    COND_SET_PROPERTY( FPairKern );
    COND_SET_PROPERTY( FCase );
    COND_SET_PROPERTY( FDX );
    COND_SET_PROPERTY( FDY );
    COND_SET_PROPERTY( FDW );
}

void MifDoc::FontFormat::out( MifOutputByteStream &os,
                              unsigned properties,
                              FontStatement fontStatement ) {

    const char *statementName = (fontStatement == stPgfFont) ? "PgfFont" : "Font";

    if( properties ) {

        os << '\n' << MifOutputByteStream::INDENT << "<" << statementName << " ";
        os.indent();

        PROPERTY_OUT( FFamily );
        PROPERTY_OUT( FAngle );
        PROPERTY_OUT( FWeight );
        PROPERTY_OUT( FVar );
        PROPERTY_OUT( FSize );
        PROPERTY_OUT( FColor );
        PROPERTY_OUT( FUnderlining );
        PROPERTY_OUT( FOverline );
        PROPERTY_OUT( FStrike );
        PROPERTY_OUT( FPosition );
        PROPERTY_OUT( FPairKern );
        PROPERTY_OUT( FCase );
        PROPERTY_OUT( FDX );
        PROPERTY_OUT( FDY );
        PROPERTY_OUT( FDW );

        os.undent();
        os << '\n' << MifOutputByteStream::INDENT << ">";
    }
}

void MifDoc::FontFormat::updateFrom( FontFormat &f ) {

    unsigned differingProperties = FontFormat::compare( f );
    setFrom( f, differingProperties );
    setProperties = differingProperties;
}

// --------- MifDoc::ParagraphFormat ------------------------------------------

unsigned MifDoc::ParagraphFormat::compare( ParagraphFormat &f ) {

    unsigned differingProperties = 0;

    COMPARE_PROPERTY( PgfTag );
    COMPARE_PROPERTY( PgfLanguage );
    COMPARE_PROPERTY( PgfFIndent );
    COMPARE_PROPERTY( PgfLIndent );
    COMPARE_PROPERTY( PgfRIndent );
    COMPARE_PROPERTY( PgfAlignment );
    COMPARE_PROPERTY( PgfSpBefore );
    COMPARE_PROPERTY( PgfSpAfter );
    COMPARE_PROPERTY( PgfLineSpacing );
    COMPARE_PROPERTY( PgfLeading );
    COMPARE_PROPERTY( PgfNumTabs );
    COMPARE_PROPERTY( PgfPlacement );
    COMPARE_PROPERTY( PgfPlacementStyle );
    COMPARE_PROPERTY( PgfWithPrev );
    COMPARE_PROPERTY( PgfWithNext );
    COMPARE_PROPERTY( PgfBlockSize );
    COMPARE_PROPERTY( PgfAutoNum );
    COMPARE_PROPERTY( PgfNumFormat );
    COMPARE_PROPERTY( PgfNumberFont );
    COMPARE_PROPERTY( PgfHyphenate );
    COMPARE_PROPERTY( HyphenMaxLines );
    COMPARE_PROPERTY( HyphenMinPrefix );
    COMPARE_PROPERTY( HyphenMinSuffix );
    COMPARE_PROPERTY( HyphenMinWord );
    COMPARE_PROPERTY( PgfLetterSpace );
    COMPARE_PROPERTY( PgfCellAlignment );
    COMPARE_PROPERTY( PgfCellMargins );
    COMPARE_PROPERTY( PgfCellLMarginFixed );
    COMPARE_PROPERTY( PgfCellTMarginFixed );
    COMPARE_PROPERTY( PgfCellRMarginFixed );
    COMPARE_PROPERTY( PgfCellBMarginFixed );

    return differingProperties;
}

void MifDoc::ParagraphFormat::setFrom( ParagraphFormat &f,
                                       unsigned properties,
                                       unsigned fontProperties ) {
    COND_SET_PROPERTY( PgfTag );
    COND_SET_PROPERTY( PgfLanguage );
    COND_SET_PROPERTY( PgfFIndent );
    COND_SET_PROPERTY( PgfLIndent );
    COND_SET_PROPERTY( PgfRIndent );
    COND_SET_PROPERTY( PgfAlignment );
    COND_SET_PROPERTY( PgfSpBefore );
    COND_SET_PROPERTY( PgfSpAfter );
    COND_SET_PROPERTY( PgfLineSpacing );
    COND_SET_PROPERTY( PgfLeading );
    COND_SET_PROPERTY( PgfNumTabs );
    COND_SET_PROPERTY( PgfPlacement );
    COND_SET_PROPERTY( PgfPlacementStyle );
    COND_SET_PROPERTY( PgfWithPrev );
    COND_SET_PROPERTY( PgfWithNext );
    COND_SET_PROPERTY( PgfBlockSize );
    COND_SET_PROPERTY( PgfAutoNum );
    COND_SET_PROPERTY( PgfNumFormat );
    COND_SET_PROPERTY( PgfNumberFont );
    COND_SET_PROPERTY( PgfHyphenate );
    COND_SET_PROPERTY( HyphenMaxLines );
    COND_SET_PROPERTY( HyphenMinPrefix );
    COND_SET_PROPERTY( HyphenMinSuffix );
    COND_SET_PROPERTY( HyphenMinWord );
    COND_SET_PROPERTY( PgfLetterSpace );
    COND_SET_PROPERTY( PgfCellAlignment );
    COND_SET_PROPERTY( PgfCellMargins );
    COND_SET_PROPERTY( PgfCellLMarginFixed );
    COND_SET_PROPERTY( PgfCellTMarginFixed );
    COND_SET_PROPERTY( PgfCellRMarginFixed );
    COND_SET_PROPERTY( PgfCellBMarginFixed );

    FontFormat::setFrom( f, fontProperties );
}

void MifDoc::ParagraphFormat::out( MifOutputByteStream &os,
                                   unsigned properties,
                                   unsigned fontProperties,
                                   bool excludeCellProperties ) {

    bool outPgfTag = false;
    T_tagstring pgfTag;

    if( !( properties & fPgfTag ) ) {
        MifDoc::TagStream &ts = MifDoc::CurInstance->curTagStream();
        if( !ts.PgfTagUsed ) {
            pgfTag = ts.InitialPgfTag;
            outPgfTag = true;
            ts.PgfTagUsed = true;
        }
    }

    if( properties != 0 || fontProperties != 0 || outPgfTag ) {

        os << '\n' << MifOutputByteStream::INDENT << "<Pgf ";
        os.indent();

        if( outPgfTag )
          os << '\n' << MifOutputByteStream::INDENT << "<PgfTag " << pgfTag << ">"; 
        else 
          PROPERTY_OUT( PgfTag );

        FontFormat::out( os, fontProperties, FontFormat::stPgfFont );

        PROPERTY_OUT( PgfLanguage );
        PROPERTY_OUT( PgfFIndent );
        PROPERTY_OUT( PgfLIndent );
        PROPERTY_OUT( PgfRIndent );
        PROPERTY_OUT( PgfAlignment );
        PROPERTY_OUT( PgfSpBefore );
        PROPERTY_OUT( PgfSpAfter );
        PROPERTY_OUT( PgfLineSpacing );
        PROPERTY_OUT( PgfLeading );

        PROPERTY_OUT( PgfNumTabs );
        if( properties & fTabStops )
            for( size_t i = 0; i < TabStops.size(); i++ ) TabStops[i].out( os );

        PROPERTY_OUT( PgfPlacement );
        PROPERTY_OUT( PgfPlacementStyle );
        PROPERTY_OUT( PgfWithPrev );
        PROPERTY_OUT( PgfWithNext );
        PROPERTY_OUT( PgfBlockSize );
        PROPERTY_OUT( PgfAutoNum );
        PROPERTY_OUT( PgfNumFormat );
        PROPERTY_OUT( PgfNumberFont );
        PROPERTY_OUT( PgfHyphenate );
        PROPERTY_OUT( HyphenMaxLines );
        PROPERTY_OUT( HyphenMinPrefix );
        PROPERTY_OUT( HyphenMinSuffix );
        PROPERTY_OUT( HyphenMinWord );
        PROPERTY_OUT( PgfLetterSpace );

        if( MifDoc::CurInstance->curCell() != NULL || !excludeCellProperties ) {

            PROPERTY_OUT( PgfCellAlignment );
            PROPERTY_OUT( PgfCellMargins );
            PROPERTY_OUT( PgfCellLMarginFixed );
            PROPERTY_OUT( PgfCellTMarginFixed );
            PROPERTY_OUT( PgfCellRMarginFixed );
            PROPERTY_OUT( PgfCellBMarginFixed );
        }

        os.undent();
        os << '\n' << MifOutputByteStream::INDENT << ">";
    }
}

void MifDoc::ParagraphFormat::updateFrom( ParagraphFormat &f ) {

    FontFormat::updateFrom( f );

    unsigned differingProperties = compare( f );
    setFrom( f, differingProperties, 0 );
    setProperties = differingProperties;
}

// --------- MifDoc::BookComponent --------------------------------------------

MifDoc::BookComponent::BookComponent( MifTmpOutputByteStream *epilogOs )
  : epilogOs( epilogOs ), pageNumXRefFormatGenerated( false ) {
}  

void MifDoc::BookComponent::commit
( const String<CmdLineApp::AppChar> &dirName, bool resolveCrossReferences ) {

    assert( FileName.size() > 0 );
    String<CmdLineApp::AppChar> fileLoc;
    fileLoc += dirName; fileLoc += FileName; fileLoc += 0;
    FileOutputByteStream outFile;
    if( outFile.open( fileLoc.data() ) ) {
    
        MifOutputByteStream os( outFile );

        os << "<MIFFile 5.0>";

        colorCatalog.out( os );
        pgfCatalog.out( os );
        rulingCatalog.out( os );

        size_t i;
        if( AFrames.size() ) {
            os << "\n<AFrames ";
            os.indent();
            for( i = 0; i < AFrames.size(); i++ ) AFrames[i].out( os );
            os.undent();
            os << '\n' << ">";
        }

        if( XRefFormats.size() ) {
            os << "\n<XRefFormats ";
            os.indent();
            for( i = 0; i < XRefFormats.size(); i++ ) XRefFormats[i].out( os );
            os.undent();
            os << '\n' << ">";
        }

        tblCatalog.out( os );
        document.out( os );
    
        if( Tbls.size() ) {
            os << "\n<Tbls ";
            os.indent();
            for( i = 0; i < Tbls.size(); i++ ) Tbls[i].out( os, resolveCrossReferences );
            os.undent();
            os << '\n' << ">";
        }
    
        for( i = 0; i < Pages.size(); i++ ) Pages[i].out( os );
        for( i = 0; i < TextFlows.size(); i++ ) TextFlows[i].out( os, resolveCrossReferences );
    
        if( epilogOs )
            epilogOs->commit( os.stream(), resolveCrossReferences );    

    } else {
        MifDoc::curInstance().App->message
         ( MifMessages::cannotOpenOutputError,
           StringMessageArg( MifDoc::curInstance().App->convertInput( fileLoc.data() ) ),
           ErrnoMessageArg( errno ) );
        return;
    }
}

// --------- MifDoc -----------------------------------------------------------

MifDoc::MifDoc( const String<CmdLineApp::AppChar> &fileLoc, CmdLineApp *app )
 : App( app ), RootOutputFileLoc( fileLoc ), CurTblNum( 0 ), CurTextFlow( NULL ),
   CurCell( NULL ), CurPara( NULL ), NextID( 0 ) {

    if( RootOutputFileLoc.size() > 0 && RootOutputFileLoc[RootOutputFileLoc.size()-1] == 0 )
        RootOutputFileLoc.resize( RootOutputFileLoc.size() - 1 );

    CurInstance = this;
    enterBookComponent();
}

MifDoc::~MifDoc() {

    commit();
}

void MifDoc::commit() {

    String<CmdLineApp::AppChar> outDir = rootOutputFileLoc();
    String<CmdLineApp::AppChar> outFileName;

    { int i;
    for( i = outDir.size() - 1; i >= 0; i-- )
        if( outDir[i] == '/' || outDir[i] == '\\' )
            break;
    if( outDir.size() - (i+1) > 0 )
        outFileName.assign( &outDir.data()[i+1], outDir.size() - (i+1) );
    outDir.resize( i + 1 ); }
  
    if( BookComponents.size() > 1 ) {
        String<CmdLineApp::AppChar> bookFileLoc( rootOutputFileLoc() );
        String<CmdLineApp::AppChar> fileNameExt;

        { size_t idx;
        for( idx = bookFileLoc.size(); idx > 0; idx-- )
            if( bookFileLoc[idx-1] == char( '.' ) )
                break;
        if( idx > 0 && bookFileLoc.size() - idx > 0 )
            fileNameExt.assign( &bookFileLoc[idx], bookFileLoc.size() - idx );
        else {
            fileNameExt += 'm'; fileNameExt += 'i'; fileNameExt += 'f';
        } }

        String<CmdLineApp::AppChar> fileName;
        char idxStr[32];
        for( size_t i = 0; i < BookComponents.size(); i++ ) {
            fileName.resize( 0 );          
            sprintf( idxStr, "%u", i+1 );
            for( const char *c = idxStr; *c; c++)
                fileName += CmdLineApp::AppChar(*c);
            fileName += '.'; fileName += fileNameExt;
            BookComponents[i].FileName = fileName;  
        }

        bookFileLoc += 0;
        FileOutputByteStream bookFile;
        if( bookFile.open( bookFileLoc.data() ) ) {
    
            MifOutputByteStream os( bookFile );
            os << "<Book 5.0>";
            for( size_t i = 0; i < BookComponents.size(); i++ )
                os << "\n<BookComponent"
                   << "\n  <FileName `<c\\>" << BookComponents[i].FileName << "\'>"
                   << "\n>";
        } else {

            MifDoc::curInstance().App->message
             ( MifMessages::cannotOpenOutputError,
               StringMessageArg( MifDoc::curInstance().App->convertInput( bookFileLoc.data() ) ),
               ErrnoMessageArg( errno ) );
        }
    
    } else
        if( BookComponents.size() == 1 )
            BookComponents[0].FileName = outFileName;
        
    for( size_t i = 0; i < BookComponents.size(); i++ )
        BookComponents[i].commit( outDir );
}

void MifDoc::enterBookComponent() {

    TagStream *defaultTagStream = new TagStream( MifDoc::T_indent( 0 ) );
    enterTagStream( *defaultTagStream );
    BookComponent newBookComponent( &defaultTagStream->content() );
    bookComponents().push_back( newBookComponent );
}

void MifDoc::exitBookComponent() {

    exitTagStream();
}

void MifDoc::outAFrame( T_ID ID, MifOutputByteStream &os ) {

    os << '\n' << MifOutputByteStream::INDENT << "<AFrame " << ID << ">";
}

void MifDoc::switchToTagStream( TagStream &tagStream, bool startWithDefaultPgfFormat ) {

    if( tagStream.TagStreamClass == TagStream::tsTextFlow )
        setCurTextFlow( (MifDoc::TextFlow*)&tagStream );
    else
        if( tagStream.TagStreamClass == TagStream::tsCell )
            setCurCell( (MifDoc::Cell*)&tagStream );

    if( tagStream.TagStreamClass == TagStream::tsPara ) {
        MifDoc::Para::currentlyOpened = true;
    } else
        if( startWithDefaultPgfFormat ) {
            tagStream.PgfTagUsed = false;
            MifDoc::ParagraphFormat defaultPgfFormat;
            defaultPgfFormat.setDSSSLDefaults();
            setCurParagraphFormat( defaultPgfFormat );
        }
    setCurOs( tagStream.content().stream() );
}

void MifDoc::enterTagStream( TagStream &tagStream ) {

    TagStreamStack.resize( TagStreamStack.size() + 1 );
    TagStreamStack.back() = &tagStream;
    switchToTagStream( *TagStreamStack.back() );
}

void MifDoc::exitTagStream() {

    bool startWithDefaultPgfFormat = true;

    assert( TagStreamStack.size() > 0 );
    if( TagStreamStack.back()->TagStreamClass == TagStream::tsPara ) {
        MifDoc::Para::currentlyOpened = false;
        startWithDefaultPgfFormat = false;
    }
    TagStreamStack.resize( TagStreamStack.size() - 1 );
    if( TagStreamStack.size() >= 1 )
        switchToTagStream( *TagStreamStack.back(), startWithDefaultPgfFormat );
}

void MifDoc::outTagEnd() { os() << ">"; }

void MifDoc::outHexChar( unsigned code, MifOutputByteStream *o ) {

    MifOutputByteStream &outS = ( o != NULL ) ? *o : os();
    static const char hex[] = "0123456789abcdef";
    outS << "\\x" << hex[(code >> 4) & 0xf] << hex[code & 0xf] << " ";
}

void MifDoc::outHexChar( unsigned code, String<char> &targetString ) {

    static const char hex[] = "0123456789abcdef";
    targetString += '\\'; targetString += 'x';
    targetString += hex[(code >> 4) & 0xf];
    targetString += hex[code & 0xf];
    targetString += ' ';
}

void MifDoc::outSpecialChar( const String<char> &charName, MifOutputByteStream *o ) {

    MifOutputByteStream &outS = ( o != NULL ) ? *o : os();
    outS << '\n' << MifOutputByteStream::INDENT << "<Char " << charName << ">";
}

void MifDoc::outBreakingPara( MifDoc::T_keyword pgfPlacement ) {

        curFormat().setPgfSpBefore( 0 ),
        curFormat().setPgfSpAfter( 0 ),
        curFormat().setPgfPlacement( pgfPlacement );
        Para::outSimpleProlog( os() );
        curFormat().out( os() );
        ParaLine::outProlog( os() );
        ParaLine::outEpilog( os() );
        Para::outEpilog( os() );
}

void MifDoc::beginParaLine() {

    MifDoc::ParaLine::outProlog( os() );
}

void MifDoc::endParaLine() {

    MifDoc::ParaLine::outEpilog( os() );
}

void MifDoc::outPageNumber() {

    os() << '\n' << MifOutputByteStream::INDENT
         << "<Variable <VariableName `Current Page #'>>";
}

// --------- MifDoc::ElementSet -----------------------------------------------

MifDoc::ElementSet::ElementSet() {}

int MifDoc::ElementSet::bookComponentIndex( unsigned long groveIndex, unsigned long n ) const {

    unsigned short flags;
    if( getFlags( groveIndex, n, flags ) )
        return int( flags & BOOK_COMPONENT_INDEX_M() );

    assert( false );
    return 0;
}

int MifDoc::ElementSet::bookComponentIndex
 ( unsigned long groveIndex, const StringC &sgmlId ) {

    SgmlIdInfo &sgmlIdInfo = enforceSgmlId( sgmlId, groveIndex );
    return int( sgmlIdInfo.flags & BOOK_COMPONENT_INDEX_M() );
}

void MifDoc::ElementSet::setBookComponentIndex
 ( unsigned long groveIndex, unsigned long n, int i ) {

    unsigned short flags = 0;
    getFlags( groveIndex, n, flags );
    add( groveIndex, n,
         ( flags & LINK_TYPE_MASK( AnyReference ) )
          | (unsigned short)( i & BOOK_COMPONENT_INDEX_M() ) );
}

void MifDoc::ElementSet::setBookComponentIndex
 ( unsigned long groveIndex, const StringC &sgmlId, int i ) {

    SgmlIdInfo &sgmlIdInfo = enforceSgmlId( sgmlId, groveIndex );
    unsigned short flags = sgmlIdInfo.flags;
    sgmlIdInfo.flags = ( flags & LINK_TYPE_MASK( AnyReference ) )
                          | (unsigned short)( i & BOOK_COMPONENT_INDEX_M() );
}

bool MifDoc::ElementSet::hasBeenReferenced
 ( ReferenceType refType, unsigned long groveIndex, unsigned long n ) const {

    unsigned short flags = 0;
    getFlags( groveIndex, n, flags );
    return ( flags & LINK_TYPE_MASK( refType ) ) != 0 ? true : false;
}

bool MifDoc::ElementSet::hasBeenReferenced
 ( ReferenceType refType, unsigned long groveIndex, const StringC &sgmlId )  {

    SgmlIdInfo &sgmlIdInfo = enforceSgmlId( sgmlId, groveIndex );
    return ( sgmlIdInfo.flags & LINK_TYPE_MASK( refType ) ) != 0 ? true : false;
}

MifDoc::ElementSet::SgmlIdInfo &MifDoc::ElementSet::enforceSgmlId
 ( const StringC &sgmlId, unsigned long groveIndex ) {

    SgmlIdInfo *result = SgmlIdInfos.lookup( sgmlId );
    if( result == NULL ) {
        result = new SgmlIdInfo( sgmlId, groveIndex );
        SgmlIdInfos.insert( result );
    }
    return *result;
}

void MifDoc::ElementSet::setReferencedFlag
 ( ReferenceType refType, unsigned long groveIndex, unsigned long n ) {

    assert( refType != AnyReference );
    unsigned short flags = 0;
    getFlags( groveIndex, n, flags );
    add( groveIndex, n, flags | LINK_TYPE_MASK( refType ) );
}

void MifDoc::ElementSet::setReferencedFlag
 ( ReferenceType refType, unsigned long groveIndex, const StringC &sgmlId ) {

    SgmlIdInfo &sgmlIdInfo = enforceSgmlId( sgmlId, groveIndex );
    assert( refType != AnyReference );
    sgmlIdInfo.flags |= LINK_TYPE_MASK( refType );
}

void MifDoc::ElementSet::add
 ( unsigned long groveIndex, unsigned long n, unsigned short flags  ) {

    if( groveIndex >= Flags.size() )
        Flags.resize( groveIndex + 1 );
    Vector<unsigned short> &elems = Flags[groveIndex];
    if( n >= elems.size() ) {
        for( size_t k = 1 + (n - elems.size()); k > 0; --k )
            elems.push_back(0);
    }
    elems[n] = flags;
}

bool MifDoc::ElementSet::getFlags
 ( unsigned long groveIndex, unsigned long n, unsigned short &result ) const {

  if( groveIndex < Flags.size() && n < Flags[groveIndex].size() ) {
    result = Flags[groveIndex][n];
    return true;        
  } else
    return false;
}

#ifdef DSSSL_NAMESPACE
}
#endif

#include "MifFOTBuilder_inst.cxx"

#endif /* JADE_MIF */
