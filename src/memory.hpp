#pragma once

#include "base/memory/poolalloc.hpp"
#include "base/memory/arena.hpp"
#include "base/memory/pool.hpp"
#include "base/util.hpp"

namespace minote {

using namespace base;
using namespace base::literals;

enum struct PoolSlot {
	Permanent = 0,
	PerFrame = 1,
	Temporary = 2,
	MaxSlot = Pool::MaxSlots,
};

inline auto GlobalPool = Pool();

template<typename T>
using Permanent = PoolAllocator<T, GlobalPool, +PoolSlot::Permanent>;

template<typename T>
using PerFrame = PoolAllocator<T, GlobalPool, +PoolSlot::PerFrame>;

template<typename T>
using Temporary = PoolAllocator<T, GlobalPool, +PoolSlot::Temporary>;

inline void attachArenas() {
	
	GlobalPool.attach(+PoolSlot::Permanent, Arena("Permanent", 16_mb));
	GlobalPool.attach(+PoolSlot::PerFrame, Arena("Per-frame", 16_mb));
	GlobalPool.attach(+PoolSlot::Temporary, Arena("Temporary", 16_mb));
	
}

}
