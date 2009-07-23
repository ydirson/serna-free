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
    Singleton pattern implementation. Includes phoenix singletons and singletons with
    longevity. Code mostly derived from the Loki library, with hacked away template
    templates (because windows compilers don't support those).
 */
#ifndef SINGLETON_H_
#define SINGLETON_H_

#include "common/common_defs.h"
#include "common/ThreadingPolicies.h"


#include <algorithm>
#include <stdexcept>
#include <cstdlib>
#include <new>
#include <assert.h>

COMMON_NS_BEGIN

namespace SingletonPrivate
{
    /*! A helper class for setSingletonLongevity
     */
    class LifetimeTracker
    {
    public:
        LifetimeTracker(unsigned int x) : longevity_(x)
        {}

        virtual ~LifetimeTracker() = 0;

        static bool compare(const LifetimeTracker* lhs,
            const LifetimeTracker* rhs)
        {
            return rhs->longevity_ > lhs->longevity_;
        }

    private:
        unsigned int longevity_;
    };

    // Definition required
    inline LifetimeTracker::~LifetimeTracker() {}

    // Helper data
    typedef LifetimeTracker** TrackerArray;
    extern TrackerArray pTrackerArray;
    extern unsigned int elements;

    // Helper destroyer function
    template <typename T> struct Deleter
    {
        static void Delete(T* pObj) { delete pObj; }
    };

    // Concrete lifetime tracker for objects of type T
    template <typename T, typename Destroyer>
      class ConcreteLifetimeTracker : public LifetimeTracker {

    public:
        ConcreteLifetimeTracker(T* p, unsigned int longevity, Destroyer d)
            : LifetimeTracker(longevity), pTracked_(p), destroyer_(d) {}

        ~ConcreteLifetimeTracker() { destroyer_(pTracked_); }

    private:
        T* pTracked_;
        Destroyer destroyer_;
    };
    void atExitFn(); // declaration needed below
    extern "C" {
        typedef void (*atexit_fn_t)(void);
    }
    // kill warning on picky compilers (sun forte)
    inline void atExit(void (*pFun)()) {
        using namespace std;
        atexit(reinterpret_cast<atexit_fn_t>(pFun));
    }

} // namespace SingletonPrivate

/*! Assigns longevity to an object; ensures ordered destructions of thusly
    registered objects during the exit sequence of the application
 */
template <typename T, typename Destroyer>
  void setSingletonLongevity(T* pDynObject,
                             unsigned int longevity,
                             Destroyer d =
                                 SingletonPrivate::Deleter<T>::Delete)
{
    using namespace SingletonPrivate;
    using namespace std;

    TrackerArray pNewArray = static_cast<TrackerArray>
        (std::realloc(pTrackerArray, elements + 1));

    if (!pNewArray)
        throw std::bad_alloc();

    LifetimeTracker* p =
        new ConcreteLifetimeTracker<T, Destroyer>(pDynObject, longevity, d);

    // Delayed assignment for exception safety
    pTrackerArray = pNewArray;

    // Insert a pointer to the object into the queue
    TrackerArray pos =
        std::upper_bound(pTrackerArray, pTrackerArray + elements,
            p, LifetimeTracker::compare);

    std::copy_backward(pos, pTrackerArray + elements,
        pTrackerArray + elements + 1);

    *pos = p;
    ++elements;

    // Register a call to atExitFn
    SingletonPrivate::atExit(SingletonPrivate::atExitFn);
}

/*! Implementation of the CreationPolicy used by SingletonHolder:
    creates objects using a straight call to the new operator.
 */
template <class T> class CreateUsingNew {
public:
    static T* create() { return new T; }
    static void destroy(T* p) { delete p; }
};

/*! Implementation of the CreationPolicy used by SingletonHolder:
    creates objects using a call to std::malloc, followed by a call to the
    placement new operator.
 */
template <class T> class CreateUsingMalloc {
public:
    static T* create()
    {
        void* p = std::malloc(sizeof(T));
        if (!p)
          return 0;
        return new(p) T;
    }
    static void destroy(T* p)
    {
        p->~T();
        std::free(p);
    }
};

/*! Implementation of the CreationPolicy used by SingletonHolder:
    creates an object in static memory.
 */
template <class T> class CreateStatic {
public:
    // deal with alignment
    typedef union { char t_[sizeof(T)]; int foo; } Placeholder;

    static T* create()
    {
        static Placeholder staticMemory_;
        return new(&staticMemory_) T;
    }
    static void destroy(T* p)
    {
        p->~T();
    }
};

/*! Implementation of the LifetimePolicy used by SingletonHolder:
    schedules an object's destruction as per C++ rules
    (forwards to atexit).
 */
template <class T> class DefaultLifetime {
public:
    static void scheduleDestruction(T*, void (*pFun)()) {
        using namespace std;
        SingletonPrivate::atExit(pFun);
    }
    static void onDeadReference() {
        throw std::logic_error("Dead Reference Detected");
    }
};

/*! Implementation of the LifetimePolicy used by SingletonHolder:
    schedules an object's destruction as per C++ rules, and it allows
    magical object recreation by not throwing an exception from
    onDeadReference.
 */
template <class T> class PhoenixSingleton {
public:
    static void scheduleDestruction(T*, void (*pFun)())
    {
        using namespace std;
#ifndef ATEXIT_FIXED
        if (!destroyedOnce_)
#endif
            SingletonPrivate::atExit(pFun);
    }
    static void onDeadReference()
    {
#ifndef ATEXIT_FIXED
        destroyedOnce_ = true;
#endif
    }

private:
#ifndef ATEXIT_FIXED
    static bool destroyedOnce_;
#endif
};

#ifndef ATEXIT_FIXED
template <class T> bool PhoenixSingleton<T>::destroyedOnce_ = false;
#endif

/*! Helper for SingletonWithLongevity below
 */
namespace SingletonPrivate
{
    template <class T> struct Adapter
    {
        void operator()(T*) { return pFun_(); }
        void (*pFun_)();
    };
}

/*! Implementation of the LifetimePolicy used by SingletonHolder:
    schedules an object's destruction in order of their longevities.
    Assumes a visible function getSingletonLongevity(T*) that returns
    the longevity of the object
 */
template <class T>
class SingletonWithLongevity
{
public:
    static void scheduleDestruction(T* pObj, void (*pFun)()) {
        SingletonPrivate::Adapter<T> adapter = { pFun };
        setSingletonLongevity(pObj, getSingletonLongevity(pObj), adapter);
    }
    static void onDeadReference() {
        throw std::logic_error("Dead Reference Detected");
    }
};

/*! Implementation of the LifetimePolicy used by SingletonHolder:
    never destroys the object
 */
template <class T> class NoDestroy {
public:
    static void scheduleDestruction(T*, void (*)()) {}
    static void onDeadReference() {}
};

/*! Provides Singleton amenities for a type T.
    To protect that type from spurious instantiations, you have to
    protect it yourself.
 */
template
<
    typename T,
    class CreationPolicy = CreateUsingNew<T>,
    class LifetimePolicy = DefaultLifetime<T>,
    class ThreadingModel = DEFAULT_THREADING_CL<>
>
  class SingletonHolder {
public:
    static T& instance();
    static bool isDead() { return destroyed_; }

private:
    // Helpers
    static void makeInstance();
    static void destroySingleton();

    // Protection
    SingletonHolder();

    // Data
    typedef T* PtrInstanceType;
    static PtrInstanceType pinstance_;
    static bool destroyed_;
};

/*! SingletonHolder's data
 */
template <class T, class C, class L, class M>
    typename SingletonHolder<T, C, L, M>::PtrInstanceType
        SingletonHolder<T, C, L, M>::pinstance_;

template <class T, class C, class L, class M>
    bool SingletonHolder<T, C, L, M>::destroyed_;

/*! SingletonHolder::instance
 */
template
<
    class T,
    class CreationPolicy,
    class LifetimePolicy,
    class ThreadingModel
>
  inline T& SingletonHolder<T, CreationPolicy,
                            LifetimePolicy, ThreadingModel>::instance()
{
    if (!pinstance_)
        makeInstance();
    return *pinstance_;
}

/*! SingletonHolder::makeInstance (helper for instance)
 */
template
<
    class T,
    class CreationPolicy,
    class LifetimePolicy,
    class ThreadingModel
>
  void SingletonHolder<T, CreationPolicy,
                       LifetimePolicy, ThreadingModel>::makeInstance()
{
    typename ThreadingModel::Lock guard;

    if (!pinstance_) {
        if (destroyed_) {
            LifetimePolicy::onDeadReference();
            destroyed_ = false;
        }
        pinstance_ = CreationPolicy::create();
        LifetimePolicy::scheduleDestruction(pinstance_, &destroySingleton);
    }
}

template <class T, class CreationPolicy, class L, class M>
  void SingletonHolder<T, CreationPolicy, L, M>::destroySingleton()
{
    assert(!destroyed_);
    CreationPolicy::destroy(pinstance_);
    pinstance_ = 0;
    destroyed_ = true;
}

COMMON_NS_END

#endif // SINGLETON_H_
