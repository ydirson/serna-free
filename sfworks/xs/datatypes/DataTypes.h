// 
// Copyright(c) 2009 Syntext, Inc. All Rights Reserved.
// Contact: info@syntext.com, http://www.syntext.com
// 
// This file is part of Syntext Serna XML Editor.
// 
// COMMERCIAL USAGE
// Licensees holding valid Syntext Serna commercial licenses may use this file
// in accordance with the Syntext Serna Commercial License Agreement provided
// with the software, or, alternatively, in accorance with the terms contained
// in a written agreement between you and Syntext, Inc.
// 
// GNU GENERAL PUBLIC LICENSE USAGE
// Alternatively, this file may be used under the terms of the GNU General 
// Public License versions 2.0 or 3.0 as published by the Free Software 
// Foundation and appearing in the file LICENSE.GPL included in the packaging 
// of this file. In addition, as a special exception, Syntext, Inc. gives you
// certain additional rights, which are described in the Syntext, Inc. GPL 
// Exception for Syntext Serna Free Edition, included in the file 
// GPL_EXCEPTION.txt in this package.
// 
// You should have received a copy of appropriate licenses along with this 
// package. If not, see <http://www.syntext.com/legal/>. If you are unsure
// which license is appropriate for your use, please contact the sales 
// department at sales@syntext.com.
// 
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
// 
/*! \file
 *  $RCSfile: DataTypes.h,v $
 *
 ***********************************************************************/
#ifndef SCHEMA_DATA_TYPES_H
#define SCHEMA_DATA_TYPES_H

#include "xs/xs_defs.h"
#include "xs/datatypes/XsSimpleTypeImpl.h"
#include "xs/ComponentRef.h"
#include "common/prtti.h"
#include "common/StringDecl.h"
#include "common/String.h"
#include "xs/datatypes/SchemaDateTime.h"
#include "xs/datatypes/AtomicType.h"

class Schema;

XS_NAMESPACE_BEGIN

class WhiteSpaceFacet;
class EnumerationFacet;
class EnumerationContainer;
class PatternFacet;
class PatternContainer;
class LengthFacet;
class MinLengthFacet;
class MaxLengthFacet;
class MaxInclusiveFacet;
class MinInclusiveFacet;
class MaxExclusiveFacet;
class MinExclusiveFacet;
class TotalDigitsFacet;
class FractionDigitsFacet;
class EncodingFacet;
class DurationFacet;
class PeriodFacet;

/*! StringType class represents Schema String type.
 */
class StringType: public AtomicType {
public:
    /*! Constructor with delegated Resource interface.
     */
    StringType(const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;
    virtual void    dump(int indent) const;

    /*! Type of the type is Type::STRING.
     */
    virtual Type type() const;

    //! Constraining Facets
    virtual bool addFacet(Schema* schema, XsFacet* facet);
    bool setLength(Schema* schema, LengthFacet* lenFacet);
    bool setMinLength(Schema* schema, MinLengthFacet* minLenFacet);
    bool setMaxLength(Schema* schema, MaxLengthFacet* maxLenFacet);
    bool addPattern(Schema* schema, PatternFacet* pattern);
    virtual bool addEnumeration(Schema* schema, EnumerationFacet* enumeration);
    bool  setWhiteSpace  (Schema* schema, WhiteSpaceFacet* whiteSpaceFacet);

    virtual AtomicType* clone() const;

    XS_OALLOC(StringType);

protected:
    void whiteSpacePrepare(const String& source, String& result) const;

    String value_;
    COMMON_NS::RefCntPtr<LengthFacet>     lenFacet_;
    COMMON_NS::RefCntPtr<MinLengthFacet>  minLenFacet_;
    COMMON_NS::RefCntPtr<MaxLengthFacet>  maxLenFacet_;
    COMMON_NS::RefCntPtr<WhiteSpaceFacet> whiteSpaceFacet_;
private:
    COMMON_NS::OwnerPtr<PatternContainer>    pattern_;
    COMMON_NS::OwnerPtr<EnumerationContainer> enumeration_;
};

/*! QNameType class represents Schema QName type.
 */
class QNameType: public AtomicType {
public:
    /*! Constructor with delegated Resource interface.
     */
    QNameType(  const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;
    virtual void    dump(int indent) const;

    /*! Type of the type is Type::QNAME.
     */
    virtual Type type() const;

    //! Constraining Facets
    virtual bool addFacet(Schema* schema, XsFacet* facet);
    bool  setLength      (Schema* schema, LengthFacet* lenFacet);
    bool  setMinLength   (Schema* schema, MinLengthFacet* minLenFacet);
    bool  setMaxLength   (Schema* schema, MaxLengthFacet* maxLenFacet);
    bool  setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet);
    bool  setMaxExclusive(Schema* schema, MaxExclusiveFacet* maxExclFacet);
    bool  setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet);
    bool  setMinExclusive(Schema* schema, MinExclusiveFacet* minExclFacet);
    bool  addPattern     (Schema* schema, PatternFacet* pattern);
    virtual bool addEnumeration(Schema* schema, EnumerationFacet* enumeration);
    bool  setWhiteSpace  (Schema* schema, WhiteSpaceFacet*);

    virtual AtomicType* clone() const;

