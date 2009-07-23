#!/bin/sh

. ./test_common.sh
branch="common/"

SWAIT=wait
simplerun fixed_test 0 ${TEST_DATA_DIR}/fixed_test.data
simplerun fixed_test2 0
simplerun qint_test 0 ${TEST_DATA_DIR}/qint_test.data
simplerun radix_test 0 ${TEST_DATA_DIR}/radix_test.data
simplerun refcnt_test 0
simplerun trace_test 0 ${TEST_DATA_DIR}/trace_test.data
simplerun tz_test 1 "`(cd ${ZONEINFO}; ls -Fd [A-Z]* | grep -v /)`"
simplerun tz_test 2 "`(cd ${ZONEINFO}; ls -Fd [A-Z]* | grep -v / ; echo [A-Z]*/[A-Z]* )`"
