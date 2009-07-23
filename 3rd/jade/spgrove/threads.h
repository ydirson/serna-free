// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef threads_INCLUDED
#define threads_INCLUDED 1

// Thread primitives.

#if defined(_MSC_VER) && defined(_MT)

// WIN32

#define SP_THREAD

#define STRICT 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
// <windows.h> appears to turn these warnings back on
#ifdef _MSC_VER
#pragma warning ( disable : 4237 )
#endif

class Thread {
public:
  Thread(int (*func)(void *), void *arg) {
    unsigned threadId;
    arg_ = new Arg;
    arg_->func = func;
    arg_->arg = arg;
    arg_->exited = 0;
    handle_ = HANDLE(_beginthreadex(NULL, 0, start, arg_, 0, &threadId));
  }
  int wait() {
    if (WaitForSingleObject(handle_, INFINITE) != WAIT_OBJECT_0)
      abort();
    if (!CloseHandle(handle_))
      abort();
    if (arg_->exited) {
      int ret = arg_->ret;
      delete arg_;
      return ret;
    }
    // Something's gone wrong.  We'll get a leak.
    return 0;
  }
private:
  struct Arg {
    int (*func)(void *);
    void *arg;
    int ret;
    int exited;
  };
  static unsigned __stdcall start(void *p) {
    Arg *arg = (Arg *)p;
    arg->ret = (*arg->func)(arg->arg);
    arg->exited = 1;
    return 0;
  }
  HANDLE handle_;
  Arg *arg_;
};

class Mutex {
public:
  class Lock {
  public:
    Lock(Mutex *mp) : mp_(mp) {
      if (mp) EnterCriticalSection(&mp->cs_);
    }
    ~Lock() {
      if (mp_) LeaveCriticalSection(&mp_->cs_);
    }
  private:
    Mutex *mp_;
  };
  Mutex() {
    InitializeCriticalSection(&cs_);
  }
  ~Mutex() {
    DeleteCriticalSection(&cs_);
  }
  friend class Lock;
private:
  CRITICAL_SECTION cs_;
};

// Thread-safe reference counts.

class RefCount {
public:
  RefCount() : count_(0) { }
  void operator++() { InterlockedIncrement(&count_); }
  bool operator--() { return InterlockedDecrement(&count_) != 0; }
  operator unsigned long() const { return count_; }
private:
  long count_;
};

#ifndef SP_NO_BLOCK

#define SP_CONDITION

class Condition {
public:
  // Manual reset event, initially non-signalled
  Condition() : handle_(CreateEvent(NULL, 1, 0, NULL)), nWaiters_(0) { }
  ~Condition() { CloseHandle(handle_); }
  // Set the event to signalled, releasing all threads waiting on it,
  // then set it back to non-signalled.
  // For performance reasons, we want to avoid calling PulseEvent unless we have
  // some thread waiting on this condition.
  // The obvious solution of a boolean variable that's set by wait() and
  // cleared by pulse() fails in the following scenario:
  // reader: needPulse_ = 1;
  // writer: needPulse_ = 0;
  // writer: PulseEvent()
  // reader: WaitForSingleObject()
  void pulse() {
    if (nWaiters_) {
      PulseEvent(handle_);
    }
  }
  // Wait for the event to become signalled.
  bool wait() const {
    InterlockedIncrement((long *)&nWaiters_);
    bool ret = (WaitForSingleObject(handle_, INFINITE) == WAIT_OBJECT_0);
    InterlockedDecrement((long *)&nWaiters_);
    return ret;
  }
  // Set the event to signalled, releasing all threads waiting on it.
  // Don't set it back to non-signalled.  Any thread that subsequently
  // waits will return immediately.
  void set() { SetEvent(handle_); }
private:
  long nWaiters_;
  HANDLE handle_;
};

#endif /* not SP_NO_BLOCK */

#endif /* _MSC_VER && _MT */

#ifdef __MACH__

// For Mach, using C Threads. May or may not work as-is on your Mach-based OS.
// Written by Raf Schietekat <RfSchtkt@maze.ruca.ua.ac.be> on 1996-11-10.
// Platform used: NEXTSTEP_(Dev_)3.2 with sp-1.1.1 and jade-0.1.
// Maturity: only dsssl/demo.sgm tested (successfully).

#define SP_THREAD

