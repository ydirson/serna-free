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
    Test of TimeZone
 */
#include "common/common_defs.h"
#include "common/TimeZone.h"
#include <stdlib.h>
#include <iostream>


USING_COMMON_NAMESPACE
using namespace std;

const int HOW_MANY = 10000;

int mktime_test(TimeZone& tz)
{
    int res = 0;

    TimeZone::tm tztm, tztm2;
    struct tm tm;

    for (int i = 0; i < HOW_MANY; i++) {
        int mksec = rand() / 4;
        tm.tm_year = 2 + rand()/(RAND_MAX/135);
        tm.tm_mon = -5 + rand()/(RAND_MAX/20);
        tm.tm_mday = -40 + rand()/(RAND_MAX/120);
        tm.tm_hour = -40 + rand()/(RAND_MAX/120);
        tm.tm_min = -140 + rand()/(RAND_MAX/420);
        tm.tm_sec = -140 + rand()/(RAND_MAX/420);
        tm.tm_isdst = -1;

        tztm.tm_year = tm.tm_year+1900;
        tztm.tm_mon = (TimeZone::Months)tm.tm_mon;
        tztm.tm_mday = tm.tm_mday;
        tztm.tm_hour = tm.tm_hour;
        tztm.tm_min = tm.tm_min;
        tztm.tm_sec = tm.tm_sec;
        tztm.tm_isdst = (TimeZone::DST)tm.tm_isdst;
        tztm.tm_mksec = mksec;
        tztm2 = tztm;
        etime_t et = tz.mktime(&tztm2);
        tm.tm_sec += mksec / 1000000;
        time_t  t  = mktime(&tm);
        if (t != -1 && 1000000LL*(int64)t+mksec%1000000 != et) {
            cerr << "y=" << tztm.tm_year
                 << " m=" << (int) tztm.tm_mon
                 << " d=" << tztm.tm_mday
                 << " H=" << tztm.tm_hour
                 << " M=" << tztm.tm_min
                 << " S=" << tztm.tm_sec
                 << " t=" << et
                 << " exp: " << 1000000LL*t+mksec%1000000
                 << endl;
            res = 1;
        }
    }

    return res;
}


int
localtime_test(TimeZone& tz)
{
    int res = 0;
    // [-2145974400, 2114323200]
    TimeZone::tm tztm;
    time_t t;
    tm* tms;

    for (int i = 0, j = 0; i < HOW_MANY; i++, j = !j) {
        t = rand();
        if (t > 2114323200)
            t /= 2;
#ifndef _WINDOWS_
        if (j)
            t = -t;
#endif
        tms = localtime(&t);
        (void) tz.localtime(1000000LL*t, &tztm);
        if (tms->tm_sec != tztm.tm_sec) {
            cerr << "t=" << t << " tm_sec=" << tztm.tm_sec
                 << " exp:" << tms->tm_sec << endl;
            res = 1;
        }
        if (tms->tm_min != tztm.tm_min) {
            cerr << "t=" << t << " tm_min=" << tztm.tm_min
                 << " exp:" << tms->tm_min << endl;
            res = 1;
        }
        if (tms->tm_hour != tztm.tm_hour) {
            cerr << "t=" << t << " tm_hour=" << tztm.tm_hour
                 << " exp:" << tms->tm_hour << endl;
            res = 1;
        }
        if (tms->tm_mday != tztm.tm_mday) {
            cerr << "t=" << t << " tm_mday=" << tztm.tm_mday
                 << " exp:" << tms->tm_mday << endl;
            res = 1;
        }
        if (tms->tm_mon != tztm.tm_mon) {
            cerr << "t=" << t << " tm_mon=" << (int) tztm.tm_mon
                 << " exp:" << tms->tm_mon << endl;
            res = 1;
        }
        if (tms->tm_year + 1900 != tztm.tm_year) {
            cerr << "t=" << t << " tm_year=" << tztm.tm_year
                 << " exp:" << tms->tm_year + 1900 << endl;
            res = 1;
        }
        if (tms->tm_wday != tztm.tm_wday) {
            cerr << "t=" << t << " tm_wday=" << (int) tztm.tm_wday
                 << " exp:" << tms->tm_wday << endl;
            res = 1;
        }
        if (tms->tm_yday != tztm.tm_yday) {
            cerr << "t=" << t << " tm_yday=" << tztm.tm_yday
                 << " exp:" << tms->tm_yday << endl;
            res = 1;
        }
        if (tms->tm_isdst != tztm.tm_isdst) {
            cerr << "t=" << t << " tm_isdst=" << (int) tztm.tm_isdst
                 << " exp:" << tms->tm_isdst << endl;
            res = 1;
        }
#ifndef _WINDOWS_
        if ((int64)tms->tm_gmtoff*1000000 != tztm.tm_gmtoff) {
            cerr << "t=" << t << " tm_gmtoff=" << tztm.tm_gmtoff
                 << " exp:" << tms->tm_gmtoff << endl;
           res = 1;
        }
#endif
    }
    return res;
}

