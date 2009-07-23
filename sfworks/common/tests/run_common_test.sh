#!/bin/sh

. ./test_common.sh
branch="common/"
SWAIT=wait
TEST_DATA_DIR=.

simplerun fixed_test 0 ${TEST_DATA_DIR}/fixed_test.data
simplerun fixed_test2 0
simplerun qint_test 0 ${TEST_DATA_DIR}/qint_test.data
simplerun radix_test 0 ${TEST_DATA_DIR}/radix_test.data
simplerun refcnt_test 0
## it is impossible to test it on NT because dll hjave all EOL tags registered and they are printed here ## simplerun trace_test 0 ${TEST_DATA_DIR}/trace_test.data
rm -f *.pid
