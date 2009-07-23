// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.
//

class Char {
%TypeHeaderCode
# include "sapi/common/Char.h"
using namespace SernaApi;
%End
public:
    Char();
    Char(char);
    Char(unsigned char, unsigned char);
    Char(int);
                                                                                
    static const Char null;
    static const Char replacement;
    static const Char byteOrderMark;
    static const Char byteOrderSwapped;
    static const Char nbsp;

    enum Category {
        NoCategory,
                                                                        
        Mark_NonSpacing,
        Mark_SpacingCombining,
        Mark_Enclosing,
                                                                        
        Number_DecimalDigit,
        Number_Letter,
        Number_Other,
                                                                        
        Separator_Space,
        Separator_Line,
        Separator_Paragraph,
                                                                        
        Other_Control,
        Other_Format,
        Other_Surrogate,
        Other_PrivateUse,
        Other_NotAssigned,
                                                                        
        Letter_Uppercase,
        Letter_Lowercase,
        Letter_Titlecase,
        Letter_Modifier,
        Letter_Other,
                                                                        
        Punctuation_Connector,
        Punctuation_Dash,
        Punctuation_Open,
        Punctuation_Close,
        Punctuation_InitialQuote,
        Punctuation_FinalQuote,
        Punctuation_Other,
                                                                        
        Symbol_Math,
        Symbol_Currency,
        Symbol_Modifier,
        Symbol_Other
    };
    enum Direction {
        DirL,   
        DirR,
        DirEN,
        DirES,
        DirET,
        DirAN,
        DirCS,
        DirB,
        DirS,
        DirWS,
        DirON,
        DirLRE,
        DirLRO,
        DirAL,
        DirRLE,
        DirRLO,
        DirPDF,
        DirNSM,
        DirBN
    };
    enum Decomposition {
        Single,
        Canonical,
        Font,
        NoBreak,
        Initial,
        Medial,
        Final,
        Isolated,
        Circle,
        Super,
        Sub,
        Vertical,
        Wide,
        Narrow,
        Small,
        Square,
        Compat,
        Fraction
    };
    enum Joining {
        OtherJoining,
        Dual,
        Right,
        Center
    };
    enum CombiningClass {
        Combining_BelowLeftAttached,
        Combining_BelowAttached,
        Combining_BelowRightAttached,
        Combining_LeftAttached,
        Combining_RightAttached,
        Combining_AboveLeftAttached,
        Combining_AboveAttached,
        Combining_AboveRightAttached,
                                                                        
        Combining_BelowLeft,
        Combining_Below,
        Combining_BelowRight,
        Combining_Left,
        Combining_Right,
        Combining_AboveLeft,
        Combining_Above,
        Combining_AboveRight,
                                                                        
        Combining_DoubleBelow,
        Combining_DoubleAbove,
        Combining_IotaSubscript
    };
    int digitValue() const;
    Char lower() const;
    Char upper() const;
                                                                                
    Category category() const;
    Direction direction() const;
    Joining joining() const;
    bool mirrored() const;
    Char mirroredChar() const;
    Decomposition decompositionTag() const;
    unsigned char combiningClass() const;

    unsigned char latin1() const;
    unsigned short unicode() const;
    
    bool isNull() const;
    bool isPrint() const;
    bool isPunct() const;
    bool isSpace() const;
    bool isMark() const;
    bool isLetter() const;
    bool isNumber() const;
    bool isLetterOrNumber() const;
    bool isDigit() const;
    bool isSymbol() const;

    unsigned char cell() const;
    unsigned char row() const;
    
    bool operator==(Char) const;
    bool operator!=(Char) const;
    bool operator<=(Char) const;
    bool operator>=(Char) const;
    bool operator<(Char) const; 
    bool operator>(Char) const;
};

class SString {

%TypeHeaderCode
# include "sapi/common/SString.h"
using namespace SernaApi;
%End

public:
    typedef Char                value_type;
    typedef unsigned int        size_type;
    typedef value_type&         reference;
    typedef const value_type&   const_reference;
    typedef value_type*         iterator;
    typedef const value_type*   const_iterator;
    
    SString();
    SString(const SString&);
    
    bool        isEmpty() const;
    bool        isNull() const;
    bool        empty() const;
  
    Char        at(size_type) const;
    size_type   length() const;
  
    void        resize(size_type, Char = Char());
    void        reserve(size_type);

    SString&    append(const SString&);
    SString&    append(const SString&, size_type, size_type);
    SString&    append(size_type, value_type);
    void        push_back(value_type);
    
    SString&    insert(size_type, const SString&);
    SString&    insert(size_type, const SString&, size_type, size_type);
    SString&    insert(size_type, size_type, value_type);

    SString&    erase(size_type, size_type);
    SString&    erase(size_type);
    SString&    erase();
    
    SString&    replace(size_type, size_type, const SString&);
    SString&    replace(size_type, size_type, const SString&,
                        size_type, size_type);
    SString&    replace(size_type, size_type, size_type, value_type);

    void        swap(SString&);

    size_type find(const SString&, size_type = 0) const;
    size_type find(value_type, size_type = 0) const;
    size_type rfind(const SString&, size_type) const;
    size_type rfind(const SString&) const;
    size_type rfind(value_type, size_type) const;
    size_type rfind(value_type) const;
    size_type find_first_of(const SString&, size_type = 0) const;
    size_type find_first_of(value_type, size_type = 0) const;
    size_type find_last_of(const SString&, size_type) const;
    size_type find_last_of(const SString&) const;
    size_type find_last_of(value_type, size_type) const;
    size_type find_last_of(value_type) const;
    size_type find_first_not_of(const SString&, size_type = 0) const;
    size_type find_last_not_of(const SString&, size_type) const;
    size_type find_last_not_of(const SString&) const;
    
