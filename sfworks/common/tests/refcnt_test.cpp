// Copyright (c) 2001 Syntext, Inc. All Rights Reserved.
//
/*! \file
 *  This is a test for RefCntPtr.
 */
#include "common/RefCntPtr.h"
#include "common/RefCounted.h"
#include "common/asserts.h"
#include "stdlib.h"

USING_COMMON_NS

struct ObjCounter {
    static int obj_count;
    ObjCounter() { ++obj_count; }
    ~ObjCounter() { --obj_count; }
};
int ObjCounter::obj_count = 0;

class A : public ObjCounter, public RefCounted<> {};
class B : public A {
public:
    RefCntPtr<B> p;
};

#define CHECK(n) RT_ASSERT(ObjCounter::obj_count == (n))

void do_test()
{
    {
        RefCntPtr<A> ap = new A;
        RefCntPtr<A> ap1;
        ap1 = ap;
        CHECK(1);
        ap1 = 0;
        ap = 0;
        CHECK(0);
    }
    CHECK(0);
    {
        RefCntPtr<B> bp = new B;
        bp->p = new B;
        CHECK(2);
        bp->p->p = bp;
        CHECK(2);
        bp = bp->p->p;
        CHECK(2);
        bp->p = 0;
        CHECK(1);
    }
    CHECK(0);
    {
        RefCntPtr<A> ap = new A;
        RefCntPtr<B> bp = new B;
        ap = bp;
        RT_ASSERT(ap == bp);
        CHECK(1);
        RT_ASSERT(ap != 0);
        RT_ASSERT(0 != ap);
        RT_ASSERT(ap);
        bp = 0;
    }
    CHECK(0);
    {
        RefCntPtr<A> ap1 = new A;
        RefCntPtr<A> ap2 = new A;
        const A* sap1 = ap1.pointer();
        const A* sap2 = ap2.pointer();
        (void) sap1;
        (void) sap2;
        ap1.swap(ap2);
        RT_ASSERT(sap1 == ap2);
        RT_ASSERT(ap1 == sap2);
        CHECK(2);
        ap1 = 0;
    }
    CHECK(0);
    {
        CastedRefCntPtr<A, B> bp;
        bp = new B;
        RefCntPtr<A> ap;
        ap = bp;
        CHECK(1);
        B* bpp = bp.pointer();
        (void) bpp;
        RT_ASSERT(bpp);
        RefCntPtr<B> rbp;
        rbp = bp;
        rbp = 0;
        bp = 0;
        ap = 0;
        CHECK(0);
    }
    {
        const B* bpp = new B;
        ConstRefCntPtr<B> bp;
        bp = bpp;
        RefCntPtr<B> b2 = new B;
        bp = b2;
        CHECK(1);
    }
    CHECK(0);
}

int main()
{
    do_test();
    CHECK(0);
    exit(0);
}