    XS_OALLOC(QNameType);

protected:
    COMMON_NS::RefCntPtr<LengthFacet>       lenFacet_;
    COMMON_NS::RefCntPtr<MinLengthFacet>    minLenFacet_;
    COMMON_NS::RefCntPtr<MaxLengthFacet>    maxLenFacet_;
    COMMON_NS::RefCntPtr<MinInclusiveFacet> minInclFacet_;
    COMMON_NS::RefCntPtr<MaxInclusiveFacet> maxInclFacet_;
    COMMON_NS::RefCntPtr<MinExclusiveFacet> minExclFacet_;
    COMMON_NS::RefCntPtr<MaxExclusiveFacet> maxExclFacet_;
    COMMON_NS::OwnerPtr<PatternContainer>       pattern_;
    COMMON_NS::OwnerPtr<EnumerationContainer>   enumeration_;
};

/*! RecurringType class represents Schema RecurringDuration type.
 */
class RecurringType: public AtomicType {
public:
    /*! Constructor with delegated Resource interface.
     */
    RecurringType(const Origin& origin, const NcnCred& cred);
    virtual ~RecurringType();

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;
    void    dump(int) const;

    /*! Type of the type is Type::RecurringDuration.
     */
    virtual AtomicType::Type  type() const;

    //! Constraining Facets
    virtual bool addFacet(Schema* schema, XsFacet* facet);
    bool  setDuration    (Schema* schema, DurationFacet* durationFacet);
    bool  setPeriod      (Schema* schema, PeriodFacet* periodFacet);
    bool  addPattern     (Schema* schema, PatternFacet* pattern);
    virtual bool addEnumeration (Schema* schema, EnumerationFacet* enumeration);
    bool  setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet);
    bool  setMaxExclusive(Schema* schema, MaxExclusiveFacet* maxExclFacet);
    bool  setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet);
    bool  setMinExclusive(Schema* schema, MinExclusiveFacet* minExclFacet);
    bool  setWhiteSpace  (Schema* schema, WhiteSpaceFacet*);

    /*! Lexical Representation, used for Right representation
    of derived types of RecurringType.
    */
    virtual void formLexicalRepresentation(String& result, bool sign,
                                           String& year, String& month,
                                           String& day, String& hour,
                                           String& minute, String& sec,
                                           String& msec) const;
    XS_OALLOC(RecurringType);

protected:

    SchemaDateTime shift(const String& toShift, bool isIncrement) const;

    /*! Translator: string representation of RecurringType
      to SchemaDateTime
    */
    bool toDateTime(SchemaDateTime&  result, const String& str) const;

    mutable SchemaDateTime*   startInstant_ ;
    COMMON_NS::RefCntPtr<DurationFacet>        durationFacet_;
    COMMON_NS::RefCntPtr<PeriodFacet>          periodFacet_  ;
    COMMON_NS::RefCntPtr<MinInclusiveFacet>    minInclFacet_ ;
    COMMON_NS::RefCntPtr<MaxInclusiveFacet>    maxInclFacet_ ;
    COMMON_NS::RefCntPtr<MinExclusiveFacet>    minExclFacet_ ;
    COMMON_NS::RefCntPtr<MaxExclusiveFacet>    maxExclFacet_ ;
    COMMON_NS::OwnerPtr<PatternContainer>         pattern_      ;
    COMMON_NS::OwnerPtr<EnumerationContainer>     enumeration_  ;
    SchemaDateTime            maxInclValue_ ;
    SchemaDateTime            maxExclValue_ ;
    SchemaDateTime            minInclValue_ ;
    SchemaDateTime            minExclValue_ ;
};

/*! NumberType class represents Schema Decimal type.
 */
class NumberType: public AtomicType {
public:
    /*! Constructor with delegated Resource interface.
     */
    NumberType(const Origin& origin, const NcnCred& cred);


    bool less(String& first, String& second) const
        { return first.toDouble() < second.toDouble();}
    bool lessEq(String& first, String& second) const
        { return first.toDouble() <= second.toDouble();}

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;
    void    dump(int) const;

    /*! Type of the type is Type::DECIMAL.
     */
    virtual AtomicType::Type   type() const;

    //! Constraining Facets
    virtual bool addFacet       (Schema* schema, XsFacet* facet);
    bool         setPrecision   (Schema* schema, TotalDigitsFacet* precision);
    virtual bool setScale       (Schema* schema, FractionDigitsFacet* scale);
    bool         addPattern     (Schema* schema, PatternFacet* pattern);
    virtual bool addEnumeration (Schema* schema, EnumerationFacet* enumeration);
    virtual bool setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet);
    bool         setMaxExclusive(Schema* schema, MaxExclusiveFacet* maxExclFacet);
    virtual bool setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet);
    bool         setMinExclusive(Schema* schema, MinExclusiveFacet* minExclFacet);
    bool         setWhiteSpace  (Schema* schema, WhiteSpaceFacet*);

    virtual AtomicType* clone() const;

    XS_OALLOC(NumberType);

protected:
    /*! Shift method is to make decrement or increment on the smallest value,
      calculated by relation to other numeric facets and Scale facet.
    */
    void shift(const String& toShift, bool isIncrement, String& result) const;

    /*! Routine validation string value to have right decimals.
     */
    bool    checkScale(bool isToChange, String& result) const;
    bool    checkPrecision(bool isToChange, String& result) const;

    COMMON_NS::RefCntPtr<TotalDigitsFacet>     precision_;
    COMMON_NS::RefCntPtr<FractionDigitsFacet>  scale_;
    COMMON_NS::RefCntPtr<MinInclusiveFacet>    minInclFacet_;
    COMMON_NS::RefCntPtr<MaxInclusiveFacet>    maxInclFacet_;
    COMMON_NS::RefCntPtr<MinExclusiveFacet>    minExclFacet_;
    COMMON_NS::RefCntPtr<MaxExclusiveFacet>    maxExclFacet_;
    COMMON_NS::OwnerPtr<PatternContainer>         pattern_;
    COMMON_NS::OwnerPtr<EnumerationContainer>     enumeration_;
};

