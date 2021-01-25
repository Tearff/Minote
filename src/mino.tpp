#pragma once

#include "base/util.hpp"

namespace minote {

using namespace base;

template<size_t W, size_t H>
auto Grid<W, H>::get(glm::ivec2 position) const -> std::optional<Mino4> {
	if (position.x < 0 || position.x >= static_cast<i32>(Width) || position.y < 0)
		return Mino4::Garbage;
	if (position.y >= static_cast<i32>(Height))
		return std::nullopt;
	return m_grid[position.y * Width + position.x];
}

template<size_t W, size_t H>
void Grid<W, H>::set(glm::ivec2 position, Mino4 value) {
	if (position.x < 0 || position.x >= static_cast<i32>(Width) ||
		position.y < 0 || position.y >= static_cast<i32>(Height))
		return;
	m_grid[position.y * Width + position.x] = value;
}

template<size_t W, size_t H>
auto Grid<W, H>::stackHeight() -> size_t {
	for (auto y: nrange(0_zu, Height)) {
		bool occupied = false;
		for (auto x: nrange(0_zu, Width)) {
			if (get({x, y})) {
				occupied = true;
				break;
			}
		}
		if (!occupied)
			return y;
	}
	return Height;
}

}
