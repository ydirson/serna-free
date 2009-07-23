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
 #include <process.h>    /* _beginthread, _endthread */
#include <iostream>
#include "common/common_defs.h"

//This test is checking behavior of strings in multithreaded environment.
//To check memory leaks of strings use Purify


//Uncomment needed string ( String or typedef...)

#include "common/String.h"
//or
//#include <string>
//typedef std::basic_string<char> String;





USING_COMMON_NS

unsigned __stdcall Bounce( void* str );
unsigned counter;

void main()
{
    counter = 0;

    while( counter < 1000 )
    {
        counter++;
        String s1("blah blah blah");
        String s2 = s1;
        unsigned tId;
        HANDLE h[2];
        h[0] = (HANDLE)_beginthreadex( NULL, 0, &Bounce, (void*) &s1, 0, &tId);
        h[1] = (HANDLE)_beginthreadex( NULL, 0, &Bounce, (void*) &s2, 0, &tId);

        WaitForMultipleObjects(2, h, TRUE, INFINITE);
    }

    std::cout << "PASSED" << std::endl;
}

unsigned __stdcall Bounce( void* str )
{
    char cur[20];
    _ltoa(counter, cur, 10);
    String& a = *(String*)str;
    a += cur;
    _endthreadex(0);
    return 0;
}
