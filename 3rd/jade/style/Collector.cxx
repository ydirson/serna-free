// Copyright (c) 1996 James Clark
// See the file copying.txt for copying permission.

#include "config.h"
#include "Collector.h"
#include "macros.h"
#include <stdlib.h>

#if 0
#define DEBUG
#endif

Collector::Collector(size_t maxSize)
: freePtr_(&allObjectsList_), currentColor_(Object::someColor),
  blocks_(0), lastTraced_(0), totalObjects_(0), maxSize_(maxSize)
{
  allObjectsList_.makeHead();
  permanentFinalizersList_.makeHead();
}

Collector::~Collector()
{
  if (freePtr_ != &allObjectsList_) {
    for (Object *p = allObjectsList_.next(); p != freePtr_; p = p->next()) {
      if (!p->hasFinalizer_)
	break;
      p->finalize();
    }
  }
  {
    for (Object *p = permanentFinalizersList_.next();
	 p != &permanentFinalizersList_;
	 p = p->next()) {
      ASSERT(p->hasFinalizer_);
      p->finalize();
    }
  }
  while (blocks_) {
    Block *tem = blocks_;
    blocks_ = blocks_->next;
    delete tem;
  }
}

void Collector::makeSpace()
{
  unsigned long nLive = collect();
  // Ensure that at least one-quarter of the heap is free, but don't allocate fewer
  // than 512 objects.
  if (freePtr_ == &allObjectsList_  || totalObjects_ - nLive < (totalObjects_ >> 2)
      || totalObjects_ < 128) {
    size_t allocObjs;
    if (totalObjects_ < 128)
      allocObjs = 512;
    else {
      allocObjs = (totalObjects_ >> 2) - (totalObjects_ - nLive);
      if (allocObjs < 512)
	allocObjs = 512;
    }
    if (freePtr_ == &allObjectsList_) {
      blocks_ = new Block(blocks_, allocObjs, maxSize_, freePtr_->prev());
      freePtr_ = blocks_->firstObj;
    }
    else
      blocks_ = new Block(blocks_, allocObjs, maxSize_, freePtr_);
    totalObjects_ += allocObjs;
  }
#ifdef DEBUG
  check();
#endif
}

void Collector::check()
{
  size_t n = 0;

  bool allocated = 1;
  bool allowFinalizer = 1;
  for (Object *p = allObjectsList_.next();
       p != &allObjectsList_;
       p = p->next()) {
    if (p == freePtr_)
      allocated = 0;
    else if (allocated) {
      if (p->color() != currentColor_)
	abort();
      if (allowFinalizer) {
	if (!p->hasFinalizer_)
	  allowFinalizer = 0;
      }
      else if (p->hasFinalizer_)
	abort();
    }
    if (p->next()->prev() != p)
      abort();
    if (p->prev()->next() != p)
      abort();
    n++;
  }
  if (n != totalObjects_)
    abort();
  
}

// Link block in after follow.

Collector::Block::Block(Block *p, size_t n, size_t sz, Object *follow)
: next(p)
{
  Object *next = follow->next_;
  Object *prev = follow;
  Object *cur = (Object *)::operator new(n * sz);
  firstObj = follow->next_ = cur;
  for (size_t i = 0; i < n; i++) {
    Object *tem = (i == n - 1 ? next : (Object *)((char *)cur + sz));
    cur->next_ = tem;
    cur->prev_ = prev;
    prev = cur;
    cur = tem;
  }
  next->prev_ = prev;
}

unsigned long Collector::collect()
{
  Object *oldFreePtr = freePtr_;
  unsigned long nLive = 0;
  currentColor_ = (currentColor_ == Object::someColor 
		   ? Object::anotherColor
		   : Object::someColor);
  lastTraced_ = &allObjectsList_;
  traceStaticRoots();
  traceDynamicRoots();
  if (lastTraced_ != &allObjectsList_) {
    Object *scanPtr = allObjectsList_.next();
    for (;;) {
      if (scanPtr->hasSubObjects())
	scanPtr->traceSubObjects(*this);
      nLive++;
      Object *next = scanPtr->next();
      if (scanPtr->hasFinalizer_)
	scanPtr->moveAfter(&allObjectsList_);
      if (scanPtr == lastTraced_) {
	freePtr_ = next;
	break;
      }
      scanPtr = next;
    }
  }
  else
    freePtr_ = allObjectsList_.next();
  lastTraced_ = 0;
  for (Object *p = freePtr_; p != oldFreePtr; p = p->next()) {
    if (!p->hasFinalizer_)
      break;
    p->finalize();
  }
#ifdef DEBUG
    check();
#endif
  return nLive;
}

