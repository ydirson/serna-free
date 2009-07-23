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
    The test driver for RadixTree
 */
#define RADIX_TREE_DEBUG
#include "common/RadixTree.h"
#include <iostream>
#include <fstream>
#if !defined(_MSC_VER) && !defined(__SUNPRO_CC)
#include <streambuf.h>
#endif
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>


USING_COMMON_NAMESPACE
using namespace std;

#ifdef _MSC_VER
# define SNPRINTF _snprintf
#else
# define SNPRINTF snprintf
#endif

int
main(int ac, char** av)
{
    char cmd[256];
    char resbuf[256];
    char *p;
    int errcnt = 0;
    istream *in = &cin;
    ostream *record = 0;
    int manual = 1;
    int32 idx, val, lineno;
    char* result;

    RadixTree<char>* t;
    RadixTree<char>::iterator it;

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
        record = new ofstream("radix.tmp", ofstream::app);
        if (!record)
            cerr << "Warning: cannot open recorder file radix.tmp\n";
        else {
            time_t t = time(0);
            *record << "# MANUAL TEST DATA INPUT: " << ctime(&t);
        }
    }

    lineno = 0;
    t = new RadixTree<char>;

    while (in->getline(cmd, sizeof cmd)) {
        lineno++;

        // Isolate the command
        p = cmd;
        while (!isspace(*p) && *p)
             p++;

        //
        // Parse the command and operands
        //
        switch (cmd[0]) {
        case 0:
        case '#':       // Comment...
        nextline:
            if (manual)
                cout << "? ";
            continue;

        case 'q':
            exit(0);

        case '?':
            cout << "q                 - quit\n"
                    "f <index>         - find value by index\n"
                    "s <index> <value> - set value with the index\n"
                    "r <index>         - remove value by index\n"
                    "m                 - print max number of elements\n"
                    "c                 - count the tree nodes\n"
                    "b                 - create iterator at the beginning\n"
                    "+                 - advance iterator\n"
                    "e                 - create iterator at the end\n"
                    "S <value>         - set value pointed to by iterator\n"
                    "R                 - remove value pointed to by iterator\n"
                    "N                 - destroy old and construct new tree\n"
                    "C                 - clean the radix tree\n"
                    "p                 - print the tree\n";
            goto nextline;

        default:
            cerr << "*** Unknown test command '" << cmd << "'\n";
            errcnt++;
            goto nextline;

        case 'p':
            t->printTree();
            goto nextline;

        case 'f':
            idx = strtol(p, 0, 10);
            val = int32(t->get(idx));
            SNPRINTF(resbuf, sizeof resbuf, "%lx", val);
            result = resbuf;
            break;

        case 's':
            idx = strtol(p, &p, 10);
            val = strtol(p, &p, 16);
            if (val == 0) {
                cerr << "*** Invalid zero leaf value\n";
                errcnt++;
                goto nextline;
            }
            t->set((char*) val, idx);
            result = 0;
            break;

        case 'r':
            idx = strtol(p, &p, 10);
            t->remove(idx);
            result = 0;
            break;

        case 'm':
            SNPRINTF(resbuf, sizeof resbuf, "%ld", t->maxElements());
            result = resbuf;
            break;

        case 'c':
            SNPRINTF(resbuf, sizeof resbuf, "%ld", t->countNodes());
            result = resbuf;
            break;

        case 'N':
            delete t;
            t = new RadixTree<char>;
            result = 0;
            break;

        case 'C':
            t->clean();
            result = 0;
            break;

        case 'b':
            it = t->begin();
            goto print_iterator;

        case 'e':
            it = t->end();
            goto print_iterator;

        case '+':
            ++it;
            goto print_iterator;

        case 'S':
            val = strtol(p, &p, 16);
            if (val == 0) {
                cerr << "*** Invalid zero leaf value\n";
                errcnt++;
                goto nextline;
            }
            it.set((char*) val);
            goto print_iterator;

        case 'R':
            it.remove();

        print_iterator:
            SNPRINTF(resbuf, sizeof resbuf, "<cur=%lx index=%ld>", int32(it.get()), it.getIndex());
            result = resbuf;
            break;
        }

        if (record) {
            *record << cmd;
            if (result)
                *record << " : " << result;
            *record << "\n";
            record->flush();
        }

        if (manual) {
            if (result)
                cout << "RESULT: " << result;
            cout << "\n? ";
        }
        else if (result) {
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
                    cerr << "MISMATCH (line " << lineno << "): " << cmd << "\n";
                    cerr << "      result: '" << result << "'\n";
                    cerr << "    expected: '" << p << "'\n";
                    errcnt++;
                }
            }
        }
    }

    if (!manual && errcnt == 0)
        cerr << "Radix Tree test passed.\n";
    exit(errcnt != 0);
}

//
// The radix tree debug printout
//
template<class T>
void
RadixTree<T>::printTree()
{
    cout << "====== TOTAL BITS=" << bits_;
    printTreeNode(bits_);
    cout << "\n";
}

namespace RadixTreePrivate {

//
// Print the tree node
//
void RTNode::printTreeNode(int32 bits)
{
        static char buf[128];
    for (int32 i = 0; i < EPL; i++) {
        if ((i % 8) == 0) {
            if (i == 0) {
                SNPRINTF(buf, 128, "\n<%2d> %08x:", bits, int32(this));
                cout << buf;
            }
            else
                cout << "\n              ";
        }
        SNPRINTF(buf, 128, " %8x", int32(p[i]));
        cout << buf;
    }
    bits -= BPL;
    if (bits > 0) {
        for (int32 i = 0; i < EPL; i++) {
            if (p[i])
                p[i]->printTreeNode(bits);
        }
    }
    cout.flush();
}

//
// Count subtree nodes
//
int32 RTNode::countTreeNodes(int32 bits)
{
    int32 cnt = 1;

    bits -= BPL;
    if (bits > 0) {
        for (int32 i = 0; i < EPL; i++) {
            if (p[i])
                cnt += p[i]->countTreeNodes(bits);
        }
    }
    return cnt;
}

} // namespace RadixTreePrivate
