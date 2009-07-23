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
    Qint quad-integer arithmetic package test driver
 */
#include "common/common_defs.h"
#include "common/QuadInt.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fstream>
#include <time.h>


USING_COMMON_NAMESPACE
using namespace std;

int
main(int ac, char **av)
{
    char cmd[256];
    QuadInt x, y, r;
    char result[256];
    enum {
        RSH = 0200,
        LSH, EQ, NE, GE, LE,
        NEG, NOP, INV,
        CVT, SETNAN
    };
    int op, iarg, base;
    char *p;
    int errcnt = 0;
    istream *in = &cin;
    ostream *record = 0;
    int manual = 1;

    //
    // If argument is given, treat it as a file name
    //
    if (ac > 1) {
        in = new ifstream(av[1]);

        if (!in) {
            cerr << "Cannot open test data file " << av[1] << " for reading\n";
            exit(100);
        }
        manual = 0;
    }
    else {
        cout << "? ";
        record = new ofstream("qint.tmp", ofstream::app);
        if (!record)
            cerr << "Warning: cannot open recorder file qint.tmp\n";
        else {
            time_t t = time(0);
            *record << "# MANUAL TEST DATA INPUT: " << ctime(&t);
        }
    }

    while( in->getline(cmd, sizeof cmd) ) {
        // Isolate the command
        p = cmd;
        while (!isspace(*p) && *p)
             p++;

        try {
            //
            // Parse the command and operands
            //
            switch (cmd[0]) {
            case 0:
            case '#':       // Comment...
                if (manual)
                    cout << "? ";
                continue;

            case 'q':
                exit(0);

            default:
            error:
                cerr << "*** Unknown test command '" << cmd << "'\n";
                errcnt++;
                if (manual)
                    cout << "? ";
                continue;

            case '>':
                switch (cmd[1]) {
                default:
                    goto error;

                case '>':
                    op = RSH;
                    p = &p[x.fromString(p)];
                    iarg = strtol(p, &p, 10);
                    break;

                case '=':
                    op = GE;
                    goto binary;

                case ' ':
                    op = cmd[0];
                    goto binary;
                }
                goto binary;

            case '<':
                switch (cmd[1]) {
                default:
                    goto error;

                case '<':
                    op = LSH;
                    p = &p[x.fromString(p)];
                    iarg = strtol(p, &p, 10);
                    break;

                case '=':
                    op = LE;
                    goto binary;

                case ' ':
                    op = cmd[0];
                    goto binary;
                }
                break;

            case '=':
                if (cmd[1] == '=') {
                    op = EQ;
                    goto binary;
                }
                goto error;

            case '!':
                if (cmd[1] == '=') {
                    op = NE;
                    goto binary;
                }
                goto error;

            case '+':
            case '-':
            case '*':
            case '/':
            case '%':
            case '&':
            case '|':
            case '^':
                op = cmd[0];
                if (cmd[1] != ' ')
                    goto error;

            binary:
                p = &p[x.fromString(p)];
                p = &p[y.fromString(p)];
                break;

            case 'u':
                switch (cmd[1]) {
                default:
                    goto error;

                case '+': op = NOP; break;
                case '-': op = NEG; break;
                case '~': op = INV; break;
                }
                p = &p[x.fromString(p)];
                break;

            case 'c':
                p = &p[x.fromString(p)];
                iarg = strtol(p, &p, 0);        // base
                op = CVT;
                break;

            case 'n':
                op = SETNAN;
                break;
            }

            base = 10;
            switch( op ) {
            case '+':   r = x + y; break;
            case '-':   r = x - y; break;
            case '*':   r = x * y; break;
            case '/':   r = x / y; break;
            case '%':   r = x % y; break;
            case '&':   r = x & y; break;
            case '|':   r = x | y; break;
            case '^':   r = x ^ y; break;
            case RSH:   r = x >> iarg; break;
            case LSH:   r = x << iarg; break;
            case '>':   r = (x > y); break;
            case '<':   r = (x < y); break;
            case EQ:    r = (x == y); break;
            case NE:    r = (x != y); break;
            case GE:    r = (x >= y); break;
            case LE:    r = (x <= y); break;
            case NEG:   r = -x; break;
            case NOP:   r = +x; break;
            case INV:   r = ~x; break;
            case CVT:   r = x; base = iarg; break;
            case SETNAN: r.setNaN(); break;
            }
            r.toString(result, sizeof result, base);
        }
        catch (NumericError &err) {
            static const char* EIDS[] = {
                    "NAN", "OVERFLOW", "UNDERFLOW", "ZERODIV",
                    "BIGNUM", "BADNUM", "CONVARG",
            };

            strcpy(result, EIDS[err.getErrorType()]);
        }

        if (record) {
            *record << cmd << " : " << result << "\n";
            record->flush();
        }

        if (manual) {
            cout << "result: '" << result << "'\n";
            cout << "? ";
        }
        else {
            // End of string is the expected result
            p = strchr(cmd, ':');
            if (!p) {
                cerr << "*** Missing expected value: '" << cmd << "'\n";
                errcnt++;
            }
            else {
                while (isspace(*++p))
                    ;

                if (strcmp(result, p)) {
                    cerr << "MISMATCH: " << cmd << "\n";
                    cerr << "      result: '" << result << "'\n";
                    cerr << "    expected: '" << p << "'\n";
                    errcnt++;
                }
            }
        }
    }
    if (!manual && errcnt == 0)
        cerr << "QuadInt test passed.\n";
    exit(errcnt != 0);
}