void Collector::makePermanent(Object *obj)
{
  if (!obj->hasSubObjects()) {
    // Handle the simple case quickly.
    if (obj->color() != Object::permanentColor) {
      totalObjects_--;
      obj->setColor(Object::permanentColor);
      obj->readOnly_ = 1;
      if (obj->hasFinalizer_)
	obj->moveAfter(&permanentFinalizersList_);
      else {
	obj->next_->prev_ = obj->prev_;
	obj->prev_->next_ = obj->next_;
      }
    }
  }
  else {
    Object::Color saveColor = currentColor_;
    currentColor_ = Object::permanentColor;
    lastTraced_ = &allObjectsList_;
    trace(obj);
    if (lastTraced_ != &allObjectsList_) {
      Object *scanPtr = allObjectsList_.next();
      for (;;) {
	scanPtr->readOnly_ = 1;
	if (scanPtr->hasSubObjects())
	  scanPtr->traceSubObjects(*this);
	totalObjects_--;
	Object *next = scanPtr->next();
	if (scanPtr->hasFinalizer_)
	  scanPtr->moveAfter(&permanentFinalizersList_);
	else {
	  // unlink from allObjectsList_
	  scanPtr->next_->prev_ = scanPtr->prev_;
	  scanPtr->prev_->next_ = scanPtr->next_;
	}
	if (scanPtr == lastTraced_)
	  break;
	scanPtr = next;
      }
    }
    lastTraced_ = 0;
    currentColor_ = saveColor;
  }
}

void Collector::makeReadOnly1(Object *obj)
{
  Object::Color saveColor = currentColor_;
  currentColor_ = (currentColor_ == Object::someColor 
		   ? Object::anotherColor
		   : Object::someColor);
  lastTraced_ = &allObjectsList_;
  trace(obj);
  if (lastTraced_ != &allObjectsList_) {
    Object *scanPtr = allObjectsList_.next();
    Object *firstNonFinal = 0;
    Object *lim;
    for (;;) {
      if (scanPtr->hasSubObjects())
  	scanPtr->traceSubObjects(*this);
      Object *next = scanPtr->next();
      if (scanPtr->hasFinalizer_)
	scanPtr->moveAfter(&allObjectsList_);
      else if (!firstNonFinal)
	firstNonFinal = scanPtr;
      if (scanPtr == lastTraced_) {
	lim = next;
  	break;
      }
      scanPtr = next;
    }
    // We have 1 or more objects to be made read-only in currentColor
    // Followed by 0 or more objects with finalizers in saveColor
    // Followed by 0 or more objects without finalizers in saveColor
    for (scanPtr = allObjectsList_.next(); scanPtr != lim; scanPtr = scanPtr->next()) {
      scanPtr->readOnly_ = 1;
      scanPtr->setColor(saveColor);
    }
    if (firstNonFinal) {
      for (;
           scanPtr != freePtr_ && scanPtr->hasFinalizer_;
	   scanPtr = scanPtr->next())
	;
      if (scanPtr != lim) {
	Object *last = lim->prev();
	// Move section of list from firstNonFinal up to lastTraced but not including
	// lim to before scanPtr
	firstNonFinal->prev()->next_ = last->next();
	last->next()->prev_ = firstNonFinal->prev();
	firstNonFinal->prev_ = scanPtr->prev();
	last->next_ = scanPtr->prev()->next();
	firstNonFinal->prev()->next_ = firstNonFinal;
	last->next()->prev_ = last;
      }
    }
  }
  lastTraced_ = 0;
  currentColor_ = saveColor;
#ifdef DEBUG
  check();
#endif
} 

void Collector::traceDynamicRoots()
{
  for (DynamicRoot *p = dynRootList_.next_; p != &dynRootList_; p = p->next_)
    p->trace(*this);
}

Collector::DynamicRoot::~DynamicRoot()
{
  unlink();
}

void Collector::unallocateObject(void *obj)
{
  ((Object *)obj)->moveAfter(freePtr_);
}
