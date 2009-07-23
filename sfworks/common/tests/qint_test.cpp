// Copyright (c) 2001 Syntext, Inc. All Rights Reserved.
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

CVSID(QINT_TEST_CPP, "");

USING_COMMON_NS
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
    int op = 0;
    int iarg = 0;
    int base = 0;
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
    return 0;
}
