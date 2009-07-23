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
#ifndef SAPI_COMMON_VECTOR_H_
#define SAPI_COMMON_VECTOR_H_

#include "sapi/sapi_defs.h"
#include <new>
#include <string.h> // for memmove

namespace SernaApi {

/// Portable, light-weight and fast version of a vector.
/** This vector has one important difference from the STL one: this
 *  vector does not use copy constructors for moving objects around;
 *  it uses memmove() instead. So, objects which keep pointers
 *  to themselves may not behave correctly in this Vector.
 */
template <class T> class Vector {
public:
    typedef size_t      size_type;
    typedef T*          iterator;
    typedef const T*    const_iterator;

    /// default constructor
    Vector() : size_(0), ptr_(0), alloc_(0) { }

    /// reserve place for n objects
    Vector(size_type n) : size_(0), ptr_(0), alloc_(0) { append(n); }

    /// create n identical copies of the object in a vector
    Vector(size_type n, const T& t)
        : size_(0), ptr_(0), alloc_(0)
    {
        insert(ptr_ + size_, n, t);
    }
    /// copy constructor
    Vector(const Vector<T> & v) 
        : size_(0), ptr_(0), alloc_(0)
    {
        insert(ptr_ + size_, v.ptr_, v.ptr_ + v.size_);
    }
    ~Vector()
    {
        if (ptr_) {
            erase(ptr_, ptr_ + size_);
            ::operator delete(ptr_);
        }
    }
    /// adjust vector size to n
    void resize(size_type n) {
        if (n < size_)
            erase(ptr_ + n, ptr_ + size_);
        else if (n > size_)
            append(n - size_);
    }

    /// assignment operator
    Vector<T>&  operator=(const Vector<T> &);

    /// initialize vector with n objects of type T
    void        assign(size_type n, const T &);

    /// add object T to the tail of the vector
    iterator    push_back(const T &t) {
        reserve(size_ + 1);
        (void)new (ptr_ + size_) T(t);
        return (ptr_ + size_++);
    }
    /// remove object from the tail of the vector
    void        pop_back() { --size_; ptr_[size_].~T(); }

    /// insert n objects T before p
    iterator    insert(const_iterator p, size_type n, const T& t);

    /// insert objects q1...q2 before p
    iterator    insert(const_iterator p, const_iterator q1, const_iterator q2);

    /// insert single object T before p
    iterator    insert(const_iterator p, const T& t) { return insert(p, 1, t); }

    /// swap contents of the vector with another vector
    void        swap(Vector<T> &);

    /// clear vector
    void        clear() { erase(ptr_, ptr_ + size_); }

    /// returns size of the vector
    size_type   size() const { return size_; }

    T&          operator[](size_type i) { return ptr_[i]; }
    const T&    operator[](size_type i) const { return ptr_[i]; }

    /// returns begin of the vector
    iterator    begin() { return ptr_; }
    const_iterator  begin() const { return ptr_; }

    /// returns pointer right beyond the last element of the vector
    iterator    end() { return ptr_ + size_; }
    const_iterator end() const { return ptr_ + size_; }

    /// returns reference to the last element of the vector
    T&          back() { return ptr_[size_ - 1]; }
    const T&    back() const { return ptr_[size_ - 1]; }

    /// reserve memory for at least n objects
    void        reserve(size_type n) { if (n > alloc_) reserve1(n); }

    /// erase objects q1...q2
    iterator    erase(const_iterator q1, const_iterator q2);

    /// erase  object with index idx
    void        erase(size_type idx);

    /// append vector v1 to the current vector
    Vector<T>&  operator+=(const Vector<T>& v1);

    /// Give up ownership of the data; reset the vector
    T*          release() {
        T* temp = ptr_;
        ptr_ = 0;
        size_ = alloc_ = 0;
        return temp;
    }

private:
    void        append(size_type);
    void        reserve1(size_type);

    size_type   size_;
    T*          ptr_;
    size_type   alloc_; // allocated size
};


////////////////////////////////////////////////////////////////

template <class T> Vector<T>& Vector<T>::operator=(const Vector<T> &v)
{
    if (&v != this) {
        size_type n = v.size_;
        if (n > size_) {
            n = size_;
            insert(ptr_ + size_, v.ptr_ + size_, v.ptr_ + v.size_);
        }
        else if (n < size_)
            erase(ptr_ + n, ptr_ + size_);
        while (n-- > 0)
            ptr_[n] = v.ptr_[n];
    }
    return *this;
}

template <class T> void Vector<T>::assign(size_type n, const T &t)
{
    size_type sz = n;
    if (n > size_) {
        sz = size_;
        insert(ptr_ + size_, n - size_, t);
    }
    else if (n < size_)
        erase(ptr_ + n, ptr_ + size_);
    while (sz-- > 0)
        ptr_[sz] = t;
}

template <class T>
  typename Vector<T>::iterator Vector<T>::insert(const_iterator p,
                                        size_type n,
                                        const T &t)
{
    size_type i = p - ptr_;
    reserve(size_ + n);
    if (i != size_)
        memmove(ptr_ + i + n, ptr_ + i, (size_ - i)*sizeof(T));
    for (T *pp = ptr_ + i; n-- > 0; pp++) {
        (void)new (pp) T(t);
        size_++;
    }
    return &ptr_[i];
}

template <class T>
  typename Vector<T>::iterator Vector<T>::insert(const_iterator p,
                                        const_iterator q1,
                                        const_iterator q2)
{
    size_type i = p - ptr_;
    size_type n = q2 - q1;
    reserve(size_ + n);
    if (i != size_)
        memmove(ptr_ + i + n, ptr_ + i, (size_ - i)*sizeof(T));
    for (T *pp = ptr_ + i; q1 != q2; q1++, pp++) {
        (void)new (pp) T(*q1);
        size_++;
    }
    return &ptr_[i];
}

template <class T> void Vector<T>::swap(Vector<T> &v)
{
    {
        T *tem = ptr_;
        ptr_ = v.ptr_;
        v.ptr_ = tem;
    }
    {
        size_type tem = size_;
        size_ = v.size_;
        v.size_ = tem;
    }
    {
        size_type tem = alloc_;
        alloc_ = v.alloc_;
        v.alloc_ = tem;
    }
}

template <class T> void Vector<T>::append(size_type n)
{
    reserve(size_ + n);
    while (n-- > 0)
        (void)new (ptr_ + size_++) T;
}

template <class T> void Vector<T>::erase(size_type idx)
{
    T* p = ptr_ + idx;
    p->~T();
    --size_;
    if (size_ > idx)
        memmove(p, p + 1, (size_ - idx) * sizeof(T));
}

template <class T> T* Vector<T>::erase(const_iterator p1, const_iterator p2)
{
    for (const T *p = p1; p != p2; p++)
#ifdef __SUNPRO_CC
        const_cast<T*>(p)->~T();
#else
        p->~T();
#endif
    if (p2 != ptr_ + size_)
        memmove(const_cast<T*>(p1), p2, ((ptr_ + size_) - p2) * sizeof(T));
    size_ -= p2 - p1;
    return const_cast<T*>(p1);
}

template <class T>
  Vector<T>& Vector<T>::operator+=(const Vector<T>& v)
{
    if (0 == v.size_)
        return *this;
    reserve(size_ + v.size_);
    for (size_t i = 0; i < v.size_; ++i)
        (void)new (ptr_ + size_ + i) T(*(v.ptr_ + i));
    size_ += v.size_;
    return *this;
}

template <class T> void Vector<T>::reserve1(size_type size)
{
    // Try to preserve a consistent start in the
    // event of an out of memory exception.
    size_type newAlloc = alloc_*2;
    if (size > newAlloc)
        newAlloc += size;
    void *p = ::operator new(newAlloc * sizeof(T));
    alloc_ = newAlloc;
    if (ptr_) {
        memcpy(p, ptr_, size_ * sizeof(T));
        ::operator delete(ptr_);
    }
    ptr_ = reinterpret_cast<T*>(p);
}

} // namespace SernaApi

#endif // SAPI_VECTOR_H