/*! DoubleType class represents Schema Double type.
 */
class DoubleType: public AtomicType {
public:

    /*! Constructor with delegated Resource interface.
     */
    DoubleType(const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o, const String& source, String& result) const;

    /*! Dump type.
     */
    void    dump(int) const;

    /*! Type of the type is Type::DOUBLE.
     */
    virtual AtomicType::Type   type() const;

    //! Constraining Facets
    virtual bool addFacet(Schema* schema, XsFacet* facet);
    bool  addPattern     (Schema* schema, PatternFacet* pattern);
    virtual bool  addEnumeration (Schema* schema, EnumerationFacet* enumeration);
    bool  setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet);
    bool  setMaxExclusive(Schema* schema, MaxExclusiveFacet* maxExclFacet);
    bool  setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet);
    bool  setMinExclusive(Schema* schema, MinExclusiveFacet* minExclFacet);
    bool  setWhiteSpace  (Schema* schema, WhiteSpaceFacet*);

    virtual AtomicType* clone() const;

    XS_OALLOC(DoubleType);

private:
    COMMON_NS::RefCntPtr<MinInclusiveFacet> minInclFacet_;
    COMMON_NS::RefCntPtr<MaxInclusiveFacet> maxInclFacet_;
    COMMON_NS::RefCntPtr<MinExclusiveFacet> minExclFacet_;
    COMMON_NS::RefCntPtr<MaxExclusiveFacet> maxExclFacet_;
    COMMON_NS::OwnerPtr<PatternContainer>      pattern_;
    COMMON_NS::OwnerPtr<EnumerationContainer>  enumeration_;
};

/*! DurationType class represents Schema TimeDuration type.
 */
class DurationType: public AtomicType {
public:
    /*! Constructor with delegated Resource interface.
     */
    DurationType(const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;
    /*! Dump type.
     */
    void    dump(int) const;

    /*! Type of the type is Type::DOUBLE.
     */
    virtual AtomicType::Type   type() const;

    //! Translator: string representation of DurationType
    //! to SchemaDateTime
    static bool toDateTime(SchemaDateTime& result, const String& str);

    //! Constraining Facets
    virtual bool addFacet(Schema* schema, XsFacet* facet);
    bool         addPattern(Schema* schema, PatternFacet* pattern);
    virtual bool addEnumeration (Schema* schema, EnumerationFacet* enumeration);

    bool  setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet);
    bool  setMaxExclusive(Schema* schema, MaxExclusiveFacet* maxExclFacet);
    bool  setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet);
    bool  setMinExclusive(Schema* schema, MinExclusiveFacet* minExclFacet);
    bool  setWhiteSpace  (Schema* schema, WhiteSpaceFacet*);

    virtual AtomicType* clone() const;

    XS_OALLOC(DurationType);

private:
    COMMON_NS::RefCntPtr<MinInclusiveFacet>    minInclFacet_;
    COMMON_NS::RefCntPtr<MaxInclusiveFacet>    maxInclFacet_;
    COMMON_NS::RefCntPtr<MinExclusiveFacet>    minExclFacet_;
    COMMON_NS::RefCntPtr<MaxExclusiveFacet>    maxExclFacet_;
    COMMON_NS::OwnerPtr<PatternContainer>         pattern_     ;
    COMMON_NS::OwnerPtr<EnumerationContainer>     enumeration_ ;
    SchemaDateTime            maxInclValue_;
    SchemaDateTime            maxExclValue_;
    SchemaDateTime            minInclValue_;
    SchemaDateTime            minExclValue_;
};

/*! FloatType class represents Schema Float type.
 */
class FloatType: public AtomicType {
public:
    /*! Constructor with delegated Resource interface.
     */
    FloatType(const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;
    /*! Dump type.
     */
    void    dump(int) const;

    /*! Type of the type is Type::FLOAT.
     */
    virtual AtomicType::Type   type() const;

    //! Constraining Facets
    virtual bool addFacet(Schema* schema, XsFacet* facet);
    bool         addPattern(Schema* schema, PatternFacet* pattern);
    virtual bool addEnumeration (Schema* schema, EnumerationFacet* enumeration);

    bool  setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet);
    bool  setMaxExclusive(Schema* schema, MaxExclusiveFacet* maxExclFacet);
    bool  setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet);
    bool  setMinExclusive(Schema* schema, MinExclusiveFacet* minExclFacet);
    bool  setWhiteSpace  (Schema* schema, WhiteSpaceFacet*);

    virtual AtomicType* clone() const;

    XS_OALLOC(FloatType);

private:
    COMMON_NS::RefCntPtr<MinInclusiveFacet> minInclFacet_;
    COMMON_NS::RefCntPtr<MaxInclusiveFacet> maxInclFacet_;
    COMMON_NS::RefCntPtr<MinExclusiveFacet> minExclFacet_;
    COMMON_NS::RefCntPtr<MaxExclusiveFacet> maxExclFacet_;
    COMMON_NS::OwnerPtr<PatternContainer>      pattern_;
    COMMON_NS::OwnerPtr<EnumerationContainer>  enumeration_;
};

/*! IDType class represents Schema ID type.
 */
