// Copyright (c) 2001 Syntext, Inc. All Rights Reserved.
//
/*! \file
    The test driver for RadixTree
 */
#define RADIX_TREE_DEBUG
#include "common/RadixTree.h"
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

CVSID(RADIX_TEST_CPP, "");

USING_COMMON_NS
using namespace std;

#ifdef _WIN32
# define SNPRINT _snprintf
# include <streambuf>
#else
# define SNPRINT snprintf
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
            SNPRINT(resbuf, sizeof resbuf, "%lx", val);
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
            SNPRINT(resbuf, sizeof resbuf, "%ld", t->maxElements());
            result = resbuf;
            break;

        case 'c':
            SNPRINT(resbuf, sizeof resbuf, "%ld", t->countNodes());
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
            SNPRINT(resbuf, sizeof resbuf, "<cur=%lx index=%ld>", int32(it.get()), it.getIndex());
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
    return 0;
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
    for (int32 i = 0; i < EPL; i++) {
        if ((i % 8) == 0) {
            if (i == 0)
                cout <<"\n<" << bits << "2d> " << int32(this) << "08x:" << endl;
            else
                cout << "\n              ";
        }
        cout <<" " << int32(p[i]) << "8x" << endl;
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