extern "C" {
#ifdef __NeXT__
// catch is a reserved word in C++, it's only used to declare a struct member
#define catch catch22
#include <mach/cthreads.h>
#undef catch
// don't want ASSERT to clash with ASSERT in SP's macros.h
#undef ASSERT
#else
// Change to whatever works for you, and tell James Clark (or me).
// catch is a reserved word in C++, it's only used to declare a struct member
#define catch catch22
#include <mach/cthreads.h>
#undef catch
// don't want ASSERT to clash with ASSERT in SP's macros.h
#undef ASSERT
#endif
}

class Thread {
public:
  Thread(int (*func)(void *), void *arg) {
    arg_ = new Arg;
    arg_->func = func;
    arg_->arg = arg;
    arg_->exited = 0;
    handle_ = cthread_fork((cthread_fn_t)start, (any_t)arg_);
    // will be cthread_join()'ed in wait()
  }
  int wait() {
    cthread_join(handle_);
    if (arg_ && arg_->exited) {
      int ret = arg_->ret;
      delete arg_;
      arg_ = 0;
      return ret;
    }
    // Something's gone wrong.  We'll get a leak.
    return 0;
  }
private:
  struct Arg {
    int (*func)(void *);
    void *arg;
    int ret;
    int exited;
  };
  static void *start(void *p) {
    Arg *arg = (Arg *)p;
    arg->ret = (*arg->func)(arg->arg);
    arg->exited = 1;
    return 0;
  }
  cthread_t handle_;
  Arg *arg_;
};

class Mutex {
public:
  class Lock {
  // Lock serves to automatically unlock Mutex, however control leaves
  // a block. Don't let any "warning: unused variable `class Mutex::Lock lock'"
  // mislead you; hopefully your compiler won't optimise this away...
  public:
    Lock(Mutex *mp) : mp_(mp) { if (mp_) mutex_lock  (&mp_->cs_); }
    ~Lock()                   { if (mp_) mutex_unlock(&mp_->cs_); }
  private:
    Mutex *mp_;
  };
  Mutex()  { mutex_init (&cs_); }
  ~Mutex() { mutex_clear(&cs_); }
  friend class Lock;
private:
  struct mutex cs_;
};

// Thread-safe reference counts.

class RefCount {
public:
  RefCount() : count_(0) { mutex_init (&cs_); }
  ~RefCount()            { mutex_clear(&cs_); }
  void operator++() { mutex_lock(&cs_); ++count_; mutex_unlock(&cs_); }
  bool operator--() {
    bool ret;
    mutex_lock(&cs_); ret=(0!=--count_); mutex_unlock(&cs_);
    return ret; // safe iff reference-count semantics are observed
  }
  operator unsigned long() const { return count_; }
    // safe iff reference-count semantics are observed
private:
  long count_; // hmm, () returns unsigned long...
  struct mutex cs_; // don't build on top of Mutex class, for efficiency
};

#ifndef SP_NO_BLOCK

#define SP_CONDITION

class Condition { // doesn't trivially map to a condition_t!
public:
  Condition() : set_(0) {
    /**/mutex_=    mutex_alloc();
    condition_=condition_alloc();
  }
  ~Condition() {
    /**/mutex_free(    mutex_);
    condition_free(condition_);
  }
  void pulse() { condition_broadcast(condition_); }
  bool wait() const {
    mutex_lock  (                        mutex_);
    if(!set_) condition_wait(condition_, mutex_);
    mutex_unlock(                        mutex_);
    return 1;
      // 0 would be interpreted as timeout, which is not supported directly
      // do we need that? would require some more coding, with another thread
  }
  void set() {
    mutex_lock  (mutex_);
    set_=1;
    mutex_unlock(mutex_);
    condition_broadcast(condition_);
  }
private:
  // nWaiters_ functionality already in Mach C Threads
  mutex_t mutex_;
    // every C Threads condition_t needs a mutex_t
    // but also see set_ below
  condition_t condition_;
    // can't have struct and struct mutex because of the const in wait()
    // (can/should this const-ness be revised?)
  bool set_;
    // born as false, does a single transition to true, then dies
    // therefore, no mutex (with its overhead) seems required, right?
    // wrong:
    //   consumer reads set_ as false
    //   producer sets set_ to true
    //   producer signals condition for the last time
    //   consumer waits, and waits forever
    //     (consumer didn't get last condition because it was not yet waiting)
};

#endif /* SP_NO_BLOCK */

#endif /* __MACH__ */

#ifdef SP_USE_PTHREADS

// Support for pthreads on Linux.
// Written by Matthias Clasen <clasen@mathematik.uni-freiburg.de>

extern "C" {
// for some reason sigset_t is missing here (with glibc 2.0.7);
// so we define it manually.
typedef __sigset_t sigset_t;
#include <pthread.h>
}