int range_test(TimeZone& tz)
{
    int res = 0;
    TimeZone::tm tztm;
    etime_t t9002 = (int64)(9002-1970)/4*(365*4+1)*24*3600*1000000;
    tz.localtime(t9002, &tztm);
    if (tztm.tm_year != 9002) {
        cerr << "localtime range test failed" << endl;
        res = 1;
    }
    if (t9002 != tz.mktime(&tztm)) {
        cerr << "mktime range test failed" << endl;
        res = 1;
    }
    etime_t tm6003 = (int64)(-6002-1970)/4*(365*4+1)*24*3600*1000000;
    tz.localtime(tm6003, &tztm);
    if (tztm.tm_year != -6003) {
        cerr << "localtime neg. range test failed" << endl;
        res = 1;
    }
    if (tm6003 != tz.mktime(&tztm)) {
        cerr << "mktime neg. range test failed" << endl;
        res = 1;
    }
    /*
    cerr << "y=" << tztm.tm_year
         << " m=" << tztm.tm_mon
         << " d=" << tztm.tm_mday
         << " H=" << tztm.tm_hour
         << " M=" << tztm.tm_min
         << " S=" << tztm.tm_sec << endl;*/
    return res;
}

int
alldays_test(TimeZone& tz)
{
    TimeZone::tm tztm, tztm2;
    tm* tms;
    long i,is,itz;
    etime_t et;
    int res = 0;
    time_t  t;
#ifdef _WINDOWS_
    int start=0;
#else
    int start=0x80000000;
#endif

    for (i = start; i < 0x7FFFF000; i+=0x00000FFF) {
        is=i;
        itz=i;
        tms = localtime(&is);
        (void) tz.localtime(1000000LL*(int64)itz, &tztm);

#ifndef _WINDOWS_
        if (tms->tm_sec != tztm.tm_sec) {
            cerr << "t=" << i << " tm_sec=" << tztm.tm_sec
                 << " exp:" << tms->tm_sec << endl;
            res = 1;
        }
        if (tms->tm_min != tztm.tm_min) {
            cerr << "t=" << i << " tm_min=" << tztm.tm_min
                 << " exp:" << tms->tm_min << endl;
            res = 1;
        }
        if (tms->tm_hour != tztm.tm_hour) {
            cerr << "t=" << i << " tm_hour=" << tztm.tm_hour
                 << " exp:" << tms->tm_hour << endl;
            res = 1;
        }
        if (tms->tm_mday != tztm.tm_mday) {
            cerr << "t=" << i << " tm_mday=" << tztm.tm_mday
                 << " exp:" << tms->tm_mday << endl;
            res = 1;
        }
        if (tms->tm_mon != tztm.tm_mon) {
            cerr << "t=" << i << " tm_mon=" << (int) tztm.tm_mon
                 << " exp:" << tms->tm_mon << endl;
            res = 1;
        }
        if (tms->tm_year + 1900 != tztm.tm_year) {
            cerr << "t=" << i << " tm_year=" << tztm.tm_year
                 << " exp:" << tms->tm_year + 1900 << endl;
            res = 1;
        }
        if (tms->tm_wday != tztm.tm_wday) {
            cerr << "t=" << i << " tm_wday=" << (int) tztm.tm_wday
                 << " exp:" << tms->tm_wday << endl;
            res = 1;
        }
        if (tms->tm_yday != tztm.tm_yday) {
            cerr << "t=" << i << " tm_yday=" << tztm.tm_yday
                 << " exp:" << tms->tm_yday << endl;
            res = 1;
        }
        if (tms->tm_isdst != tztm.tm_isdst) {
            cerr << "t=" << i << " tm_isdst=" << (int) tztm.tm_isdst
                 << " exp:" << tms->tm_isdst << endl;
            res = 1;
        }

        if ((int64)tms->tm_gmtoff*1000000 != tztm.tm_gmtoff) {
            cerr << "t=" << i << " tm_gmtoff=" << tztm.tm_gmtoff
                 << " exp:" << tms->tm_gmtoff << endl;
           res = 1;
        }
#endif
        tztm2=tztm;
        et = tz.mktime(&tztm2);
//        t  = mktime(tms);
        if (et!=1000000LL*(int64)itz) {
                 cerr << "y=" << tztm.tm_year
                      << " m=" << (int) tztm.tm_mon
                      << " d=" << (int) tztm.tm_mday
                      << " H=" << (int) tztm.tm_hour
                      << " M=" << (int) tztm.tm_min
                      << " S=" << (int) tztm.tm_sec
                      << " DST="<< (int) tztm.tm_isdst
                      << " exp: " << 1000000LL*(int64)i
                      << endl;
            res = 1;

/*        if (t!=is) {
                 cerr << "OS shit time_t="<<i<<" forth and back gives "<<t<<endl;
                 cerr << "  y=" << tms->tm_year
                         << " m=" << (int) tms->tm_mon
                 << " d=" << tms->tm_mday
                 << " H=" << tms->tm_hour
                 << " M=" << tms->tm_min
                 << " S=" << tms->tm_sec
                 << " DST="<<tms->tm_isdst
                 << endl;
        }
*/        }
        et = 0;
        t = 0;
        tms->tm_isdst =0;

        tztm.tm_isdst = TimeZone::OFF;
    }
    return res;
}