class IDType: public AtomicType {
public:
    /*! Constructor with delegated Resource interface.
     */
    IDType(  const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;
    /*! Dump type.
     */
    virtual void    dump(int indent) const;

    /*! Type of the type is Type::ID.
     */
    virtual Type type() const;

    //! Constraining Facets
    virtual bool addFacet(Schema* schema, XsFacet* facet);
    bool  setLength      (Schema* schema, LengthFacet* lenFacet);
    bool  setMinLength   (Schema* schema, MinLengthFacet* minLenFacet);
    bool  setMaxLength   (Schema* schema, MaxLengthFacet* maxLenFacet);
    bool  setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet);
    bool  setMaxExclusive(Schema* schema, MaxExclusiveFacet* maxExclFacet);
    bool  setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet);
    bool  setMinExclusive(Schema* schema, MinExclusiveFacet* minExclFacet);
    bool  addPattern     (Schema* schema, PatternFacet* pattern);
    virtual bool addEnumeration(Schema* schema, EnumerationFacet* enumeration);
    bool  setWhiteSpace  (Schema* schema, WhiteSpaceFacet*);

    virtual AtomicType* clone() const;

    XS_OALLOC(IDType);

protected:
    COMMON_NS::RefCntPtr<LengthFacet>       lenFacet_;
    COMMON_NS::RefCntPtr<MinLengthFacet>    minLenFacet_;
    COMMON_NS::RefCntPtr<MaxLengthFacet>    maxLenFacet_;
    COMMON_NS::RefCntPtr<MinInclusiveFacet> minInclFacet_;
    COMMON_NS::RefCntPtr<MaxInclusiveFacet> maxInclFacet_;
    COMMON_NS::RefCntPtr<MinExclusiveFacet> minExclFacet_;
    COMMON_NS::RefCntPtr<MaxExclusiveFacet> maxExclFacet_;
    COMMON_NS::OwnerPtr<PatternContainer>      pattern_;
    COMMON_NS::OwnerPtr<EnumerationContainer>  enumeration_;
};

/*! IDREFType class represents Schema IDREF type.
 */
class IDREFType: public AtomicType {
public:
    /*! Constructor with delegated Resource interface.
     */
    IDREFType(  const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;
    /*! Dump type.
     */
    virtual void    dump(int indent) const;

    /*! Type of the type is Type::IDREF.
     */
    virtual Type type() const;

    //! Constraining Facets
    virtual bool addFacet(Schema* schema, XsFacet* facet);
    bool  setLength      (Schema* schema, LengthFacet* lenFacet);
    bool  setMinLength   (Schema* schema, MinLengthFacet* minLenFacet);
    bool  setMaxLength   (Schema* schema, MaxLengthFacet* maxLenFacet);
    bool  setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet);
    bool  setMaxExclusive(Schema* schema, MaxExclusiveFacet* maxExclFacet);
    bool  setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet);
    bool  setMinExclusive(Schema* schema, MinExclusiveFacet* minExclFacet);
    bool  addPattern     (Schema* schema, PatternFacet* pattern);
    virtual bool addEnumeration(Schema* schema, EnumerationFacet* enumeration);
    bool  setWhiteSpace  (Schema* schema, WhiteSpaceFacet*);

    virtual AtomicType* clone() const;

    XS_OALLOC(IDREFType);

protected:
    COMMON_NS::RefCntPtr<LengthFacet>       lenFacet_;
    COMMON_NS::RefCntPtr<MinLengthFacet>    minLenFacet_;
    COMMON_NS::RefCntPtr<MaxLengthFacet>    maxLenFacet_;
    COMMON_NS::OwnerPtr<PatternContainer>      pattern_;
    COMMON_NS::OwnerPtr<EnumerationContainer>  enumeration_;
};

/*! IDREFSType class represents Schema IDREFS type.
 */
class IDREFSType: public IDREFType {
public:
    /*! Constructor with delegated Resource interface.
     */
    IDREFSType(  const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;
    /*! Dump type.
     */
    virtual void    dump(int indent) const;

    /*! Type of the type is Type::IDREFS.
     */
    virtual Type type() const;

    //! Constraining Facets

    virtual AtomicType* clone() const;

    XS_OALLOC(IDREFSType);
};

/*! BooleanType class represents Schema Boolean type.
 */
class BooleanType: public AtomicType {
public:
    /*! By default it's false.
     */
    BooleanType(  const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o, const String& source, String& result) const;

    /*! Dump type.
     */
    void    dump(int indent) const;

    /*! Type of the type is Type::BOOLEAN.
     */
    virtual AtomicType::Type   type() const;

    //! Constraining Facets
    virtual bool addFacet(Schema* schema, XsFacet* facet);
    bool addPattern(Schema* schema, PatternFacet* pattern);
    bool setWhiteSpace(Schema* schema, WhiteSpaceFacet*);
    virtual bool addEnumeration(Schema* schema, EnumerationFacet* enumeration);

    virtual AtomicType* clone() const;

    XS_OALLOC(BooleanType);

private:
    COMMON_NS::OwnerPtr<PatternContainer>     pattern_;
};


/*! AnyUriType class represents Schema String type.
 */
class AnyUriType: public AtomicType {
public:
    /*! By default it's empty string.
     */
    AnyUriType(  const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o, const String& source, String& result) const;

    /*! Dump type.
     */
    void    dump(int) const;

    /*! Type of the type is Type::STRING.
     */
    virtual Type type() const;

    //! Constraining Facets
    virtual bool addFacet(Schema* schema, XsFacet* facet);
    bool setLength(Schema* schema, LengthFacet* lenFacet);
    bool setMinLength(Schema* schema, MinLengthFacet* minLenFacet);
    bool setMaxLength(Schema* schema, MaxLengthFacet* maxLenFacet);
    bool addPattern(Schema* schema, PatternFacet* pattern);
    virtual bool addEnumeration(Schema* schema, EnumerationFacet* enumeration);
    bool  setWhiteSpace  (Schema* schema, WhiteSpaceFacet*);

