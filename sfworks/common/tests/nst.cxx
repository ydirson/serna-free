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
 #include "common/String.h"
#include <iostream>

#include "../impl/String.cxx"

void dump(String::StringDataRef& d);
void regress();

void testq(QString qs)
{
    std::cerr << "TESTQ: " << qs.local8Bit() << std::endl;
}

int main()
{
    String s5;
    dump(s5.d);
    std::cerr << "1 --------------\n";
    s5.truncate(0);
    dump(s5.d);
    std::cerr << "2 --------------\n";
    s5 += 'a';
    dump(s5.d);
    std::cerr << "3 --------------\n";
    s5 += 'b';
    dump(s5.d);
    std::cerr << "4 --------------\n";
    std::cerr << s5 << std::endl;
    exit(0);

    std::cerr << "sd size = " << sizeof(String::StringData) << std::endl;
    String s("abcdefjhdjfhdjhfjdhhieidjw1234567", true);
    dump(s.d);
    s.setLength(5);
    dump(s.d);
    std::cerr << "1 --------------\n";
    String s1;
    std::cerr << int(s1.isNull()) << int(s1.isEmpty()) << std::endl;
    s1 = "";
    std::cerr << int(s1.isNull()) << int(s1.isEmpty()) << std::endl;
    s1.unicode(); s1.utf8();
    std::cerr << int(s1.isNull()) << int(s1.isEmpty()) << std::endl;
    s1 = String::null;
    s1.fill('g', 5);
    std::cerr << int(s1.isNull()) << int(s1.isEmpty()) << std::endl;
    dump(s1.d);

    testq(QSREF(s1));

    std::cerr << "------------- REGRESSION ------------------\n";
    for (ulong i = 0; i < 100000; ++i) {
        if (!(i % 10000))
            std::cerr << '.';
        regress();
    }
}

void regress()
{
    String s1("abcdef8327483274", 1);
    String s2(s1);
    s2.setLength(5);
    //dump(s2.d);
    s2 = "QWERTYUIOIPASDFGHJKLZXCVBNM";
    //dump(s2.d);
    s2.unicode();
    s1.unicode();
    QString qs("poiuytrwyuwywywyw");
    s1 = qs;
    s1 += 'a';
    s2 = s1;
    s1.setLength(10);
    s1.unicode();
}

void dump(String::StringDataRef& d)
{
    if (d.operator->() == String::StringData::null()) {
        std::cerr << "SHARED-NULL\n";
        return;
    }
    const String::StringData* dp = d.operator->();
    std::cerr << "SD: " << hex << (ulong)dp
        << " RCNT" << dp->getRefCnt()
        << " UniP" << (ulong)dp->unicode
        << " UniL" << dp->len
        << " UtfP" << (ulong)dp->utf8
        << " CU" << (bool)dp->isconstu
        << " DU" << (bool)dp->dirtyutf
        << " MAXL" << dp->maxlen
        << " C8" << (bool)dp->isconst8
        << " ISQ" << (bool)dp->isqstr
        << std::endl;
    if (dp->len > 0) {
        std::cerr << "    UNICODE:<";
        for (ulong i = 0; i < dp->len; ++i)
            std::cerr << (char)(dp->unicode[i] & 0xFF);
        std::cerr << ">\n";
    }
    if (dp->utf8)
        std::cerr << "    UTF8: <" << dp->utf8 << ">\n";
}