    SString substr(size_type, size_type) const;
    SString substr(size_type) const;
    
    int compare(const SString&) const;
    int compare(size_type, size_type, const SString&) const;
    int compare(size_type, size_type, const SString&,
                size_type, size_type) const;

    /// converts string to numbers
    short           toShort(bool* = 0, int = 10) const;
    unsigned short  toUShort(bool* = 0, int = 10) const;
    int             toInt(bool* = 0, int = 10) const;
    unsigned int    toUInt(bool* = 0, int = 10) const;
    long            toLong(bool* = 0, int = 10) const;
    unsigned long   toULong(bool* = 0, int = 10) const;
    float           toFloat(bool* = 0) const;
    double          toDouble(bool* = 0) const;
    
    // converts number to string
    static SString  number(int /Constrained/, int = 10);
    static SString  number(double, char = 'g', int = 6);
    
    //////////////////////////////////////////////////////////////////
    
    SString         lower() const;
    SString         upper() const;
                                                                                
    SString         stripWhiteSpace() const;
    SString         simplifyWhiteSpace() const;
                                                                                
    SString         left(size_type)  const;
    SString         right(size_type) const;
    SString         mid(size_type, size_type) const;
    SString         mid(size_type) const;

    SString     operator+(const SString&) const;
    SString&    operator+=(const SString&);
    SString&    operator+=(const char*);
    
    bool operator==(const SString &) const;
%MethodCode
        sipRes = (*sipCpp) == *a0;
%End
    bool operator!=(const SString &) const;
%MethodCode
        sipRes = (*sipCpp) != *a0;
%End
    bool operator<(const SString &) const;
%MethodCode
        sipRes = (*sipCpp) < *a0;
%End
    bool operator<=(const SString &) const;
%MethodCode
        sipRes = (*sipCpp) <= *a0;
%End
    bool operator>(const SString &) const;
%MethodCode
        sipRes = (*sipCpp) > *a0;
%End
    bool operator>=(const SString &) const;
%MethodCode
        sipRes = (*sipCpp) >= *a0;
%End

    ///////////////////////////////////////////////////////////////////
        
    int __len__() const;
%MethodCode
        sipRes = sipCpp->length();
%End
    
    int __nonzero__();
%MethodCode
        sipRes = !sipCpp->isNull();
%End

    ///////////////////////////////////////////////////////////////////
    
    SIP_PYOBJECT __unicode__();
%MethodCode
        sipRes = SStringToPyUnicode(sipCpp);
%End

    ///////////////////////////////////////////////////////////////////
    
    SIP_PYOBJECT __str__();
%MethodCode
	sipRes = SStringToPyUnicode(sipCpp);
%End
        
    ///////////////////////////////////////////////////////////////////
    
    SString operator[](int) const;
%MethodCode
        int len = sipCpp->length();
        if ((a0 = sipConvertFromSequenceIndex(a0, len)) < 0)
            sipIsErr = 1;
        else
            sipRes = new SString(sipCpp->at(a0));
%End

    ///////////////////////////////////////////////////////////////////
        
    SString operator[](SIP_PYSLICE) const;
%MethodCode
        Py_ssize_t start, stop, step, slicelength;
        Py_ssize_t len = sipCpp -> length();

        if (sipConvertFromSliceObject(a0, len, &start, &stop,
            &step, &slicelength) < 0)
                sipIsErr = 1;
        else {
            sipRes = new SString;
            sipRes->reserve(slicelength);
            for (int i = 0; i < slicelength; ++i)
            {
                sipRes->append(sipCpp->at(start));
                start += step;
            }
        }
%End

    ///////////////////////////////////////////////////////////////////

%TypeCode
static PyObject* SStringToPyUnicode(SString *ss)
{
	PyObject* uobj = PyUnicode_FromUnicode(NULL, ss->length());

	if (NULL == uobj)
	    return NULL;

	Py_UNICODE* pyu = PyUnicode_AS_UNICODE(uobj);

	for (unsigned int i = 0; i < ss->length(); ++i)
	    *pyu++ = (ss->unicode() + i)->unicode();
	return uobj;
}
%End

%ConvertToTypeCode
	// Allow a Python string (or a unicode string) whenever a SString is
	// expected.
	if (sipIsErr == NULL)
	    return (PyString_Check(sipPy) || PyUnicode_Check(sipPy) ||
		sipIsSubClassInstance(sipPy, sipClass_SString));

	if (sipPy == Py_None) {
	    *sipCppPtr = new SString;
	    return 1;
	}
	if (PyUnicode_Check(sipPy))
	{
                int len = PyUnicode_GET_SIZE(sipPy);
#ifndef Py_UNICODE_WIDE
                SString* ss = 
                    new SString((Char*)PyUnicode_AS_UNICODE(sipPy), len);
#else // Py_UNICODE_WIDE
		SString* ss = new SString;
		PY_UNICODE_TYPE *ucode = PyUnicode_AS_UNICODE(sipPy);
                ss->reserve(len);
                for (int i = 0; i < len; ++i)
                    ss->ref(i) = ucode[i];
#endif // Py_UNICODE_WIDE   
		*sipCppPtr = ss;
		return 1;
	}
	if (PyString_Check(sipPy))
	{
		*sipCppPtr = new SString(PyString_AS_STRING(sipPy));
		return 1;
	}
	*sipCppPtr = (SString*)sipConvertToCpp(sipPy, 
            sipClass_SString, sipIsErr);
	return 0;
%End

};