    virtual AtomicType* clone() const;

    XS_OALLOC(DAnyUriType);

private:
    String value_;
    COMMON_NS::RefCntPtr<LengthFacet>      lenFacet_;
    COMMON_NS::RefCntPtr<MinLengthFacet>   minLenFacet_;
    COMMON_NS::RefCntPtr<MaxLengthFacet>   maxLenFacet_;
    COMMON_NS::OwnerPtr<PatternContainer>     pattern_;
    COMMON_NS::OwnerPtr<EnumerationContainer> enumeration_;
};



/*! AnyType class represents Schema AnyType.
 */
class AnyType: public AtomicType {
public:
    /*! Constructor with delegated Resource interface.
     */
    AnyType(const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;

    /*! Dump type.
     */
    virtual void    dump(int indent) const;

    /*! Type of the type is Type::ANY.
     */
    virtual Type type() const;

    //! Constraining Facets
    virtual bool addFacet(Schema* schema, XsFacet* facet);
    bool setLength(Schema* schema, LengthFacet* lenFacet);
    bool setMinLength(Schema* schema, MinLengthFacet* minLenFacet);
    bool setMaxLength(Schema* schema, MaxLengthFacet* maxLenFacet);
    bool addPattern(Schema* schema, PatternFacet* pattern);
    virtual bool addEnumeration(Schema* schema, EnumerationFacet* enumeration);
    bool  setWhiteSpace  (Schema* schema, WhiteSpaceFacet* whiteSpaceFacet);

    virtual AtomicType* clone() const;

    XS_OALLOC(DAnyType);

protected:
    void whiteSpacePrepare(const String& source, String& result) const;

    COMMON_NS::RefCntPtr<LengthFacet>     lenFacet_;
    COMMON_NS::RefCntPtr<MinLengthFacet>  minLenFacet_;
    COMMON_NS::RefCntPtr<MaxLengthFacet>  maxLenFacet_;
    COMMON_NS::RefCntPtr<WhiteSpaceFacet> whiteSpaceFacet_;
private:
    COMMON_NS::OwnerPtr<PatternContainer>    pattern_;
    COMMON_NS::OwnerPtr<EnumerationContainer> enumeration_;
};

/*! IntegerType class represents Schema Integer type.
 */
class IntegerType: public NumberType {
public:
    IntegerType(const Origin& origin, const NcnCred& cred);

    bool less(String& first, String& second) const
        { return first.toInt() < second.toInt();}
    bool lessEq(String& first, String& second) const
        { return first.toInt() < second.toInt();}

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;
    /*! Dump type.
     */
    void    dump(int) const;

    /*! Type of the type is Type::INTEGER.
     */
    virtual AtomicType::Type   type() const;

    virtual AtomicType* clone() const;

    XS_OALLOC(IntegerType);
};


/*! LongType class represents Schema Long type.
  There is some processor depended restriction on representation long type.
  By the Schema Part2 [3.3.11] long type is restricted by the value
  9223372036854775807. But in this realisation I use LONG_MAX, declared in
  limits.h, which is actually 2147483647 for i386 and 9223372036854775807 for
  alpha and sparc processors.
 */
class LongType: public IntegerType {
public:
    LongType(const Origin& origin, const NcnCred& cred);

    /*! Dump type.
     */
    void    dump(int) const;

    /*! Type of the type is Type::LONG.
     */
    virtual AtomicType::Type   type() const;

    /*! See Schema Part 2 WD [3.3.11]. Long is derived
      from integer by setting the value of maxInclusive and minInclusive.
    */
    virtual bool setMaxInclusive(Schema* schema,
                                 MaxInclusiveFacet* maxInclFacet);
    virtual bool setMinInclusive(Schema* schema,
                                 MinInclusiveFacet* minInclFacet);

    virtual AtomicType* clone() const;

    XS_OALLOC(LongType);
};

/*! NonNegativeIntegerType class represents Schema NonNegativeInteger type.
 */
class NonNegativeIntegerType: public IntegerType {
public:
    NonNegativeIntegerType(COMMON_NS::RefCounted<>* rp,
                           const Origin& origin, const NcnCred& cred);
    NonNegativeIntegerType(const Origin& origin, const NcnCred& cred);

    void    dump(int) const;

    /*! Type of the type is Type::NONNEGATIVEINTEGER.
     */
    virtual AtomicType::Type   type() const;

    /*! See Schema Part 2 WD [3.3.9]. NonNegativeInteger is derived
      from integer by setting the value of minInclusive to be 0.
    */
    virtual bool setMinInclusive(Schema* schema,
                                 MinInclusiveFacet* minInclFacet);
    virtual AtomicType* clone() const;

    XS_OALLOC(NonNegIntType);
};

/*! NonPositiveIntegerType class represents Schema NonPositiveInteger type.
 */
class NonPositiveIntegerType: public IntegerType {
public:
    NonPositiveIntegerType(const Origin& origin, const NcnCred& cred);

    void    dump(int) const;

    /*! Type of the type is Type::NONPOSITIVEINTEGER.
     */
    virtual AtomicType::Type   type() const;

    /*! See Schema Part 2 WD [3.3.9]. NonPositiveInteger is derived
      from integer by setting the value of maxInclusive to be 0.
    */
    virtual bool setMaxInclusive(Schema* schema,
                                 MaxInclusiveFacet* maxInclFacet);
    virtual AtomicType* clone() const;

