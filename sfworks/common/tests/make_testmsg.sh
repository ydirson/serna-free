#!/bin/sh
MSGGEN=$MBASE/tools/msggen/msggen.py
$MSGGEN -H TestMessages.h   TestMessages.msg
$MSGGEN -C TestMessages.cpp TestMessages.msg
c++ -I../include -shared -s -o TestMessages.so \
    TestMessages.cpp ../build.freebsd-MTd/libcommon.a
