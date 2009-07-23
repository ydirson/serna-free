// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#ifndef Collector_INCLUDED
#define Collector_INCLUDED 1

#include <stddef.h>
#include "Boolean.h"

// A garbage collector.

class Collector {
public:
  struct Block;

  class Object {
    friend struct Block;
    friend class Collector;
  public:
    bool readOnly() const { return readOnly_; }
    bool permanent() const { return color_ == permanentColor; }
  protected:
    Object() : readOnly_(0) { }
    virtual ~Object() { }
    // If we copy an object, don't copy inappropriate info.
    Object(const Object &obj) { hasSubObjects_ = obj.hasSubObjects_; }
    // For objects with subobjects, this must be overriden
    // to call Collector::trace for each direct subobject.
    virtual void traceSubObjects(Collector &) const { }
  private:
    enum Color {
      someColor,
      anotherColor,
      permanentColor
    };
    Object *next() const { return next_; }
    Object *prev() const { return prev_; }
    void makeHead() { prev_ = next_ = this; }
    Color color() const { return Color(color_); }
    void setColor(Color color) { color_ = color; }
    void moveAfter(Object *tail);
    bool hasSubObjects() const { return hasSubObjects_; }
    void finalize() { this->~Object(); }
  private:
    Object *prev_;
    Object *next_;
    char color_;
    char hasFinalizer_;
  protected:
    char hasSubObjects_;
    char readOnly_;
  };

  class DynamicRoot {
  public:
    DynamicRoot(Collector &);
    virtual ~DynamicRoot();
    virtual void trace(Collector &) const { }
    DynamicRoot(const DynamicRoot &);
    void operator=(const DynamicRoot &);
  private:
    DynamicRoot();
    void link(const DynamicRoot *);
    void unlink();

    DynamicRoot *next_;
    DynamicRoot *prev_;
  friend class Collector;
  };

  class ObjectDynamicRoot : public DynamicRoot {
  public:
    ObjectDynamicRoot(Collector &, Object * = 0);
    ObjectDynamicRoot &operator=(Object *obj);
    void trace(Collector &) const;
    operator Object *() const { return obj_; }
  private:
    Object *obj_;
  };

  struct Block {
    Block(Block *, size_t, size_t, Object *);
    ~Block();
    Block *next;
    Object *firstObj;
  };

  Collector(size_t maxSize);
  virtual ~Collector();
  void *allocateObject(bool hasFinalizer);
  // This is called only when the constructor throws an exception.
  void unallocateObject(void *);
  void trace(const Object *obj);
  // Permanent objects must not be altered in such a way that
  // they contain pointers to non-permanent objects.
  void makePermanent(Object *);
  // Returns the number of live objects.
  unsigned long collect();
  void makeReadOnly(Object *);
  bool objectMaybeLive(Object *);
protected:
  virtual void traceStaticRoots() const { }
private:
  Collector(const Collector &);	// undefined
  void operator=(const Collector &); // undefined
  Object *freePtr_;
  Object allObjectsList_;	// doubly-linked circular list of all objects
  Object permanentFinalizersList_;
  Object *lastTraced_;
  DynamicRoot dynRootList_;	// doubly-linked circular list
  Object::Color currentColor_;
  Block *blocks_;
  unsigned long totalObjects_;
  size_t maxSize_;

  void makeSpace();
  void traceDynamicRoots();
  void check();
  void makeReadOnly1(Object *);
  friend class DynamicRoot;
  friend class Object;
};


inline
void Collector::Object::moveAfter(Object *tail)
{
  // unlink from old position
    next_->prev_ = prev_;
  prev_->next_ = next_;
  // link in following tail
    next_ = tail->next_;
  tail->next_->prev_ = this;
  prev_ = tail;
  tail->next_ = this;
}

inline
void *Collector::allocateObject(bool hasFinalizer)
{
  if (freePtr_ == &allObjectsList_)
    makeSpace();
  Object *tem = freePtr_;
  freePtr_ = freePtr_->next();
  tem->setColor(currentColor_);
  tem->hasFinalizer_ = hasFinalizer;
  if (hasFinalizer)
    tem->moveAfter(&allObjectsList_);
  return tem;
}

inline
void Collector::trace(const Object *obj)
{
  if (obj && obj->color() != currentColor_
      && obj->color() != Object::permanentColor) {
    ((Object *)obj)->setColor(currentColor_);
    ((Object *)obj)->moveAfter(lastTraced_);
    lastTraced_ = (Object *)obj;
  }
}

inline
void Collector::DynamicRoot::link(const DynamicRoot *root)
{
  DynamicRoot *list = (DynamicRoot *)root;
  // link in just after list
  prev_ = list;
  next_ = list->next_;
  list->next_->prev_ = this;
  list->next_ = this;
}

inline
void Collector::DynamicRoot::unlink()
{
  next_->prev_ = prev_;
  prev_->next_ = next_;
}

inline
Collector::DynamicRoot::DynamicRoot(Collector &c)
{
  link(&c.dynRootList_);
}

inline
Collector::DynamicRoot::DynamicRoot()
: next_(this), prev_(this)
{
}

inline
Collector::DynamicRoot::DynamicRoot(const DynamicRoot &root)
{
  link(&root);
}

inline
void Collector::DynamicRoot::operator=(const DynamicRoot &root)
{
  unlink();
  link(&root);
}

inline
Collector::ObjectDynamicRoot::ObjectDynamicRoot(Collector &c, Object *obj)
: Collector::DynamicRoot(c), obj_(obj)
{
}

inline
Collector::ObjectDynamicRoot &
Collector::ObjectDynamicRoot::operator=(Object *obj)
{
  obj_ = obj;
  return *this;
}

inline
void Collector::ObjectDynamicRoot::trace(Collector &c) const
{
  c.trace(obj_);
}

inline
Collector::Block::~Block()
{
  ::operator delete(firstObj);
}

inline
bool Collector::objectMaybeLive(Object *obj)
{
  return obj->color() == currentColor_ || obj->color() == Object::permanentColor;
}

inline
void Collector::makeReadOnly(Object *obj)
{
  if (!obj->hasSubObjects())
    obj->readOnly_ = 1;
  else if (!obj->readOnly())
    makeReadOnly1(obj);
}

#endif /* not Collector_INCLUDED */