    XS_OALLOC(NonPosIntType);
};

/*! NegativeIntegerType class represents Schema NegativeInteger type.
 */
class NegativeIntegerType: public NonPositiveIntegerType {
public:

    NegativeIntegerType(const Origin& origin, const NcnCred& cred);

    /*! Dump type.
     */
    void    dump(int) const;

    /*! Type of the type is Type::NEGATIVEINTEGER.
     */
    virtual AtomicType::Type   type() const;

    /*! See Schema Part 2 WD [3.3.9]. NegativeInteger is derived
      from nonPositiveInteger by setting the value of maxInclusive to be -1.
    */
    virtual bool setMaxInclusive(Schema* schema,
                                 MaxInclusiveFacet* maxInclFacet);
    virtual AtomicType* clone() const;

    XS_OALLOC(NegativeIntType);
};

/*! PositiveIntegerType class represents Schema PositiveInteger type.
 */
class PositiveIntegerType: public NonNegativeIntegerType {
public:

    PositiveIntegerType(const Origin& origin, const NcnCred& cred);

    void    dump(int) const;
    /*! Type of the type is Type::POSITIVEINTEGER.
     */
    virtual AtomicType::Type   type() const;

    /*! See Schema Part 2 WD [3.3.9]. PositiveInteger is derived
      from nonNegativeInteger by setting the value of minInclusive to be 1.
    */
    virtual bool setMinInclusive(Schema* schema,
                                 MinInclusiveFacet* minInclFacet);
    virtual AtomicType* clone() const;

    XS_OALLOC(PosIntType);
};

/*! IntType class represents Schema Int type.
 */
class IntType: public LongType {
public:
    IntType(const Origin& origin, const NcnCred& cred);

    /*! Dump type.
     */
    void    dump(int) const;

    /*! Type of the type is Type::INT.
     */
    virtual AtomicType::Type   type() const;

    /*! See Schema Part 2 WD [3.3.9]. Int is derived
      from long by setting the value of maxInclusive and minInclusive.
    */
    virtual bool setMaxInclusive(Schema* schema,
                                 MaxInclusiveFacet* maxInclFacet);
    virtual bool setMinInclusive(Schema* schema,
                                 MinInclusiveFacet* minInclFacet);
    virtual AtomicType* clone() const;

    XS_OALLOC(IntType);
};

/*! ShortType class represents Schema Short type.
 */
class ShortType: public IntType {
public:
    ShortType(const Origin& origin, const NcnCred& cred);

    void    dump(int) const;
    /*! Type of the type is Type::SHORT.
     */
    virtual AtomicType::Type   type() const;

    /*! See Schema Part 2 WD [3.3.9]. Short is derived
      from long by setting the value of maxInclusive and minInclusive.
    */
    virtual bool setMaxInclusive(Schema* schema,
                                 MaxInclusiveFacet* maxInclFacet);
    virtual bool setMinInclusive(Schema* schema,
                                 MinInclusiveFacet* minInclFacet);
    virtual AtomicType* clone() const;

    XS_OALLOC(ShortType);
};

/*! ByteType class represents Schema Byte type.
 */
class ByteType: public ShortType {
public:

    ByteType(  const Origin& origin, const NcnCred& cred);

    /*! Dump type.
     */
    void    dump(int) const;

    /*! Type of the type is Type::BYTE.
     */
    virtual AtomicType::Type   type() const;

    /*! See Schema Part 2 WD [3.3.14]. Byte is derived
      from short by setting the value of maxInclusive and minInclusive.
      to [-128;127].
    */
    virtual bool setMaxInclusive(Schema* schema,
                                 MaxInclusiveFacet* maxInclFacet);
    virtual bool setMinInclusive(Schema* schema,
                                 MinInclusiveFacet* minInclFacet);

    virtual AtomicType* clone() const;

    XS_OALLOC(ByteType);
};

/*! UnsignedLongType class represents Schema UnsignedLong type.
 */
class UnsignedLongType: public NonNegativeIntegerType {
public:
    UnsignedLongType(const Origin& origin, const NcnCred& cred);

    void    dump(int) const;
    /*! Type of the type is Type::UNSIGNEDLONG.
     */
    virtual AtomicType::Type   type() const;

    /*! See Schema Part 2 WD [3.3.16]. UnsignedLong is derived
      from nonNegativeInteger by setting the value of maxInclusive.
    */
    virtual bool setMaxInclusive(Schema* schema,
                                 MaxInclusiveFacet* maxInclFacet);
    virtual AtomicType* clone() const;

    USE_OALLOC(UnsLongType);
};

/*! UnsignedIntType class represents Schema Int type.
 */
class UnsignedIntType: public UnsignedLongType {
public:
    UnsignedIntType(const Origin& origin, const NcnCred& cred);

    void    dump(int) const;
    /*! Type of the type is Type::UNSIGNEDINT.
     */
    virtual AtomicType::Type   type() const;

    /*! See Schema Part 2 WD [3.3.17]. UnsignedInt is derived
      from UnsignedLong by setting the value of maxInclusive.
    */
    virtual bool setMaxInclusive(Schema* schema,
                                 MaxInclusiveFacet* maxInclFacet);
    virtual AtomicType* clone() const;

    XS_OALLOC(UnsIntType);
};

/*! UnsignedShortType class represents Schema UnsignedShort type.
 */
class UnsignedShortType: public UnsignedIntType {
public:
    UnsignedShortType(const Origin& origin, const NcnCred& cred);