int
main(int argc, char* argv[])
{
    if (argc < 2)
        return -1;

    int res = 0;

    for (int i = 1; i < argc; i++) {
#ifndef _WINDOWS_
        setenv("TZ", argv[i], 1);
#else
/*
   if( _putenv( "TZ="+argv[i] ) == -1 )
   {
      cout<<"Unable to set TZ\"<<endl;
      continue;
   }
   else
   {
      _tzset();
   }
*/
#endif
        TimeZone tz(argv[i]);
        if (tz.isStatusOk()) {
            cerr << "checking " << argv[i] << "..." << endl;
            if (alldays_test(tz)) {
                cerr << "***alldays for " << argv[i] << " failed" << endl;
                res = 1;
            }
/*            if (localtime_test(tz)) {
                cerr << "***localtime for " << argv[i] << " failed" << endl;
                res = 1;
            }
            if (mktime_test(tz)) {
                cerr << "***mktime for " << argv[i] << " failed" << endl;
                res = 1;
            }
*/
            if (range_test(tz)) {
                cerr << "***range_test for " << argv[i] << " failed" << endl;
                res = 1;
            }
        } else {
            res = 1;
            cerr << "Initialization failed for zone " << argv[i] << endl;
        }
    }
    return res;
}

//
// $Log: tz_test.cxx,v $
// Revision 1.3  2004/11/15 14:40:06  alex
// removed $Id$ keyword
//
// Revision 1.2  2004/08/10 15:15:58  apg
// removed exigen copyright
//
// Revision 1.1.1.1  2002/10/03 15:17:05  alex
// home, sweet home
//
// Revision 1.3  2002/02/23 11:54:11  ego
// Global changes (part I) applied: EOL_NS, common_defs.h
//
// Revision 1.2  2001/11/02 15:58:43  alex
// *** empty log message ***
//
// Revision 1.1  2001/10/25 13:56:32  alex
// standardized tests
//
// Revision 1.2  2001/09/25 13:16:31  apg
// *** empty log message ***
//
// Revision 1.1  2001/09/18 09:34:25  apg
// *** empty log message ***
//
// Revision 1.3  2001/07/31 14:07:36  zvonov
// minor update
//
// Revision 1.2  2001/07/17 03:00:45  avg
// Eliminated compiler warnings
//
// Revision 1.1  2001/07/03 00:26:38  alex
// cpp suffix
//
// Revision 1.5  2001/06/16 06:45:50  avg
// Removed CR-LFs.
//
// Revision 1.4  2001/05/28 09:00:05  zvonov
// Ported to Windows, tested, bugs removed
//
// Revision 1.2  2001/01/07 02:20:06  avg
//
//
// Major clean-up effort (coding standards compliance for Fixed and Qint,
// file names) and replacement of NaNs with exceptions. Also, numerous
// buxfixes.
//
//
