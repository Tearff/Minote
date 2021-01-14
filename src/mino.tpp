#pragma once

namespace minote {

template<size_t W, size_t H>
auto Grid<W, H>::get(glm::ivec2 position) const -> std::optional<Mino> {
	if (position.x < 0 || position.x >= static_cast<i32>(Width) || position.y < 0)
		return Mino::Garbage;
	if (position.y >= static_cast<i32>(Height))
		return std::nullopt;
	return m_grid[position.y * Width + position.x];
}

template<size_t W, size_t H>
void Grid<W, H>::set(glm::ivec2 position, Mino value) {
	if (position.x < 0 || position.x >= static_cast<i32>(Width) ||
		position.y < 0 || position.y >= static_cast<i32>(Height))
		return;
	m_grid[position.y * Width + position.x] = value;
}

}