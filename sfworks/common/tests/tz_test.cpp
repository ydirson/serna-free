// Copyright (c) 2001 Syntext, Inc. All Rights Reserved.
//
/*! \file
    Test of TimeZone
 */
#include "common/common_defs.h"
#include "common/TimeZone.h"
#include <stdlib.h>
#include <iostream>

CVSID(TZ_TEST_CPP, "");

USING_COMMON_NS
using namespace std;

#ifdef _WIN32
# ifndef USE_DINKUMWARE
  inline std::ostream&
  operator<<(std::ostream& out, int64 x)
  {
      char buf[33];
      _i64toa(x,buf,10);
      return out << buf;
  }
# endif
#endif

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
        if (t != -1 && INT64_VALUE(1000000)*(int64)t+mksec%1000000 != et) {
            cerr << "y=" << tztm.tm_year
                 << " m=" << (int) tztm.tm_mon
                 << " d=" << tztm.tm_mday
                 << " H=" << tztm.tm_hour
                 << " M=" << tztm.tm_min
                 << " S=" << tztm.tm_sec
                 << " t=" << et
                 << " exp: " << (int64)(INT64_VALUE(1000000)*t+mksec%1000000)
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
#ifndef _WIN32
        if (j)
            t = -t;
#endif
        tms = localtime(&t);
        (void) tz.localtime(INT64_VALUE(1000000)*t, &tztm);
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
#if !defined(_WIN32) && !defined(sun)
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
#ifdef _WIN32
    int start=0;
#else
    int start=0x80000000;
#endif

    for (i = start; i < 0x7FFFF000; i+=0x00000FFF) {
        is=i;
        itz=i;
        tms = localtime(&is);
        (void) tz.localtime(INT64_VALUE(1000000)*(int64)itz, &tztm);

#ifndef _WIN32
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

# if !defined(sun)
        if ((int64)tms->tm_gmtoff*1000000 != tztm.tm_gmtoff) {
            cerr << "t=" << i << " tm_gmtoff=" << tztm.tm_gmtoff
                 << " exp:" << tms->tm_gmtoff << endl;
           res = 1;
        }
# endif
#endif
        tztm2=tztm;
        et = tz.mktime(&tztm2);
//        t  = mktime(tms);
        if (et!=INT64_VALUE(1000000)*(int64)itz) {
                 cerr << "y=" << tztm.tm_year
                      << " m=" << (int) tztm.tm_mon
                      << " d=" << (int) tztm.tm_mday
                      << " H=" << (int) tztm.tm_hour
                      << " M=" << (int) tztm.tm_min
                      << " S=" << (int) tztm.tm_sec
                      << " DST="<< (int) tztm.tm_isdst
                      << " exp: " << INT64_VALUE(1000000)*(int64)i
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
#ifndef _WIN32
# if !defined(sun)
        setenv("TZ", argv[i], 1);
# else
        static char tz_var[256] = "TZ=";
        int nsyms = strlen(strncpy(tz_var + 3, argv[i], 255 - 3)) - 3;
        tz_var[nsyms] = '\0';
        putenv(tz_var);
# endif
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
// $Log: tz_test.cpp,v $
// Revision 1.3  2004/11/15 14:40:06  alex
// removed $Id$ keyword
//
// Revision 1.2  2004/08/10 15:15:58  apg
// removed exigen copyright
//
// Revision 1.1.1.1  2002/10/03 15:17:05  alex
// home, sweet home
//
// Revision 1.9  2002/02/23 11:54:11  ego
// Global changes (part I) applied: EOL_NS, common_defs.h
//
// Revision 1.8  2002/02/20 15:26:40  alexb
// solaris related changes
//
// Revision 1.7  2002/02/11 14:12:48  ego
// !!!!! global changes: sed -e 's/NAMESPACE/NS/g'
//
// Revision 1.6  2002/01/31 15:50:42  ego
// Dinkumware adoptation.
// To turn on using Dinkumware under MSVC switch
// enable_dinkumware variable to 1.
//
// Revision 1.5  2002/01/18 10:10:12  ego
// fix under unix.
//
// Revision 1.4  2002/01/18 10:07:00  ego
// Now writes log to regression.log.
//
// Revision 1.1  2002/01/17 14:39:02  ego
// skip two tests.
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
