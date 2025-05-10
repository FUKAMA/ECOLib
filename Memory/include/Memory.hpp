#pragma once

#include "src/Allocator/MemoryAllocator.h"
#include "src/Pointer/Unique/UniquePtr.h"
#include "src/Pointer/Shared/SharedPtr.h"

template<typename U>
inline U&& Move(U& v) { return static_cast<U&&>(v); }