#define SP_THREAD

class Thread {
public:
  Thread(int (*func)(void *), void *arg) {
    arg_ = new Arg;
    arg_->func = func;
    arg_->arg = arg;
    arg_->exited = 0;
    pthread_create (&handle_, NULL, start, (void *)arg_);
  }
  int wait() {
    pthread_join(handle_, NULL);
    if (arg_ && arg_->exited) {
      int ret = arg_->ret;
      delete arg_;
      arg_ = 0;
      return ret;
    }
    // Something's gone wrong.  We'll get a leak.
    return 0;
  }
private:
  struct Arg {
    int (*func)(void *);
    void *arg;
    int ret;
    int exited;
  };
  static void *start(void *p) {
    Arg *arg = (Arg *)p;
    arg->ret = (*arg->func)(arg->arg);
    arg->exited = 1;
    return 0;
  }
  pthread_t handle_;
  Arg *arg_;
};

class Mutex {
public:
  class Lock {
  // Lock serves to automatically unlock Mutex, however control leaves
  // a block. Don't let any "warning: unused variable `class Mutex::Lock lock'"
  // mislead you; hopefully your compiler won't optimise this away...
  public:
    Lock(Mutex *mp) : mp_(mp) { if (mp_) pthread_mutex_lock  (&mp_->cs_); }
    ~Lock()                   { if (mp_) pthread_mutex_unlock(&mp_->cs_); }
  private:
    Mutex *mp_;
  };
  Mutex()  { pthread_mutex_init (&cs_, NULL); }
  ~Mutex() { pthread_mutex_destroy (&cs_); }
  friend class Lock;
private:
  pthread_mutex_t cs_;
};

// Thread-safe reference counts.

class RefCount {
public:
  RefCount() : count_(0) { pthread_mutex_init (&cs_, NULL); }
  ~RefCount()            { pthread_mutex_destroy (&cs_); }
  void operator++() { pthread_mutex_lock(&cs_); ++count_; pthread_mutex_unlock(&cs_); }
  bool operator--() {
    bool ret;
    pthread_mutex_lock(&cs_); ret=(0!=--count_); pthread_mutex_unlock(&cs_);
    return ret; // safe iff reference-count semantics are observed
  }
  operator unsigned long() const { return count_; }
    // safe iff reference-count semantics are observed
private:
  long count_; // hmm, () returns unsigned long...
  pthread_mutex_t cs_; // don't build on top of Mutex class, for efficiency
};

#ifndef SP_NO_BLOCK

#define SP_CONDITION

class Condition { // doesn't trivially map to a condition_t!
public:
  Condition() : set_(0) {
    pthread_mutex_init(&mutex_, NULL);
    pthread_cond_init(&condition_, NULL);
  }
  ~Condition() {
    pthread_mutex_destroy(&mutex_);
    pthread_cond_destroy(&condition_);
  }
  void pulse() { pthread_cond_broadcast(&condition_); }
  bool wait() {
    pthread_mutex_lock(&mutex_);
    if (!set_) pthread_cond_wait(&condition_, &mutex_);
    pthread_mutex_unlock(&mutex_);
    return 1;
      // 0 would be interpreted as timeout, which is not supported directly
      // do we need that? would require some more coding, with another thread
  }
  void set() {
    pthread_mutex_lock(&mutex_);
    set_=1;
    pthread_mutex_unlock(&mutex_);
    pthread_cond_broadcast(&condition_);
  }
private:
  pthread_mutex_t mutex_;
  pthread_cond_t condition_;
  bool set_;
};

#endif /* SP_NO_BLOCK */

#endif /* SP_USE_PTHREADS */

#ifndef SP_THREAD

class Thread {
public:
  Thread(int (*func)(void *), void *args) : ret_((*func)(args)) { }
  int wait() { return ret_; }
private:
  int ret_;
};

class RefCount {
public:
  RefCount() : count_(0) { }
  void operator++() { ++count_; }
  bool operator--() { return --count_ != 0; }
  operator unsigned long() const { return count_; }
private:
  unsigned long count_;
};

class Mutex {
public:
  class Lock {
  public:
    Lock(Mutex *) { }
  };
  Mutex() { }
};

#endif /* not SP_THREAD */

#ifndef SP_CONDITION

class Condition {
public:
  Condition() { }
  void pulse() { }
  void set() { }
  bool wait() const { return 0; }
};

#endif /* not SP_CONDITION */

#endif /* not threads_INCLUDED */
