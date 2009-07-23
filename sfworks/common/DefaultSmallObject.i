// Copyright (c) 2001 Syntext, Inc. All Rights Reserved.
//
/*! \file
    Implementation of DefaultSmallObject with default parameters
 */
#include "common/common_defs.h"
#include "common/DefaultSmallObject.h"
#include "common/SmallObject.h"
#include <iostream>

CVSID(DEFAULT_SMALL_OBJECT_I, "");

COMMON_NS_BEGIN

void* DefaultSmallObject::operator new(size_t size)
{
    void* p = SmallObject<>::operator new(size);
//DEBUG    std::cerr << "SMALLOBJ_ALLOCATE: chunk " << std::hex << p << std::dec << " size " << size << std::endl; 
    return p;
}

void DefaultSmallObject::operator delete(void* p, size_t size)
{
//DEBUG    std::cerr << "SMALLOBJ_DELETE: chunk " << std::hex << p << std::dec << " size " << size << std::endl;	
    SmallObject<>::operator delete(p, size);
}

COMMON_NS_END