    void    dump(int) const;
    /*! Type of the type is Type::UNSIGNEDSHORT.
     */
    virtual AtomicType::Type   type() const;

    /*! See Schema Part 2 WD [3.3.18]. UnsignedShort is derived
      from UnsignedInt by setting the value of maxInclusive.                .
    */
    virtual bool setMaxInclusive(Schema* schema,
                                 MaxInclusiveFacet* maxInclFacet);
    virtual AtomicType* clone() const;

    XS_OALLOC(UnsShortType);
};

/*! UnsignedByteType class represents Schema UnsignedByte type.
 */
class UnsignedByteType: public UnsignedShortType {
public:

    UnsignedByteType(const Origin& origin, const NcnCred& cred);

    void    dump(int) const;
    /*! Type of the type is Type::UNSIGNEDBYTE.
     */
    virtual AtomicType::Type   type() const;

    /*! See Schema Part 2 WD [3.3.19]. UnsignedByte is derived
      from UnsignedShort by setting the value of maxInclusive to be 255.
    */
    virtual bool setMaxInclusive(Schema* schema,
                                 MaxInclusiveFacet* maxInclFacet);
    virtual AtomicType* clone() const;

    XS_OALLOC(UnsByteType);
};


/*! NormalizedStringType class represents Schema Cdata type.
 */
class NormalizedStringType: public StringType {
public:
    /*! Constructor with delegated Resource interface.
     */
    NormalizedStringType(const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;
    virtual void    dump(int) const;

    /*! Type of the type is Type::CDATA.
     */
    virtual Type type() const;

    virtual bool addEnumeration(Schema* schema, EnumerationFacet* enumeration);

    virtual AtomicType* clone() const;

    XS_OALLOC(NormStringType);
};

/*! TokenType class represents Schema Token type.
  The base type of token is CDATA. In C++ representation
  token is derived from StringType directly, to not make
  additional checking in CDATAType.
 */
class TokenType: public StringType {
public:
    /*! Constructor with delegated Resource interface.
     */
    TokenType(const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;
    virtual void    dump(int) const;

    /*! Type of the type is Type::TOKEN.
     */
    virtual Type type() const;

    virtual bool addEnumeration(Schema* schema, EnumerationFacet* enumeration);
    virtual AtomicType* clone() const;

    XS_OALLOC(TokenType);
};


/*! LanguageType class represents Schema Language type.
  The base type of language is TokenType.
 */
class LanguageType: public TokenType {
public:
    /*! Constructor with delegated Resource interface.
     */
    LanguageType(const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;
    /*! Dump type.
     */
    virtual void    dump(int) const;

    /*! Type of the type is Type::LANGUAGE.
     */
    virtual Type type() const;

    virtual bool addEnumeration(Schema* schema, EnumerationFacet* enumeration);

    virtual AtomicType* clone() const;

    XS_OALLOC(LanguageType);
};

class NameType: public TokenType {
public:
    /*! Constructor with delegated Resource interface.
     */
    NameType(const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;
    virtual void    dump(int) const;

    /*! Type of the type is Type::NAME.
     */
    virtual Type type() const;

    virtual bool addEnumeration(Schema* schema, EnumerationFacet* enumeration);

    virtual AtomicType* clone() const;

    XS_OALLOC(NameType);
};

/*! NmtokenType class represents Schema Nmtoken type.
  The base type of nmtoken is TokenType.
 */
class NmtokenType: public TokenType {
public:
    /*! Constructor with delegated Resource interface.
     */
    NmtokenType(  const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;
    virtual void    dump(int) const;

    /*! Type of the type is Type::NMTOKEN.
     */
    virtual Type type() const;

    virtual bool addEnumeration(Schema* schema, EnumerationFacet* enumeration);

    virtual AtomicType* clone() const;

    XS_OALLOC(NmtokenType);
};

/*! NmtokensType class represents Schema Nmtokens type.
  The base type of nmtokens is NmtokenType.
  There is a question. What is the difference between NMTOKEN and NMTOKENS
  in implementation and representation mode. For now NMTOKENS does nothing,
  it just uses NMTOKEN interface.
 */
class NmtokensType: public NmtokenType {
public:
    /*! Constructor with delegated Resource interface.
     */
    NmtokensType(  const Origin& origin, const NcnCred& cred);

    /*! Dump type.
     */
    virtual void    dump(int) const;

    /*! Type of the type is Type::NMTOKENS.
     */
    virtual Type type() const;

    virtual AtomicType* clone() const;

    XS_OALLOC(NmtokensType);
};

class EntityType: public NmtokenType {
public:
    /*! Constructor with delegated Resource interface.
     */
    EntityType(const Origin& origin, const NcnCred& cred);

    /*! Dump type.
     */
    virtual void    dump(int) const;

    /*! Type of the type is Type::NMTOKENS.
     */
    virtual Type type() const;

    virtual AtomicType* clone() const;

    XS_OALLOC(EntityType);
};


/*! NCNameType class represents Schema NCName type.
  The base type of NCName is NameType.
 */
class NCNameType: public NameType {
public:
    /*! Constructor with delegated Resource interface.
     */
    NCNameType(  const Origin& origin, const NcnCred& cred);

    /*! Validate source and return result - modified source in accordance
       with other constrains.
    */
    virtual bool validate(Schema* schema, const GroveLib::Node* o,
                          const String& source, String& result) const;
    virtual void    dump(int) const;

    /*! Type of the type is Type::NAME.
     */
    virtual Type type() const;

