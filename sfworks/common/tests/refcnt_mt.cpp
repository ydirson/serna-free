// Copyright (c) 2002 Syntext, Inc. All Rights Reserved.
//
/*! \file
    Quick and dirty mt test of atomic ops
 */
#include "common/common_defs.h"

CVSID(REFCNT_MT_CPP, "");

#include "common/common_types.h"
#include "common/ThreadMutex.h"

#ifdef _WIN32
# include <windows.h>
# include <process.h>
#else
# include <stdio.h>
# include <unistd.h>
# include <time.h>
# include <pthread.h>
#endif

#include <stdlib.h>
#include <iostream>

USING_COMMON_NS
using namespace std;

bool use_atomic = true;
ThreadMutex::VolatileIntType counter;
bool counter_dropped_below_0 = false;

inline int inc(ThreadMutex::VolatileIntType& i)
{
    return use_atomic ? ThreadMutex::atomicIncrement(i) : ++i;
}

inline int dec(ThreadMutex::VolatileIntType& i)
{
    return use_atomic ? ThreadMutex::atomicDecrement(i) : --i;
}

void
printerr(const char* msg)
{
    if (0 == msg)
        msg = "";
#ifdef _WIN32
    DWORD err = GetLastError();
    void* buf = 0;
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                  0, err, 0, (char*)&buf, 1024, 0);
    cerr << msg << (char*) buf << endl;
    if (0 != buf)
        LocalFree(buf);
#else
    perror(msg);
#endif
}

inline int nsleep(int s)
{
    int slp = (s * rand()) / RAND_MAX;
#ifdef _WIN32
    Sleep(slp/1000);
#else
    timespec slp_spec = {0, slp};
    nanosleep(&slp_spec, 0);
#endif
    return slp;
}

bool start = false;
int thread_count;

#ifdef _WIN32
HANDLE starter = CreateEvent(0, TRUE, FALSE, 0);
#else
pthread_mutex_t starter_mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  starter     = PTHREAD_COND_INITIALIZER;
#endif

unsigned int
#ifdef _MSC_VER
__stdcall
#endif
thr_func(void*)
{
#ifdef _WIN32
    WaitForSingleObject(starter, INFINITE);
    cerr << "start: " << GetCurrentThreadId() << endl;
#else
    pthread_mutex_lock(&starter_mtx);
    while (!start)
        pthread_cond_wait(&starter, &starter_mtx);
    cerr << "start: " << pthread_self() << endl;
    pthread_mutex_unlock(&starter_mtx);
#endif
    for (int i = 0; i < 1024; ++i) {
        inc(counter);
        nsleep(500);
        if (0 > dec(counter))
            counter_dropped_below_0 = true;
    }
#ifdef _WIN32
    PulseEvent(starter);
    --thread_count;
    cerr << "end: " << GetCurrentThreadId() << endl;
#else
    pthread_mutex_lock(&starter_mtx);
    cerr << "end: " << pthread_self() << endl;
    --thread_count;
    pthread_cond_signal(&starter);
    pthread_mutex_unlock(&starter_mtx);
#endif
    return 0;
}

class Thread {
public:
    Thread() : thr_id_(0) {}
    ~Thread() {}

    static bool wait(int nthreads, Thread* thrp)
    {
        bool ok = true;
#ifdef _WIN32
        ok = (WAIT_FAILED != WaitForMultipleObjects(nthreads, (HANDLE*)thrp, TRUE, INFINITE));
#else
        pthread_mutex_lock(&starter_mtx);
        while (0 < thread_count)
            pthread_cond_wait(&starter, &starter_mtx);
        pthread_mutex_unlock(&starter_mtx);
#endif
        return ok;
    }
    bool start()
    {
        bool ok = true;
#ifdef _MSC_VER
        thr_id_ = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)thr_func, 0, 0, 0);
        ok = (0 != thr_id_);
#else
        typedef void* (*pthread_func)(void*);
        ok = (0 == pthread_create(&thr_id_, 0, (pthread_func)&thr_func, 0));
#endif
        return ok;
    }
#ifdef _MSC_VER
    HANDLE thr_id_;
#else
    pthread_t thr_id_;
#endif
};

int main(int argc, char* argv[])
{
    if (argc > 1)
        use_atomic = false;
    srand(static_cast<uint32>(time(0)));

    const int maxthreads = 32;
    Thread threads[maxthreads];
    bool ok = true;

    for (int i = 0; i < maxthreads; ++i) {
        if (threads[i].start())
            ++thread_count;
        else
            ok = false;
    }
    if (!ok)
        printerr("error at start");

#ifdef _WIN32
    SetEvent(starter);
#else
    pthread_mutex_lock(&starter_mtx);
    start = true;
    pthread_cond_broadcast(&starter);
    pthread_mutex_unlock(&starter_mtx);
#endif

    if (!Thread::wait(maxthreads, threads)) {
        printerr("error at wait");
        ok = false;
    }

#ifdef _WIN32
    CloseHandle(starter);
#else
    pthread_cond_destroy(&starter);
    pthread_mutex_destroy(&starter_mtx);
#endif

    if (ok && 0 == counter && !counter_dropped_below_0)
        cerr << "PASSED" << endl;
    else {
        if (0 != counter)
            cerr << "invalid counter final value ";
        if (!ok)
            cerr << "start and/or wait failed ";
        if (counter_dropped_below_0)
            cerr << "at least once counter dropped below zero";
        cerr  << endl << "FAILED: " << counter << endl;
    }
    return counter;
}
