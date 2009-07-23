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
    Yet another Fixed-point test
 */
#include "common/common_defs.h"
#include "common/FixedDecimal.h"
#include <iostream>
#include <assert.h>


USING_COMMON_NAMESPACE

void abort()
{
    exit(1);
}

int
main(int ac, char** av)
{
    FixedDecimal* m_value0;

    //
    // Testing for equivalence
    //
    assert (FixedDecimal("12345.6789") == FixedDecimal("12345.6789"));
    assert (FixedDecimal(12345,6) == FixedDecimal(12345,6));
    assert (FixedDecimal(1234) == FixedDecimal(1234));
    assert (FixedDecimal("12345.6789",3) == FixedDecimal("12345.6789",3));

    assert (FixedDecimal("12345.678900") == FixedDecimal("12345.6789"));
    assert (FixedDecimal(12345,6) == FixedDecimal(123450,7));
    assert (FixedDecimal("12345.67890",3) == FixedDecimal("12345.679",3));

    assert (FixedDecimal("12345",3) == FixedDecimal("12345.000",3));

    assert (FixedDecimal("12345.67890") == FixedDecimal(1234567890,5));

    assert (FixedDecimal("12345") == FixedDecimal(12345));
    assert (FixedDecimal("12345.67890") == FixedDecimal("12345.67890",5));

    assert (FixedDecimal("123456789") == (int64)123456789);
    assert (FixedDecimal(12345) == (int64)12345);

    assert (FixedDecimal("12345.6789") == -FixedDecimal("-12345.6789"));
    assert (FixedDecimal(12345,6) == -FixedDecimal(-12345,6));
    assert (FixedDecimal(1234) == -FixedDecimal(-1234));
    assert (FixedDecimal("12345.6789",3) == -FixedDecimal("-12345.6789",3));

    assert (FixedDecimal("12345.678900") == -FixedDecimal("-12345.6789"));
    assert (FixedDecimal(12345,6) == -FixedDecimal(-123450,7));
    assert (FixedDecimal("12345.67890",3) == -FixedDecimal("-12345.67890",3));

    assert (FixedDecimal("12345",3) == -FixedDecimal("-12345.000",3));

    assert (FixedDecimal("12345.67890") == -FixedDecimal(-1234567890,5));
    assert (FixedDecimal("12345") == -FixedDecimal(-12345));
    assert (FixedDecimal("12345.67890") == -FixedDecimal("-12345.67890",5));

    assert (FixedDecimal("-123456789") == (int64)-123456789);
    assert (FixedDecimal(-12345) == (int64)-12345);

    assert (FixedDecimal("12345.67820",3) == FixedDecimal("12345.678",3));
    assert (FixedDecimal("12345.67820",3) == -FixedDecimal("-12345.678",3));

    //
    // Create/delete
    try {
        m_value0 = new FixedDecimal("123456789012345678");
        delete m_value0;
    } catch (...) {
     cerr << "Creation Failed: FixedDecimal(\"123456789012345678\")" << endl;
    }

    try {
        m_value0 = new FixedDecimal("1234567890123456789");
        delete m_value0;
    } catch (...) {
         cerr << "Creation Failed: FixedDecimal(\"1234567890123456789\")" << endl;
    }

    try {
        m_value0 = new FixedDecimal("1234567890123456789.1");
        delete m_value0;
        cerr << "Creation NOT! Failed: FixedDecimal(\"1234567890123456789.1\")"  << endl;
    } catch (...) {
        // ok!
    }

    try {
        m_value0 = new FixedDecimal(FixedDecimal::MAX_VALUE);
        delete m_value0;
    } catch (...) {
        cerr << "Creation Failed: FixedDecimal(MAX_VALUE)" << endl;
    }

    try {
        m_value0 = new FixedDecimal("9223372036854775808");
        delete m_value0;
        cerr << "Creation NOT! Failed: FixedDecimal(\"9223372036854775808\")" << endl;
    } catch (...) {
        // ok
    }

    try {
        m_value0 = new FixedDecimal(-1);
        delete m_value0;
    } catch (...) {
        cerr << "Creation Failed: FixedDecimal(-1)" << endl;
    }

    //
    // Test for inequality
    //
    assert (FixedDecimal("12345.6789") != FixedDecimal("12345.67891"));
    assert (FixedDecimal(12345,6) != FixedDecimal(12345,7));
    assert (FixedDecimal(1234) != FixedDecimal(2234));
    assert (FixedDecimal("123456789",4) != FixedDecimal("12345.6789",4));

    assert (FixedDecimal("12345.678901") != FixedDecimal("12345.6789"));
    assert (FixedDecimal(12345,6) != FixedDecimal(12345,7));
    assert (FixedDecimal("12345.67820",3) != FixedDecimal("12345.67820",6));

    assert (FixedDecimal("12345000",3) != FixedDecimal("12345.000",3));

    assert (FixedDecimal("12345.67890") != FixedDecimal(1234567890));
    assert (FixedDecimal("12345.67890") != FixedDecimal("1234567890",6));

    assert (FixedDecimal("12345.6789") != (int64)1234567891);
    assert (FixedDecimal(12345,6) != (int64)12345);
    assert (FixedDecimal(1234) != (int64)2234);
    assert (FixedDecimal("123456789",4) != (int64)12345.6789);

    assert (FixedDecimal("12345.6789") != FixedDecimal("-12345.6789"));
    assert (FixedDecimal(12345,6) != FixedDecimal(-12345,6));
    assert (FixedDecimal(1234) != FixedDecimal(-1234));
    assert (FixedDecimal("12345.6789",3) != FixedDecimal("-12345.6789",3));

    assert (FixedDecimal("12345.678900") != FixedDecimal("-12345.6789"));
    assert (FixedDecimal(12345,6) != FixedDecimal(-123450,7));
    assert (FixedDecimal("12345.67890",3) != FixedDecimal("-12345.67890",6));

    assert (FixedDecimal("12345",3) != FixedDecimal("-12345.000",3));

    assert (FixedDecimal("12345.67890") != FixedDecimal(-1234567890,5));
    assert (FixedDecimal("12345") != FixedDecimal(-12345));
    assert (FixedDecimal("12345.67890") != FixedDecimal("-12345.67890",5));

    assert (FixedDecimal("123456789") != (int64)-123456789);
    assert (FixedDecimal(12345) != (int64)-12345);

    assert (FixedDecimal("12345.67890",3) != FixedDecimal("12345.67890",6));

    //
    // Multiplication tests
    //
    assert ((FixedDecimal("123")*FixedDecimal(24)) == FixedDecimal("2952"));
    assert (FixedDecimal("12345.04")*(int64)250 == FixedDecimal(3086260));
    assert (FixedDecimal("12567856345.056")*(int64)457 == FixedDecimal("5743510349690.592"));
    assert (FixedDecimal(-12345)*(int64)-1 == FixedDecimal(12345));
    assert (FixedDecimal("-12345")*FixedDecimal("-0") == FixedDecimal("0"));
    assert (FixedDecimal("500000000000000000.5")*FixedDecimal("0.02")==FixedDecimal("10000000000000000.01"));
    assert (FixedDecimal(12345)*(int64)24 == (int64)24*FixedDecimal(12345));

    assert (FixedDecimal(12345)*(int64)24*FixedDecimal("456.4555") == FixedDecimal(12345)*((int64)24*FixedDecimal("456.4555")));

    assert (FixedDecimal("4611686018427387903")*(int64)2 == FixedDecimal("4611686018427387903")*(int64)2);

    try {
        assert (FixedDecimal("4611686018427387904")*(int64)2 == FixedDecimal("4611686018427387904")*(int64)2);
        cerr << "Multiplication NOT! Failed: FixedDecimal(0x4000000000000000)*(int64)2" << endl;
    } catch (...) {

    }

    m_value0 = new FixedDecimal(3246546);
    *m_value0 *= FixedDecimal(564);
    assert (*m_value0 == FixedDecimal(3246546*564));
    delete m_value0;

    m_value0 = new FixedDecimal("12567856345.056");
    *m_value0 *= FixedDecimal("45.7");
    assert (*m_value0 == FixedDecimal("574351034969.059"));
    delete m_value0;

    m_value0 = new FixedDecimal("12567856345.057");
    *m_value0 *= FixedDecimal("6.8");
    assert (*m_value0 == FixedDecimal("85461423146.388"));
    delete m_value0;

    //
    // Tests for addition
    //
    assert (FixedDecimal(12345)+(int64)24 == FixedDecimal(12345+24));
    assert (FixedDecimal("12345.0456789")+FixedDecimal("25000000000") == FixedDecimal("25000012345.0456789"));

    assert (FixedDecimal(12345)+(int64)24 == (int64)24+FixedDecimal(12345));

    assert ((int64)24+FixedDecimal("456.4555")+(int64)54564564 == (int64)24+(FixedDecimal("456.4555")+(int64)54564564));

    assert (FixedDecimal(12345)+(int64)-12345 == FixedDecimal((int64)0));

    assert (FixedDecimal("4611686018427387903")+FixedDecimal("4611686018427387903") ==
            FixedDecimal("8070450532247928830")+FixedDecimal("1152921504606846976"));

    try {
        assert (FixedDecimal("4611686018427387904")+FixedDecimal("4611686018427387904") ==
                FixedDecimal("4611686018427387904")+FixedDecimal("4611686018427387904"));
        cerr << "Addition NOT! Failed: FixedDecimal(0x4000000000000000)+FixedDecimal(0x4000000000000000) " << endl;
    } catch (...) { }

    try {
        assert (FixedDecimal("-4611686018427387904")+FixedDecimal("-4611686018427387904") ==
                FixedDecimal("-4611686018427387904")+FixedDecimal("-4611686018427387904"));
        cerr << "Addition NOT! Failed: FixedDecimal(\"-4611686018427387904\")+FixedDecimal(\"-4611686018427387904\") == "
                "FixedDecimal(\"-4611686018427387904\")+FixedDecimal(\"-4611686018427387904\")" << endl;
    } catch (...) { }

    try {
        m_value0 = new FixedDecimal("100000000000000000");
        *m_value0 += FixedDecimal("0.001");
        delete m_value0;
        cerr << "100000000000000000+0.001 not failed."<< endl;
    } catch (...) {
    }

    m_value0 = new FixedDecimal(32465);
    *m_value0 += FixedDecimal(564);
    assert (*m_value0 == FixedDecimal(32465+564));
    delete m_value0;

    m_value0 = new FixedDecimal("12567856300.056");
    *m_value0 += FixedDecimal("45.0006446");
    assert (*m_value0 == FixedDecimal("12567856345.0566446"));
    delete m_value0;

    //
    // Tests for subtraction
    //
    assert (FixedDecimal(12345)-(int64)24 == FixedDecimal(12345-24));
    assert ((int64)24-FixedDecimal(12345) == FixedDecimal(24-12345));
    assert (FixedDecimal("12345.0456789")-FixedDecimal("25000000000") == FixedDecimal("-24999987654.9543211"));

    assert (FixedDecimal(12345)-(int64)24 == -((int64)24-FixedDecimal(12345)));

    assert ((int64)24-FixedDecimal("456.4555")-(int64)54564564 == (int64)24-(FixedDecimal("456.4555")+(int64)54564564));

    assert (FixedDecimal(12345)-(int64)12345 == FixedDecimal((int64)0));

    assert (-FixedDecimal("4611686018427387903")-FixedDecimal("4611686018427387903") ==
            -FixedDecimal("8070450532247928830")-FixedDecimal("1152921504606846976"));

    try {
        assert (-FixedDecimal("4611686018427387904")-FixedDecimal("4611686018427387904") ==
                -FixedDecimal("4611686018427387904")-FixedDecimal("4611686018427387904"));
        cerr << "Subtraction NOT! Failed: -FixedDecimal(0x4000000000000000)-(int64)0x4000000000000000 " << endl;
    } catch (...) { }

    m_value0 = new FixedDecimal(3246546);
    *m_value0 -= FixedDecimal(564);
    assert (*m_value0 == FixedDecimal(3246546-564));
    delete m_value0;

    m_value0 = new FixedDecimal("12567856300.056");
    *m_value0 -= FixedDecimal("45.0006446");
    assert (*m_value0 == FixedDecimal("12567856255.0553554"));
    delete m_value0;


    //
    // Tests for division
    //
    assert (FixedDecimal(12345)/(int64)24 == FixedDecimal(12345/24));
    assert ((int64)24/FixedDecimal(12345) == FixedDecimal("0"));
    assert (FixedDecimal("0")/FixedDecimal(12345) == FixedDecimal("0"));
    assert ((int64)65466/FixedDecimal("6.4555865")/(int64)545 == (int64)65466/(FixedDecimal("6.4555865")*(int64)545));

    assert (FixedDecimal(12345)/(int64)12345 == FixedDecimal((int64)1));
    assert (FixedDecimal("10000000000000000.5")/FixedDecimal("2.00")==FixedDecimal("5000000000000000.25"));
    assert (FixedDecimal("100000000000000000.5")/FixedDecimal("2.00")==FixedDecimal("50000000000000000.25"));

    m_value0 = new FixedDecimal(3246546);
    *m_value0 /= FixedDecimal(564);
    assert (*m_value0 == FixedDecimal(5756));
    delete m_value0;

    try {
        m_value0 = new FixedDecimal("12567856345.056");
        *m_value0 /= FixedDecimal("45.7");
        assert (*m_value0 == FixedDecimal("275007797.485"));
        delete m_value0;
    } catch (...) {delete m_value0;}

    try {
        assert (FixedDecimal("12345.3453400")/(int64)24 != (int64)1/((int64)24/FixedDecimal("12345.3453400")));
    } catch (...) {}

    try {
        *m_value0=FixedDecimal("12345")/FixedDecimal("0");
        cerr << "Division by 0 is legal! " << endl;
    } catch(...) {}

    try {
        *m_value0=FixedDecimal("123457457.4557457")/FixedDecimal("0.0000005435435");
        cerr << "Division is not overflowed! FixedDecimal(\"123457457.4557457\")/FixedDecimal(\"0.0000005435435\")" << endl;
    } catch(...) {}

    assert(FixedDecimal("333.455")/FixedDecimal("135463354")==FixedDecimal("0"));

    assert (FixedDecimal("12345.0456789")/(int64)254565 == FixedDecimal("0.0484947"));

    //
    // Tests for comparison
    //
    assert(FixedDecimal("3336546546546.45")>FixedDecimal("0.14"));
    assert(FixedDecimal("3336546546546.455")>FixedDecimal("0.14"));
    assert(FixedDecimal(1)>FixedDecimal(-654243));
    assert(FixedDecimal(345)>(int64)0);
    assert(FixedDecimal(56)>=FixedDecimal(56));
    assert(FixedDecimal(56)>=FixedDecimal(56));
    assert(FixedDecimal("0.999999999999999999")<FixedDecimal(1));
    assert(FixedDecimal("56.555")<=FixedDecimal("56.555000000000001"));

    //
    // Tests for rescaling
    //
    assert((FixedDecimal(56)^2)==FixedDecimal(56));
    assert((FixedDecimal("56.456456")^2)==FixedDecimal("56.46"));

    //
    // Tests for rounding
    //
    assert(FixedDecimal("56.50").round()==(int64)57);
    assert(FixedDecimal("56.49").round()==(int64)56);
    assert(FixedDecimal("-56.49").round()==(int64)-56);
    assert(FixedDecimal("-56.50").round()==(int64)-57);

    assert(FixedDecimal("56.50").trunc()==(int64)56);
    assert(FixedDecimal("-56.49").trunc()==(int64)-56);

    assert(FixedDecimal("56.50").ceil()==(int64)57);
    assert(FixedDecimal("56.49").ceil()==(int64)57);
    assert(FixedDecimal("-56.49").ceil()==(int64)-56);
    assert(FixedDecimal("-56.50").ceil()==(int64)-56);

    assert(FixedDecimal("56.50").floor()==(int64)56);
    assert(FixedDecimal("56.49").floor()==(int64)56);
    assert(FixedDecimal("-56.49").floor()==(int64)-57);
    assert(FixedDecimal("-56.50").floor()==(int64)-57);

    //
    // FixedDecimal-to-string conversions
    //
    char buf[16];
    try{
    FixedDecimal("1234567890.1234").toString(buf,16);
    assert(FixedDecimal(buf)==FixedDecimal("1234567890.1234"));
    } catch(...) {
        cerr<< "FixedDecimal(\"1234567890.1234\").toString(buf,16) failed"<<endl;
    }

    try{
    FixedDecimal("1234567890.12345").toString(buf,16);
    cerr<< "toString not overflowed!"<<endl;
    } catch(...) {}

    //
    // FixedDecimal-to-double conversions
    //
    assert((double)FixedDecimal("1234567890.1234")-(double)1234567890.1234<(double)0.0000001);
    assert((double)FixedDecimal("0.123456789012345678")-(double)0.123456789012345678<(double)1e-18);

    FixedDecimal f;
    f.fromDouble(22.55, 17);
    assert(f == FixedDecimal("22.55"));
    // cerr << "FixedDecimal(\"22.55\", 17) = " << FixedDecimal("22.55", 17) << endl;
    // cerr << "FixedDecimal.fromDouble(22.55, 17) = " << f << endl;
    cerr << "Test passed\n";
    exit(0);
}
