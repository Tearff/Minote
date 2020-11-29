// Minote - sys/glfw.hpp
// Encapsulated handling of the GLFW library' global state

#pragma once

#include <GLFW/glfw3.h>
#include "base/string.hpp"
#include "base/util.hpp"
#include "base/time.hpp"

namespace minote {

struct Glfw {

	// Initialize the windowing system and relevant OS-specific bits.
	Glfw();

	// Clean up the windowing system.
	~Glfw();

	// Collect pending events for all open windows and keep them responsive.
	// Call this as often as your target resolution of user input; at least
	// 240Hz is recommended.
	void poll();

	// Retrieve the description of the most recently encountered GLFW error
	// and clear GLFW error state. The description must be used before the next
	// GLFW call.
	// This function can be used from any thread.
	static auto getError() -> string_view;

	// Return the time passed since Glfw() was last called. If it was never
	// called, 0 is returned instead.
	// This function can be used from any thread.
	static auto getTime() -> nsec;

	// No copying or moving
	Glfw(Glfw const&) = delete;
	auto operator=(Glfw const&) -> Glfw& = delete;

private:

	// Ensure only one instance can exist
	inline static bool exists = false;

};

}