    virtual bool addEnumeration(Schema* schema, EnumerationFacet* enumeration);

    virtual AtomicType* clone() const;

    XS_OALLOC(NCNameType);
};

/*! DateTimeType class represents Schema TimeInstant type.
 */
class DateTimeType: public RecurringType {
public:
    DateTimeType(const Origin& origin, const NcnCred& cred);

    /*! Dump type.
     */
    void    dump(int) const;

    /*! Type of the type is Type::TimeInstant.
     */
    virtual AtomicType::Type  type() const;

    virtual AtomicType* clone() const;

    XS_OALLOC(DateTimeType);
};

/*! DayType class represents Schema RecurringDay type.
 */
class DayType: public RecurringType {
public:
    DayType(const Origin& origin, const NcnCred& cred);

    /*! Lexical Representation, used for Right representation
    of derived types of RecurringType.
    */
    virtual void formLexicalRepresentation(String& result,bool sign,
                                           String&,String&,
                                           String& day,String&,
                                           String&,String&,
                                           String&) const;
    /*! Dump type.
     */
    void    dump(int) const;

    /*! Type of the type is Type::RecurringDay.
     */
    virtual AtomicType::Type  type() const;

    virtual AtomicType* clone() const;

    XS_OALLOC(DayType);
};

/*! MonthDayType class represents Schema RecurringDate type.
 */
class MonthDayType: public RecurringType {
public:
    MonthDayType(const Origin& origin, const NcnCred& cred);

    /*! Lexical Representation, used for Right representation
    of derived types of RecurringType.
    */
    virtual void formLexicalRepresentation(String& result, bool sign,
                                           String&, String& month,
                                           String& day, String&,
                                           String&, String&,
                                           String&) const;
    /*! Dump type.
     */
    void    dump(int) const;

    /*! Type of the type is Type::RecurringDate.
     */
    virtual AtomicType::Type  type() const;

    virtual AtomicType* clone() const;

    XS_OALLOC(MonthDayType);
};

/*! TimePeriodType class represents Schema TimePeriod type.
 */
class TimePeriodType: public RecurringType {
public:
    TimePeriodType(const Origin& origin, const NcnCred& cred);

    /*! Dump type.
     */
    void    dump(int) const;

    /*! Type of the type is Type::TimePeriod.
     */
    virtual AtomicType::Type  type() const;

    XS_OALLOC(TimePeriodType);
};

/*! TimeType class represents Schema Time type.
 */
class TimeType: public RecurringType {
public:
    TimeType(const Origin& origin, const NcnCred& cred);

    /*! Lexical Representation of Time Type
    */
    virtual void formLexicalRepresentation(String& result,bool sign,String&,
                                   String&, String&, String& hour,
                                   String& minute, String& sec,
                                   String& msec) const;

    void    dump(int) const;
    /*! Type of the type is Type::Time.
     */
    virtual AtomicType::Type  type() const;
    virtual AtomicType* clone() const;

    XS_OALLOC(TimeType);
};

/*! DateType class represents Schema Date type.
 */
class DateType: public TimePeriodType {
public:
    DateType(const Origin& origin, const NcnCred& cred);

    /*! Lexical Representation, used for Right representation
    of derived types of RecurringType.
    */
    virtual void formLexicalRepresentation(String& result,bool sign,
                    String& year, String& month, String& day, String&,
                    String&, String&, String& ) const;
    void    dump(int) const;

    /*! Type of the type is Type::Date.
     */
    virtual AtomicType::Type  type() const;

    virtual AtomicType* clone() const;

    XS_OALLOC(TimePeriodType);
};

/*! MonthType class represents Schema Month type.
 */
class MonthType: public TimePeriodType {
public:
    MonthType(const Origin& origin,
              const NcnCred& cred);

    /*! Lexical Representation, used for Right representation
    of derived types of DateTimeType.
    */
    virtual void formLexicalRepresentation(String& result,bool sign,
                                           String& year,
                                           String& month,
                                           String&, String&,
                                           String&, String&,
                                           String&) const;
    /*! Dump type.
     */
    void    dump(int) const;

    /*! Type of the type is Type::Month.
     */
    virtual AtomicType::Type  type() const;

    virtual AtomicType* clone() const;

    XS_OALLOC(MonthType);
};

/*! YearMonthType class represents Schema Month type.
 */
class YearMonthType: public TimePeriodType {
public:
    YearMonthType(const Origin& origin, const NcnCred& cred);

    /*! Lexical Representation, used for Right representation
    of derived types of RecurringType.
    */
    virtual void formLexicalRepresentation(String& result,bool sign,
                                           String& year,String& month,
                                           String& , String&,
                                           String& , String&,
                                           String& ) const;
    void    dump(int) const;
    /*! Type of the type is Type::Month.
     */
    virtual AtomicType::Type  type() const;
    virtual AtomicType* clone() const;

    XS_OALLOC(YearMonthType);
};

/*! YearType class represents Schema Year type.
 */
class YearType: public TimePeriodType {
public:
    YearType(const Origin& origin, const NcnCred& cred);

    /*! Lexical Representation, used for Right representation
    of derived types of RecurringType.
    */
    virtual void formLexicalRepresentation(String& result,bool sign,
                                           String& year, String&,
                                           String&, String&,
                                           String&, String&,
                                           String&) const;
    void    dump(int) const;
    /*! Type of the type is Type::Year.
     */
    virtual AtomicType::Type  type() const;
    virtual AtomicType* clone() const;

    XS_OALLOC(YearType);
};

XS_NAMESPACE_END

#endif // SCHEMA_DATA_TYPES_H
