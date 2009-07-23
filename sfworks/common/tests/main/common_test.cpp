// Copyright (c) 2001 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 *
 */

#include "common_test.h"

namespace COMMON_TEST_NS {

using namespace TEST_NS;

CommonTest::CommonTest(const TestContext& ctx) : TestFixture(ctx), ctx_(ctx)
{
}

const TestContext&
CommonTest::getCtx() const
{
    return ctx_;
}

} // end of namespace COMMON_TEST_NS

